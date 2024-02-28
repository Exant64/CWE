#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include "alsbhv.h"
#include <random>
#include "al_knowledge.h"
#include "..//AL_ModAPI.h"
#include "al_intention.h"

struct FRUIT_INFO
{
	float growth;
	float AimGrowth;
	int SwingCounter;
	NJS_POINT3 pos;
	int angx;
	int angy;
};

#pragma pack(push, 8)
struct __declspec(align(4)) ALO_GrowTreeExecutor_Data
{
	EntityData1 entityData;
	char kind;
	char state;
	__int16 field_32;
	float growth;
	int life;
	int water;
	int pos_num;
	FRUIT_INFO fruit[3];
	float scale;
	float ScaleSpd;
	int ThirstyFlag;
	int WaterFlag;
	int LeafRotAng;
	float LeafWidth;
	int LeafDispNum;
	int FruitRotAng;
	float FruitWidth;
	int FruitDispNum;
	int SwingFlag;
	int SwingCountFlag;
	float SwingDist;
	int RollAngle;
	int TouchAngle;
	int ChaoSwingPhase;
	int RollPhase;
	int garden;
	void* pMySaveInfo;
	NJS_TEXLIST* texlist;
	NJS_OBJECT* pLocalObject;
	NJS_OBJECT* pShadowObject;
	NJS_OBJECT* pCopyObject;
};
#pragma pack(pop)


//FunctionPointer(al_perception_link*, AL_GetFoundTree, (ObjectMaster* a1), 0x073E660);
int ALBHV_TreeShake(ObjectMaster* a1)
{
	chaowk* wk = (chaowk*)a1->Data1.Chao;
	ALO_GrowTreeExecutor_Data* treedata = nullptr;

	switch (wk->Behavior.Mode)
	{
	case 0:
		AL_SetMotionLink(a1, 211);
		wk->Behavior.Mode++;
		wk->Behavior.Timer = 60;
		break;
	case 1:
		wk->Behavior.Timer--;
		if (wk->Behavior.Timer <= 0)
		{
			AL_SetMotionLink(a1, 213);
			wk->Behavior.Timer = (int)(500 + (njRandom() * 100.f));
			wk->Behavior.Mode++;
		}
		break;
	case 2:
		wk->Behavior.Timer--;
		if (wk->Behavior.Timer <= 0)
		{
			wk->Behavior.Mode++;
			wk->Behavior.Timer = 30;
			AL_SetMotionLink(a1, 214);
		}
		else
		{
			ObjectMaster* tree = ALW_GetLockOnTask(a1);
			if (!tree)
				return BHV_RET_FINISH;

			//maps -1 to 1 to 0-1
			tree->EntityData2->speed.x = njSin(a1->Data1.Entity->Rotation.y) * ((njSin(wk->Behavior.SubTimer) + 1) / 2.0f);
			tree->EntityData2->speed.z = njCos(a1->Data1.Entity->Rotation.y) * ((njSin(wk->Behavior.SubTimer) + 1) / 2.0f);
			wk->Behavior.SubTimer += 1024 + 512;
		}
		break;
	case 3:
		wk->Behavior.Timer--;
		if (wk->Behavior.Timer <= 0)
		{
			return BHV_RET_FINISH;
		}
		break;
	}
	return BHV_RET_CONTINUE;
}
signed int __cdecl ALBHV_GoToAimTree(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	float a2; // ST08_4
	double v5; // st7
	int v7; // eax

	v1 = a1->Data1.Chao;
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;

	((UnknownData2*)a1->EntityData2)->Waypoint.y = a1->Data1.Entity->Position.y; //DISGUSTING hack

	if (v2)
	{
		if (v2 == 1)
		{
			MOV_TurnToAim2(a1, 384);
			a2 = ChaoGlobal.WalkAcc * 0.8f;
			AL_ForwardAcc(a1, a2);
			v5 = MOV_DistFromAim(a1);
			if (v5 >= 0.0)
			{
				if (v5 < 4.0)
				{
					return BHV_RET_FINISH;
				}
			}
			else
			{
				AL_SetBehavior(a1, ALBHV_Think);
			}
		}
	}
	else
	{
		//AL_FaceChangeEye(a1, 4);
		v7 = v3->Timer;
		v3->LimitTimer = 1800;
		v3->Timer = v7 + 1;
		if (v7 > 30)
		{
			AL_SetMotionLink(a1, 100);
			++v3->Mode;
		}
	}
	AL_SetMotionSpd(a1, 1.5);
	v3->LimitTimer--;
	if (!v3->LimitTimer)
	{
		AL_SetBehavior(a1, ALBHV_Think);
	}
	return BHV_RET_CONTINUE;
}
#pragma optimize("gty", off)
void __cdecl AL_CalcIntentionScore_Tree(ObjectMaster* a1, float* a2)
{
	if (njRandom() < 0.6f) return;
	
	int Hunger = AL_EmotionGetValue(a1, EM_ST_HUNGER);
	float score = 0.99f * AL_CalcScoreTypeA(Hunger, 8200);
	AL_ScoreRandomize(&score);
	if (score > *a2)
	{
		al_perception_link* v1 = AL_GetFoundTree(a1);
			
		//check if tree found and other chao isn't shaking it + check if tree is adult
		if (v1 && !ALW_IsSheAttentionOtherOne(a1, v1->pEntry->tp) && v1->pEntry->tp->Data1.Entity->Action == 7) 
		{
			ALW_LockOn(a1, v1->pEntry->tp);
			MOV_SetAimPos(a1, &v1->pEntry->tp->Data1.Entity->Position);
			AL_SetBehavior(a1, (BHV_FUNC)0x0056B480);
			AL_SetNextBehavior(a1, ALBHV_GoToAimTree);
			AL_SetNextBehavior(a1, ALBHV_TurnToLockOn);
			AL_SetNextBehavior(a1, ALBHV_TreeShake);
			*a2 = score;
		}
	}
}
#pragma optimize("gty", on)