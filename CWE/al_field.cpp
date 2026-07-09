#include "stdafx.h"

const int sub_55A920Ptr = 0x55A920;
task* sub_55A920(int a1, NJS_VECTOR* a2, task* parent, Uint8 a4, float a5)
{
	__asm
	{
		push a5
		push dword ptr[a4]
		push parent
		mov edi, a2
		mov ebx, a1
		call sub_55A920Ptr
		add esp, 12
	}
}
void AL_ChildFieldCreateT(task* a1, Uint8 a2, NJS_VECTOR* a3, float a4, int timer)
{
	sub_55A920(timer, a3, a1, a2, a4);
}