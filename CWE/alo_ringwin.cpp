#include "stdafx.h"

static ASM_FUNC void ALO_RingWinAdd_(int a1) {
    // arguments
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // result

    // call
    ASM_CALL_R( edx, 0x005A6B30 );

    // return
    ASM_RET( 0 );
}

DataPointer(int, RingDisplayFieldC, 0x01A2787C);

void ALO_RingWinAdd(int num) {
	ALO_RingWinAdd_(RingDisplayFieldC + num);
}