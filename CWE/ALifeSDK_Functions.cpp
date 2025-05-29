#pragma once
#include "stdafx.h"
#include "ALifeSDK_Functions.h"

#pragma warning(push)
#pragma warning(disable: 4409)
const int ALO_Field_Find_Ptr = 0x054B1F0;
ObjectMaster* ALO_Field_Find_(ObjectMaster *a1, int a2, Uint8 a3)
{
	ObjectMaster* retval;
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

const int ScaleObjectMaster_XYZPtr = 0x0054AC70;
int ScaleObjectMaster_XYZ(ObjectMaster* a1, float a2, float a3, float a4)
{
	int retval;
	__asm
	{
		push a4
		push a3
		push a2
		mov eax, a1
		call ScaleObjectMaster_XYZPtr
		add esp, 12
		mov retval, eax
	}
	return retval;
}

void DrawToyTransform(ObjectMaster *a1, int a2, int a3)
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
void sub_5669B0(ObjectMaster *a1, int a2, int a3)
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