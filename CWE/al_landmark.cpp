#include "stdafx.h"

#include "al_landmark.h"
#include "asmutil.h"

ASM_FUNC int AL_GetCurrLandAttr(NJS_POINT3* pPos) {
	// arguments
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x535010 );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void sub_534F80(int a1, NJS_VECTOR* a2, signed int a3) {
    // save regs
    ASM_PUSH( edi );

    // arguments
    ASM_PUSH(      ASM_ESP(3+0+1) ); // a3
    ASM_MOVE( edi, ASM_ESP(2+1+1) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+1+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x534F80 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // restore regs
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}

void AL_GetRandomAttrPos(eLANDMARK_ATTR attr, NJS_POINT3* pPos) {
	if (stru_1A15938[attr].nbIndex > 0) {
		sub_534F80((int)&stru_1A15938[attr], pPos, stru_1A15938[attr].nbIndex);
	}
}
