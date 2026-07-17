#include "stdafx.h"

#include "ninja_functions.h"
#include "albhv.h"
#include "asm_util.h"

// todo: move to SAMT
#define AL_SetRunawayAimPos_p 0x59F490
ASM_FUNC
void
AL_SetRunawayAimPos(task* tp)
{
    // arguments
    ASM_MOVE( eax, ASM_ESP(1+0) ); // tp

    // call
    ASM_CALL_R( edx, AL_SetRunawayAimPos_p );

    // return
    ASM_RET( 0 );
}

// todo: move to al_misc
Float AL_Dist2FromAim(task* tp) {
	taskwk* twp = tp->twp;
	MOVE_WORK* mwp = GET_MOVE_WORK(tp);

	NJS_VECTOR* pos = &twp->pos;
	NJS_VECTOR* aim = &mwp->AimPos;

	return (aim->z - pos->z) * (aim->z - pos->z) + (aim->x - pos->x) * (aim->x - pos->x);
}

int ALBHV_Runaway(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;
    chaowk* work = GET_CHAOWK(tp);
    MOVE_WORK* move = GET_MOVE_WORK(tp);

    switch (bhv->Mode) {
        case 0: {
            // they definitely intended more anims here
            float randval = njRandom();
            if (randval < 0.5f) {
                AL_SetMotionLinkStep(tp, ALM_OIKAKE, 10);
            }
            else {
                AL_SetMotionLinkStep(tp, ALM_RUN_BANZAI, 10);
            }
            
            AL_FaceChangeEye(tp, ChaoEyes_Painful);
            AL_FaceChangeMouth(tp, ChaoMouth_Squiggly);

            if(ChaoStageNumber == ChaoNextStageNumber) {
                SE_CallV2(24622, 0, 0, 110, &GET_CHAOWK(tp)->pos);
            }

            AL_SetRunawayAimPos(tp);
            
            bhv->Mode++;
            bhv->SubMode = RAND_RANGE(2,5);
            bhv->Timer = 0;
            bhv->SubTimer = 0;
        }
        case 1:
            break;
    }
    
    if (AL_IsEmotionTimerReset(tp)) {
        AL_EmotionAdd(tp, EM_ST_TIRE, GET_GLOBAL()->ParamAddTireRun);
    }

    if (bhv->Timer++ > 1000) {
        return BHV_RET_FINISH;
    }

    if (!(bhv->Timer % 100)) {
        if (njRandom() < 0.03f) {
            AL_SetBehavior(tp, ALBHV_Koke);
        }
    }

    if (AL_Dist2FromAim(tp) < 8*8) {
        if (!--bhv->SubMode) {
            bhv->Mode = 0;
        } else {            
            return BHV_RET_FINISH;
        }
    }

    if (move->Flag & 0x4000) {
        bhv->Mode = 0;
    }
    
    MOV_TurnToAim2(tp, 288);

    if (move->Flag & 0x400) {        
        move->Acc.x = njSin(work->ang.y) * GET_GLOBAL()->RunAcc;
        move->Acc.z = njCos(work->ang.y) * GET_GLOBAL()->RunAcc;
    }
    
    {
        float velo = njScalor(&move->Velo);
        AL_SetMotionSpd(tp, velo * 8);
    }
    
    return BHV_RET_CONTINUE;
}
