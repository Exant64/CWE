#include "SA2Functions.h"
#include "chaoenum.h"
#include "playsound.h"
#include "stdafx.h"
#include "ChaoMain.h"
#include "al_behavior/al_intention.h"
#include "al_emotion.h"
#include "al_motion.h"
#include "Chao.h"
#include "al_parameter.h"
#include "move.h"
#include "ninja_functions.h"
#include "albhv.h"
#include "al_behavior.h"

static ASM_FUNC int WalkControl(task* tp) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x5A3000 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

static int GenericWalk(task* tp) {
    MOVE_WORK* move = GET_MOVE_WORK(tp);

    if(move->Flag & 0x400) {
        AL_ForwardAcc(tp, GET_GLOBAL()->WalkAcc);
    }

    {
        float velo = njScalor(&move->Velo);
        AL_SetMotionSpd(tp, velo * 16);
    }
    
    return WalkControl(tp);
}

static int ALBHV_PainWalk(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
        case 0:
            AL_SetMotionLink(tp, ALM_YORO_WALK);

            AL_FaceChangeEye(tp, AL_EYE_NUM_GURUGURU);
            AL_FaceChangeMouth(tp, AL_MOUTH_NUM_MUSU);

            bhv->Mode++;
            break;
        case 1:
            break;
    }

    return GenericWalk(tp);
}

static int ALBHV_AngerWalk(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
        case 0:
            if (njRandom() < 0.5f) {
                AL_SetMotionLink(tp, ALM_ANGER_WALK);
            }
            else {
                AL_SetMotionLink(tp, ALM_ANGER_WALK_CHIKUSYOU);
            }

            AL_FaceChangeEye(tp, AL_EYE_NUM_ANGER);

            if (GET_CHAOWK(tp)->Face.MouthDefaultNum == AL_MOUTH_NUM_SHARK) {
                AL_FaceChangeMouth(tp, AL_MOUTH_NUM_GEE);
            } else {
                AL_FaceChangeMouth(tp, AL_MOUTH_NUM_MUSU);
            }

            bhv->Mode++;
            break;
        case 1:
            break;
    }

    return GenericWalk(tp);
}

static int ALBHV_SorrowWalk(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
        case 0: {
            const auto sorrow = AL_EmotionGetValue(tp, EM_MD_SORROW);
            float score = AL_CalcMoodScoreTypeA(NJM_MIN(200, sorrow + 100), 90);
            AL_ScoreRandomize(&score);

            if(score > 0.1f) {
                if(njRandom() < 0.5f) {
                    AL_SetMotionLink(tp, ALM_CRY_WALK_A);
                }
                else {
                    AL_SetMotionLink(tp, ALM_CRY_WALK_B);
                }

                bhv->Mode = 1;
                bhv->Timer = RAND_RANGE(200, 360);
            }
            else {
                AL_SetMotionLink(tp, ALM_TOBOTOBO);
                bhv->Mode = 2;
            }

            AL_FaceChangeEye(tp, ChaoEyes_ClosedHappy);
            AL_FaceChangeMouth(tp, ChaoMouth_ClosedFrown); 
        } break;

        case 1:
            if(!--bhv->Timer) {
                if (njRandom() < 0.75f) {
                    switch(AL_GetMotionNum(tp)) {
                        case ALM_CRY_WALK_A:
                            AL_SetMotionLink(tp, ALM_CRY_WALK_A2B);

                            if(njRandom() < 0.3f) {
                                AL_SE_CallV2(TONE(6, 1), 0, 0, 110, &tp->twp->pos);
                            }
                            break;
                        case ALM_CRY_WALK_B:
                            AL_SetMotionLink(tp, ALM_CRY_WALK_B2A);
                            break;
                    }            
                }

                bhv->Timer = RAND_RANGE(200, 360);
            }

            break;
        
        case 2:
            break;
    }

    return GenericWalk(tp);
}

int ALBHV_Move_r(task* tp) {
    int ret = ALBHV_Move(tp);

    if(gConfigVal.MoreAnimation) {
        if(AL_ParameterGetSkill(tp, SKILL_RUN) < GET_GLOBAL()->SkillWalk) {
            return ret;
        }

        // adding alt walk behaviors here, only running if the chao was gonna walk anyways
        if (njRandom() < 0.33f) {
            return ret;
        }

        if(AL_EmotionGetValue(tp, EM_MD_PAIN) > 10) {
            AL_SetBehavior(tp, ALBHV_PainWalk);
            return BHV_RET_CONTINUE;
        }

        if(AL_EmotionGetValue(tp, EM_MD_ANGER) > 10 || AL_EmotionGetValue(tp, EM_MD_SURPRISE) > 10) {
            AL_SetBehavior(tp, ALBHV_AngerWalk);
            return BHV_RET_CONTINUE;
        }

        if(AL_EmotionGetValue(tp, EM_MD_SORROW) > 10) {
            AL_SetBehavior(tp, ALBHV_SorrowWalk);
            return BHV_RET_CONTINUE;
        }
    }

    return ret;
}