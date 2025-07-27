#pragma once
#include <save/save_item.h>

void AL_ClearItemSaveInfo(ItemSaveInfoBase* pSaveInfo);
void AL_ClearItemSaveInfo(ITEM_SAVE_INFO* pSaveInfo);

void* AL_GetHoldingItemSaveInfo();
void AL_ClearHoldingItemInfo();
int AL_GetHoldingItemKind();
ChaoItemCategory AL_GetHoldingItemCategory();

void AL_GardenInfo_Init();
