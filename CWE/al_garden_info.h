#pragma once
#include <stdafx.h>
#include <save/save_item.h>

DataPointer(CHAO_SAVE_INFO*, pHoldingChaoSaveInfo, 0x1A5CA5C);

void AL_ClearItemSaveInfo(ItemSaveInfoBase* pSaveInfo);
void AL_ClearItemSaveInfo(ITEM_SAVE_INFO* pSaveInfo);

ITEM_SAVE_INFO* AL_GetHoldingItemSaveInfo();
void AL_ClearHoldingItemInfo();
int AL_GetHoldingItemKind();
Sint8 AL_GetHoldingItemCategory();

void AL_GardenInfo_Init();
