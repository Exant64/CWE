#include "stdafx.h"

#include "move.h"
#include "ninja_functions.h"
#include "asmutil.h"

ASM_FUNC void MOV_Control(task* eax0) {
    // arguments
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // result

    // call
    ASM_CALL_R( edx, 0x00796780 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void sub_54B230(task* eax0, float a2) {
    // arguments
    ASM_PUSH(      ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+1+0) ); // eax0

    // call
    ASM_CALL_R( edx, 0x54B230 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void ObjectMovableInitialize(taskwk* a1, int a2) {
    // arguments
    ASM_MOVE( edx, ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // a1

    // call
    ASM_CALL_R( ecx, 0x47D9E0 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void MOV_DetectCollision(task* a1) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x00798300 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

float MOV_DistFromAimXZ(task* tp) {
	NJS_POINT3* pos = &tp->twp->pos;
	NJS_POINT3* aimPos = &GET_MOVE_WORK(tp)->AimPos;

	NJS_VECTOR v = {
		pos->x - aimPos->x,
		0,
		pos->z - aimPos->z
	};

	return njScalor(&v);
}

ASM_FUNC int MOV_TurnToAim2(task* tp, int ang) {
    // arguments
    ASM_PUSH(      ASM_ESP(2+0+0) ); // a3
    ASM_MOVE( eax, ASM_ESP(1+1+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x796910 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

void MOV_SetAimPos(task* tp, NJS_POINT3* pPos) {
	if (!pPos || !tp || !tp->mwp) {
		___OutputDebugString("MOV_SetAimPos error");
		return;
	}

	GET_MOVE_WORK(tp)->AimPos = *pPos;
}

void MOV_SetVelo(task* tp, NJS_VECTOR* pVelo) {
	GET_MOVE_WORK(tp)->Velo = *pVelo;
}