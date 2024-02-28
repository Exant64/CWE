#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include <random>
#include "al_knowledge.h"
#include "..//AL_ModAPI.h"
#include "al_intention.h"
#include "../ChaoMain.h"
#include "../al_parameter.h"


DataArray(BHV_FUNC, ALBHV_SToyFunc, 0x012FE978, 12);
bool __cdecl AL_KW_IsSToyFlagOn(ObjectMaster* a1, char a2)
{
	return (a1->Data1.Chao->pParamGC->Knowledge.SToyFlag & (1 << a2)) != 0;
}
signed int __cdecl AL_DecideBehaviorSToy(ObjectMaster* a1)
{
	signed int v1; // edi
	int v5[32]; // [esp+14h] [ebp-80h]

	v1 = 0;
	for (int i = 0; i < NB_AL_STOY; i++)
	{
		if (AL_KW_IsSToyFlagOn(a1, i))
		{
			if (ALBHV_SToyFunc[i])
			{
				v5[v1++] = i;
			}
		}
	}

	if (v1 <= 0)
		return BHV_RET_CONTINUE;

	//this probably deserves some explanation
	//we check if the only toys unlocked are the shovel and/or the watering can
	//if yes then don't run anything	
	//this is done to prevent every single chao that has the shovel (for planting purposes) 
	//making a sandcastle when theyre bored
	if (a1->Data1.Chao->pParamGC->Knowledge.SToyFlag == (1 << AL_STOY_SCOP) ||
		a1->Data1.Chao->pParamGC->Knowledge.SToyFlag == (1 << AL_STOY_JYOURO) ||
		a1->Data1.Chao->pParamGC->Knowledge.SToyFlag == ((1 << AL_STOY_SCOP) | (1 << AL_STOY_JYOURO)))
		return BHV_RET_CONTINUE;

	NJS_VECTOR a3;
	a3.x = njSin(a1->Data1.Chao->entity.Rotation.y) * 8 + a1->Data1.Chao->entity.Position.x;
	a3.y = a1->Data1.Chao->entity.Position.y;
	a3.z = njCos(a1->Data1.Chao->entity.Rotation.y) * 8 + a1->Data1.Chao->entity.Position.z;

	AL_SetBehavior(
		a1,
		ALBHV_SToyFunc[v5[(unsigned int)(njRandom() * (v1 - 0.0001f))]]
	);
	ALOField_Load(a1, CI_KIND_AL_RANDOM_STOY, &a3, 20.0f, (int)(1800 + (njRandom() * 2401.f)));
	return BHV_RET_FINISH;
}
void __cdecl AL_CalcIntentionScore_JoinSToy(ObjectMaster* a1, float* a2)
{
	unsigned int v2; // edi
	ObjectMaster* v3; // ebp
	int v5; // eax
	signed int v9; // [esp+10h] [ebp-2Ch]
	float v10; // [esp+14h] [ebp-28h]
	signed int value; // [esp+18h] [ebp-24h]
	int v12[32]; // [esp+1Ch] [ebp-20h]

	v10 = 0.0;
	v2 = ChaoGlobal.IntentionHimaTrigger >> 1;
	value = AL_EmotionGetValue(a1, EM_ST_TEDIOUS);
	v3 = ALO_Field_Find_(a1, 1, CI_KIND_AL_RANDOM_STOY);
	v9 = 0;
	if (*a2 < 1)
	{
		if (v3)
		{
			for (int i = 0; i < NB_AL_STOY; i++)
			{
				if (AL_KW_IsSToyFlagOn(a1, i))
				{
					if (ALBHV_SToyFunc[i])
					{
						v12[v9++] = i;
					}
				}
			}
			v5 = value;
			if (value > (int)v2)
			{
				if (value < 7000)
				{
					v5 = value + 3000;
				}
				v10 = ChaoGlobal.IntentionHimaMaxScore * AL_CalcScoreTypeA(v5, v2);
				AL_ScoreRandomize(&v10);
			}

			if (v10 > *a2)
			{
				if (v9 > 0)
				{
					if (a1->Data1.Chao->pParamGC->Knowledge.SToyFlag == (1 << AL_STOY_SCOP) ||
						a1->Data1.Chao->pParamGC->Knowledge.SToyFlag == (1 << AL_STOY_JYOURO) ||
						a1->Data1.Chao->pParamGC->Knowledge.SToyFlag == ((1 << AL_STOY_SCOP) | (1 << AL_STOY_JYOURO)))
						return;

					MOV_SetAimPos(a1, &v3->Data1.Chao->entity.Position);
					AL_SetBehavior(a1, (BHV_FUNC)0x56B500);

					AL_SetNextBehavior(
						a1,
						ALBHV_SToyFunc[v12[(int)(njRandom() * (v9 - 0.00001f))]]
					);

					AL_EmotionAdd(a1, EM_ST_TEDIOUS, -7500);
					AL_EmotionAdd(a1, EM_ST_LONELY, -4000);
					//AL_ClearIntention(a1);
					if (v3->Parent)
					{
						AL_EmotionAdd(v3->Parent, EM_ST_LONELY, -4000);
					}
					*a2 = 0.99f;
				}

			}
		}
	}
}

int __cdecl ALBHV_Garagara(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	int v3; // eax

	v1 = a1->Data1.Chao;
	if (v1->Behavior.Mode)
	{
		if (v1->Behavior.Mode != 1)
		{
			return BHV_RET_CONTINUE;
		}
	}
	else
	{
		AL_SetMotionLink(a1, 330);
		AL_FaceChangeEye(a1, 4);

		if(AL_IsHero(a1->Data1.Chao->pParamGC->Type))
			sub_5669B0(a1, (int)0x012F9994, 11);
		else if(AL_IsDark(a1->Data1.Chao->pParamGC->Type))
			sub_5669B0(a1, (int)0x012F8BA4, 11);
		else 
			sub_5669B0(a1, (int)0x012FA174, 11);

		++v1->Behavior.Mode;
		v1->Behavior.Timer = (unsigned __int16)(600 + (int)(njRandom() * 201.f));
	}
	v3 = v1->Behavior.Timer;
	v1->Behavior.Timer = v3 - 1;

	if (v3 > 0)
		return BHV_RET_CONTINUE;
	return BHV_RET_FINISH;
}