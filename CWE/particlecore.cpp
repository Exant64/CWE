#include "stdafx.h"
#include "particlecore.h"

ASM_FUNC int AllocateParticle(ParticleUserData* a2) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( ebx, ASM_ESP(1+0+1) ); // a2

    // call
    ASM_CALL_R( edx, 0x0492660 );

    // restore regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}
