#include "stdafx.h"
#include <ChaoMain.h>
#include <al_stage.h>
#include <alo_special.h>
#include <ninja_functions.h>
#include <save/save_item.h>
#include <al_toy_move.h>

const int sub_46E5E0Ptr = 0x46E5E0;
void sub_46E5E0(int a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		call sub_46E5E0Ptr
	}
}

const int sub_46E5B0Ptr = 0x46E5B0;
void sub_46E5B0(task* a1, int a2)
{
	__asm
	{
		push a1
		mov ecx, a2
		call sub_46E5B0Ptr
		add esp, 4
	}
}

DataPointer(int, HeldItemType, 0x19F6450);
DataPointer(ITEM_SAVE_INFO*, dword_19F6454, 0x19F6454);

void* AL_GetHoldingItemSaveInfo() {
	return dword_19F6454;
}

void AL_ClearHoldingItemInfo() {
	HeldItemType = -1;
	dword_19F6454 = NULL;
}

int AL_GetHoldingItemKind() {
	if (!dword_19F6454) return -1;
	
	if (CWE_IsCustomItemSaveInfoCategory(HeldItemType)) {
		return ((ItemSaveInfoBase*)dword_19F6454)->IndexID;
	}

	return dword_19F6454->kind;
}

Sint8 AL_GetHoldingItemCategory() {
	return HeldItemType;
}

void AL_ClearItemSaveInfo(ITEM_SAVE_INFO* pSaveInfo) {
	pSaveInfo->kind = -1;
	pSaveInfo->pos = { 0, 0, 0 };
	pSaveInfo->place = -1;
	pSaveInfo->nbVisit = 0;
	pSaveInfo->status = 0;
}

void AL_ClearItemSaveInfo(ItemSaveInfoBase* pSaveInfo) {
	pSaveInfo->Clear();
}

// new SetItemOnTheGarden with the new item saves
void AL_SetCustomItemOnTheGarden() {
	for (auto& item : AccessoryItemList) {
		if (item.Garden != AL_GetStageNumber() || (void*)&item == dword_19F6454) {
			continue;
		}

		if (item.IndexID == -1) continue;

		const Angle ang = NJM_DEG_ANG(njRandom() * 360.f);
		const NJS_VECTOR velocity = { 0,0,0 };

		switch (item.GetCategory()) {
			case ALW_CATEGORY_ACCESSORY:
				Accessory_Load(item.IndexID, &item.Position, ang, &velocity, &item);
				break;
		}
	}
}

// this handles spawning (kept rn for special)
void AL_SetItemOnTheGarden(int a1)
{
	int v1; // esi
	double v2; // st7
	ALFSave* result; // eax
	int v4; // ebx
	ITEM_SAVE_INFO* v5; // edi
	ITEM_SAVE_INFO* v6; // ebp
	NJS_VECTOR* v8; // esi
	int v10; // [esp+18h] [ebp-10h]
	NJS_VECTOR a4; // [esp+1Ch] [ebp-Ch]
	a4 = { 0,0,0 };
	v1 = AL_GetStageNumber();
	v10 = AL_GetStageNumber();
	v2 = NJM_DEG_ANG(njRandom() * 360);
	result = (ALFSave*)(signed int)v2;
	v4 = (signed int)v2;
	if ((unsigned int)(v1 - 1) <= 2)
	{
		result = (ALFSave*)(a1 - 2);
		switch (a1)
		{
		case 2:
			result = (ALFSave*)GetChaoSavePointer();
			v5 = result->ChaoAnimalSlots;
			v6 = (ITEM_SAVE_INFO*)&result->RaceData;
			break;
		case 3:
			result = (ALFSave*)GetChaoSavePointer();
			v5 = result->ChaoFruitSlots;
			v6 = result->ChaoSeedSlots;
			break;
		case 7:
			result = (ALFSave*)GetChaoSavePointer();
			v5 = result->ChaoSeedSlots;
			v6 = result->ChaoHatSlots;
			break;
		case 10:
			result = (ALFSave*)GetChaoSavePointer();
			v5 = (ITEM_SAVE_INFO*)cweSaveFile.specialItems;
			v6 = (ITEM_SAVE_INFO*)&cweSaveFile.specialItems[30];
			break;
		case 9:
			result = (ALFSave*)GetChaoSavePointer();
			v5 = result->ChaoHatSlots;
			v6 = result->ChaoAnimalSlots;
			break;
		default:
			return;
			//v5 = v10;
			//v6 = v10;
			break;
		}
		if (v5 < v6)
		{
			v8 = &v5->pos;
			do
			{
				result = (ALFSave*)(unsigned __int16)v5->kind;
				if ((signed __int16)v5->kind < 0)
				{
					goto LABEL_30;
				}
				if (v5->place == AL_GetStageNumber())
				{
					if (v5 != (ITEM_SAVE_INFO*)dword_19F6454)
					{
						a4.x = 0.0f;
						a4.y = 0.0f;
						a4.z = 0.0f;
						switch (a1)
						{
						case ALW_CATEGORY_MINIMAL:
							if (v5->kind >= 21 && v5->kind < 25)
							{
								ALO_ChaosDriveCreate(
									LOBYTE(v5->kind) - 21,
									v8,
									&a4,
									v5);
							}
							else
							{
								AL_MinimalCreate((char)v5->kind, v8, v4, &a4, v5);
							}
							break;
						case ALW_CATEGORY_FRUIT:
							if (v5->status)
							{
								ALO_FruitCreate(
									v5->kind,
									v8,
									v4,
									&a4,
									v5);
							}
							else
							{
								AL_ClearItemSaveInfo(v5);
							}
							break;
						case ALW_CATEGORY_SEED:
							ALO_SeedCreate(v5->kind, v8, &a4, v5);
							break;
						case ALW_CATEGORY_SPECIAL:
							ALO_SpecialCreate(
								v5->kind,
								v8,
								v4,
								&a4,
								v5);
							break;
						case ALW_CATEGORY_MASK:
							if (v5->kind > 0) {
								ALO_ObakeHeadCreate(
									v5->kind,
									v8,
									v4,
									&a4,
									v5);
							}
							break;
						default:
							goto LABEL_27;
						}
					}
					goto LABEL_27;
				}
				if (v5->place <= 0 || v5->place > 3)
				{
				LABEL_30:
					AL_ClearItemSaveInfo(v5);
					result = 0;
				}
			LABEL_27:
				++v5;
				v8 = (NJS_VECTOR*)((char*)v8 + 20);
			} while (v5 < v6);
		}
	}
}

static void AL_SetObjectOnTheGarden_Hook(int a1) {
	AL_SetItemOnTheGarden(3);
	AL_SetCustomItemOnTheGarden();
	AL_SetItemOnTheGarden(ALW_CATEGORY_SPECIAL);
}

static void AL_CreateCustomHoldingItem() {
	const NJS_VECTOR velocity = { 0,0,0 };
	ItemSaveInfoBase* pSaveInfo = (ItemSaveInfoBase*)dword_19F6454;

	if (playerpwp[0])
	{
		sub_46E5E0(0, (int)playerpwp[0]);
	}
	playerpwp[0]->htp = 0;

	task* tp = NULL;

	switch (AL_GetHoldingItemCategory()) {
		case ALW_CATEGORY_ACCESSORY:
			tp = Accessory_Load(pSaveInfo->IndexID, &pSaveInfo->Position, pSaveInfo->Angle, &velocity, (AccessorySaveInfo*)pSaveInfo);
			break;
	}

	sub_46E5B0(tp, 0);
}

static void AL_CreateHoldingItem() {
	ITEM_SAVE_INFO* v1; // esi
	taskwk* v2; // edi
	int v3; // ebx
	task* v6; // esi
	ITEM_SAVE_INFO* v7; // [esp+8h] [ebp-1Ch]
	NJS_VECTOR a2; // [esp+Ch] [ebp-18h]
	NJS_VECTOR a4; // [esp+18h] [ebp-Ch]

	a4 = { 0,0,0 };
	v1 = dword_19F6454;
	v7 = dword_19F6454;

	if (CWE_IsCustomItemSaveInfoCategory(AL_GetHoldingItemCategory())) {
		AL_CreateCustomHoldingItem();
		return;
	}

	switch (AL_GetHoldingItemCategory())
	{
	case ALW_CATEGORY_MINIMAL:
	case ALW_CATEGORY_FRUIT:
	case ALW_CATEGORY_SEED:
	case ALW_CATEGORY_MASK:
	case ALW_CATEGORY_SPECIAL:
		if (dword_19F6454 && dword_19F6454->kind >= 0 && playertwp[0])
		{
			v2 = playertwp[0];

			a2 = playertwp[0]->pos;
			v3 = playertwp[0]->ang.y;

			a2.x += njSin(v3) * 3.0f;
			a2.z += njCos(v3) * 3.0f;

			if (v2)
			{
				sub_46E5E0(0, (int)v2);
			}
			playerpwp[0]->htp = 0;
			switch (HeldItemType)
			{
			case ALW_CATEGORY_MINIMAL:
				if (v1->kind >= 21 && v1->kind < 25)
				{
					v6 = ALO_ChaosDriveCreate(LOBYTE(v1->kind) - 21, &a2, &a4, v1);
				}
				else
				{
					v6 = AL_MinimalCreate((char)v1->kind, &a2, playertwp[0]->ang.y, &a4, v1);
				}
				break;
			case ALW_CATEGORY_FRUIT:
				v6 = ALO_FruitCreate(v1->kind, &a2, playertwp[0]->ang.y, &a4, v1);
				break;
			case ALW_CATEGORY_SEED:
				v6 = ALO_SeedCreate(v1->kind, &a2, &a4, v1);
				break;
			case ALW_CATEGORY_MASK:
				v6 = ALO_ObakeHeadCreate(v1->kind, &a2, playertwp[0]->ang.y, &a4, v1);
				break;
			case ALW_CATEGORY_SPECIAL:
				v6 = ALO_SpecialCreate(v1->kind, &a2, playertwp[0]->ang.y, &a4, v1);
				break;
			default:
				throw std::exception("CWE: holding incorrect item");
				return;
			}

			if (AL_IsGarden())
			{
				v7->place = AL_GetStageNumber();
			}
			sub_46E5B0(v6, 0);
		}
		break;
	default:
		return;
	}
}

const int sub_5319F0Ptr = 0x5319F0;
int AL_GetLocalChaoCount(int a1)
{
	int retVal;
	__asm
	{
		mov esi, a1
		call sub_5319F0Ptr
		mov retVal, eax
	}
	return retVal;
}

static bool AL_CreatePurchasedCustomItem(const SAlItemCwe& item, NJS_POINT3& position, NJS_VECTOR& velocity) {
	if (!CWE_IsCustomItemSaveInfoCategory(item.mCategory)) {
		return false;
	}

	// only accessory for now
	Accessory_Load(item.mType, &position, playertwp[0]->ang.y, &velocity, (AccessorySaveInfo*)CWE_GetNewItemSaveInfo(ALW_CATEGORY_ACCESSORY));

	return true;
}

// this is a hook that redirects responsibility to AL_CreatePurchasedCustomItem to create the new item types (except special)
static void AL_MinimalCreateManagerExecutor_New(task* a2) {
	taskwk* v1; // esi
	double v5; // st7
	int v7; // eax
	int v8; // ebp
	int v9; // eax
	int v10; // esi
	int v27; // eax
	int v28; // ecx
	int v29; // eax
	SAlItemCwe* v30; // esi
	SAlItemCwe* v31; // edi
	int v32; // [esp+8h] [ebp-D0h]
	NJS_VECTOR position; // [esp+Ch] [ebp-CCh]
	NJS_VECTOR output; // [esp+18h] [ebp-C0h]
	NJS_VECTOR result; // [esp+24h] [ebp-B4h]


	v1 = (taskwk*)a2->twp;
	v1->btimer++;
	if (v1->btimer > 15)
	{
		v1->btimer = 0;

		position.x = playertwp[0]->pos.x;
		position.y = playertwp[0]->pos.y + 4.5f;
		position.z = playertwp[0]->pos.z;

		result.x = 0;
		result.y = (0.5f - njRandom()) * 0.2f * 1.6f + 0.96f;
		result.z = (0.5f - njRandom()) * 0.25f * 1.6f + 0.56f;

		njPushMatrixEx();
		memcpy(_nj_current_matrix_ptr_, (void*)0x025F02A0, 0x30);
		v5 = (0.5 - njRandom()) * 60.0 * -182.0444488525391;
		RotateY(0x4000 - (signed int)v5 - playertwp[0]->ang.y);
		njCalcVector(&result, &output, _nj_current_matrix_ptr_);
		njPopMatrixEx();

		v7 = cweSaveFile.purchasedItemCount;
		if (cweSaveFile.purchasedItemCount <= 0)
		{
			a2->exec = DestroyTask;
		}
		else
		{
			v8 = v7 - 1;
			v1->ang.x = v7 - 1;
			v9 = save::CWE_PurchasedItems[v7 - 1].mCategory;
			v10 = save::CWE_PurchasedItems[v8].mType;
			--cweSaveFile.purchasedItemCount;
			v32 = v10;

			if (!AL_CreatePurchasedCustomItem(save::CWE_PurchasedItems[v8], position, output)) {
				switch (v9) {
				case ALW_CATEGORY_EGG:
					if (AL_GetLocalChaoCount(AL_GetStageNumber()) < 8)
					{
						AL_GENE gene;
						task* v12;
						CHAO_SAVE_INFO* v11 = AL_GetNewChaoSaveInfo();

						InitChaoDNA(&gene);
						gene.EggColor = v32;

						if (v11) {
							v12 = CreateEgg(&gene, v11, 0, &position, 3);
						}
						else {
							v12 = CreateEgg(&gene, 0, 0, &position, 3);
						}

						if (v12) {
							MOV_SetVelo(v12, &output);
						}
						break;
					}
					++cweSaveFile.purchasedItemCount;
					return;
				case ALW_CATEGORY_FRUIT:
					ALO_FruitCreate(v10, &position, playertwp[0]->ang.y, &output, AL_GetNewItemSaveInfo(ALW_CATEGORY_FRUIT));
					break;
				case ALW_CATEGORY_SEED:
					ALO_SeedCreate(v10, &position, &output, AL_GetNewItemSaveInfo(ALW_CATEGORY_SEED));
					break;
				case ALW_CATEGORY_MASK:
					ALO_ObakeHeadCreate(v10, &position, playertwp[0]->ang.y, &output, AL_GetNewItemSaveInfo(ALW_CATEGORY_MASK));
					break;
				case ALW_CATEGORY_SPECIAL:
					ALO_SpecialCreate(v10, &position, playertwp[0]->ang.y, &output, AL_GetSpecialItemSave());
					break;
				default:
					goto LABEL_34;
				}
			}
			save::CWE_PurchasedItems[v8].mCategory = -1;
		LABEL_34:
			if (v8 != cweSaveFile.purchasedItemCount)
			{
				v27 = v8;
				if (v8 < cweSaveFile.purchasedItemCount)
				{
					v28 = cweSaveFile.purchasedItemCount - v8;
					v29 = cweSaveFile.purchasedItemCount - v8;
					v30 = &save::CWE_PurchasedItems[v8 + 1];
					v31 = &save::CWE_PurchasedItems[v8];
					while (v28)
					{
						*v31 = *v30;
						++v30;
						++v31;
						--v28;
					}
					v27 = v8 + v29;
				}
				save::CWE_PurchasedItems[v27].mCategory = -1;
			}
		}
	}
}

FunctionPointer(void, AL_PackageItemSaveInfo, (int a1), 0x52E710);

static void AL_PackageCustomItemSaveInfo(const Sint8 category) {
	if (!AL_IsGarden()) return;

	int count = CWE_ALW_CountEntry(category);
	while (count) {
		task* tp;
		--count;
		tp = CWE_ALW_GetTaskCount(category, count);
		if (tp) {
			const auto stage = AL_GetStageNumber();
			taskwk* work = tp->twp;
			ItemSaveInfoBase* pSaveInfo = (ItemSaveInfoBase*)(((ALW_ENTRY_WORK*)tp->fwp)->pSaveInfo);
			
			// not held (! & 0x8000) most likely
			if (work->flag >= 0) {
				pSaveInfo->Position = work->pos;
				pSaveInfo->Angle = work->ang.y;
			}
			else if (AL_IsGarden()) {
				pSaveInfo->Position = ProbablyChaoSpawnPoints[(stage - 1) * 16 + int(njRandom() * 15.f)];
			}
			else {
				pSaveInfo->Position = { 0,0,0 };
			}

			if (AL_IsGarden()) {
				pSaveInfo->Garden = stage;
			}
		}
	}
}

static void AL_PackageItemSaveInfo_Hook() {
	AL_PackageItemSaveInfo(3);
	AL_PackageCustomItemSaveInfo(ALW_CATEGORY_ACCESSORY);
	SaveToyPos();
}

void AL_GardenInfo_Init() {
	// we hook the fruit state saving function call to package our own stuff too
	// code is super ugly because i used the other save functions decompiled as a base
	WriteCall((void*)0x0052F09F, AL_PackageItemSaveInfo_Hook);

	//fixed hat/accessory/special slot problem
	DataArray(int, dword_8AB838, 0x8AB838, 1);
	dword_8AB838[ALW_CATEGORY_MASK] = 64;

	WriteJump((void*)0x548F40, AL_MinimalCreateManagerExecutor_New);
	WriteCall((void*)0x0052EBD8, AL_SetObjectOnTheGarden_Hook);
	WriteJump((void*)0x52E920, AL_SetItemOnTheGarden);
	WriteJump((void*)0x52F2A0, AL_CreateHoldingItem);
}