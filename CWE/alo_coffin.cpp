#pragma once
#include "stdafx.h"
#include "Chao.h"
#include "al_field.h"
#include "al_toy_move.h"
#include "ninja_functions.h"
#include "renderfix.h"

#include "njdef.h"
#include "al_stage.h"
#include "data/toy/motion_alo_coffin_open.nja"

DataArray(NJS_CNK_OBJECT, object_alo_coffin, 0x1301B7C, 1);

void ALO_GetCoffinTouchPos(task* tp, NJS_POINT3* pPos) {
	const taskwk* work = tp->twp;
    
	const Angle ang = work->ang.y + Angle(0.8f * NJM_DEG_ANG(njRandom() * 180 - 90));
	const float rad = 6.5f;

	pPos->x = njSin(ang) * rad + work->pos.x;
	pPos->y = work->pos.y;
	pPos->z = njCos(ang) * rad + work->pos.z;
}

static void ALO_CoffinExecutor(task* tp) {
    taskwk* work = tp->twp;

    switch (work->mode) {
        case 0: {
            task* touch = CCL_IsHitKind(tp, CI_KIND_AL_TOUCH);
            if (touch && touch->ptp) {
                const Uint16 skill = AL_ParameterGetSkill(touch->ptp, SKILL_GUTS);
                
                if (50000 - (skill * 4) > njRandom() * 80000) {
                    work->scl.y = 0.01f;
                    work->scl.z = 0;
                    work->scl.x = 0;
                };

                work->mode = 1;
                work->smode = 0;
                work->wtimer = RAND_RANGE(30, 150);
            }
        } break;
        case 1:
            if (--work->wtimer <= 0) {
                if(AL_GetStageNumber() == CHAO_STG_DARK) {
                    SE_CallV2(1, 0, 0, 0, &work->pos);
                }

                work->mode = 2;
                work->smode = 0;
                work->wtimer = 0;
            }
            break;
        case 2:
            work->scl.x += 0.3f;
            if (work->smode == 0) {
                if (work->scl.x > 4) {
                    if (work->scl.y > 0) {
                        SE_CallV2(0x1029, 0, 0, 0, &work->pos);
                        ALW_SendCommand(tp, ALW_CMD_GO);
                    }
                    else {
                        ALW_SendCommand(tp, ALW_CMD_FINISH);
                    }
                    work->smode = 1;
                }
            }

            if (work->scl.x > motion_coffin_open[0].nbFrame - 1.0f) {
                work->scl.x = motion_coffin_open[0].nbFrame - 1.0f;
                work->mode = 3;
                work->smode = 0;
                work->wtimer = 0;
            }

            break;
        case 3:
            if (work->wtimer++ > 120) {
                work->mode = 4;
                work->smode = 0;
                work->wtimer = 0;
            }
            break;
        case 4:
            work->scl.x -= 0.3f;
            if (work->scl.x < 0) {
                if(AL_GetStageNumber() == CHAO_STG_DARK) {
                    SE_CallV2(2, 0, 0, 0, &work->pos);
                }

                work->scl.x = 0;
                work->scl.y = 0;
                work->scl.z = 0;
                work->ang.x = 0;
                work->ang.z = 0;
                work->mode = 0;
                work->smode = 0;
                work->wtimer = 0;
            }
            break;
    }
    switch (work->mode) {
        case 2:
        case 3:
            if (work->scl.x > 5.f && work->scl.y > 0) {
				static float float_C6352BC = 2.f;
				static float float_C6352C0 = 0.15f;
				static float float_C6352C4 = 0.96f;

                work->scl.z += float_C6352C0 * (float_C6352BC - work->scl.y);

                work->scl.z *= float_C6352C4;
                work->scl.y += work->scl.z;

                work->ang.x += 4096;
                work->ang.z += 4864;
            }

            break;
        case 4:
            if (work->scl.y > 0 && work->scl.x < 7.f) {
                work->scl.z = 0;
                work->scl.y *= 0.8f;
            }

            work->ang.x += 4096, work->ang.z += 4864;
            break;
    }

	AL_Toy_Move_Update(tp);

    if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
        ALW_CommunicationOff(tp);
    }

    if (!ALW_IsAttention(tp)) {
		work->flag |= 0x240u;
    }
	else {
		work->flag &= ~0x240u;
    }

    CCL_Entry(tp);
}

static void ALO_CoffinDisplayer(task* tp) {
    taskwk* work = tp->twp;

    if (AL_IsOnScreen2(tp, 4.5f, 1.2f)) {
        njSetTexture((NJS_TEXLIST*)0x01366AFC);

        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(0, work->ang.y);

        njPushMatrixEx();
        njRotateY(0, 0x8000);

        njScale(NULL, 0.75f, 0.75f, 0.75f);
        if (work->scl.x < 0.01f) {
            njCnkDrawObject(object_alo_coffin);
        } else {
            njCnkMotion(object_alo_coffin, motion_coffin_open, work->scl.x);
        }

        njPopMatrixEx();
		
        if (work->scl.y > 0.01f) {
            int angX = NJM_DEG_ANG(njSin(work->ang.x) * 15);
            int angZ = NJM_DEG_ANG(njSin(work->ang.z) * 15);

            float scl;
            if (work->scl.y < 2) {
                scl = work->scl.y * 0.25f + 0.5f;
            } else {
                scl = 1.5f;
            }

            njPushMatrixEx();
            njTranslate(0, 0, work->scl.y, 0);
            njRotateX(0, angX);
            njRotateZ(0, angZ);
            njRotateY(0, 0x8000);
            njScale(0, scl, scl, scl);
            njCnkDrawModel((NJS_CNK_MODEL*)0x121D264);
            njPopMatrixEx();
        }

		if(RenderFix_IsEnabled()) {
			njTranslate(0, 0, 0.5f, 0);
			njScale(0, 3.25f, 1, 1.85f);
			rfapi_core->pDraw->AL_ShadowDraw();
		}

        njPopMatrixEx();
    }
}

static void ALO_CoffinDestroyer(task* tp) {
    ALW_CancelEntry(tp);
}

static const CCL_INFO colli[] = {{
    0, CI_FORM_SPHERE, 0x77, 0xC, 0, 
    {0.f, 1.6f* .75f, 0.f}, 
    2.25f * .75f, 0, 0, 0, 
    0, 0, 0
}};

static void ALO_CoffinInit(task* tp) {
	tp->exec = ALO_CoffinExecutor;
	tp->disp = ALO_CoffinDisplayer;
    tp->dest = ALO_CoffinDestroyer;

	AL_Toy_Move_Register(tp, ALW_KIND_COFFIN);
}

void ALO_CoffinCreate(NJS_POINT3* pPos, Angle angY) {
    task* box = CreateElementalTask(IM_TWK, LEV_4, ALO_CoffinInit, "ALO_CoffinExecutor");
    taskwk* work = box->twp;

    work->pos = *pPos;
    work->ang.y = angY;

    work->scl.y = 0;
    work->scl.z = 0;
    work->ang.x = 0;
    work->ang.z = 0;

	AL_Toy_Move_Init(box, colli, 1);
}