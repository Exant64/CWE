#include "stdafx.h"
#include "al_odekake.h"
#include "al_ode_guide.h"
#include "asmutil.h"

static ASM_FUNC void sub_669B90(int a1, int a2, int a3, int a4) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0+1) ); // a4
    ASM_PUSH(      ASM_ESP(3+1+1) ); // a3
    ASM_PUSH(      ASM_ESP(2+2+1) ); // a2
    ASM_MOVE( esi, ASM_ESP(1+3+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x669B90 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

void ButtonGuideDisp(task* tp) {
	*(Uint32*)0x12D27A4 = 0xFFFFFFFF;

	sub_669B90(-1,
		(tp->twp->btimer & SELECT),
		(tp->twp->btimer & CONFIRM),
		(tp->twp->btimer & BACK)
	);
}

void ButtonGuideExec(task* tp) {
	if (!AL_OdekakeMenuMaster_Data_ptr || AL_OdekakeMenuMaster_Data_ptr->EndFlag) {
		DestroyTask(tp);
	}
}

void CreateButtonGuide(int _flags) {
	task* pTask = CreateElementalTask(IM_TWK, LEV_4, ButtonGuideExec, "ButtonGuide");
	pTask->twp->btimer = _flags;
	pTask->disp = ButtonGuideDisp;
}