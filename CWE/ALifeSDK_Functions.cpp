#pragma once
#include "stdafx.h"
#include "ALifeSDK_Functions.h"

const int sub_534F80Ptr = 0x534F80;
void sub_534F80(int a1, NJS_VECTOR* a2, signed int a3)
{
	__asm
	{
		mov eax, a1
		mov edi, a2
		push a3
		call sub_534F80Ptr
		add esp, 4
	}
}

const int sub_561740Ptr = 0x561740;
void sub_561740(int a1)
{
	__asm
	{
		mov eax, a1
		call sub_561740Ptr
	}
}

void DrawToyTransform(task *a1, int a2, int a3)
{
	int asd = 0x0566A20;
	__asm
	{
		mov ecx, a3
		mov edx, a2
		mov eax, a1
		call asd

	}
}
void sub_5669B0(task *a1, int a2, int a3)
{
	int asd = 0x5669B0;
	__asm
	{
		mov ecx, a3
		mov edx, a2
		mov eax, a1
		call asd

	}
}

const int PlayJinglePtr = 0x00443480;
void PlayJingle(const char *a1)
{
	__asm
	{
		mov ebx, a1
		call PlayJinglePtr
	}
}

const int SetShadersPtr = 0x0041B1F0;
void SetShaders_(int result)
{
	__asm
	{
		mov eax, result
		call SetShadersPtr
	}
}

int AdjustAngle(__int16 bams_a, unsigned __int16 bams_b, int dang)
{
	int result; // eax
	__int16 v4; // cx

	result = bams_b;
	v4 = bams_b - bams_a;
	if ((signed __int16)(bams_b - bams_a) > dang || v4 < -dang)
	{
		if (v4 >= 0)
		{
			result = (unsigned __int16)(dang + bams_a);
		}
		else
		{
			result = (unsigned __int16)(bams_a - dang);
		}
	}
	return result;
}