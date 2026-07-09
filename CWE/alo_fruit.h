#pragma once

#include "al_parameter.h"
#include <vector>

extern std::vector<ChaoItemStats> ModAPI_FruitStats;
DataArray(NJS_CNK_OBJECT*, FruitModels, 0x13340C8, 35);
DataArray(ChaoItemStats, ChaoFruitStatArray, 0x008A6448, 24);

void ALO_FruitExecutor_DisplayHack(task *eax0);
void ALO_Fruit_Init();