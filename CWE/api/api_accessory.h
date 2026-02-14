#pragma once

#include "cwe_api.h"
#include <optional>

extern CWE_API_REGISTER_ACCESSORY AL_ModAPI_Accessory;

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
const size_t GetAccessoryCount();
const bool IsAccessoryRFSupported(const int id);

bool AccessoryCheckID(const char* ID);
size_t GetAccessoryID(const char* pID);
size_t AddChaoAccessory(const CWE_API_ACCESSORY_DATA* pAccessoryData);
extern "C" __declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);