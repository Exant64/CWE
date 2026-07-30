#include "stdafx.h"

#include "asmutil.h"

ASM_FUNC Sint8 SE_Call_Timer(int tone, const void* id, int pri, int volofs, int timer) {
    // arguments
    ASM_PUSH(      ASM_ESP(5+0) ); // timer
    ASM_PUSH(      ASM_ESP(4+1) ); // volofs
    ASM_PUSH(      ASM_ESP(3+2) ); // pri
    ASM_PUSH(      ASM_ESP(2+3) ); // id
    ASM_MOVE( edi, ASM_ESP(1+4) ); // tone

    // call
    ASM_CALL_R( edx, 0x004374D0 );

    // end arguments
    ASM_ESP_ADD( 4 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC Sint8 SE_CallV2_Timer(int tone, const void* id, int pri, int volofs, int timer, const NJS_POINT3* pPos) {
    // save regs
    ASM_PUSH( esi );
    ASM_PUSH( ebx );
    ASM_PUSH( edi );
    
    // arguments
    ASM_MOVE( esi, ASM_ESP(6+0 +3) ); // pPos
    ASM_PUSH(      ASM_ESP(5+0 +3) ); // timer
    ASM_PUSH(      ASM_ESP(4+1 +3) ); // volofs
    ASM_PUSH(      ASM_ESP(3+2 +3) ); // pri
    ASM_MOVE( ebx, ASM_ESP(2+3 +3) ); // id
    ASM_MOVE( edi, ASM_ESP(1+3 +3) ); // tone

    // call
    ASM_CALL_R( eax, 0x437590 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // pull regs
    ASM_POP( edi );
    ASM_POP( ebx );
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}
