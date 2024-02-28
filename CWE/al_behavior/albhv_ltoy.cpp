#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "../ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include "../Chao.h"
#include <random>
#include "../al_world.h"
#include "al_intention.h"
#include "albhv.h"



DataPointer(ObjectMaster*, TVObject, 0x1AED288);
int ALO_GetTVWatchPos(NJS_VECTOR* a1)
{
	Data1Ptr v2; // edi
	float v3; // ST08_4
	signed int v4; // esi
	if (TVObject)
	{
		v2.Entity = (EntityData1*)TVObject->Data1.Entity;
		v3 = njRandom() * 3.0f + 10.0f;
		v4 = v2.Entity->Rotation.y
			- (signed int)((njRandom() * 80.0f - 40.0f) * -182.0444488525391f)
			+ 0x8000;
		a1->x = njSin(v4) * v3 + v2.Entity->Position.x;
		a1->y = v2.Entity->Position.y;
		a1->z = njCos(v4) * v3 + v2.Entity->Position.z;
		return 1;
	}
	return 0;
}

DataPointer(ObjectMaster*, pRadicaseTask, 0x01AED2E0);
int ALO_GetRadicaseListenPos(NJS_VECTOR* a1)
{
	Data1Ptr v2; // edi
	float v3; // ST08_4
	signed int v4; // esi
	if (pRadicaseTask)
	{
		v2.Entity = (EntityData1*)pRadicaseTask->Data1.Entity;
		v3 = njRandom() * 3.0f + 10.0f;
		v4 = v2.Entity->Rotation.y
			- (signed int)((njRandom() * 80.0f - 40.0f) * -182.0444488525391f)
			+ 0x8000;
		a1->x = njSin(v4) * v3 + v2.Entity->Position.x;
		a1->y = v2.Entity->Position.y;
		a1->z = njCos(v4) * v3 + v2.Entity->Position.z;
		return 1;
	}
	return 0;
}

int __cdecl ALBHV_GoToRadicase(ObjectMaster* tp)
{
	ObjectMaster* v2; // r30
	NJS_POINT3 v4; // [sp+50h] [-30h] BYREF

	v2 = AL_GetFoundToyTask(tp);
	if (!v2 || !ALO_GetRadicaseListenPos(&v4))
		return BHV_RET_FINISH;
	ALW_LockOn(tp, v2);
	tp->EntityData2->Waypoint = v4;
	AL_SetBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x55C3C0>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B480>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056BA80>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056BBA0>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056B560>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x056B6C0>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x00598B90>);
	return BHV_RET_CONTINUE;
}

int __cdecl ALBHV_GoToTV(ObjectMaster* tp)
{
	ObjectMaster* v2; // r30
	NJS_POINT3 v4; // [sp+50h] [-30h] BYREF

	v2 = AL_GetFoundToyTask(tp);
	if (!v2 || !ALO_GetTVWatchPos(&v4))
		return BHV_RET_FINISH;
	ALW_LockOn(tp, v2);
	tp->EntityData2->Waypoint = v4;
	AL_SetBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x55C3C0>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B480>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056BA80>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056BBA0>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056B560>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x056B6C0>);
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x00598890>);
	return BHV_RET_CONTINUE;
}
signed int __cdecl ALBHV_GoToHorse(ObjectMaster* a1)
{
	ObjectMaster* v1; // edi
	
	v1 = AL_GetFoundToyTask(a1);
	if (!v1 || ALW_IsSheAttentionOtherOne(a1, v1))
	{
		return BHV_RET_FINISH;
	}

	MOV_SetAimPos(a1, &v1->Data1.Entity->Position);
	ALW_LockOn(a1, v1);
	ALW_CommunicationOn(a1, v1);

	AL_SetBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x55C3C0>);
	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B480>);
	
	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x056B560>);
	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x0056B500>);

	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x00598F40>);

	return BHV_RET_CONTINUE;

}

const int sub_535010Ptr = 0x535010;
int sub_535010(NJS_VECTOR* a1)
{
	int rtn;
	__asm
	{
		mov eax, a1
		call sub_535010Ptr
		mov rtn, eax
	}
}

extern signed int __cdecl ALBHV_GoToPiano(ObjectMaster* a1);
void __cdecl  AL_CalcIntentionScore_LToy(ObjectMaster* a1, float* a2)
{
	Data1Ptr v2; // esi
	ObjectMaster* v3; // ebx
	int v4; // eax
	float v11; // [esp+8h] [ebp-4h]

	v2.Chao = (ChaoData1*)a1->Data1.Chao;
	if (!v2.Chao->Behavior.IntentionTimer[7])
	{
		v3 = AL_GetFoundToyTask(a1);
		if (v3)
		{
			v11 = AL_CalcScoreTypeA(AL_EmotionGetValue(a1, EM_ST_TEDIOUS), *(int*)0x1312E8C) * 0.99f;
			AL_ScoreRandomize(&v11);
			if (*a2 < (double)v11)
			{
				al_entry_work* entry = (al_entry_work*)v3->UnknownA_ptr;
				switch (entry->kind)
				{
				case ALW_KIND_TV:
					AL_SetBehavior(a1, ALBHV_GoToTV);
					break;
				case ALW_KIND_RADICASE:
					AL_SetBehavior(a1, ALBHV_GoToRadicase);
					break;
				case ALW_KIND_BALL:
					AL_SetBehavior(a1, (BHV_FUNC)0x40AD80);
					break;
				case ALW_KIND_PIANO:
					AL_SetBehavior(a1, ALBHV_GoToPiano);
					break;
				case ALW_KIND_ORGAN:
					AL_SetBehavior(a1, (BHV_FUNC)0x40AD80);
					break;
				case ALW_KIND_HORSE:
					v4 = sub_535010(&a1->Data1.Entity->Position);
					if (v4 != 1 && v4 != 2 && v4 == 3)
					{
						return;
					}
					AL_SetBehavior(a1, ALBHV_GoToHorse);
					break;
				case ALW_KIND_BOX:
					AL_SetBehavior(a1, (BHV_FUNC)0x599770);
					break;
				case ALW_KIND_BOAT:
					AL_SetBehavior(a1, ALBHV_GoToBoat);
					break;
				case ALW_KIND_UKIWA:
					AL_SetBehavior(a1, ALBHV_GoToFloat);
					break;
				default:
					break;
				}
				AL_EmotionAdd(a1, EM_ST_TEDIOUS, -5000);
				*a2 = v11;

			}
		}
	}
}