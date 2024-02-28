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

void ButtonGuideDisp(ObjectMaster* tp) {
	*(Uint32*)0x12D27A4 = 0xFFFFFFFF;

	sub_669B90(-1,
		(tp->Data1.Entity->Index & SELECT),
		(tp->Data1.Entity->Index & CONFIRM),
		(tp->Data1.Entity->Index & BACK)
	);
}

void ButtonGuideExec(ObjectMaster* tp) {
	if (!AL_OdekakeMenuMaster_Data_ptr || AL_OdekakeMenuMaster_Data_ptr->EndFlag) {
		DeleteObject_(tp);
	}
}

void CreateButtonGuide(int _flags) {
	ObjectMaster* pTask = LoadObject(4, "ButtonGuide", ButtonGuideExec, LoadObj_Data1);
	pTask->Data1.Entity->Index = _flags;
	pTask->DisplaySub = ButtonGuideDisp;
}