#include "stdafx.h"
#include "api_idhash.h"
#include <cwe_api.h>
#include <al_modelcontainer.h>
#include <al_marketattr.h>
#include "api_metadata.h"
#include "api_accessory.h"
#include "api_texture.h"
#include <optional>
#include <cnk_util.h>
#include "api_util.h"

struct AccessoryInternalData {
	CWE_API_ACCESSORY_DATA Data;
	std::vector<std::pair<uint32_t*, size_t>> MaterialUpdatePointers;
	uint32_t DefaultColor[8];
	size_t ColorCount;
	std::optional <CWE_API_ACCESSORY_BALD_DATA> BaldData;
};

std::vector<AccessoryInternalData> ModAPI_AccessoryDataList;

// the idea is that this is an optimization to check for already used ids faster
static std::unordered_set<std::string> AccessoryIDLookupTable;

// i honestly don't remember why this is exported but i'll keep it exported for a good while i guess
extern "C" __declspec(dllexport) EAccessoryType GetAccessoryType(int index) {
	return ModAPI_AccessoryDataList[index].Data.SlotType;
}

bool IsAccessoryGeneric(int index) {
	const auto& type = ModAPI_AccessoryDataList[index].Data.SlotType;
	return type == EAccessoryType::Generic1 || type == EAccessoryType::Generic2;
}

const uint32_t* GetAccessoryDefaultColors(size_t index) {
	return ModAPI_AccessoryDataList[index].DefaultColor;
}

const size_t GetAccessoryColorCount(size_t index) {
	return ModAPI_AccessoryDataList[index].ColorCount;
}

const std::optional <CWE_API_ACCESSORY_BALD_DATA>& GetAccessoryBaldData(size_t index) {
	return ModAPI_AccessoryDataList[index].BaldData;
}

CWE_API_ACCESSORY_DATA& GetAccessoryData(int index) {
	return ModAPI_AccessoryDataList[index].Data;
}

const size_t GetAccessoryCount() {
	return ModAPI_AccessoryDataList.size();
}

extern "C" __declspec(dllexport) void AccessoryMakeBald(int accessory_id) {
	auto& data = GetAccessoryData(accessory_id);

	data.Flags |= CWE_API_ACCESSORY_FLAGS_LEGACY_BALD;
	data.Flags |= CWE_API_ACCESSORY_FLAGS_NO_JIGGLE;
}

extern "C" __declspec(dllexport) void AccessoryDisableJiggle(int accessory_id) {
	GetAccessoryData(accessory_id).Flags |= CWE_API_ACCESSORY_FLAGS_NO_JIGGLE;
}

bool AccessoryCheckID(const char* ID) {
	return !AccessoryIDLookupTable.contains(ID);
}

static uint32_t* AccessoryFindMaterial(NJS_CNK_MODEL* pModel, size_t materialIndex) {
	const Sint16* start = pModel->plist;
	const Sint16* plist = pModel->plist;

	size_t materialCount = 0;

	for (; ; )
	{
		const int type = CNK_GET_OFFTYPE(plist);

		if (type == NJD_CE)
		{
			/** NJD_ENDOFF **/
			break;
		}

		if (type == NJD_CN)
		{
			/** NJD_NULLOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type <= CNK_BITSOFF_MAX)
		{
			/** NJD_BITSOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type <= CNK_TINYOFF_MAX)
		{
			/** NJD_TINYOFF **/

			/** Next offset **/
			plist += 2;
			continue;
		}

		if (type <= CNK_MATOFF_MAX)
		{
			/** NJD_MATOFF **/

			if (materialCount == materialIndex) {
				return (uint32_t*)(plist + 2);
			}
			
			materialCount++;

			/** Next offset **/
			plist += ((uint16_t*)plist)[1] + 2;
			continue;
		}

		if (type <= CNK_STRIPOFF_MAX) // and volume
		{
			/** NJD_STRIPOFF **/

			/** Next offset **/
			plist += ((uint16_t*)plist)[1] + 2;
			continue;
		}

		/** An error occured, stop **/
		break;
	}

	return NULL;
}

static int IterateNodeCount = 0;
static bool AccessoryIterateObjects(AccessoryInternalData& data, size_t* lookupTable, NJS_OBJECT* pObject) {
	while (pObject) {
		for (size_t i = 0; i < data.Data.ColorEntryCount; ++i) {
			const auto& entry = data.Data.pColorEntries[i];
			if (entry.NodeIndex != IterateNodeCount) {
				continue;
			}
			
			if (!pObject->chunkmodel) {
				return false;
			}

			uint32_t* pMaterial = AccessoryFindMaterial(pObject->chunkmodel, entry.MaterialIndex);
			if (!pMaterial) {
				return false;
			}

			data.MaterialUpdatePointers.push_back(std::make_pair(pMaterial, lookupTable[entry.ColorSlot]));
		}

		IterateNodeCount++;

		if (pObject->child) {
			AccessoryIterateObjects(data, lookupTable, pObject->child);
		}

		pObject = pObject->sibling;
	}

	return true;
}

static bool AccessoryFindColors(AccessoryInternalData& data) {
	if (!data.Data.pColorEntries || !data.Data.UsedColorSlots) {
		return true;
	}
	
	data.ColorCount = 0;

	size_t lookupTable[8];

	for (size_t i = 0; i < 8; ++i) {
		lookupTable[i] = -1;

		if (!(data.Data.UsedColorSlots & (1 << i))) {
			continue;
		}

		data.DefaultColor[data.ColorCount] = data.Data.DefaultColors[i];
		lookupTable[i] = data.ColorCount;

		data.ColorCount++;
	}

	if (data.ColorCount <= 0) {
		return true;
	}

	IterateNodeCount = 0;
	return AccessoryIterateObjects(data, lookupTable, data.Data.pObject);
}

void AccessorySetupDraw(const size_t index, const Uint32 colors[8], const Uint32 usedColorFlags) {
	auto& data = ModAPI_AccessoryDataList[index];
	
	for (const auto& updatePointers : data.MaterialUpdatePointers) {
		auto* pointer = updatePointers.first;
		const auto slotIndex = updatePointers.second;

		if (usedColorFlags & (1 << slotIndex)) {
			*pointer = colors[slotIndex];
			((NJS_COLOR*)pointer)->argb.a = 255;
		}
		else {
			*pointer = data.DefaultColor[slotIndex];
		}
	}
}

size_t GetAccessoryID(const char* pID) {
	return ItemMetadata::Get()->GetIndex(ChaoItemCategory_Accessory, pID);
}

size_t AddChaoAccessory(const CWE_API_ACCESSORY_DATA* pAccessoryData) {
	APIErrorUtil error("Error in AddChaoAccessory: ");

	if (!pAccessoryData) {
		error.print("pAccessoryData is NULL!");
		return -1;
	}

	if (!pAccessoryData->ID) {
		error.print("ID is NULL!");
		return -1;
	}

	error = { "Error in AddChaoAccessory (id: %s): ", pAccessoryData->ID };

	if (AccessoryIDLookupTable.contains(pAccessoryData->ID)) {
		error.print("ID is already taken!", pAccessoryData->ID);
		return -1;
	}

	NJS_TEXLIST* pTexlist = pAccessoryData->pTexlist;
	if (!pTexlist) {
		if (pAccessoryData->pTextureName) {
			pTexlist = AddAutoTextureLoad(pAccessoryData->pTextureName);
		}
		else {
			error.print("no texture loading source specified (no texlist or texturename)!");
			return -1;
		}
	}

	AccessoryInternalData entry = {
		.Data = *pAccessoryData
	};

	entry.BaldData = pAccessoryData->pBaldData ? std::optional(*pAccessoryData->pBaldData) : std::nullopt;

	if (!AccessoryFindColors(entry)) {
		error.print("failed to parse color data! mismatched node/material entries to model?");
		return -1;
	}

	if (!pAccessoryData->pMarketAttrib) {
		error.print("no market attributes specified!");
		return -1;
	}

	if (!pAccessoryData->pObject) {
		error.print("no object specified!");
		return -1;
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

	ModAPI_AccessoryDataList.push_back(entry);

	if (pAccessoryData->Flags & CWE_API_ACCESSORY_FLAGS_FREE_BALD) {
		free(pAccessoryData->pBaldData);
	}

	if (pAccessoryData->Flags & CWE_API_ACCESSORY_FLAGS_FREE_COLORS) {
		free(pAccessoryData->pColorEntries);
	}

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
		.pDescription = description,
		.ColorEntryCount = 0
	};

	strcpy_s(accessory_data.ID, hash_id);

	return int(AddChaoAccessory(&accessory_data));
}