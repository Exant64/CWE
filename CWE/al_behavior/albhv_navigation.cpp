#include "stdafx.h"

#include <navigation/navsys.h>
#include <move.h>
#include <memory.h>
#include <playsound.h>
#include <al_motion.h>
#include <Chao.h>
#include <ninja_functions.h>
#include <ALifeSDK_Functions.h>

#include "albhv_navigation.h"

// for now, the only thing it does is to check if they can swim
static uint32_t GetChaoNaviExcludeFlags(task *tp) {
    uint32_t flags = 0;

    if (AL_ParameterGetSkill(tp, SKILL_SWIM) < GET_GLOBAL()->SkillSwimBataashi) {
        flags |= NAV_FLAGS_SWIM;
    }

    return flags;
}

int ALBHV_CheckNavigate(task* tp) {
    enum {
        MD_QUERY_THINK,
        MD_QUERY_WAIT
    };

    auto work = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;
    MOVE_WORK* move = (MOVE_WORK*)tp->EntityData2;

    switch(bhv->Mode) {
        case MD_QUERY_THINK:
            AL_SetMotionLink(tp, ALM_STAND);

            work->NaviCurrQueryIndex = NavSysQueryPath(
                &work->entity.Position, 
                &work->NaviTargetPos, 
                GetChaoNaviExcludeFlags(tp)
            );
            
            bhv->Mode = MD_QUERY_WAIT;

            break;

        case MD_QUERY_WAIT: {
            const auto result = NavSysGetResult(work->NaviCurrQueryIndex);
            if (result) {
                #ifndef IMGUIDEBUG
                    NavSysDiscardResult(work->NaviCurrQueryIndex);
                #endif

                if(result->empty()) {
                    // failed
                    // todo: do we want any visual?

                    return BHV_RET_BREAK;
                }

                work->NaviPointCount = result->size();
                
                work->pNaviPoints = ALLOC_ARRAY(work->NaviPointCount, NJS_POINT3);
                memcpy(work->pNaviPoints, result->data(), work->NaviPointCount * sizeof(*work->pNaviPoints));

                return BHV_RET_FINISH;
            }
        } break;
    }

    return BHV_RET_CONTINUE;
}

static void TurnToNaviAim2(task* tp, int adjust) {
    auto work = GET_CHAOWK(tp);
    MOVE_WORK* move = (MOVE_WORK*)tp->EntityData2;

    move->AimAng.y = njArcTan2(work->NaviAimPos.x - work->entity.Position.x, work->NaviAimPos.z - work->entity.Position.z);
    work->entity.Rotation.y = AdjustAngle(work->entity.Rotation.y, move->AimAng.y, adjust);
}

int ALBHV_Navigation(task* tp) {
    enum {
        MD_START,
        MD_WALK_START,
        MD_WALK,
        MD_JUMP_TO_POND_START,
        MD_JUMP_TO_POND,
        MD_SWIM_START,
        MD_SWIM,
        MD_CLIMB_START,
        MD_CLIMB_1,
        MD_CLIMB_2,
        MD_ARRIVE
    };

    auto work = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;
    MOVE_WORK* move = (MOVE_WORK*)tp->EntityData2;

    switch(bhv->Mode) {
        case MD_START:
            // always start with walking, since they won't start navigation from water
            bhv->Mode = MD_WALK_START;
            bhv->SubMode = 0;
            work->NaviAimPos = work->pNaviPoints[bhv->SubMode];
            [[fallthrough]];
        case MD_WALK_START:
            if(AL_ParameterGetSkill(tp, SKILL_RUN) < GET_GLOBAL()->SkillWalk) {
                AL_SetMotionLink(tp, ALM_HAIHAI);
            }
            else {
                // intentionally left out the rest of the run anims
                // we don't want the chao to go too fast based on skill
                // but the actual fast running anims will look off with the walking speeds
                AL_SetMotionLink(tp, ALM_ARUKU);
            }

			bhv->Mode = MD_WALK;
            break;

        case MD_WALK: {
            if(work->entity.Position.y + 2 < move->WaterY) {
                bhv->Mode = MD_SWIM_START;
                break;
            }

            TurnToNaviAim2(tp, int(384 / 1.5f));

            // match-decomp WalkControl code
            if(move->Flag & 0x400) {
                if(AL_ParameterGetSkill(tp, SKILL_RUN) < GET_GLOBAL()->SkillWalk) {
                    AL_ForwardAcc(tp, GET_GLOBAL()->WalkSlowAcc);
                }
                else {
                    AL_ForwardAcc(tp, GET_GLOBAL()->WalkAcc * 0.8f);
                }

                const NJS_VECTOR up = {0, 1, 0};
                const float dot = njInnerProduct(&move->Shadow.hit[XYZS_ASS_YL].normal, &up);

                if(dot < 0.7f) {
                    move->Velo.y += 0.55f;
                    bhv->Mode = MD_JUMP_TO_POND_START;
                }
            }
            break;
        }

        case MD_JUMP_TO_POND_START: {
            Angle ang = work->entity.Rotation.y;

            move->Velo.x = njSin(ang) * 0.4f;
            move->Velo.y = 0.35f;
            move->Velo.z = njCos(ang) * 0.4f;

            bhv->Mode = MD_JUMP_TO_POND;
            break;
        }

        case MD_JUMP_TO_POND:
            move->Acc.y = move->Gravity * -0.45f;
            if (move->Flag & 0x400) {
                bhv->Mode = MD_WALK_START;
            }
            break;

        case MD_SWIM_START: {
            if(AL_ParameterGetSkill(tp, SKILL_SWIM) < GET_GLOBAL()->SkillSwimCrawl) {
                AL_SetMotionLink(tp, ALM_BATAASHI_B);
            }
            else {
                AL_SetMotionLink(tp, ALM_CRAWL);
            }

            PlaySound_XYZ(0x1020, &GET_CHAOWK(tp)->entity.Position, 0, 0, 0);

            bhv->Mode = MD_SWIM;
            break;
        }

        case MD_SWIM: {
            // attempt at code for climbing out
            if(move->Flag & 0x4000) {
                bhv->Timer = 0;
                bhv->Mode = MD_CLIMB_START;
            }
            
            // AL_CheckWater logic, but with flag logic removed, we can do that right here cleaner
            if(work->entity.Position.y + 2 < move->WaterY) {
                if(move->Velo.y < 0) {
                    move->Velo.y *= 0.1f;    
                }
            }
            else if (move->Flag & 0x400) {
                bhv->Mode = MD_WALK_START;
            }

            // swimcontrol code from match decomp
            TurnToNaviAim2(tp, 288);

            if(work->entity.Position.y <= move->WaterY - 2.1f) {
                // swim at minimum speed
                const float spd = GET_GLOBAL()->SkillSwimAccBase + GET_GLOBAL()->SkillSwimBataashi * GET_GLOBAL()->SkillSwimAccRatio;

                work->entity.Position.y += 0.1f;
                if(work->entity.Position.y > move->WaterY - 2.1f)
                    work->entity.Position.y = move->WaterY - 2.1f;    

                move->Acc.y = -move->Gravity - 0.1f * move->Velo.y;

                move->Acc.x = njSin(work->entity.Rotation.y) * spd - move->Velo.x * 0.05f;
                move->Acc.z = njCos(work->entity.Rotation.y) * spd - move->Velo.z * 0.05f;
            }
            
            SE_CallV2_TIMER(tp, 0x101F, &GET_CHAOWK(tp)->entity.Position, 1, -25, 90);

            break;
        }

        case MD_CLIMB_START:
            AL_SetMotionLink(tp, ALM_NOBORU);

            move->Velo = {}; // MOV_ClearVelo(tp);

            bhv->Mode = MD_CLIMB_1;
            break;

        case MD_CLIMB_1:
            if(move->Flag & 0x4800) {
                Angle ang = njArcTan2(move->FrontWall.vx, move->FrontWall.vz);
                work->entity.Rotation.y = AdjustAngle(work->entity.Rotation.y, ang + NJM_DEG_ANG(180), 0x800);
                move->Flag &= ~0x4000;
                bhv->Timer = 0;
            }
            else {
                if(bhv->Timer++ > 2) {
                    bhv->Timer = 0;
                    work->ClimbFirstPos = work->entity.Position.y;
                    bhv->Mode++;
                }
            }
            break;

        case MD_CLIMB_2:
            bhv->Timer++;
            if(bhv->Timer > 3 && (move->Flag & 0x400)) {
                const NJS_VECTOR up = {0, 1, 0};
        
                if(njInnerProduct(&move->Shadow.hit[XYZS_ASS_YL].normal, &up) > 0.7f) {
                    bhv->Mode = MD_WALK_START;
                }
            }
            else if(move->Flag & 0x4800) {
                bhv->Mode--;
            }

            if(bhv->Timer > 30 || (GET_CHAOWK(tp)->entity.Position.y - work->ClimbFirstPos) > 2.5f) {
                bhv->Mode = MD_WALK_START;
            }
            break;

        case MD_ARRIVE: {
            return BHV_RET_FINISH;
        }
    }

    switch (bhv->Mode) {
        case MD_CLIMB_1:
        case MD_CLIMB_2: {
            const Angle ang = work->entity.Rotation.y;

            work->entity.Position.x += 0.1f * njSin(ang);
            work->entity.Position.y += 0.08f;
            work->entity.Position.z += 0.1f * njCos(ang);

            move->Acc.y = -move->Gravity;
        } break;

        case MD_WALK:
        case MD_SWIM:
            const float dist = CheckDistance(&work->entity.Position, &work->NaviAimPos);
			if (dist > 0 && dist < 4) {
                if(bhv->SubMode == work->NaviPointCount - 1) {
                    bhv->Mode = MD_ARRIVE;
                }
                else {
                    work->NaviAimPos = work->pNaviPoints[++bhv->SubMode];
                }
			}
            
            break;
    }

    return BHV_RET_CONTINUE;
}

void CreatePathAtPos(size_t chaoID, NJS_POINT3& endPos) {
    auto task = GetChaoObject(0, chaoID);
    
    MOV_SetAimPos(task, &endPos);

    AL_SetBehavior(task, ALBHV_SetNaviTarget<NAVIGATION_TYPE::AIM>);
    AL_SetNextBehavior(task, ALBHV_CheckNavigate);
    AL_SetNextBehavior(task, ALBHV_Navigation);
}
