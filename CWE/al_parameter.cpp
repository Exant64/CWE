#include "stdafx.h"

#include "Chao.h"
#include <cwe_api.h>
#include "ChaoMain.h"

void AL_NameSet(char* lval, char* rval) {
	memcpy(lval, rval, sizeof(AL_NAME));
}

bool AL_IsChild(task* tp) {
	return GET_CHAOPARAM(tp)->type == TYPE_CHILD;
}

bool AL_IsCustomChao(task* tp) {
	return GET_CHAOPARAM(tp)->type == 26 || GET_CHAOWK_CWE(tp)->IsCustomChaoTypeLoaded;
}

bool AL_IsHero(unsigned __int8 a1) {
	return a1 >= 6u && !((a1 - 6) % 3);
}
bool AL_IsDark(unsigned __int8 a1) {
	return a1 >= 7u && !((a1 - 7) % 3);
}

bool AL_IsHero2(task* a1) {
	return AL_IsHero(GET_CHAOPARAM(a1)->type);
}
bool AL_IsDark2(task* a1) {
	return AL_IsDark(GET_CHAOPARAM(a1)->type);
}

bool AL_IsNegative(task* tp) {
	if (AL_IsCustomChao(tp)) {
		return false;
	}

	bool negative = GET_CWEPARAM(tp)->Negative;

	if (ChaoSaveText == 0x4B4C425F) {
		negative = !negative;
	}

	return negative;
}

void AL_ParameterClearAccessory(task* tp, int slot) {
	auto pParam = GET_CWEPARAM(tp);

	memset(&pParam->Accessories[slot], 0, sizeof(pParam->Accessories[slot]));
}

void AL_ParameterAddAPos(task* a1, float a2)
{
	CHAO_PARAM_GC* v3; // ecx
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
	work->Shape.Flag |= 2u;
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
	chaowk* v2; // edx
	CHAO_PARAM_GC* v3; // ecx

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
	v2->Shape.Flag |= 2u;
}
void IncrementPowerRun(task* a1, float a2)
{
	chaowk* v2; // edx
	CHAO_PARAM_GC* v3; // ecx

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
	v2->Shape.Flag |= 2u;
}

// levelup function in AL_ParameterGrow, no symbols name :(
void sub_535E90(CHAO_PARAM_GC *a1, int a2) {
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
	if(!param->LifeTimer) {
		if(gConfigVal.ChaoAttention) {
			// intentionally not using the real function (AL_ParameterAddUserLike)
			// because it has many sideeffects

			param->like--;

			if (param->like < -100) {
				param->like = -100;
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
				if(param->Lev[i] >= 10) continue;

				// doing this to prevent overflow
				int fract = int(param->Exp[i]) + 20;

				if(fract >= 100) {
					fract -= 100;
					param->Lev[i]++;

					sub_535E90(param, i);
				}

				param->Exp[i] = fract;
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