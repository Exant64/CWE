#include "stdafx.h"

#include "Chao.h"
#include <cwe_api.h>

void AL_NameSet(char* lval, char* rval) {
	memcpy(lval, rval, sizeof(AL_NAME));
}

bool AL_IsChild(ObjectMaster* tp) {
	return tp->Data1.Chao->pParamGC->Type == 2;
}

bool AL_IsCustomChao(ObjectMaster* tp) {
	return tp->Data1.Chao->pParamGC->Type == 26 || tp->Data1.Chao->IsCustomChaoTypeLoaded;
}

bool AL_IsHero(unsigned __int8 a1) {
	return a1 >= 6u && !((a1 - 6) % 3);
}
bool AL_IsDark(unsigned __int8 a1) {
	return a1 >= 7u && !((a1 - 7) % 3);
}

bool AL_IsHero2(ObjectMaster* a1) {
	return AL_IsHero(a1->Data1.Chao->pParamGC->Type);
}
bool AL_IsDark2(ObjectMaster* a1) {
	return AL_IsDark(a1->Data1.Chao->pParamGC->Type);
}


DataPointer(unsigned int, ChaoSaveText, 0x0136606E);
bool AL_IsNegative(ObjectMaster* a1) {
	if (AL_IsCustomChao(a1)) {
		return false;
	}

	bool negative = a1->Data1.Chao->pParamGC->Negative;
	if (ChaoSaveText == 0x4B4C425F)
	{
		negative = !negative;
	}
	return negative;
}

void AL_ParameterClearAccessory(ObjectMaster* tp, int slot) {
	ChaoDataBase* pParam = GET_CHAOPARAM(tp);

	memset(&pParam->Accessories[slot], 0, sizeof(pParam->Accessories[slot]));
}

void AL_ParameterAddAPos(ObjectMaster* a1, float a2)
{
	Data1Ptr v2; // edx
	ChaoDataBase* v3; // ecx
	float v4; // [esp+4h] [ebp+4h]

	v2.Chao = (ChaoData1*)a1->Data1.Chao;
	v3 = v2.Chao->pParamGC;
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
	v2.Chao->Flag |= 2u;
}

const int sub_536320Ptr = 0x536320;
void AL_ParameterAddUserLike(ObjectMaster* tp, int add, float AddAttr)
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
void AL_ParameterGrow(ObjectMaster* a1, unsigned __int16 a2, unsigned __int16 a3, int a4)
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

void IncrementFlySwim(ObjectMaster* a1, float a2)
{
	ChaoData1* v2; // edx
	ChaoDataBase* v3; // ecx

	v2 = a1->Data1.Chao;
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
void IncrementPowerRun(ObjectMaster* a1, float a2)
{
	ChaoData1* v2; // edx
	ChaoDataBase* v3; // ecx

	v2 = a1->Data1.Chao;
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