#pragma once
#include <vector>
#include <map>
#include "cwe_api.h"
#include <save/save_item.h>

bool ALO_IsAccessoryGeneric(int index);
task* Accessory_Load(const int ID, const NJS_POINT3* pPos, const int AngY, const NJS_VECTOR* pVelo, AccessorySaveInfo* savedata);