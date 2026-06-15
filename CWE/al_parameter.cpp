#include "stdafx.h"

#include "Chao.h"
#include <cwe_api.h>
#include "ChaoMain.h"

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

// levelup function in AL_ParameterGrow, no symbols name :(
void sub_535E90(ChaoDataBase *a1, int a2) {
	static const void* fptr = (void*)0x535E90;
	__asm {
		mov edi, a1
		mov esi, a2
		call fptr
	}
}

static void AL_CalcParameter(task* tp) {
	static const void* fptr = (const void*)0x005353C0;
	__asm {
		mov esi, tp
		call fptr
	}
}

static void AL_CalcParameter_r(task* tp) {
	AL_CalcParameter(tp);

	auto param = GET_CHAOPARAM(tp);

	// if this is at zero *after* AL_CalcParameter was called
	// that means the lifetimer stuff was just ran
	// a lot of these lst codes did
	// if lifetimer == 13:
	//		do something
	//		write lifetimer 14
	// so the way i'm rewriting them is that they're supposed to happen per-lifetimer cycle
	if(!param->TimescaleTimer) {
		if(gConfigVal.ChaoAttention) {
			// intentionally not using the real function (AL_ParameterAddUserLike)
			// because it has many sideeffects

			param->Happiness--;

			if (param->Happiness < -100) {
				param->Happiness = -100;
			}
		}

		if(gConfigVal.AnnoyingMeanChao) {
			// same here, we're not using AL_KW_AddPlayerLike because it calls MeetPlayer
			// which decreases distance, we don't want that here, they didn't actually interact
			// it also only sets the current player

			for(size_t i = 0; i < _countof(param->Knowledge.player); ++i) {
				auto& player = param->Knowledge.player[i];
				int like = int(player.like) - 1;

				if(like < -100) {
					like = -100;
				}

				player.like = like;
			}
		}

		if (gConfigVal.AutoGenerateStats) {
			for(size_t i = 0; i < 5; ++i) {
				if(param->StatLevels[i] >= 10) continue;

				// doing this to prevent overflow
				int fract = int(param->StatFractions[i]) + 20;

				if(fract >= 100) {
					fract -= 100;
					param->StatLevels[i]++;

					sub_535E90(param, i);
				}

				param->StatFractions[i] = fract;
			}
		}
	}
}

static void __declspec(naked) AL_CalcParameter_t()
{
	__asm {
		push esi // a1

		// Call your __cdecl function here:
		call AL_CalcParameter_r

		pop esi // a1
		retn
	}
}

void AL_ParameterInit() {
	WriteCall((void*)0x53640F, AL_CalcParameter_t);
}