#include "stdafx.h"

#include "asmutil.h"

ASM_FUNC void AL_GrabObjectBothHands(task* a2, task* a1) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( ecx, ASM_ESP(2+0+1) ); // a1
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a2

    // call
    ASM_CALL_R( edx, 0x0056CFB0 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}
