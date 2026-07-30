#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_msg_win.h"
#include "asmutil.h"

ASM_FUNC void AlMsgWinCreate(KinderCoMessageThing* self) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x00579BB0 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void AlMsgWinAddLineC(KinderCoMessageThing* self , const char* cstr, int ja) {
    // save regs
    ASM_PUSH( esi );
    ASM_PUSH( ebx );

    // arguments
    ASM_PUSH(      ASM_ESP(3+0+2) ); // a3
    ASM_MOVE( ebx, ASM_ESP(2+1+2) ); // a1
    ASM_MOVE( esi, ASM_ESP(1+1+2) ); // a2

    // call
    ASM_CALL_R( edx, 0x00579E30 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // restore regs
    ASM_POP( ebx );
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void AlMsgWinOpen(KinderCoMessageThing* self) {
    // arguments
    ASM_MOVE( eax, ASM_ESP(1+0+0) ); // result

    // call
    ASM_CALL_R( edx, 0x579D80 );

    // return
    ASM_RET( 0 );
}

void AlMsgWinSetPos(KinderCoMessageThing* self, float px, float py, float pz) {
	self->mPosX = px;
	self->mPosY = py;
	self->mPosZ = pz;
}

void AlMsgWinSetSize(KinderCoMessageThing* self, float sx, float sy) {
	self->mSizeX = sx;
	self->mSizeY = sy;
}

ASM_FUNC void AlMsgWinExec(KinderCoMessageThing* a1) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( ebx, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x0057A060 );

    // restore regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}