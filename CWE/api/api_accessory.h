#pragma once
#include "cwe_api.h"
#include <optional>

struct CWE_API_ACCESSORY_BALD_DATA {
	NJS_POINT3 Center;
	NJS_POINT3 Influence;
	float Radius;
	bool ClipFace;
};

enum CWE_API_ACCESSORY_FLAGS {
	CWE_API_ACCESSORY_FLAGS_LEGACY_BALD = BIT_0,
	CWE_API_ACCESSORY_FLAGS_NO_JIGGLE = BIT_1,
	CWE_API_ACCESSORY_FLAGS_FREE_COLORS = BIT_2,
	CWE_API_ACCESSORY_FLAGS_FREE_BALD = BIT_3,
	CWE_API_ACCESSORY_FLAGS_BALD_KEEP_HEAD_PARTS = BIT_4,
	CWE_API_ACCESSORY_FLAGS_BALD_PRESET_X = BIT_5,
	CWE_API_ACCESSORY_FLAGS_BALD_PRESET_Y = BIT_6,
	CWE_API_ACCESSORY_FLAGS_BALD_PRESET_Z = BIT_7,
	CWE_API_ACCESSORY_FLAGS_NO_RF_NORMALDRAW_SUPPORT = BIT_8,
};

struct CWE_API_ACCESSORY_COLOR_ENTRY {
	Uint32 NodeIndex;
	Uint32 MaterialIndex;
	Uint32 ColorSlot;
};

struct CWE_API_ACCESSORY_DATA {
	char ID[METADATA_ID_SIZE];

	NJS_OBJECT* pObject;

	const char* pTextureName;
	NJS_TEXLIST* pTexlist;

	EAccessoryType SlotType;

	BlackMarketItemAttributes* pMarketAttrib;
	const char* pName;
	const char* pDescription;

	uint64_t HideNodes; // bitfield

	CWE_API_ACCESSORY_COLOR_ENTRY* pColorEntries;
	Uint32 ColorEntryCount;
	Uint32 DefaultColors[8];
	Uint32 UsedColorSlots;

	CWE_API_ACCESSORY_BALD_DATA* pBaldData;

	Uint32 Flags;
};

#define ACCESSORY_ID_LENGTH sizeof(CWE_API_ACCESSORY_DATA::ID)

void AccessorySetupDraw(const size_t index, const Uint32 colors[8], const Uint32 usedColorFlags);

bool IsAccessoryGeneric(int index);
CWE_API_ACCESSORY_DATA& GetAccessoryData(int index);
extern "C" __declspec(dllexport) EAccessoryType GetAccessoryType(int index);

const uint32_t* GetAccessoryDefaultColors(size_t index);
const size_t GetAccessoryColorCount(size_t index);
const std::optional <CWE_API_ACCESSORY_BALD_DATA>& GetAccessoryBaldData(size_t index);
extern "C" __declspec(dllexport) void AccessoryMakeBald(int accessory_id);
extern "C" __declspec(dllexport) void AccessoryDisableJiggle(int accessory_id);

bool AccessoryCheckID(const char* ID);
size_t GetAccessoryID(const char* pID);
size_t AddChaoAccessory(const CWE_API_ACCESSORY_DATA* pAccessoryData);
extern "C" __declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);