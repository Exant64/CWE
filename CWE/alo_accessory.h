#pragma once
#include <vector>
#include <map>
#include "cwe_api.h"

extern std::map<int, EAccessoryType> AccessoryTypeMap;
void __cdecl sub_548F40(ObjectMaster* a2);
bool ALO_IsAccessoryGeneric(int type);
ObjectMaster* Accessory_Load(int ID, NJS_VECTOR* position, int rotY, NJS_VECTOR* velocity, short* savedata);