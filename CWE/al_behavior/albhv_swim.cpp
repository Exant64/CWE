#include "stdafx.h"

#include "asmutil.h"

ASM_FUNC void AL_DecideAimSwimPosition(task* tp) {
    // arguments
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // result

    // call
    ASM_CALL_R( edx, 0x561740 );

    // return
    ASM_RET( 0 );
}

