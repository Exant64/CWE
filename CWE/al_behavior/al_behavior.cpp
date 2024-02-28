#include "stdafx.h"
#include <Chao.h>
#include <al_behavior/al_intention.h>
#include <al_behavior/albhv.h>
#include <alo_accessory.h>
#include <ChaoMain.h>
#include <ALifeSDK_Functions.h>
#include <AL_ModAPI.h>

extern void ALBHV_Life_Init();

const int Chao_BehaviourPtr = 0x0053D890;
const int Chao_BehaviourQueuePtr = 0x0053D970;

void AL_SetBehaviorWithTimer(ObjectMaster* a1, int a2, int a3)
{
	__asm
	{
		push a3
		push a2
		mov eax, a1
		call Chao_BehaviourPtr
		add esp, 8
	}
}

void Chao_BehaviourQueue(ObjectMaster* a1, int a2)
{
	__asm
	{
		mov edx, a2
		mov eax, a1
		call Chao_BehaviourQueuePtr
	}
}

BHV_FUNC AL_GetBehavior(ObjectMaster* a1)
{
	return a1->Data1.Chao->Behavior.BhvFuncList[a1->Data1.Chao->Behavior.CurrBhvFuncNum];
}
void AL_SetBehavior(ObjectMaster* a1, BHV_FUNC a2)
{
	AL_SetBehaviorWithTimer(a1, (int)a2, -1);
}
void AL_SetNextBehavior(ObjectMaster* a1, BHV_FUNC a2)
{
	Chao_BehaviourQueue(a1, (int)a2);
}

extern "C" __declspec(dllexport) void AL_SetAccessory(ObjectMaster * a1, int type)
{
	a1->Data1.Chao->pParamGC->Accessories[AccessoryTypeMap[type]] = type + 1;
}
extern "C" __declspec(dllexport) int AL_GetAccessory(ObjectMaster * a1, int type)
{
	return a1->Data1.Chao->pParamGC->Accessories[type];
}
extern "C" __declspec(dllexport) int GetAccessoryType(int ID) {
	return AccessoryTypeMap[ID];
}

const int AL_GrabObjectBothHandsPtr = 0x0056CFB0;
void AL_GrabObjectBothHands(ObjectMaster* a2, ObjectMaster* a1)
{
	__asm
	{
		mov esi, a2
		mov ecx, a1
		call AL_GrabObjectBothHandsPtr
	}
}
const int sub_5691B0Ptr = 0x5691B0;
signed int sub_5691B0(ObjectMaster* a1)
{
	int result;
	__asm
	{
		mov esi, a1
		call sub_5691B0Ptr
		mov result, eax
	}
	return result;
}

//putting accessory on
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_WearAccessory(ObjectMaster * a1)
{
	ChaoData1* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	int v4; // eax
	al_entry_work* v6; // eax
	al_entry_work* v7; // esi
	ObjectMaster* v8; // eax
	void* v9; // eax

	v1 = a1->Data1.Chao;
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	if (!v2)
	{
		AL_SetMotionLinkStep(a1, 202, 35);
		++v3->Mode;
		v3->Timer = 0;
		return AL_MoveHoldingObject(a1) == 0;
	}
	v4 = v2 - 1;
	if (v4)
	{
		if (v4 == 1 && AL_IsMotionStop(a1))
		{
			return 1;
		}
		return AL_MoveHoldingObject(a1) == 0;
	}
	v3->Timer++;
	if (v3->Timer < 35 || !AL_IsMotionStop(a1))
	{
		return AL_MoveHoldingObject(a1) == 0;
	}
	v6 = ALW_IsCommunication(a1);
	v7 = v6;
	if (v6)
	{
		v8 = v6->tp;
		if (v8)
		{
			AL_SetAccessory(a1, v8->Data1.Entity->Rotation.x);
			if (AL_GetItemSaveInfo(v7->tp))
			{
				v9 = AL_GetItemSaveInfo(v7->tp);
				AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)v9);
				AL_ClearItemSaveInfoPtr(v7->tp);
			}
			v7->tp->MainSub = DeleteObject_;
		}
	}
	AL_SetMotionLinkStep(a1, 203, 35);
	return AL_MoveHoldingObject(a1) == 0;
}
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_PutOnAccessoryTemp(ObjectMaster * a1)
{
	ChaoData1* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	al_entry_work* v6; // eax
	al_entry_work* v7; // esi
	ObjectMaster* v8; // eax
	void* v9; // eax

	v1 = a1->Data1.Chao;
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	v6 = ALW_IsCommunication(a1);
	v7 = v6;
	if (v6)
	{
		v8 = v6->tp;
		if (v8)
		{
			AL_SetAccessory(a1, v8->Data1.Entity->Rotation.x);
			if (AL_GetItemSaveInfo(v7->tp))
			{
				v9 = AL_GetItemSaveInfo(v7->tp);
				AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)v9);
				AL_ClearItemSaveInfoPtr(v7->tp);
			}
			v7->tp->MainSub = DeleteObject_;
		}
	}
	return 1;
}
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_TurnToAccessory(ObjectMaster * a1)
{
	ChaoData1* v1; // edi
	c_colli_hit_info* v2; // eax
	ObjectMaster* v3; // ebx
	ChaoDataBase* v5; // ecx
	signed int result; // eax
	int v7; // eax

	v1 = a1->Data1.Chao;
	if (!v1->Behavior.Mode)
	{
		AL_SetMotionLinkStep(a1, 202, 15);
		++v1->Behavior.Mode;
		v1->Behavior.Timer = 60;
	}
	if (MOV_TurnToAim2(a1, 0x600) >= 0xB6)
	{
		return 0;
	}
	v2 = CCL_IsHitKindEx(a1, 0x94);
	if (v2 && (v3 = v2->hit_twp) != 0)
	{
		if (ALW_IsHeld(v3))
		{
			return 0;
		}
		if (MainCharObj1[0])
		{
			sub_46E5E0(0, (int)MainCharObj1[0]);
		}
		AL_GrabObjectBothHands(a1, v3);
		AL_SetBehaviorWithTimer(a1, 0x569340, -1);
		v5 = a1->Data1.Chao->pParamGC;
		if (!AL_GetAccessory(a1, AccessoryTypeMap[v3->Data1.Entity->Rotation.x]))
			Chao_BehaviourQueue(a1, (int)ALBHV_PutOnAccessoryTemp);
		else
			Chao_BehaviourQueue(a1, (int)0x569550);

		return 0;
	}
	else
	{
		v7 = v1->Behavior.Timer;
		v1->Behavior.Timer = v7 - 1;
		if (v7 > 0)
		{
			return 0;
		}
		result = 1;
	}
	return result;
}
signed int __cdecl AL_CheckAccessory(ObjectMaster* a1)
{
	signed int(__cdecl * v1)(ObjectMaster*); // eax
	//ObjectMaster* v2; // eax
	ObjectMaster* v3; // esi

	v1 = a1->Data1.Chao->Behavior.BhvFuncList[a1->Data1.Chao->Behavior.CurrBhvFuncNum];
	if ((int)v1 == 0x5607C0)
	{
		return 0;
	}
	if ((int)v1 == 0x54EF10)
	{
		return 0;
	}
	if (v1 == ALBHV_TurnToAccessory)
	{
		return 0;
	}
	c_colli_hit_info* v2 = CCL_IsHitKindEx(a1, 0x94);
	if (!v2)
	{
		return 0;
	}
	v3 = v2->hit_twp;
	if (!v3)
	{
		return 0;
	}
	al_entry_work* v4 = (al_entry_work*)v3->UnknownA_ptr;
	if (v4)
	{
		if (v4->flag & 2)
		{
			return 0;
		}
	}

	MOV_SetAimPos(a1, &v3->Data1.Entity->Position);
	AL_SetBehaviorWithTimer(a1, (int)ALBHV_TurnToAccessory, -1);
	return 1;
}
//putting accessory on
extern "C" __declspec(dllexport) bool CanSpecialRun(ObjectMaster * chao, ObjectMaster * special) {
	int type = special->Data1.Entity->Rotation.x;
	return specialItemFuncs[type].second &&
		specialItemFuncs[type].second(chao, special);
}
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_PutOnSpecial(ObjectMaster* tp)
{
	al_entry_work* pSpecialEntry = ALW_IsCommunication(tp);
	if (!pSpecialEntry) return BHV_RET_FINISH;
	
	ObjectMaster* pSpecial = pSpecialEntry->tp;
	if (!pSpecial) return BHV_RET_FINISH;
		
	int type = pSpecial->Data1.Entity->Rotation.x;

	if (specialItemFuncs[type].first)
		specialItemFuncs[type].first(tp, pSpecial);

	//not action special
	if (!ModAPI_SpecialAction.contains(type)) {
		if (AL_GetItemSaveInfo(pSpecial)) {
			void* pSave = AL_GetItemSaveInfo(pSpecial);
			AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)pSave);
			AL_ClearItemSaveInfoPtr(pSpecial);
		}

		pSpecial->MainSub = DeleteObject_;
		return BHV_RET_FINISH;
	}	

	return BHV_RET_CONTINUE;
}
extern "C" __declspec(dllexport) int __cdecl ALBHV_TurnToSpecial(ObjectMaster * tp) {
	ChaoData1* v1 = tp->Data1.Chao;
	AL_BEHAVIOR* bhv = &v1->Behavior;

	switch (bhv->Mode) {
	case 0:
		AL_SetMotionLinkStep(tp, 202, 15);
		bhv->Mode++;
		bhv->Timer = 60;
		break;
	}

	if (MOV_TurnToAim2(tp, 0x600) < 0xB6) {
		ObjectMaster* pSpecial;
		c_colli_hit_info* v2 = CCL_IsHitKindEx(tp, 0xCE);
		if (v2 && (pSpecial = v2->hit_twp) != 0) {
			if (!ALW_IsHeld(pSpecial)) {
				//StopHoldingTaskP(0);
				if (MainCharObj1[0]) {
					sub_46E5E0(0, (int)MainCharObj1[0]);
				}
				AL_GrabObjectBothHands(tp, pSpecial);
				AL_SetBehavior(tp, ALBHV_HoldThink);

				int id = pSpecial->Data1.Entity->Rotation.x;
				if (specialItemFuncs[id].second && specialItemFuncs[id].second(tp, pSpecial))
					AL_SetNextBehavior(tp, ALBHV_PutOnSpecial);
				else
					AL_SetNextBehavior(tp, (BHV_FUNC)0x569550);
			}
		}
		else if (bhv->Timer-- <= 0) {
			return BHV_RET_FINISH;
		}
	}

	return BHV_RET_CONTINUE;
}
signed int __cdecl AL_CheckSpecial(ObjectMaster* a1)
{
	signed int(__cdecl * v1)(ObjectMaster*); // eax
	//ObjectMaster* v2; // eax
	ObjectMaster* v3; // esi

	v1 = a1->Data1.Chao->Behavior.BhvFuncList[a1->Data1.Chao->Behavior.CurrBhvFuncNum];
	if ((int)v1 == 0x5607C0)
	{
		return 0;
	}
	if ((int)v1 == 0x54EF10)
	{
		return 0;
	}
	if (v1 == ALBHV_TurnToSpecial)
	{
		return 0;
	}
	c_colli_hit_info* v2 = CCL_IsHitKindEx(a1, 0xCE);
	if (!v2)
	{
		return 0;
	}
	v3 = v2->hit_twp;
	if (!v3)
	{
		return 0;
	}
	al_entry_work* v4 = (al_entry_work*)v3->UnknownA_ptr;
	if (v4)
	{
		if (v4->flag & 2)
		{
			return 0;
		}
	}

	MOV_SetAimPos(a1, &v3->Data1.Entity->Position);
	AL_SetBehavior(a1, ALBHV_TurnToSpecial);
	return 1;
}
signed int __cdecl AL_CheckObakeHeadAndAccessory(ObjectMaster* a1)
{
	if (AL_CheckAccessory(a1) || AL_CheckSpecial(a1) || sub_5691B0(a1))
		return 1;
	else
		return 0;
}
static void __declspec(naked) sub_5691B0Hook()
{
	__asm
	{
		push esi // a1

		// Call your __cdecl function here:
		call AL_CheckObakeHeadAndAccessory

		pop esi // a1
		retn
	}
}

//removing accessory
void __cdecl AccessoryRemove1(ObjectMaster* a1)
{
	AL_SetBehaviorWithTimer(a1, (int)0x563EB0, -1);
	for (int i = 0; i < 4; i++) //todo: remove hardcoded accessory count
	{
		if (a1->Data1.Chao->pParamGC->Accessories[i])
		{
			void* saveinfo = AL_GetNewItemSaveInfo(9);
			if (saveinfo)
			{
				Accessory_Load((a1->Data1.Chao->pParamGC->Accessories[i] - 1) + 256, &a1->Data1.Entity->Position, NJM_DEG_ANG(njRandom() * 360.0), &a1->EntityData2->velocity, (short*)saveinfo);
				a1->Data1.Chao->pParamGC->Accessories[i] = 0;
			}
		}
	}
}
static void __declspec(naked) AccessoryRemoveHook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call AccessoryRemove1

		pop eax // a1
		retn
	}
}

void __cdecl AccessoryRemove2(ObjectMaster* a1, int a2, int a3)
{
	AL_SetBehavior(a1, (BHV_FUNC)0x564320);
	for (int i = 0; i < 4; i++) //todo: remove hardcoded accessory count
	{
		if (a1->Data1.Chao->pParamGC->Accessories[i])
		{
			void* saveinfo = AL_GetNewItemSaveInfo(9);
			if (saveinfo)
			{
				Accessory_Load((a1->Data1.Chao->pParamGC->Accessories[i] - 1) + 256, &a1->Data1.Entity->Position, NJM_DEG_ANG(njRandom() * 360.f), &a1->EntityData2->velocity, (short*)saveinfo);
				a1->Data1.Chao->pParamGC->Accessories[i] = 0;
			}
		}
	}
}
static void __declspec(naked) AccessoryRemove2Hook()
{
	__asm
	{
		push edi // a1

		// Call your __cdecl function here:
		call AccessoryRemove2

		add esp, 4 // a1
		retn
	}
}


void AL_Behavior_Init() {
	AL_IntentionInit();

	ALBHV_Life_Init();

	//accessories
	WriteCall((void*)0x0053DB87, sub_5691B0Hook);
	WriteCall((void*)0x00563DE7, AccessoryRemoveHook);
	WriteCall((void*)0x00565F0B, AccessoryRemove2Hook);

	//shovel action 
	WriteData((int*)0x012FE9A0, (int)&ALBHV_SandHole);

	WriteJump((void*)0x566DF0, ALBHV_Capture);
}