#include "stdafx.h"

#include <Chao.h>
#include <ChaoMain.h>
#include "al_behavior.h"
#include "albhv.h"
#include "alo_coffin.h"

// todo: move to albhv_common
static int ALBHV_TurnToLockOnFast(task* tp) {
    chaowk* work = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;

    switch (bhv->Mode) {
        case 0:
            // AL_SetMotionLinkStep(tp, ALM_ARUKU, 15);
            bhv->Timer = 90;
            bhv->Mode = 1;
            break;

        case 1:
            if (bhv->Timer-- <= 0) {
                return BHV_RET_FINISH;
            }

            if(ALW_TurnToLockOn(tp, 512) < 512) {
                return BHV_RET_FINISH;
            }

            break;
    }

    AL_SetMotionSpd(tp, 1.6f);

    return BHV_RET_CONTINUE;
}

// this skips over setting the animations to not restart
// the animations mid-behavior chain
static int ALBHV_GoToLockOn_NoAnim(task* tp) {
    chaowk* work = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;

    if(!bhv->Mode) {
        bhv->Mode++;
    }

    return ALBHV_GoToLockOn(tp);
}

static int ALBHV_WaitForCoffin(task* tp) {
    chaowk* work = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;

    switch(bhv->Mode) {
        case 0:
            switch(AL_GetMotionPosture(tp)) {
                case 2:
                case 1:
                case 3:
                    AL_SetMotionLinkStep(tp, ALM_HIMA_SIT, 35);
                    break;
                case 0:
                default: {
                    float randval = njRandom();
                    if(randval < 0.15f) {
                        AL_SetMotionLink(tp, ALM_HATENA_HATE2);
                    }
                    else if (randval < 0.5f) {
                        AL_SetMotionLink(tp, ALM_STAND);
                    }
                    else if (randval < 0.6f) {
                        AL_SetMotionLink(tp, ALM_KYOROKYORO_UP);
                    }
                    else if (randval < 0.7f) {
                        AL_SetMotionLink(tp, ALM_KYOROKYORO_DOWN);
                    }
                    else {
                        AL_SetMotionLink(tp, ALM_OSYABURI);
                    }
                    
                    break;
                }
            }

            bhv->Mode++;
            [[fallthrough]];
        case 1:
            switch(ALW_RecieveCommand(tp)) {
                case ALW_CMD_FINISH:
                    return BHV_RET_FINISH;

                case ALW_CMD_GO:
                    if(gConfigVal.MoreAnimation && njRandom() < 0.3f) {
                        AL_SetBehavior(tp, ALBHV_Runaway);
                        break;
                    }
                    
                    AL_SetBehaviorWithTimer(tp, ALBHV_Bikkuri, 500);
                    break;
            }
            break;
    }
}

int ALBHV_GoToCoffin(task* tp) {
    task* pToy = AL_GetFoundToyTask(tp);

    if(!pToy) {
        return BHV_RET_FINISH;
    }

    NJS_POINT3 aim;
    ALO_GetCoffinTouchPos(pToy, &aim);
    MOV_SetAimPos(tp, &aim);

    ALW_LockOn(tp, pToy);

    AL_SetBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B480>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToAim>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToLockOn_NoAnim>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_TurnToLockOnFast>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56BBA0>);
    AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_WaitForCoffin>);

    ALW_CommunicationOn(tp, pToy);

    return BHV_RET_CONTINUE;
}