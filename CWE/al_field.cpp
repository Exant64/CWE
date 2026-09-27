#include "stdafx.h"
#include "asmutil.h"

ASM_FUNC task* sub_55A920(int a1, NJS_VECTOR* a2, task* parent, Uint8 a4, float a5) {
    // save regs
    ASM_PUSH( ebx );
    ASM_PUSH( edi );

    // arguments
    ASM_PUSH(      ASM_ESP(5+0+2) ); // a5
    ASM_PUSH(      ASM_ESP(4+1+2) ); // a4
    ASM_PUSH(      ASM_ESP(3+2+2) ); // parent
    ASM_MOVE( edi, ASM_ESP(2+3+2) ); // a2
    ASM_MOVE( ebx, ASM_ESP(1+3+2) ); // a1

    // call
    ASM_CALL_R( edx, 0x55A920 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // restore regs
    ASM_POP( edi );
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}
void AL_ChildFieldCreateT(task* a1, Uint8 a2, NJS_VECTOR* a3, float a4, int timer)
{
	sub_55A920(timer, a3, a1, a2, a4);
}