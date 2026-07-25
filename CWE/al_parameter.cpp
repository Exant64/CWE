#include "stdafx.h"

#include "Chao.h"
#include <cwe_api.h>
#include "ChaoMain.h"
#include "al_stage.h"
#include "asmutil.h"

ASM_FUNC void AL_ShapeElementFromParam(AL_SHAPE_ELEMENT* a1, CHAO_PARAM_GC* a2) {
    // save regs
    ASM_PUSH( edi );
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(2+0+2) ); // a2
    ASM_MOVE( edi, ASM_ESP(1+0+2) ); // a1

    // call
    ASM_CALL_R( edx, 0x536450 );

    // restore regs
    ASM_POP( esi );
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}

bool AL_ParameterIsGuest(task* tp) {
	return GET_CHAOWK(tp)->IsParamCopy;
}

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

Bool AL_IsEmotionTimerReset(task* tp) {
    AL_EMOTION* pEmotion = &GET_CHAOPARAM(tp)->emotion;

    if (0);
    else if (pEmotion->Timer);
    else {
        pEmotion->Timer++;
        return TRUE;
    }

    return FALSE;
}

void AL_ParameterAddAPos(task* a1, float a2)
{
	CHAO_PARAM_GC* v3; // ecx
	float v4; // [esp+4h] [ebp+4h]

	chaowk* work = GET_CHAOWK(a1);
	v3 = work->pParamGC;
	v4 = v3->body.APos + a2;
	v3->body.APos = v4;
	if (v4 > 1.0)
	{
		v3->body.APos = 1.0;
	}
	if (v3->body.APos < -1.0)
	{
		v3->body.APos = -1.0;
	}
	work->Shape.Flag |= 2u;
}

ASM_FUNC void AL_ParameterAddUserLike(task* tp, int add, float AddAttr) {
    // save regs
    ASM_PUSH( esi );
    ASM_PUSH( edi );

    // arguments
    ASM_PUSH(      ASM_ESP(3+0+2) ); // a3
    ASM_MOVE( edi, ASM_ESP(2+1+2) ); // a1
    ASM_MOVE( esi, ASM_ESP(1+1+2) ); // a2

    // call
    ASM_CALL_R( edx, 0x536320 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // restore regs
    ASM_POP( edi );
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void AL_ParameterGrow(task* a1, unsigned __int16 a2, unsigned __int16 a3, int a4) {
    // arguments
    ASM_PUSH(      ASM_ESP(4+0+0) ); // a4
    ASM_PUSH(      ASM_ESP(3+1+0) ); // a3
    ASM_MOVE( edx, ASM_ESP(2+2+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+2+0) ); // a1

    // call
    ASM_CALL_R( ecx, 0x535F00 );

    // end arguments
    ASM_ESP_ADD( 2 );

    // return
    ASM_RET( 0 );
}

void IncrementFlySwim(task* a1, float a2)
{
	chaowk* v2; // edx
	CHAO_PARAM_GC* v3; // ecx

	v2 = GET_CHAOWK(a1);
	v3 = v2->pParamGC;
	v3->body.VPos += a2;
	if (v3->body.VPos > *(float*)0x1312C88)
	{
		v3->body.VPos = *(float*)0x1312C88;
	}
	if (v3->body.VPos < -*(float*)0x1312C88)
	{
		v3->body.VPos = -*(float*)0x1312C88;
	}
	v2->Shape.Flag |= 2u;
}
void IncrementPowerRun(task* a1, float a2)
{
	chaowk* v2; // edx
	CHAO_PARAM_GC* v3; // ecx

	v2 = GET_CHAOWK(a1);
	v3 = v2->pParamGC;
	v3->body.HPos += a2;
	if (v3->body.HPos > *(float*)0x1312C88)
	{
		v3->body.HPos = *(float*)0x1312C88;
	}
	if (v3->body.HPos < -*(float*)0x1312C88)
	{
		v3->body.HPos = -*(float*)0x1312C88;
	}
	v2->Shape.Flag |= 2u;
}

// levelup function in AL_ParameterGrow, no symbols name :(
ASM_FUNC void sub_535E90(CHAO_PARAM_GC *a1, int a2) {
    // save regs
    ASM_PUSH( edi );
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(2+0+2) ); // a2
    ASM_MOVE( edi, ASM_ESP(1+0+2) ); // a1

    // call
    ASM_CALL_R( edx, 0x535E90 );

    // restore regs
    ASM_POP( esi );
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void AL_CalcParameter(task* tp) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x005353C0 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
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

			for(size_t i = 0; i < _countof(param->knowledge.player); ++i) {
				auto& player = param->knowledge.player[i];
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

static void ASM_FUNC AL_CalcParameter_t() {
	ASM_PUSH(esi); // a1

	ASM_PUSH(esi); // a1
	// Call your __cdecl function here:
	ASM_CALL (AL_CalcParameter_r);
	ASM_ESP_ADD(1); // a1

	ASM_POP(esi); // a1
	ASM_RET(0);
}

void AL_ParameterInit() {
	WriteCall((void*)0x53640F, (void*)AL_CalcParameter_t);
}

int AL_ParameterGetSkill(task* tp, Uint16 SkillKind) {
    const int stg = AL_GetStageNumber();

    if (stg == CHAO_STG_KINDER || stg > CHAO_STG_STADIUM && stg <= CHAO_STG_KARATE_2P) {
        return GET_CHAOWK(tp)->pParamGC->Skill[SkillKind];
    }

    return 3 * GET_CHAOWK(tp)->pParamGC->Skill[SkillKind];
}