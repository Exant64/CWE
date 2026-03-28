#include "stdafx.h"
#include <Chao.h>
#include <al_garden_info.h>
#include <al_behavior/al_intention.h>
#include <al_behavior/albhv.h>
#include <al_behavior/albhv_navigation.h>
#include <alo_accessory.h>
#include <ChaoMain.h>
#include <ALifeSDK_Functions.h>
#include <AL_ModAPI.h>
#include <api/api_accessory.h>
#include <api/api_metadata.h>
#include <FunctionHook.h>
#include <memory.h>

extern void ALBHV_Life_Init();

const int Chao_BehaviourPtr = 0x0053D890;
const int Chao_BehaviourQueuePtr = 0x0053D970;

void AL_SetBehaviorWithTimer(task* a1, int a2, int a3)
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

void Chao_BehaviourQueue(task* a1, int a2)
{
	__asm
	{
		mov edx, a2
		mov eax, a1
		call Chao_BehaviourQueuePtr
	}
}

BHV_FUNC AL_GetBehavior(task* a1)
{
	return GET_CHAOWK(a1)->Behavior.BhvFuncList[GET_CHAOWK(a1)->Behavior.CurrBhvFuncNum];
}
void AL_SetBehavior(task* a1, BHV_FUNC a2)
{
	AL_SetBehaviorWithTimer(a1, (int)a2, -1);
}
void AL_SetNextBehavior(task* a1, BHV_FUNC a2)
{
	Chao_BehaviourQueue(a1, (int)a2);
}

void* AL_BehaviorGetUserData(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	return GET_CHAOWK_CWE(tp)->BhvUserData[bhv->CurrBhvFuncNum];
}

void AL_SetNextBehaviorWithUserData(task* tp, BHV_FUNC Func, void* pUserData) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	if (bhv->nbBhvFuncEntry < 15) {
		GET_CHAOWK_CWE(tp)->BhvUserData[bhv->nbBhvFuncEntry] = pUserData;
	}

	Chao_BehaviourQueue(tp, (int)Func);
}

void AL_BehaviorSetFreeWork(task* tp, int info) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;
    bhv->FreeWork = info;
}

int AL_BehaviorGetFreeWork(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;
    return bhv->FreeWork;
}

void AL_SetBehaviorWithFreeWork(task* tp, BHV_FUNC Func, int info) {
    AL_SetBehavior(tp, Func);
    AL_BehaviorSetFreeWork(tp, info);
}

void AL_SetAccessory(CHAO_PARAM_CWE* pParamCwe, uint32_t slot, uint32_t kind) {
	char id[METADATA_ID_SIZE];
	if (!ItemMetadata::Get()->GetID(ALW_CATEGORY_ACCESSORY, kind, id)) return;

	auto& accessoryData = pParamCwe->Accessories[slot];

	memset(&accessoryData, 0, sizeof(accessoryData));
	memcpy(accessoryData.ID, id, sizeof(accessoryData.ID));
	memcpy(accessoryData.ColorSlots, GetAccessoryData(kind).DefaultColors, sizeof(accessoryData.ColorSlots));
	accessoryData.ColorFlags = 0;
}

void AL_SetAccessory(CHAO_PARAM_CWE* pParamCwe, int type) {
	char id[METADATA_ID_SIZE];
	if (!ItemMetadata::Get()->GetID(ALW_CATEGORY_ACCESSORY, type, id)) return;

	const auto slotType = GetAccessoryType(type);
	AL_SetAccessory(pParamCwe, slotType, type);
}

extern "C" __declspec(dllexport) void AL_SetAccessory(task * a1, int type) {
	AL_SetAccessory(GET_CWEPARAM(a1), type);
}

void AL_SetAccessory(task* a1, const AccessorySaveInfo* saveInfo, int type) {
	char id[METADATA_ID_SIZE];
	if (!ItemMetadata::Get()->GetID(ALW_CATEGORY_ACCESSORY, type, id)) return;

	const auto slotType = GetAccessoryType(type);
	auto& accessoryData = GET_CWEPARAM(a1)->Accessories[slotType];

	memset(&accessoryData, 0, sizeof(accessoryData));
	memcpy(accessoryData.ID, id, sizeof(accessoryData.ID));
	memcpy(accessoryData.ColorSlots, saveInfo->Colors, sizeof(accessoryData.ColorSlots));
	accessoryData.ColorFlags = saveInfo->UsedColors;
}

extern "C" __declspec(dllexport) int AL_GetAccessory(task * a1, int type)
{
	return GET_CHAOWK_CWE(a1)->AccessoryIndices[type];
}

const int AL_GrabObjectBothHandsPtr = 0x0056CFB0;
void AL_GrabObjectBothHands(task* a2, task* a1)
{
	__asm
	{
		mov esi, a2
		mov ecx, a1
		call AL_GrabObjectBothHandsPtr
	}
}
const int sub_5691B0Ptr = 0x5691B0;
signed int sub_5691B0(task* a1)
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
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_WearAccessory(task * a1)
{
	chaowk* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	int v4; // eax
	ALW_ENTRY_WORK* v6; // eax
	ALW_ENTRY_WORK* v7; // esi
	task* v8; // eax
	void* v9; // eax

	v1 = GET_CHAOWK(a1);
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
			auto* accessorySave = (AccessorySaveInfo*)AL_GetItemSaveInfo(v7->tp);
			if (accessorySave) {
				AL_SetAccessory(a1, accessorySave, v8->twp->ang.x);
				AL_ClearItemSaveInfo(accessorySave);
				AL_ClearItemSaveInfoPtr(v7->tp);
			}
			else {
				AL_SetAccessory(a1, v8->twp->ang.x);
			}
			v7->tp->exec = DestroyTask;
		}
	}
	AL_SetMotionLinkStep(a1, 203, 35);
	return AL_MoveHoldingObject(a1) == 0;
}
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_PutOnAccessoryTemp(task * a1)
{
	chaowk* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	ALW_ENTRY_WORK* v6; // eax
	ALW_ENTRY_WORK* v7; // esi
	task* v8; // eax
	void* v9; // eax

	v1 = GET_CHAOWK(a1);
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	v6 = ALW_IsCommunication(a1);
	v7 = v6;
	if (v6)
	{
		v8 = v6->tp;
		if (v8)
		{
			auto* accessorySave = (AccessorySaveInfo*)AL_GetItemSaveInfo(v7->tp);
			if (accessorySave) {
				AL_SetAccessory(a1, accessorySave, v8->twp->ang.x);
				AL_ClearItemSaveInfo(accessorySave);
				AL_ClearItemSaveInfoPtr(v7->tp);
			}
			else {
				AL_SetAccessory(a1, v8->twp->ang.x);
			}
			v7->tp->exec = DestroyTask;
		}
	}
	return 1;
}
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_TurnToAccessory(task * a1)
{
	chaowk* v1; // edi
	CCL_HIT_INFO* v2; // eax
	task* v3; // ebx
	CHAO_PARAM_GC* v5; // ecx
	signed int result; // eax
	int v7; // eax

	v1 = GET_CHAOWK(a1);
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
	if (v2 && (v3 = v2->hit_tp) != 0)
	{
		if (ALW_IsHeld(v3))
		{
			return 0;
		}
		if (playertwp[0])
		{
			sub_46E5E0(0, (int)playertwp[0]);
		}
		AL_GrabObjectBothHands(a1, v3);
		AL_SetBehaviorWithTimer(a1, 0x569340, -1);
		v5 = GET_CHAOPARAM(a1);
		if (AL_GetAccessory(a1, GetAccessoryType(v3->twp->ang.x)) == -1)
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
signed int __cdecl AL_CheckAccessory(task* a1)
{
	signed int(__cdecl * v1)(task*); // eax
	//task* v2; // eax
	task* v3; // esi

	v1 = GET_CHAOWK(a1)->Behavior.BhvFuncList[GET_CHAOWK(a1)->Behavior.CurrBhvFuncNum];
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
	CCL_HIT_INFO* v2 = CCL_IsHitKindEx(a1, 0x94);
	if (!v2)
	{
		return 0;
	}
	v3 = v2->hit_tp;
	if (!v3)
	{
		return 0;
	}
	ALW_ENTRY_WORK* v4 = (ALW_ENTRY_WORK*)v3->fwp;
	if (v4)
	{
		if (v4->flag & 2)
		{
			return 0;
		}
	}

	MOV_SetAimPos(a1, &v3->twp->pos);
	AL_SetBehaviorWithTimer(a1, (int)ALBHV_TurnToAccessory, -1);
	return 1;
}
//putting accessory on
extern "C" __declspec(dllexport) bool CanSpecialRun(task * chao, task * special) {
	int type = special->twp->ang.x;
	return specialItemFuncs[type].second &&
		specialItemFuncs[type].second(chao, special);
}
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_PutOnSpecial(task* tp)
{
	ALW_ENTRY_WORK* pSpecialEntry = ALW_IsCommunication(tp);
	if (!pSpecialEntry) return BHV_RET_FINISH;
	
	task* pSpecial = pSpecialEntry->tp;
	if (!pSpecial) return BHV_RET_FINISH;
		
	int type = pSpecial->twp->ang.x;

	if (specialItemFuncs[type].first)
		specialItemFuncs[type].first(tp, pSpecial);

	//not action special
	if (!ModAPI_SpecialAction.contains(type)) {
		if (AL_GetItemSaveInfo(pSpecial)) {
			void* pSave = AL_GetItemSaveInfo(pSpecial);
			AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)pSave);
			AL_ClearItemSaveInfoPtr(pSpecial);
		}

		pSpecial->exec = DestroyTask;
		return BHV_RET_FINISH;
	}	

	return BHV_RET_CONTINUE;
}
extern "C" __declspec(dllexport) int __cdecl ALBHV_TurnToSpecial(task * tp) {
	chaowk* v1 = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &v1->Behavior;

	switch (bhv->Mode) {
	case 0:
		AL_SetMotionLinkStep(tp, 202, 15);
		bhv->Mode++;
		bhv->Timer = 60;
		break;
	}

	if (MOV_TurnToAim2(tp, 0x600) < 0xB6) {
		task* pSpecial;
		CCL_HIT_INFO* v2 = CCL_IsHitKindEx(tp, 0xCE);
		if (v2 && (pSpecial = v2->hit_tp) != 0) {
			if (!ALW_IsHeld(pSpecial)) {
				//StopHoldingTaskP(0);
				if (playertwp[0]) {
					sub_46E5E0(0, (int)playertwp[0]);
				}
				AL_GrabObjectBothHands(tp, pSpecial);
				AL_SetBehavior(tp, ALBHV_HoldThink);

				int id = pSpecial->twp->ang.x;
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
signed int __cdecl AL_CheckSpecial(task* a1)
{
	signed int(__cdecl * v1)(task*); // eax
	//task* v2; // eax
	task* v3; // esi

	v1 = GET_CHAOWK(a1)->Behavior.BhvFuncList[GET_CHAOWK(a1)->Behavior.CurrBhvFuncNum];
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
	CCL_HIT_INFO* v2 = CCL_IsHitKindEx(a1, 0xCE);
	if (!v2)
	{
		return 0;
	}
	v3 = v2->hit_tp;
	if (!v3)
	{
		return 0;
	}
	ALW_ENTRY_WORK* v4 = (ALW_ENTRY_WORK*)v3->fwp;
	if (v4)
	{
		if (v4->flag & 2)
		{
			return 0;
		}
	}

	MOV_SetAimPos(a1, &v3->twp->pos);
	AL_SetBehavior(a1, ALBHV_TurnToSpecial);
	return 1;
}
signed int __cdecl AL_CheckObakeHeadAndAccessory(task* a1)
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

static void AccessoryRemoveAll(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	auto pParam = GET_CWEPARAM(tp);
	
	for (size_t i = 0; i < _countof(pParam->Accessories); ++i) {
		const auto index = GET_CHAOWK_CWE(tp)->AccessoryIndices[i];
		if (index == -1) continue;

		auto saveinfo = (AccessorySaveInfo*)CWE_GetNewItemSaveInfo(ALW_CATEGORY_ACCESSORY);
		if (saveinfo) {
			const auto& accessoryChaoData = pParam->Accessories[i];
			saveinfo->UsedColors = accessoryChaoData.ColorFlags;
			memcpy(saveinfo->Colors, accessoryChaoData.ColorSlots, sizeof(saveinfo->Colors));
			
			Accessory_Load(index, &work->pos, NJM_DEG_ANG(njRandom() * 360.f), &GET_MOVE_WORK(tp)->Velo, saveinfo);
			AL_ParameterClearAccessory(tp, EAccessoryType(i));
		}
	}
}

//removing accessory
void __cdecl AccessoryRemove1(task* a1)
{
	AL_SetBehaviorWithTimer(a1, (int)0x563EB0, -1);
	AccessoryRemoveAll(a1);
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

void __cdecl AccessoryRemove2(task* tp, int a2, int a3) {
	AL_SetBehavior(tp, (BHV_FUNC)0x564320);
	AccessoryRemoveAll(tp);
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

static void AL_Behavior_PostureFix() {
	// the posture behaviors don't handle transitioning from FUSE (lie face down) anims
	// they do, but they just use the same anim as LIE which looks horrible
	// this is an attempt to fix them
	// PostureChangeSit now doesn't do anything for FUSE (todo: use Stand anim as base)
	// PostureChangeStand now uses the tripping get up animation (ALM_KOKEOKE)

	// PostureChangeSit is only used in garden, so we can hook it directly
	WriteJump((void*)0x55C430, ALBHV_PostureChangeSit);
	
	// PostureChangeStand is used in race too, and i don't wanna affect the timings
	// so ill manually change all garden references to it
	// GoToTV, GoToRadicase and GoToHorse are already handled in albhv_ltoy.cpp
	// (todo: why didn't we fix the Box?)
	WriteData((int*)(0x00561551 - 4), int(ALBHV_PostureChangeStand)); // GoToEat
	WriteData((int*)(0x005997DD - 4), int(ALBHV_PostureChangeStand)); // GoToBox
	WriteData((int*)(0x005999C2 - 4), int(ALBHV_PostureChangeStand)); // ListenRadicase
	WriteData((int*)(0x00599B12 - 4), int(ALBHV_PostureChangeStand)); // WatchTV
	WriteData((int*)(0x005A36D2 - 4), int(ALBHV_PostureChangeStand)); // WalkSelect
}

// AL_BehaviorResetParameter hook on bottom to free navigation points when not needed anymore
// we make sure to free it in Chao_Delete_r aswell, incase the player leaves the garden mid-navigation behavior chain
static void AL_BehaviorResetParameter_r(task* tp) {
	auto work = GET_CHAOWK(tp);

	// original behavior of the instructions we overwrite with our call
	work->field_B0 |= 2; 

	if (work->pNaviPoints) {
		FREE(work->pNaviPoints);
		work->pNaviPoints = NULL;
	}
}
static void __declspec(naked) AL_BehaviorResetParameter_t()
{
	__asm
	{
		push edi // a1

		// Call your __cdecl function here:
		call AL_BehaviorResetParameter_r

		add esp, 4 // a1
		retn
	}
}

void AL_Behavior_Init() {
	// writecall onto existing instructions on the bottom of behaviorresetparameter
	WriteCall((void*)0x0053D881, AL_BehaviorResetParameter_t);
	WriteData<0xB-0x6>((uint8_t*)0x0053D886, (uint8_t)0x90);

	AL_Behavior_PostureFix();

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