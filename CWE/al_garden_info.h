#pragma once
#include <save/save_item.h>

void AL_ClearItemSaveInfo(ItemSaveInfoBase* pSaveInfo);

int AL_GetHoldingItemKind();
ChaoItemCategory AL_GetHoldingItemCategory();
void AL_GardenInfo_Init();
