#include "stdafx.h"
#include "api_idhash.h"
#include <cwe_api.h>
#include <al_modelcontainer.h>
#include <al_marketattr.h>
#include "api_metadata.h"
#include "api_accessory.h"
#include "api_texture.h"

std::vector<CWE_API_ACCESSORY_DATA> ModAPI_AccessoryDataList;

// the idea is that this is an optimization to check for already used ids faster
static std::unordered_set<std::string> AccessoryIDLookupTable;

// i honestly don't remember why this is exported but i'll keep it exported for a good while i guess
extern "C" __declspec(dllexport) EAccessoryType GetAccessoryType(int index) {
	return ModAPI_AccessoryDataList[index].SlotType;
}

CWE_API_ACCESSORY_DATA& GetAccessoryData(int index) {
	return ModAPI_AccessoryDataList[index];
}

__declspec(dllexport) void AccessoryMakeBald(int accessory_id) {
	auto& data = GetAccessoryData(accessory_id);

	data.Flags |= CWE_API_ACCESSORY_FLAGS_FULL_BALD;
	data.Flags |= CWE_API_ACCESSORY_FLAGS_NO_JIGGLE;
}

void AccessoryDisableJiggle(int accessory_id) {
	GetAccessoryData(accessory_id).Flags |= CWE_API_ACCESSORY_FLAGS_NO_JIGGLE;
}

bool AccessoryCheckID(const char* ID) {
	return !AccessoryIDLookupTable.contains(ID);
}

size_t AddChaoAccessory(const CWE_API_ACCESSORY_DATA* pAccessoryData) {
	// todo: errors

	if (AccessoryIDLookupTable.contains(pAccessoryData->ID)) {
		return -1;
	}

	// todo null pointer safety checks

	NJS_TEXLIST* pTexlist = pAccessoryData->pTexlist;
	if (!pTexlist && pAccessoryData->pTextureName) {
		pTexlist = AddAutoTextureLoad(pAccessoryData->pTextureName);
	}

	ItemMetadata::Get()->Add(ChaoItemCategory_Accessory, pAccessoryData->ID);

	BlackMarketAttributes::Get()->Add(
		ChaoItemCategory_Accessory,
		pAccessoryData->pMarketAttrib, 
		pAccessoryData->pName,
		pAccessoryData->pDescription
	);

	ObjectRegistry::Get(ChaoItemCategory_Accessory)->Add(pAccessoryData->pObject, pTexlist);

	const size_t id = ModAPI_AccessoryDataList.size();
	ModAPI_AccessoryDataList.push_back(*pAccessoryData);
	return id;
}

extern "C" __declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description) {
	char hash_id[ACCESSORY_ID_LENGTH];
	auto hash = GenerateHashForChunkObject(model);
	sprintf_s(hash_id, "acc%x", hash);

	CWE_API_ACCESSORY_DATA accessory_data = {
		.pObject = model,
		.pTexlist = texlist,
		.SlotType = type,
		.pMarketAttrib = attrib,
		.pName = name,
		.pDescription = description
	};

	strcpy_s(accessory_data.ID, hash_id);

	return int(AddChaoAccessory(&accessory_data));
}