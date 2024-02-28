#include "stdafx.h"
#include "SA2ModLoader.h"
#include <al_save.h>
#include "AL_ModAPI.h"
#include "api/api_customchao.h"

#define CWE_VER_DEF(a,b,c,d) ((a << 12) | (b << 8) | (c << 4) | d)

struct KCE_HELPER {
	const char** pCustomChaoMap;
	size_t CustomChaoMapSize; //pair count

	const char** pMsgAlItem;
	size_t MsgAlItemSize;

	CWESaveFile* pCweSave;

	SAlItem* pPurchasedItems;

	int* pLensSpecialMap;
	size_t LensSpecialMapSize;
};

static KCE_HELPER kce_helper;

void KCE_Update() {
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
}

void KCE_Init() {
	kce_helper.pCweSave = &cweSaveFile;
	kce_helper.pPurchasedItems = save::CWE_PurchasedItems.data();

	//krzys pointer
	WriteData((int*)0x0053FDF9, (int)&cweSaveFile);
	WriteData((int*)(0x0053FDF9 + 8), (int)save::CWE_PurchasedItems.data());

	WriteData((int*)0x53FF04, (int)&kce_helper);

	WriteData((int*)0x53FF00, (int)CWE_VER_DEF(9, 5, 3, 4));
}