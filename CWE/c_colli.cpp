#include "stdafx.h"

#include <SA2ModLoader.h>
#include <asmutil.h>

ASM_FUNC void CCL_ClearInfo(task* tp) {
    // arguments
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x47E6C0 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void CCL_Enable(task* a1, int a2) {
    // arguments
    ASM_MOVE( ecx, ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x00486CD0 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void CCL_Disable(task* a1, int a2) {
	// arguments
    ASM_MOVE( ecx, ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x0486D00 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void CCL_Entry(task* tp) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x47E750 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

ASM_FUNC task* AL_IsHitKindWithNum(task *a1, int a2, Uint8 a3) {
    // arguments
    ASM_PUSH(      ASM_ESP(3+0+0) ); // a3
    ASM_MOVE( ecx, ASM_ESP(2+1+0) ); // a2
    ASM_MOVE( edx, ASM_ESP(1+1+0) ); // a1

    // call
    ASM_CALL_R( eax, 0x054B1F0 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}