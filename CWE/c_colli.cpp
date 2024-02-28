#include "stdafx.h"
#include <SA2ModLoader.h>

const int CCL_EnablePtr = 0x00486CD0;
void CCL_Enable(ObjectMaster* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call CCL_EnablePtr
	}
}
const int CCL_DisablePtr = 0x0486D00;
void CCL_Disable(ObjectMaster* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call CCL_DisablePtr
	}
}