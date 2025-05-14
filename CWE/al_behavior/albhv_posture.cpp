#include <stdafx.h>
#include <Chao.h>

// where's the default return? undefined behavior?

enum { MD_PST_CHANGE_INIT, MD_PST_CHANGE_WAIT };

int ALBHV_PostureChangeStand(task* tp) {
    chaowk* cwk = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &cwk->Behavior;
    int posture;

    switch (bhv->Mode) {
    case MD_PST_CHANGE_INIT:
        posture = AL_GetMotionPosture(tp);
        switch (posture) {
            case AL_PST_SIT:
                AL_SetMotionLinkStep(tp, ALM_GENKI_TATI, 40);
                break;
            case AL_PST_LIE:
                AL_SetMotionLinkStep(tp, ALM_JUMP_UP, 40);
                break;
            case AL_PST_FUSE:
                AL_SetMotionLinkStep(tp, ALM_KOKEOKI, 35);
                break;
            case AL_PST_STAND:
            default:
                return BHV_RET_FINISH;
        }
        bhv->Mode = MD_PST_CHANGE_WAIT;
        break;

    case MD_PST_CHANGE_WAIT:
        if (AL_IsMotionEnd(tp))
            return BHV_RET_FINISH;
    }

    return BHV_RET_CONTINUE;
}

int ALBHV_PostureChangeSit(task* tp) {
    chaowk* cwk = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &cwk->Behavior;
    int posture;

    switch (bhv->Mode) {
    case MD_PST_CHANGE_INIT:
        posture = AL_GetMotionPosture(tp);
        switch (posture) {
            case AL_PST_LIE:
                AL_SetMotionLinkStep(tp, ALM_OKIRU, 40);
                break;            
            case AL_PST_STAND:
                AL_SetMotionLinkStep(tp, ALM_GENKI_SUWARI, 40);
                break;
            case AL_PST_FUSE:
            default:
                return BHV_RET_FINISH;
        }
        bhv->Mode = MD_PST_CHANGE_WAIT;
        break;

    case MD_PST_CHANGE_WAIT:
        if (AL_IsMotionEnd(tp))
            return BHV_RET_FINISH;
    }

    return BHV_RET_CONTINUE;
}
