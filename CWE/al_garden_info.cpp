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
void sub_46E5B0(ObjectMaster* a1, int a2)
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
DataPointer(ChaoObjectData*, dword_19F6454, 0x19F6454);

int AL_GetHoldingItemKind() {
	if (!dword_19F6454) return -1;
	
	if (HeldItemType == ChaoItemCategory_Accessory) {
		return ((ItemSaveInfoBase*)dword_19F6454)->IndexID;
	}

	return dword_19F6454->Type;
}

void AL_ClearItemSaveInfo(ITEM_SAVE_INFO* pSaveInfo) {
	pSaveInfo->Type = -1;
	pSaveInfo->position = { 0, 0, 0 };
	pSaveInfo->Garden = -1;
	pSaveInfo->Age = 0;
	pSaveInfo->Size = 0;
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

		const Angle ang = NJM_DEG_ANG(njRandom() * 360.f);
		const NJS_VECTOR velocity = { 0,0,0 };

		switch (item.GetCategory()) {
			case ChaoItemCategory_Accessory:
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
	ChaoObjectData* v5; // edi
	ChaoObjectData* v6; // ebp
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
			v6 = (ChaoObjectData*)&result->RaceData;
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
			v5 = (ChaoObjectData*)cweSaveFile.specialItems;
			v6 = (ChaoObjectData*)&cweSaveFile.specialItems[30];
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
			v8 = &v5->position;
			do
			{
				result = (ALFSave*)(unsigned __int16)v5->Type;
				if ((signed __int16)v5->Type < 0)
				{
					goto LABEL_30;
				}
				if (v5->Garden == AL_GetStageNumber())
				{
					if (v5 != (ChaoObjectData*)dword_19F6454)
					{
						a4.x = 0.0f;
						a4.y = 0.0f;
						a4.z = 0.0f;
						switch (a1)
						{
						case 2:
							if (v5->Type >= 21 && v5->Type < 25)
							{
								ALO_ChaosDriveExecutor_Load(
									LOBYTE(v5->Type) - 21,
									v8,
									&a4,
									(ChaoData*)v5);
							}
							else
							{
								AL_MinimalExecutor_Load((char)v5->Type, v8, v4, &a4, (int)v5);
							}
							break;
						case 3:
							if (v5->Size)
							{
								ALO_FruitExecutor_Load(
									v5->Type,
									v8,
									v4,
									&a4,
									(ChaoData*)v5);
							}
							else
							{
								AL_ClearItemSaveInfo(v5);
							}
							break;
						case 7:
							ALO_SeedExecutor_Load(v5->Type, v8, &a4, (int)v5);
							break;
						case ChaoItemCategory_Special:

							ALO_Special_Load(
								v5->Type,
								v8,
								v4,
								&a4,
								(short*)v5);
							break;
						case 9:
							if (v5->Type > 0) {
								ALO_ObakeHeadExecutor_Load(
									v5->Type,
									v8,
									v4,
									&a4,
									(int)v5);
							}
							break;
						default:
							goto LABEL_27;
						}
					}
					goto LABEL_27;
				}
				if (v5->Garden <= 0 || v5->Garden > 3)
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
	AL_SetItemOnTheGarden(ChaoItemCategory_Special);
}

static void AL_CreateCustomHoldingItem() {
	const NJS_VECTOR velocity = { 0,0,0 };
	ItemSaveInfoBase* pSaveInfo = (ItemSaveInfoBase*)dword_19F6454;

	if (MainCharObj2[0])
	{
		sub_46E5E0(0, (int)MainCharObj2[0]);
	}
	MainCharObj2[0]->HeldObject = 0;

	task* tp = NULL;

	switch (HeldItemType) {
		case ChaoItemCategory_Accessory:
			tp = Accessory_Load(pSaveInfo->IndexID, &pSaveInfo->Position, pSaveInfo->Angle, &velocity, (AccessorySaveInfo*)pSaveInfo);
			break;
	}

	sub_46E5B0(tp, 0);
}

static void AL_CreateHoldingItem() {
	ChaoObjectData* v1; // esi
	EntityData1* v2; // edi
	int v3; // ebx
	ObjectMaster* v6; // esi
	ChaoObjectData* v7; // [esp+8h] [ebp-1Ch]
	NJS_VECTOR a2; // [esp+Ch] [ebp-18h]
	NJS_VECTOR a4; // [esp+18h] [ebp-Ch]

	a4 = { 0,0,0 };
	v1 = dword_19F6454;
	v7 = dword_19F6454;

	if (HeldItemType == ChaoItemCategory_Accessory) {
		AL_CreateCustomHoldingItem();
		return;
	}

	switch (HeldItemType)
	{
	case 2:
	case ChaoItemCategory_Fruit:
	case ChaoItemCategory_Seed:
	case ChaoItemCategory_Hat:
	case ChaoItemCategory_Special:
		if (dword_19F6454 && dword_19F6454->Type >= 0 && MainCharObj1[0])
		{
			v2 = MainCharObj1[0];

			a2 = MainCharObj1[0]->Position;
			v3 = MainCharObj1[0]->Rotation.y;

			a2.x += njSin(v3) * 3.0f;
			a2.z += njCos(v3) * 3.0f;

			if (v2)
			{
				sub_46E5E0(0, (int)v2);
			}
			MainCharObj2[0]->HeldObject = 0;
			switch (HeldItemType)
			{
			case 2:
				if (v1->Type >= 21 && v1->Type < 25)
				{
					v6 = ALO_ChaosDriveExecutor_Load(LOBYTE(v1->Type) - 21, &a2, &a4, (ChaoData*)v1);
				}
				else
				{
					v6 = AL_MinimalExecutor_Load((char)v1->Type, &a2, MainCharObj1[0]->Rotation.y, &a4, (int)v1);
				}
				break;
			case 3:
				v6 = ALO_FruitExecutor_Load(v1->Type, &a2, MainCharObj1[0]->Rotation.y, &a4, (ChaoData*)v1);
				break;
			case 7:
				v6 = ALO_SeedExecutor_Load(v1->Type, &a2, &a4, (int)v1);
				break;
			case 9:
				v6 = ALO_ObakeHeadExecutor_Load(v1->Type, &a2, MainCharObj1[0]->Rotation.y, &a4, (int)v1);
				break;
			case ChaoItemCategory_Special:
				v6 = ALO_Special_Load(v1->Type, &a2, MainCharObj1[0]->Rotation.y, &a4, (short*)v1);
				break;
			default:
				throw std::exception("CWE: holding incorrect item");
				return;
			}

			if (AL_IsGarden())
			{
				v7->Garden = AL_GetStageNumber();
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

static bool AL_CreatePurchasedCustomItem(const SAlItem& item, NJS_POINT3& position, NJS_VECTOR& velocity) {
	if (item.mCategory != ChaoItemCategory_Accessory) {
		return false;
	}

	Accessory_Load(item.mType, &position, MainCharObj1[0]->Rotation.y, &velocity, (AccessorySaveInfo*)CWE_GetNewItemSaveInfo(ChaoItemCategory_Accessory));

	return true;
}

// this is a hook that redirects responsibility to AL_CreatePurchasedCustomItem to create the new item types (except special)
static void AL_MinimalCreateManagerExecutor_New(ObjectMaster* a2) {
	EntityData1* v1; // esi
	double v5; // st7
	int v7; // eax
	int v8; // ebp
	int v9; // eax
	int v10; // esi
	int v27; // eax
	int v28; // ecx
	int v29; // eax
	SAlItem* v30; // esi
	SAlItem* v31; // edi
	int v32; // [esp+8h] [ebp-D0h]
	NJS_VECTOR position; // [esp+Ch] [ebp-CCh]
	NJS_VECTOR output; // [esp+18h] [ebp-C0h]
	NJS_VECTOR result; // [esp+24h] [ebp-B4h]


	v1 = (EntityData1*)a2->Data1.Entity;
	v1->Index++;
	if (v1->Index > 15)
	{
		v1->Index = 0;

		position.x = MainCharObj1[0]->Position.x;
		position.y = MainCharObj1[0]->Position.y + 4.5f;
		position.z = MainCharObj1[0]->Position.z;

		result.x = 0;
		result.y = (0.5f - njRandom()) * 0.2f * 1.6f + 0.96f;
		result.z = (0.5f - njRandom()) * 0.25f * 1.6f + 0.56f;

		njPushMatrixEx();
		memcpy(_nj_current_matrix_ptr_, (void*)0x025F02A0, 0x30);
		v5 = (0.5 - njRandom()) * 60.0 * -182.0444488525391;
		RotateY(0x4000 - (signed int)v5 - MainCharObj1[0]->Rotation.y);
		njCalcVector(&result, &output, _nj_current_matrix_ptr_);
		njPopMatrixEx();

		v7 = cweSaveFile.purchasedItemCount;
		if (cweSaveFile.purchasedItemCount <= 0)
		{
			a2->MainSub = DeleteObject_;
		}
		else
		{
			v8 = v7 - 1;
			v1->Rotation.x = v7 - 1;
			v9 = save::CWE_PurchasedItems[v7 - 1].mCategory;
			v10 = save::CWE_PurchasedItems[v8].mType;
			--cweSaveFile.purchasedItemCount;
			v32 = v10;

			if (!AL_CreatePurchasedCustomItem(save::CWE_PurchasedItems[v8], position, output)) {
				switch (v9) {
				case 1:
					if (AL_GetLocalChaoCount(AL_GetStageNumber()) < 8)
					{
						AL_GENE gene;
						ObjectMaster* v12;
						ChaoData* v11 = AL_GetNewChaoSaveInfo();

						InitChaoDNA(&gene);
						gene.EggColor = v32;

						if (v11) {
							v12 = CreateChaoEgg(&gene, v11, 0, &position, 3);
						}
						else {
							v12 = CreateChaoEgg(&gene, 0, 0, &position, 3);
						}

						if (v12) {
							MOV_SetVelo(v12, &output);
						}
						break;
					}
					++cweSaveFile.purchasedItemCount;
					return;
				case 3:
					ALO_FruitExecutor_Load(v10, &position, MainCharObj1[0]->Rotation.y, &output, (ChaoData*)AL_GetNewItemSaveInfo(3));
					break;
				case 7:
					ALO_SeedExecutor_Load(v10, &position, &output, (int)AL_GetNewItemSaveInfo(7));
					break;
				case 9:
					ALO_ObakeHeadExecutor_Load(v10, &position, MainCharObj1[0]->Rotation.y, &output, (int)AL_GetNewItemSaveInfo(9));
					break;
				case ChaoItemCategory_Special:
					ALO_Special_Load(v10, &position, MainCharObj1[0]->Rotation.y, &output, (short*)AL_GetSpecialItemSave());
					break;
				default:
					goto LABEL_34;
				}
			}
			save::CWE_PurchasedItems[v8].mCategory = (ChaoItemCategory)-1;
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
				save::CWE_PurchasedItems[v27].mCategory = (ChaoItemCategory)-1;
			}
		}
	}
}

FunctionPointer(void, AL_PackageItemSaveInfo, (int a1), 0x52E710);

static void AL_PackageCustomItemSaveInfo(const ChaoItemCategory category) {
	if (!AL_IsGarden()) return;

	int count = CWE_ALW_CountEntry(category);
	while (count) {
		task* tp;
		--count;
		tp = CWE_ALW_GetTaskCount(category, count);
		if (tp) {
			const auto stage = AL_GetStageNumber();
			EntityData1* work = tp->Data1.Entity;
			ItemSaveInfoBase* pSaveInfo = (ItemSaveInfoBase*)(((al_entry_work*)tp->UnknownA_ptr)->pSaveInfo);
			
			// not held (! & 0x8000) most likely
			if (work->Status >= 0) {
				pSaveInfo->Position = work->Position;
				pSaveInfo->Angle = work->Rotation.y;
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
	AL_PackageCustomItemSaveInfo(ChaoItemCategory_Accessory);
	SaveToyPos();
}

void AL_GardenInfo_Init() {
	// we hook the fruit state saving function call to package our own stuff too
	// code is super ugly because i used the other save functions decompiled as a base
	WriteCall((void*)0x0052F09F, AL_PackageItemSaveInfo_Hook);

	//fixed hat/accessory/special slot problem
	DataArray(int, dword_8AB838, 0x8AB838, 1);
	dword_8AB838[ChaoItemCategory_Hat] = 64;

	WriteJump((void*)0x548F40, AL_MinimalCreateManagerExecutor_New);
	WriteCall((void*)0x0052EBD8, AL_SetObjectOnTheGarden_Hook);
	WriteJump((void*)0x52E920, AL_SetItemOnTheGarden);
	WriteJump((void*)0x52F2A0, AL_CreateHoldingItem);
}