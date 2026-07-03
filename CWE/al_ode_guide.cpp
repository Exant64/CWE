#include "stdafx.h"
#include "al_odekake.h"
#include "al_ode_guide.h"

const int sub_669B90Ptr = 0x669B90;
void sub_669B90(int a1, int a2, int a3, int a4) {
	__asm {
		mov esi, a1
		push a4
		push a3
		push a2
		call sub_669B90Ptr
		add esp, 12
	}
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
		DeleteObject_(tp);
	}
}

void CreateButtonGuide(int _flags) {
	task* pTask = CreateElementalTask(LoadObj_Data1, 4, ButtonGuideExec, "ButtonGuide");
	pTask->twp->btimer = _flags;
	pTask->disp = ButtonGuideDisp;
}