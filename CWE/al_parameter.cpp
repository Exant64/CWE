#include "stdafx.h"

#include "Chao.h"
#include <cwe_api.h>

void AL_NameSet(char* lval, char* rval) {
	memcpy(lval, rval, sizeof(AL_NAME));
}

bool AL_IsChild(task* tp) {
	return GET_CHAOPARAM(tp)->Type == ChaoType_Child;
}

bool AL_IsCustomChao(task* tp) {
	return GET_CHAOPARAM(tp)->Type == 26 || GET_CHAOWK(tp)->IsCustomChaoTypeLoaded;
}

bool AL_IsHero(unsigned __int8 a1) {
	return a1 >= 6u && !((a1 - 6) % 3);
}
bool AL_IsDark(unsigned __int8 a1) {
	return a1 >= 7u && !((a1 - 7) % 3);
}

bool AL_IsHero2(task* a1) {
	return AL_IsHero(GET_CHAOPARAM(a1)->Type);
}
bool AL_IsDark2(task* a1) {
	return AL_IsDark(GET_CHAOPARAM(a1)->Type);
}


DataPointer(unsigned int, ChaoSaveText, 0x0136606E);
bool AL_IsNegative(task* a1) {
	if (AL_IsCustomChao(a1)) {
		return false;
	}

	bool negative = GET_CHAOPARAM(a1)->Negative;
	if (ChaoSaveText == 0x4B4C425F)
	{
		negative = !negative;
	}
	return negative;
}

void AL_ParameterClearAccessory(task* tp, int slot) {
	ChaoDataBase* pParam = GET_CHAOPARAM(tp);

	memset(&pParam->Accessories[slot], 0, sizeof(pParam->Accessories[slot]));
}

void AL_ParameterAddAPos(task* a1, float a2)
{
	ChaoDataBase* v3; // ecx
	float v4; // [esp+4h] [ebp+4h]

	chaowk* work = GET_CHAOWK(a1);
	v3 = work->pParamGC;
	v4 = v3->Alignment + a2;
	v3->Alignment = v4;
	if (v4 > 1.0)
	{
		v3->Alignment = 1.0;
	}
	if (v3->Alignment < -1.0)
	{
		v3->Alignment = -1.0;
	}
	work->Flag |= 2u;
}

const int sub_536320Ptr = 0x536320;
void AL_ParameterAddUserLike(task* tp, int add, float AddAttr)
{
	__asm
	{
		mov edi, add
		mov esi, tp
		push AddAttr
		call sub_536320Ptr
		add esp, 4
	}
}
const int sub_535F00Ptr = 0x535F00;
void AL_ParameterGrow(task* a1, unsigned __int16 a2, unsigned __int16 a3, int a4)
{
	__asm
	{
		mov eax, a1
		mov dx, a2
		push a4
		push a3
		call sub_535F00Ptr
		add esp, 6
	}
}

void IncrementFlySwim(task* a1, float a2)
{
	ChaoData1* v2; // edx
	ChaoDataBase* v3; // ecx

	v2 = GET_CHAOWK(a1);
	v3 = v2->pParamGC;
	v3->FlySwim += a2;
	if (v3->FlySwim > *(float*)0x1312C88)
	{
		v3->FlySwim = *(float*)0x1312C88;
	}
	if (v3->FlySwim < -*(float*)0x1312C88)
	{
		v3->FlySwim = -*(float*)0x1312C88;
	}
	v2->Flag |= 2u;
}
void IncrementPowerRun(task* a1, float a2)
{
	ChaoData1* v2; // edx
	ChaoDataBase* v3; // ecx

	v2 = GET_CHAOWK(a1);
	v3 = v2->pParamGC;
	v3->PowerRun += a2;
	if (v3->PowerRun > *(float*)0x1312C88)
	{
		v3->PowerRun = *(float*)0x1312C88;
	}
	if (v3->PowerRun < -*(float*)0x1312C88)
	{
		v3->PowerRun = -*(float*)0x1312C88;
	}
	v2->Flag |= 2u;
}