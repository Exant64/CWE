#pragma once

#include "stdafx.h"

static const void* const SE_CallV2_TIMERPtr = (void*)0x437590;
static inline void SE_CallV2_TIMER(ObjectMaster* obj, int a1, NJS_VECTOR* a2, char a4, char a5, __int16 a6)
{
	__asm
	{
		push dword ptr[a6]
		push dword ptr[a5]
		push dword ptr[a4]
		mov esi, a2
		mov edi, a1
		mov ebx, obj
		call SE_CallV2_TIMERPtr
		add esp, 12
	}
}