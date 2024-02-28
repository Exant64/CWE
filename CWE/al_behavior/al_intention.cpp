#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include "alsbhv.h"
#include "albhv.h"
#include "al_knowledge.h"
#include <random>
#include <util.h>
#include "al_intention.h"

void AL_ScoreRandomize(float* pScore)
{
	float ratio = GET_GLOBAL()->IntentionScoreRandomRatio;

	float a = *pScore * ratio;
	float b = *pScore * (1 - ratio) * njRandom();

	*pScore = a + b;
}

float AL_CalcScoreTypeA(int value, int trigger) {
	float fValue = value;
	float fTrigger = trigger;

	if (value > trigger) {
		return (fValue - fTrigger) / (GET_GLOBAL()->EmotionStateLimit - fTrigger);
	}

	return 0;
}

extern int ALBHV_HoldHands_Left(ObjectMaster* a1);
extern int ALBHV_HoldHands_Right(ObjectMaster* a1);
extern void ALBHV_Gossip(ObjectMaster* a1, ObjectMaster* a2);

const BHV_FUNC ALBHV_TalkAllowed[] = {
	(BHV_FUNC)0x5A3580, (BHV_FUNC)0x5A3460, (BHV_FUNC)0x5A3380, (BHV_FUNC)0x5A3280, //walking bhavs
	(BHV_FUNC)0x05A1BA0 //resting
};

const int IntentionChatTrigger = 8500;
const float IntentionChatMaxScore = 0.99f;

void __cdecl AL_CalcIntentionScore_Chat(ObjectMaster* a1, float* a2)
{
	float score;
	ObjectMaster* pSelectedChao;
	chaowk* wk = (chaowk*)a1->Data1.Chao;

	//ALBHV_Gossip(a1, GetChaoObject(0, 1));
	//* a2 = 1.0f;
	//return;

	pSelectedChao = nullptr;
	
	score = 0.0;
	for (int i = 0; i < ALW_CountEntry(0); i++)
	{
		ObjectMaster* pChao = GetChaoObject(0, i);
		if (!pChao || pChao == a1) continue;
		
		BHV_FUNC func = AL_GetBehavior(pChao);

		//not in water 
		if (pChao->Data1.Entity->Position.y + 2.0 < pChao->EntityData2->field_DC) continue;

		//go through allowed behaviors
		for (size_t j = 0; j < LengthOfArray(ALBHV_TalkAllowed); j++)
		{
			if (func == ALBHV_TalkAllowed[j])
			{
				float dist = CheckDistance(&wk->entity.Position, &pChao->Data1.Entity->Position);
				if (dist < 50.0f && njRandom() < 0.5f)
					pSelectedChao = pChao;
				break;
			}
		}
		
		//found one, exit
		if (pSelectedChao)
			break;
	}

	if (!pSelectedChao) return;
	
	int chattyVal = AL_EmotionGetValue(a1, EM_PER_CHATTY);
	if (chattyVal < 0) chattyVal = 0;

	//chatValue formula = loneliness + (chatty clamped at 0) * 10;
	//max chatty (100) would result in 1000 being added to it for example
	int chatValue = AL_EmotionGetValue(a1, EM_ST_LONELY) + chattyVal * 10;
	if (chatValue > 10000) chatValue = 10000;

	if (chatValue > IntentionChatTrigger)
	{
		score = IntentionChatMaxScore
			* AL_CalcScoreTypeA(chatValue, IntentionChatTrigger);
		AL_ScoreRandomize(&score);
		if (score > *a2)
		{
			AL_KW_MeetEachother(a1, pSelectedChao, KW_MEET_CHAT);

			AL_EmotionAdd(a1, EM_ST_LONELY, -6000);
			AL_EmotionAdd(pSelectedChao, EM_ST_LONELY, -6000);

			ALW_LockOn(a1, pSelectedChao);

			*a2 = score;
			
			int nbTimesTheyMet = AL_KW_GetMeetChao(a1, pSelectedChao);
			if (njRandom() > 0.75f && nbTimesTheyMet >= 10) {
				//easter egg, unlock pearl medal if other chao is pearl, found by chao professor
				if (pSelectedChao->Data1.Chao->pParamGC->Texture == 12)
					a1->Data1.Chao->pParamGC->DoctorMedal |= (1 << 10);

				if(njRandom() > 0.5f)
					AL_SetBehavior(a1, ALBHV_InitHug);
				else 
					AL_SetBehavior(a1, ALBHV_InitHoldHands);
			}
			else if (njRandom() > 0.75f && nbTimesTheyMet >= 5) {
				ALBHV_Gossip(a1, pSelectedChao);
			}
			else {
				AL_SetBehavior(a1, ALBHV_Talk);
			}
		}
	}
}

const int sub_562800Ptr = 0x562800;
void sub_562800(float* a1, ObjectMaster* a2)
{
	__asm
	{
		mov esi, a2
		mov edi, a1
		call sub_562800Ptr
	}
}

signed int __cdecl ALBHV_FartReaction(ObjectMaster* a1)
{
	ChaoData1* v2;

	v2 = (ChaoData1*)a1->Data1.Chao;
	if (!v2->Behavior.Mode)
	{
		AL_SetMotionLink(a1, 565);
		//AL_IconSet(a1, 5, 180);		
		AL_FaceChangeEye(a1, 1);
		AL_FaceChangeMouth(a1, 4);
		++v2->Behavior.Mode;
	}
	v2->Behavior.Timer++;
	if (v2->Behavior.Timer >= 60 * 2)
		return 1;
	return 0;
}


static void __declspec(naked) AL_CalcIntentionScore_LToy_Hook()
{
	__asm
	{
		push[esp + 04h] // a2
		push edi // a1

		// Call your __cdecl function here:
		call AL_CalcIntentionScore_LToy

		pop edi // a1
		add esp, 4 // a2
		retn
	}
}

const int AL_CalcIntentionScore_MayuPtr = 0x00562800;
void AL_CalcIntentionScore_Mayua(ObjectMaster* a2, float* a1) {
	__asm {
		mov esi, a2
		mov edi, a1
		call AL_CalcIntentionScore_MayuPtr
	}
}
void __cdecl AL_CalcIntentionScore_All(ObjectMaster* a1, float* a2)
{
	if (ALO_Field_Find_(a1, 1, 151))
	{
		AL_SetBehavior(a1, ALBHV_FartReaction);
		*a2 = 1;
	}

	if (a1->Data1.Chao->pParamGC && a1->Data1.Chao->pParamGC->ForceReincarnate)
	{
		AL_SetBehavior(a1, (BHV_FUNC)0x0568F60);
		*a2 = 1.0f;
	}

	AL_CalcIntentionScore_JoinSToy(a1, a2);
	AL_CalcIntentionScore_Mayu(a1, a2);
	AL_CalcIntentionScore_Chat(a1, a2);
	AL_CalcIntentionScore_Tree(a1, a2);
	AL_CalcIntentionScore_Mayua(a1, a2);

}

static void __declspec(naked) AL_CalcIntentionScore_Hook()
{
	__asm
	{
		push edi // a1
		push esi // a2

		// Call your __cdecl function here:
		call AL_CalcIntentionScore_All

		pop esi // a2
		pop edi // a1
		retn
	}
}

extern void __cdecl AL_CalcIntentionScore_JoinMusic(ObjectMaster* a1, float* a2);
static void __declspec(naked) AL_CalcIntentionScore_JoinMusicH()
{
	__asm
	{
		push[esp + 04h] // a2
		push eax // a1

		// Call your __cdecl function here:
		call AL_CalcIntentionScore_JoinMusic

		pop eax // a1
		add esp, 4 // a2
		retn
	}
}
static void __declspec(naked) sub_55E7A0()
{
	__asm
	{
		push ebx // a1

		// Call your __cdecl function here:
		call AL_DecideBehaviorSToy

		pop ebx // a1
		retn
	}
}

static void __declspec(naked) AL_CalcIntentionScore_JoinDanceHook()
{
	__asm
	{
		push[esp + 04h] // a2
		push eax // a1

		// Call your __cdecl function here:
		call AL_CalcIntentionScore_JoinDance

		pop eax // a1
		add esp, 4 // a2
		retn
	}
}

Uint32 AL_SetIntervalTimer(ObjectMaster* a1, Uint16 TimerKind, Uint32 timer)
{
	a1->Data1.Chao->Behavior.IntervalTimer[TimerKind] = timer;
	return 0;
}

//possible API for this too?
void AL_IntentionInit()
{
	//small toy intention
	WriteJump((void*)0x55E7A0, sub_55E7A0);

	//dance
	WriteJump((void*)0x59C6D0, AL_DecideBehaviorDance);
	WriteJump((void*)0x0059C3D0, AL_CalcIntentionScore_JoinDanceHook);

	//new instruments
	WriteJump((void*)0x59D410, AL_DecideBehaviorMusic);
	WriteJump((void*)0x0059D0D0, AL_CalcIntentionScore_JoinMusicH);
	WriteJump((void*)0x00599B60, AL_CalcIntentionScore_LToy_Hook);
	WriteCall((void*)0x0562B65, AL_CalcIntentionScore_Hook);
}