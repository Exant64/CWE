#pragma once
#include "cwe_api.h"

enum CWE_API_ACCESSORY_FLAGS {
	CWE_API_ACCESSORY_FLAGS_FULL_BALD = BIT_0,
	CWE_API_ACCESSORY_FLAGS_NO_JIGGLE = BIT_1,
};

struct CWE_API_ACCESSORY_COLOR_ENTRY {
	Uint32 NodeIndex;
	Uint32 MaterialIndex;
	Uint32 ColorSlot;
};

struct CWE_API_ACCESSORY_DATA {
	char ID[20];

	NJS_OBJECT* pObject;

	const char* pTextureName;
	NJS_TEXLIST* pTexlist;

	EAccessoryType SlotType;

	BlackMarketItemAttributes* pMarketAttrib;
	const char* pName;
	const char* pDescription;

	// ..later wink wink
#if 0
	uint64_t HideNodes; // bitfield

	CWE_API_ACCESSORY_COLOR_ENTRY* pColorEntries;
	Uint32 ColorEntryCount;
#endif

	Uint32 Flags;
};

#define ACCESSORY_ID_LENGTH sizeof(CWE_API_ACCESSORY_DATA::ID)

extern std::vector<CWE_API_ACCESSORY_DATA> ModAPI_AccessoryDataList;

CWE_API_ACCESSORY_DATA& GetAccessoryData(int index);
extern "C" __declspec(dllexport) EAccessoryType GetAccessoryType(int index);

__declspec(dllexport) void AccessoryMakeBald(int accessory_id);
void AccessoryDisableJiggle(int accessory_id);

bool AccessoryCheckID(const char* ID);
size_t AddChaoAccessory(const CWE_API_ACCESSORY_DATA* pAccessoryData);
extern "C" __declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);