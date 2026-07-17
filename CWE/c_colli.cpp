#include "stdafx.h"
#include <SA2ModLoader.h>

const int CCL_EnablePtr = 0x00486CD0;
void CCL_Enable(task* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call CCL_EnablePtr
	}
}
const int CCL_DisablePtr = 0x0486D00;
void CCL_Disable(task* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call CCL_DisablePtr
	}
}

#pragma warning(push)
#pragma warning(disable: 4409)
const int ALO_Field_Find_Ptr = 0x054B1F0;
task* AL_IsHitKindWithNum(task *a1, int a2, Uint8 a3)
{
	task* retval;
	__asm
	{
		push a3
		mov ecx, a2
		mov edx, a1
		call ALO_Field_Find_Ptr
		add esp, 4
		mov retval, eax
	}
	return retval;
}
#pragma warning(pop)

task* CCL_IsHitKind(task* tp, Uint8 kind) {
    CCL_HIT_INFO* v2 = CCL_IsHitKindEx(tp, kind);

    return v2 ? v2->hit_tp : nullptr;
}