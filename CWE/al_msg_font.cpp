#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_msg_font.h"
#include "asmutil.h"

ASM_FUNC void AlMsgFontDrawRegionScale2(signed int a1, MessageFontThing* a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9) {
    // arguments
    ASM_PUSH(      ASM_ESP(9+0+0) ); // a9
    ASM_PUSH(      ASM_ESP(8+1+0) ); // a8
    ASM_PUSH(      ASM_ESP(7+2+0) ); // a7
    ASM_PUSH(      ASM_ESP(6+3+0) ); // a6
    ASM_PUSH(      ASM_ESP(5+4+0) ); // a5
    ASM_PUSH(      ASM_ESP(4+5+0) ); // a4
    ASM_PUSH(      ASM_ESP(3+6+0) ); // a3
    ASM_MOVE( ecx, ASM_ESP(2+7+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+7+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x0544820 );

    // end arguments
    ASM_ESP_ADD( 7 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void AlMsgFontCreateCStr(int a1, int a2, int a3, int a4) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0+1) ); // a4
    ASM_MOVE( esi, ASM_ESP(3+1+1) ); // a3
    ASM_MOVE( ecx, ASM_ESP(2+1+1) ); // a2
    ASM_MOVE( edx, ASM_ESP(1+1+1) ); // a1

    // call
    ASM_CALL_R( eax, 0x05444E0 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void AlMsgFontDelete(MessageFontThing* a1) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x00544530 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void DrawFontThing(__int16* a1, float a2, float a3, float a4) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0+1) ); // a4
    ASM_PUSH(      ASM_ESP(3+1+1) ); // a3
    ASM_PUSH(      ASM_ESP(2+2+1) ); // a2
    ASM_MOVE( ebx, ASM_ESP(1+3+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x00544580 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // restore regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

ASM_FUNC task* AL_KinderPMessageExec_Load(task* a1, AL_KinderPMessage* a2) {
    // arguments
    ASM_PUSH(      ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+1+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x00585460 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}