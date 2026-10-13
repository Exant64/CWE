#include "stdafx.h"
#include <Chao.h>
#include "asmutil.h"

ASM_FUNC void SetMotionLink(MOTION_CTRL* a1, int a2) {
	// arguments
    ASM_MOVE( edx, ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // result

    // call
    ASM_CALL_R( ecx, 0x00793C40 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void SetMotionLinkStep(MOTION_CTRL* result, int a2, unsigned __int16 a3) {
    // arguments
    ASM_PUSH(      ASM_ESP(3+0+0) ); // a3
    ASM_MOVE( edx, ASM_ESP(2+1+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+1+0) ); // result

    // call
    ASM_CALL_R( ecx, 0x00793D30 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

Bool AL_IsMotionStop(task* a1) {
	return GET_CHAOWK(a1)->MotionCtrl.flag & 8;
}

void AL_SetMotionLink(task* a1, int index) {
	SetMotionLink(&GET_CHAOWK(a1)->MotionCtrl, index);
}

void AL_SetMotionLinkStep(task* a1, int index, int spd) {
	SetMotionLinkStep(&GET_CHAOWK(a1)->MotionCtrl, index, spd);
}

void AL_SetMotionSpd(task* a1, float a2) {
	GET_CHAOWK(a1)->MotionCtrl.multi_spd = a2;
}

int AL_GetMotionNum(task* tp) {
	MOTION_CTRL* Ctrl = &GET_CHAOWK(tp)->MotionCtrl;
	return Ctrl->curr_num;
}

int AL_GetMotionPosture(task* tp) {
	MOTION_CTRL* Ctrl = &GET_CHAOWK(tp)->MotionCtrl;
	return Ctrl->posture;
}

int AL_IsMotionEnd(task* a1) {
	return GET_CHAOWK(a1)->MotionCtrl.flag & 1;
};
