#include "stdafx.h"
#include "SA2ModLoader.h"
#include <al_save.h>
#include "AL_ModAPI.h"
#include "al_parts.h"
#include "api/api_customchao.h"
#include <api/api_accessory.h>

#define CWE_VER_DEF(a,b,c,d) ((a << 12) | (b << 8) | (c << 4) | d)

struct KCE_ACCESSORY_INFO {
	char ID[METADATA_ID_SIZE];
	Uint8 ColorCount; // simple "array size" thing, 8 is max obv
	Uint32 Colors[8];
	EAccessoryType AccessoryType;
};

struct KCE_HELPER {
	const char** pCustomChaoMap;
	size_t CustomChaoMapSize; //pair count

	const char** pMsgAlItem;
	size_t MsgAlItemSize;

	CWESaveFile* pCweSave;

	SAlItem* pPurchasedItems;

	int* pLensSpecialMap;
	size_t LensSpecialMapSize;

	const char** pCustomAnimalName;
	Uint8* pCustomAnimalHasPartFlag;
	size_t CustomAnimalNameCount;

	Uint32 AccessoryInfoVersion;
	KCE_ACCESSORY_INFO* pAccessoryInfo;
	size_t AccessoryInfoCount;

	AccessorySaveInfo* pAccessorySaves;
	size_t AccessorySaveMaxCount;
};

static KCE_HELPER kce_helper;

static void KCE_UpdateAccessories() {
	kce_helper.AccessoryInfoVersion = 1;

	const auto& ids = ItemMetadata::Get()->GetIDs(ChaoItemCategory_Accessory);
	KCE_ACCESSORY_INFO* info = new KCE_ACCESSORY_INFO[ids.size()];
	for (size_t i = 0; i < ids.size(); ++i) {
		info[i].ColorCount = GetAccessoryColorCount(i);

		info[i].AccessoryType = GetAccessoryType(i);

		strcpy_s(info[i].ID, ids[i].c_str());
		memcpy(info[i].Colors, GetAccessoryDefaultColors(i), sizeof(info[i].Colors));
	}

	kce_helper.pAccessoryInfo = info;
	kce_helper.AccessoryInfoCount = ids.size();
}

void KCE_Update() {
	KCE_UpdateAccessories();

	WriteData((int*)(0x0053FDF9 + 4), (int)MsgAlItem.data());
	kce_helper.pMsgAlItem = MsgAlItem.data();
	kce_helper.MsgAlItemSize = MsgAlItem.size();

	// custom chao map
	if (kce_helper.pCustomChaoMap) {
		delete kce_helper.pCustomChaoMap;
	}

	size_t char_chao_count = CustomChaoTypeEntries.size();
	kce_helper.pCustomChaoMap = new const char* [char_chao_count * 2];
	kce_helper.CustomChaoMapSize = char_chao_count;

	for (size_t i = 0; i < char_chao_count; i++) {
		const auto& entry = CustomChaoTypeEntries[i];
		kce_helper.pCustomChaoMap[i * 2] = entry.Data.ID;
		kce_helper.pCustomChaoMap[i * 2 + 1] = entry.Data.Name;
	}
	
	// lens-special item map
	if (kce_helper.pLensSpecialMap) {
		delete kce_helper.pLensSpecialMap;
	}

	size_t lens_special_count = ModAPI_LensColorMap.size();
	kce_helper.pLensSpecialMap = new int[lens_special_count * 2];
	kce_helper.LensSpecialMapSize = lens_special_count;

	size_t i = 0;
	for (const auto& entry : ModAPI_LensColorMap) {
		kce_helper.pLensSpecialMap[i * 2] = entry.first;
		kce_helper.pLensSpecialMap[i * 2 + 1] = entry.second;
		i++;
	}

	kce_helper.CustomAnimalNameCount = ModAPI_MinimalNames.size();
	kce_helper.pCustomAnimalName = new const char* [kce_helper.CustomAnimalNameCount];
	kce_helper.pCustomAnimalHasPartFlag = new Uint8[kce_helper.CustomAnimalNameCount];
	for (size_t i = 0; i < kce_helper.CustomAnimalNameCount; ++i) {
		kce_helper.pCustomAnimalName[i] = ModAPI_MinimalNames[i];
		kce_helper.pCustomAnimalHasPartFlag[i] = 0;

		for (size_t j = 0; j < NB_PARTS_KIND; ++j) {
			if (ModAPI_MiniParts[40 + i][0].objects[PartsObjectListNumber[j][0]]) {
				kce_helper.pCustomAnimalHasPartFlag[i] |= (1 << j);
			}
		}
	}
}

void KCE_Init() {
	kce_helper.pCweSave = &cweSaveFile;
	kce_helper.pPurchasedItems = save::CWE_PurchasedItems.data();

	kce_helper.pAccessorySaves = AccessoryItemList.data();
	kce_helper.AccessorySaveMaxCount = AccessoryItemMax;

	//krzys pointer
	WriteData((int*)0x0053FDF9, (int)&cweSaveFile);
	WriteData((int*)(0x0053FDF9 + 8), (int)save::CWE_PurchasedItems.data());

	WriteData((int*)0x53FF04, (int)&kce_helper);

	WriteData((int*)0x53FF00, (int)CWE_VER_DEF(9, 6, 0, 0));
}