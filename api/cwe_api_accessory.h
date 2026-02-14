#pragma once

#include "cwe_api_common.h"

#define CWE_API_REGISTER_ACCESSORY_VER 1

enum EAccessoryType : unsigned char {
	Head = 0,
	Face,
	Generic1,
	Generic2,
	AccessoryTypeCount
};

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

struct CWE_API_REGISTER_ACCESSORY {
	int Version;

	size_t (*AddChaoAccessory)(const CWE_API_ACCESSORY_DATA* pAccessoryData);
	size_t (*GetAccessoryIndex)(const char* pID);
};
