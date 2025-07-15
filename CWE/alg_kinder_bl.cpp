#include "stdafx.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <array>

#include "alg_kinder_bl.h"
#include "al_minimal.h"
#include "ninja_functions.h"
#include "alo_obakehead.h"

#include "al_sandhole.h"
#include "ChaoMain.h"
#include "alo_fruit.h"
#include "AL_ModAPI.h"

#include "data/cwe/object_common_cnk/alo_mannequin.nja"
#include "data/accessory/ala_full_mannequin.nja"
#include "alo_special.h"
#include "al_save.h"
#include "al_growtree.h"
#include "alg_kinder_ortho.h"
#include "al_odekake.h"

#include "IniFile.h"
#include "al_modelcontainer.h"
#include "al_marketattr.h"

#include "ui/al_ortho.h"
#include "ALifeSDK_Functions.h"
#include "al_stage.h"
#include "al_msg_font.h"
#include "al_chao_info.h"
#include <api/api_tree.h>
#include <al_garden_info.h>

const std::array<int, MarketTabCount> MarketTabIndices =
{
	ChaoItemCategory_Egg,
	ChaoItemCategory_Fruit,
	ChaoItemCategory_Seed,
	ChaoItemCategory_Hat,
	ChaoItemCategory_Accessory,
	ChaoItemCategory_Special,
	ChaoItemCategory_MenuTheme
};
#define TabCategory MarketTabIndices[a1->currentTab]

#pragma pack(push, 8)
struct __declspec(align(4)) ALK_WINDOW_TABLE
{
	float posX;
	float posY;
	float posZ;
	unsigned int color;
	float sizeX;
	float sizeY;
	unsigned int attr;
	unsigned int work[20];
};
#pragma pack(pop)

struct ALK_BWINDOW_TABLE
{
	int bu_tanim_num;
	unsigned int bcolor;
	float bscale;
	float buttonX1;
	float buttonY1;
	float buttonX2;
	float buttonY2;
};

struct ALK_SWINDOW_TABLE
{
	unsigned int select_color;
	char* checkID;
};

std::array<std::unordered_set<int>, ChaoItemCategory_Count> AlItemRebuyable;
bool AlItemIsRebuyable(const SAlItem& pItem) {
	const auto& rebuyflags = AlItemRebuyable[pItem.mCategory];
	if (rebuyflags.contains(pItem.mType)) //check if dict has the item
		return true;

	return false;
}

void BlackMarketAddInventory(int cat, int item);
SAlItem* BM_GetInv(BlackMarketData* a1, int a2)
{
	if (a2 < 0)
		a2 = a1->mBuyListCursor;

	return &GetMarketInventory(TabCategory)[a2];
}

SAlItem* BM_GetSelectedItem(BlackMarketData* a1) {
	return &GetMarketInventory(TabCategory)[a1->mBuyListCursor];
}
int BM_GetInvSize(BlackMarketData* a1)
{
	return GetMarketInvSize(TabCategory);
}

const int ALO_Kinder_Window_LoadPtr = 0x05A92B0;
ObjectMaster* ALO_Kinder_Window_Load(ObjectMaster* result, char layer, ALK_WINDOW_TABLE* a3, int color)
{
	ObjectMaster* retval;
	__asm
	{
		mov eax, result
		push color
		push a3
		push dword ptr[layer]
		call ALO_Kinder_Window_LoadPtr
		add esp, 12
		mov retval, eax
	}
	return retval;
}

#define ITEMSINBUYLIST 5
#define DISTANCEBUYLIST 40 //46
#define SELECTIONBOXADJUST 46 - DISTANCEBUYLIST
#define EXTRAZ 4

std::vector<ItemChance> GeneralFruitMarket;
std::vector<ItemChance> RareFruitMarket;
std::unordered_map<int, int> OtherItemFuncs;

const int BlackMarketInventorySize = 32;
void GeneralFruit()
{
	for (size_t i = 0; i < GeneralFruitMarket.size(); i++)
	{
		if (GetMarketInvSize(ChaoItemCategory_Fruit) >= BlackMarketInventorySize)
			break;

		int v11 = (signed int)(njRandom() * 100.f);
		if (v11 < GeneralFruitMarket[i].chance)
		{
			BlackMarketAddInventory(ChaoItemCategory_Fruit, GeneralFruitMarket[i].item);
		}
	}

}
void RareFruit()
{
	for (size_t i = 0; i < RareFruitMarket.size(); i++)
	{
		if (GetMarketInvSize(ChaoItemCategory_Fruit) >= BlackMarketInventorySize)
			break;

		int v11 = (signed int)(njRandom() * 100.0f);
		int item = RareFruitMarket[i].item;
		if (item == SA2BFruit_Mushroom)
		{
			if (njRandom() < 0.3f)
				item = SA2BFruit_MushroomAlt;
		}
		if (EmblemCount >= CategoryAttribs[ChaoItemCategory_Fruit].attrib[item].RequiredEmblems)
		{
			if (v11 < RareFruitMarket[i].chance)
			{
				BlackMarketAddInventory(ChaoItemCategory_Fruit, item);
			}
		}
	}

}

//bitarray thing, has 100 bits, redone it with vectors
struct SSet100
{
	unsigned int flag[8];
};
struct __declspec(align(2)) SSellEgg
{
	__int16 emblem0;
	__int16 emblem1;
	unsigned __int8 id[3];
};
DataArray(SSellEgg, kSellEggList, 0x008A3378, 10);
DataArray(unsigned char, byte_8A2C7C, 0x8A2C7C, 4);
DataArray(unsigned char, byte_8A2EFC, 0x8A2EFC, 4);
DataArray(unsigned char, byte_8A2FA4, 0x8A2FA4, 4);

void BlackMarketAddInventory(int cat, int item)
{
	if (cweSaveFile.marketInventoryCount[cat] < BlackMarketInventorySize)
	{
		GetMarketInventory(cat)[cweSaveFile.marketInventoryCount[cat]].mCategory = cat;
		GetMarketInventory(cat)[cweSaveFile.marketInventoryCount[cat]].mType = item;
		cweSaveFile.marketInventoryCount[cat]++;
	}
}
void FBuyListAddSet100(std::vector<bool>& set, ChaoItemCategory category)
{
	for (size_t i = 0; i < set.size(); i++)
	{
		if (set[i])
		{
			BlackMarketAddInventory(category, i);
			set[i] = false;
		}
	}
}

SAlItem* __cdecl sub_58B120(SAlItem* result)
{
	result->mCategory = AL_GetHoldingItemCategory();
	result->mType = AL_GetHoldingItemKind();

	return result;
}
static void __declspec(naked) sub_58B120Hook()
{
	__asm
	{
		push eax // result

		// Call your __cdecl function here:
		call sub_58B120
		
		add esp, 4 // result<eax> is also used for return value
		retn
	}
}

void FBuyListGenericUpdate(ChaoItemCategory cat, float change = 0.75f) {
	for (int i = 0; i < BlackMarketCategories[cat].Count; i++)
	{
		if (EmblemCount >= BlackMarketCategories[cat].attrib[i].RequiredEmblems && njRandom() >= change)
		{
			BlackMarketAddInventory(cat, i);
		}
	}
}

void FBuyListGenericUpdateSeed(float change = 0.75f) {
	ChaoItemCategory cat = ChaoItemCategory_Seed;
	//+ 1 because the ones after vanilla seeds
	//theres a filler one starting at custom ones (to correspond with tree IDs), but its emblem count is 999 so it shouldnt ever be rolled
	for (int i = ChaoSeed_SquareSeed + 1; i < BlackMarketCategories[cat].Count; i++)
	{
		if (EmblemCount >= BlackMarketCategories[cat].attrib[i].RequiredEmblems && njRandom() >= change)
		{
			BlackMarketAddInventory(cat, i);
		}
	}
}

void FBuyListUpdate()
{
	int v3; // edi
	int v5; // ebx
	signed int v16; // edi
	int v22; // ebp
	char* v23; // esi
	signed int v26; // edi
	BlackMarketItemAttributes* v29; // ecx
	signed int v30; // esi
	signed int v31; // eax
	signed int v35; // ebx
	signed int v38; // eax
	char v54[255]; // [esp+38h] [ebp-24h]

	static std::vector<bool> set(255);

	BlackMarketItemCount = 1;
	memset(cweSaveFile.marketInventoryCount, 0, sizeof(cweSaveFile.marketInventoryCount));
	memset(cweSaveFile.marketInventory, 0, sizeof(cweSaveFile.marketInventory));

	v3 = 0;
	for (int i = 0; i < kSellEggList_Length; i++)
	{
		if (EmblemCount >= kSellEggList[i].emblem0)
		{
			int selectedEggID = kSellEggList[i].id[((*(int*)0x19F6464 + *(int*)0x19F6468) & 0x7FFFFFFF) % 3];
			if (EmblemCount >= kSellEggList[i].emblem1) //shiny emblem
			{
				if (njRandom() < 0.3)
				{
					selectedEggID += 27;
				}
			}
			v54[v3++] = selectedEggID;
		}
	}
	v5 = 1 + (signed int)(njRandom() * 5.f);
	if (v3 > v5)
	{
		v22 = 0;

		if (GetMarketInvSize(ChaoItemCategory_Egg) < BlackMarketInventorySize)
		{
			do
			{
				if (v22 >= v5)
				{
					break;
				}
				v23 = &v54[(signed int)(njRandom() * (float)v3)];
				--v3;
				++v22;
				set.at(*v23) = true;
				*v23 = (char)v54[v3];
			} while (v22 + GetMarketInvSize(ChaoItemCategory_Egg) < BlackMarketInventorySize);
		}
		FBuyListAddSet100(set, ChaoItemCategory_Egg);
	}
	else
	{
		if (v3 > 0)
		{
			for (int i = 0; i < v3; i++)
			{
				if (GetMarketInvSize(ChaoItemCategory_Egg) >= BlackMarketInventorySize)
				{
					break;
				}
				BlackMarketAddInventory(ChaoItemCategory_Egg, v54[i]);
			}
		}
	}

	GeneralFruit();
	v16 = 0;
	if (GetMarketInvSize(ChaoItemCategory_Fruit) < BlackMarketInventorySize)
	{
		do
		{
			if (v16 >= 2)
			{
				break;
			}
			int id = byte_8A2C7C[(signed int)(njRandom() * 3.f)];
			if (!set.at(id))
			{
				set.at(id) = true;
				++v16;
			}
		} while (v16 + GetMarketInvSize(ChaoItemCategory_Fruit) < BlackMarketInventorySize);
	}
	FBuyListAddSet100(set, ChaoItemCategory_Fruit);

	if (GetMarketInvSize(ChaoItemCategory_Seed) < BlackMarketInventorySize)
	{
		BlackMarketAddInventory(ChaoItemCategory_Seed, 0);
		if (GetMarketInvSize(ChaoItemCategory_Seed) < BlackMarketInventorySize)
		{
			if (njRandom() < 0.7f)
			{
				BlackMarketAddInventory(ChaoItemCategory_Seed, byte_8A2EFC[(signed int)(njRandom() * 3.f)]);
			}
		}
	}
	v26 = 0;
	if (GetMarketInvSize(ChaoItemCategory_Seed) < BlackMarketInventorySize)
	{
		do
		{
			if (v26 >= 2)
			{
				break;
			}
			int id = byte_8A2FA4[(signed int)(njRandom() * 3.f)];
			if (!set.at(id))
			{
				set.at(id) = true;
				++v26;
			}
		} while (v26 + GetMarketInvSize(ChaoItemCategory_Seed) < BlackMarketInventorySize);
	}
	FBuyListAddSet100(set, ChaoItemCategory_Seed);

	v29 = CategoryAttribs[ChaoItemCategory_Hat].attrib;
	v30 = 0;
	v31 = 0;
	do
	{
		if (v31 != SA2BHat_BlueWoolBeanie
			&& v31 != SA2BHat_BlackWoolBeanie
			&& v29->PurchasePrice > 0
			&& EmblemCount >= v29->RequiredEmblems)
			//&& (njRandom()) > 0.5
			//&& v31 < 32)
		{
			v54[v30++] = v31;
		}
		++v31;
		++v29;
	} while (v31 < CategoryAttribs[ChaoItemCategory_Hat].Count);
	if (v30 > 5)
	{
		v35 = 0;

		if (GetMarketInvSize(ChaoItemCategory_Hat) < BlackMarketInventorySize)
		{
			do
			{
				if (v35 >= 5)
				{
					break;
				}
				int id = v54[(signed int)(njRandom() * (float)v30)];
				if (id == SA2BHat_RedWoolBeanie)
				{
					v38 = (signed int)(njRandom() * 31.f);
					if (v38 >= 5)
					{
						if (v38 < 15
							&& EmblemCount >= CategoryAttribs[ChaoItemCategory_Hat].attrib[SA2BHat_BlueWoolBeanie].RequiredEmblems)
						{
							id = SA2BHat_BlueWoolBeanie;
						}
					}
					else if (EmblemCount >= CategoryAttribs[ChaoItemCategory_Hat].attrib[SA2BHat_BlackWoolBeanie].RequiredEmblems)
					{
						id = SA2BHat_BlackWoolBeanie;
					}
				}
				if (EmblemCount >= CategoryAttribs[ChaoItemCategory_Hat].attrib[id].RequiredEmblems)
				{
					if (!set.at(id))
					{
						set.at(id) = true;
						++v35;
					}
				}
			} while (v35 + GetMarketInvSize(ChaoItemCategory_Hat) < BlackMarketInventorySize);
		}
		FBuyListAddSet100(set, ChaoItemCategory_Hat);
	}
	else
	{
		if (v30 > 0)
		{
			for (int i = 0; i < v30; i++)
				if (GetMarketInvSize(ChaoItemCategory_Hat) < BlackMarketInventorySize)
				{
					BlackMarketAddInventory(ChaoItemCategory_Hat, v54[i]);
				}
		}
	}

	FBuyListGenericUpdateSeed();

	FBuyListGenericUpdate(ChaoItemCategory_Accessory);
	FBuyListGenericUpdate(ChaoItemCategory_Special);

	RareFruit();

	if (HaveBattleDLC && GetMarketInvSize(ChaoItemCategory_MenuTheme) < BlackMarketInventorySize)
	{
		if (!*(unsigned char*)0x1DEB367 && EmblemCount >= CategoryAttribs[16].attrib->RequiredEmblems)
		{
			if (njRandom() < 0.1f)
			{
				BlackMarketAddInventory(ChaoItemCategory_MenuTheme, 0);
			}
		}
		if (GetMarketInvSize(ChaoItemCategory_MenuTheme) < BlackMarketInventorySize && !*(unsigned char*)0x1DEB364 && EmblemCount >= CategoryAttribs[16].attrib[1].RequiredEmblems)
		{
			if (njRandom() < 0.1f)
			{
				BlackMarketAddInventory(ChaoItemCategory_MenuTheme, 1);
			}
		}
	}

}

FunctionPointer(int, sub_42D500, (NJS_CNK_MODEL* a1), 0x42D500);
FunctionPointer(void, njCnkMotion, (NJS_OBJECT* a1, NJS_MOTION* a2, float a3), 0x782780);

#define SELECTION_BOX_SX 257 //280 orig
#define SELECTION_BOX_SY 46  //46 orig
#define SELECTION_BOX_OX 40  //32 orig
#define SELECTION_BOX_OY 134 //125 orig

#define SELECTION_TEXT_OX (float)(SELECTION_BOX_OX + 64)
#define SELECTION_TEXT_OY (float)(SELECTION_BOX_OY + 11)

#define SELECTION_ITEM_OX (float)(SELECTION_BOX_OX + 24)
#define SELECTION_ITEM_OY (float)(SELECTION_BOX_OY + 39)

/*
void __cdecl FBuyListItemDisp(BlackMarketData* a1)
{
	DoLighting(10);
	njPushMatrixEx();

	float v45 = SELECTION_ITEM_OY;
	SAlItem* item = BM_GetInv(a1, a1->mBuyListScroll);
	int index = a1->mBuyListScroll;

	auto Rotate = [&]() {
		if (index == a1->mBuyListCursor)
			RotateY(a1->mBuyListAngY);
	};

	OrthoDrawBegin();
	for (float v45 = SELECTION_ITEM_OY; v45 < SELECTION_ITEM_OY + (ITEMSINBUYLIST * DISTANCEBUYLIST); v45 += DISTANCEBUYLIST)
	{
		if (index < BM_GetInvSize(a1) && item->mType < BlackMarketCategories[item->mCategory].Count)
		{
			njUnitMatrix(0);

			float size = 0.8f;
			switch (item->mCategory)
			{
			case ChaoItemCategory_Egg:
				OrthoScreenTranslate(SELECTION_ITEM_OX, (v45 + 2), 1.5f / 3.4189751f);
				Rotate();
				njSetTexture((NJS_TEXLIST*)0x013669FC);
				ColorEggModel((NJS_CNK_MODEL*)0x125D31C, item->mType);
				break;
			case ChaoItemCategory_Accessory:
			case ChaoItemCategory_Special:
			case ChaoItemCategory_Fruit:
			{
				auto object = ObjectRegistry::Get((ChaoItemCategory)item->mCategory)->GetObj(item->mType);

				if (item->mCategory != ChaoItemCategory_Accessory && object->chunkmodel) {
					float radius = object->chunkmodel->r;
					OrthoScreenTranslate(SELECTION_ITEM_OX, (v45 - 13), 1.5f / radius);
				}
				else
					OrthoScreenTranslate(SELECTION_ITEM_OX, (v45 - 13), size);
				Rotate();
				if (item->mCategory != ChaoItemCategory_Accessory) {
					if (!(object->evalflags & NJD_EVAL_UNIT_POS))
						njTranslate(NULL, 0, -object->pos[1], 0);
					if (object->chunkmodel)
						njTranslate(NULL, 0, -object->chunkmodel->center.y, 0);
					else if (object->child && object->child->chunkmodel) {
						float y = -object->child->chunkmodel->center.y;
						njTranslate(NULL, 0, y, 0);
					}
				}

				ObjectRegistry::DrawObject<njCnkDrawObject>((ChaoItemCategory)item->mCategory, item->mType);
				break;
			}
			case ChaoItemCategory_Seed:
				OrthoScreenTranslate(SELECTION_ITEM_OX, (v45 - 13), 2);

				Rotate();

				njSetTexture(ModAPI_SeedTexlists[item->mType]);
				sub_42D340();
				if (ModAPI_SeedModels[item->mType]->chunkmodel)
					sub_42D500(ModAPI_SeedModels[item->mType]->chunkmodel);
				break;
			case ChaoItemCategory_Hat:
				if (item->mType >= SA2BHat_NormalEggShell && item->mType < 85)
					break;

				ProjectToScreen(SELECTION_ITEM_OX, v45 - 15, -52 - EXTRAZ);

				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);
				switch (item->mType)
				{
				case 4:
				case 6:
				case 7:
				case 8:
				case 9:
					njRotateX(NULL, 0x8000);
					break;
				default:
					break;
				}

				njTranslate(NULL, 0.0f, -1.4f, 0.0f);
				
				ALO_ObakeHeadDraw(item->mType);
				break;
			case ChaoItemCategory_MenuTheme:
				ProjectToScreen(SELECTION_ITEM_OX, v45 - 14, -170 - EXTRAZ);
				njScale(1, 1, 0.001f);
				if (index == a1->mBuyListCursor)
				{
					int v40 = njSin(a1->mBuyListAngY) * 8192.0f;
					RotateY((signed int)v40);
					int v41 = njSin((((-3 * a1->mBuyListAngY >> 31) & 0xF) - 3 * a1->mBuyListAngY) >> 4) * 2048.0f;
					RotateX((signed int)v41);
				}
				njSetTexture((NJS_TEXLIST*)0x11E13A8);

				njCnkDrawObject(((NJS_OBJECT**)0x011D291C)[item->mType]);
				break;
			default:
				break;
			}
		}

		item = BM_GetInv(a1, ++index);
	}
	OrthoDrawEnd();
	njPopMatrixEx();

	DoLighting(LightIndexBackupMaybe);
}
*/
void __cdecl FBuyListItemDisp(BlackMarketData* a1)
{
	DoLighting(10);
	njPushMatrixEx();

	float v45 = SELECTION_ITEM_OY;
	SAlItem* item = BM_GetInv(a1, a1->mBuyListScroll);
	int index = a1->mBuyListScroll;

	for (float v45 = SELECTION_ITEM_OY; v45 < SELECTION_ITEM_OY + (ITEMSINBUYLIST * DISTANCEBUYLIST); v45 += DISTANCEBUYLIST)
	{
		if (index < BM_GetInvSize(a1) && item->mType < BlackMarketCategories[item->mCategory].Count)
		{
			njUnitMatrix(0);

			switch (item->mCategory)
			{
			case ChaoItemCategory_Egg:
				ProjectToScreen(SELECTION_ITEM_OX, (v45 + 2), -68.0 - EXTRAZ);
				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);
				njSetTexture((NJS_TEXLIST*)0x013669FC);
				ColorEggModel((NJS_CNK_MODEL*)0x125D31C, item->mType);
				break;
			case ChaoItemCategory_Accessory:
				ProjectToScreen(SELECTION_ITEM_OX, v45 - 15, -52 - EXTRAZ);

				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);
				njTranslate(NULL, 0.0f, -1.4f, 0.0f);
				ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Accessory, item->mType);
				break;
			case ChaoItemCategory_Special:
				ProjectToScreen(SELECTION_ITEM_OX, v45 - 15, -52 - EXTRAZ);

				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);
				njTranslate(NULL, 0.0f, -1.4f, 0.0f);
				ObjectRegistry::DrawObject(ChaoItemCategory_Special, item->mType);
				break;
			case ChaoItemCategory_Fruit:
				ProjectToScreen(SELECTION_ITEM_OX, (v45 - 13), -44.0 - EXTRAZ);
				if (item->mType == 20 || item->mType == 21)
				{
					njScale(NULL, 1, 1, 1);
				}
				else
				{
					njScale(NULL, 1, 1, 0.001f);
				}
				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);

				ObjectRegistry::DrawModel<njCnkDrawModel>(ChaoItemCategory_Fruit, item->mType);
				break;
			case ChaoItemCategory_Seed:
				ProjectToScreen(SELECTION_ITEM_OX, (v45), -22.0 - EXTRAZ);

				njScale(NULL, 1, 1, 0.001f);

				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);
				njSetTexture(ModAPI_SeedTexlists[item->mType]);
				sub_42D340();
				if (ModAPI_SeedModels[item->mType]->chunkmodel)
					sub_42D500(ModAPI_SeedModels[item->mType]->chunkmodel);
				break;
			case ChaoItemCategory_Hat:
				if (item->mType >= SA2BHat_NormalEggShell && item->mType < 85)
					break;

				if (item->mType == 15)
					ProjectToScreen(SELECTION_ITEM_OX, v45 - 15, *(float*)0xC712FC - EXTRAZ);
				else
					ProjectToScreen(SELECTION_ITEM_OX, v45 - 15, -52 - EXTRAZ);
				if (index == a1->mBuyListCursor)
					RotateY(a1->mBuyListAngY);
				switch (item->mType)
				{
				case 4:
				case 6:
				case 7:
				case 8:
				case 9:
					njRotateX(NULL, 0x8000);
					break;
				default:
					break;
				}

				if (item->mType == 15)
					njTranslate(NULL, 0.0f, -0.13f, 0.0f);
				else
					njTranslate(NULL, 0.0f, -1.4f, 0.0f);

				ALO_ObakeHeadDraw(item->mType);
				break;
			case ChaoItemCategory_MenuTheme:
				ProjectToScreen(SELECTION_ITEM_OX, v45 - 14, -170 - EXTRAZ);
				njScale(NULL, 1, 1, 0.001f);
				if (index == a1->mBuyListCursor)
				{
					RotateY((int)(njSin(a1->mBuyListAngY) * 8192.0f));
					//todo: figure this out
					int v41 = (int)(njSin((((-3 * a1->mBuyListAngY >> 31) & 0xF) - 3 * a1->mBuyListAngY) >> 4) * 2048.0f);
					RotateX(v41);
				}
				njSetTexture((NJS_TEXLIST*)0x11E13A8);

				njCnkDrawObject(((NJS_OBJECT**)0x011D291C)[item->mType]);
				break;
			default:
				break;
			}
		}
		item = BM_GetInv(a1, ++index);
	}

	njPopMatrixEx();

	DoLighting(LightIndexBackupMaybe);
}

void __cdecl FItemDescDisp(BlackMarketData* a1)
{
	DoLighting(10);
	njPushMatrixEx();

	if (_nj_current_matrix_ptr_) //njUnitMatrix(0)
	{
		memset(_nj_current_matrix_ptr_, 0, 0x30u);
		*_nj_current_matrix_ptr_ = 1.0;
		_nj_current_matrix_ptr_[5] = 1.0;
		_nj_current_matrix_ptr_[10] = 1.0;
	}
	int type = a1->mItemDescItem.mType;

	//HACK: since animals dont have descriptions this check prevents the animals from rendering, so I added a check to ignore that
	//if the category is animal
	if (a1->mItemDescItem.mCategory != 2 && type >= BlackMarketCategories[a1->mItemDescItem.mCategory].Count)
		return;

	switch (a1->mItemDescItem.mCategory)
	{
	case ChaoItemCategory_Accessory:
		ProjectToScreen(390.0f, 212.0f, -26.0f / a1->mItemDescScl);
		RotateX(a1->mItemDescAngX);
		RotateY(a1->mItemDescAngY);
		njTranslate(NULL, 0, -1.4f, 0);

		njSetTexture(&CWE_OBJECT_TEXLIST);
		if (ALO_IsAccessoryGeneric(type))
		{
			njTranslate(NULL, 0, -0.5f, 0);
			njScale(NULL, 0.978f, 0.978f, 0.978f);
			chCnkDrawObject(&object_ala_full_mannequin);
		}
		else
			chCnkDrawObject(&object_alo_mannequin);

		ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Accessory, type);
		break;
	case ChaoItemCategory_Special:
		ProjectToScreen(390, 212, -26.0f / a1->mItemDescScl);
		RotateX(a1->mItemDescAngX);
		RotateY(a1->mItemDescAngY);
		njTranslate(NULL, 0, -1.4f, 0);
		ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Special, type);
		break;
	case ChaoItemCategory_Egg:
		ProjectToScreen(390, 212, -34.0f / a1->mItemDescScl);
		RotateX(a1->mItemDescAngX);
		RotateY(a1->mItemDescAngY);
		njTranslate(NULL, 0, -2.8f, 0);
		njSetTexture((NJS_TEXLIST*)0x013669FC);
		ColorEggModel((NJS_CNK_MODEL*)0x125D31C, type);
		break;
	case 2:
		ProjectToScreen(390, 212, -26.0f / a1->mItemDescScl);
		njScale(NULL, 1, 1, *(float*)0x173B43C);
		RotateX(a1->mItemDescAngX);
		RotateY(a1->mItemDescAngY);
		if (type >= 21 && type <= 24)
		{
			njScale(NULL, 1.2f, 1.2f, 1.2f);
			njSetTexture((NJS_TEXLIST*)0x01717DAC);
			njCnkDrawObject(((NJS_OBJECT**)0x0171A294)[type - 21]);
		}
		else if (type >= 0 && type < ModAPI_MinimalModels.size()) //since the black market entry check isn't active for animals we have to safety check the animal here
		{
			njTranslate(NULL, 0, -2.1f, 0);
			njSetTexture(ModAPI_MinimalTexlists[type]);
			njCnkMotion(ModAPI_MinimalModels[type], ModAPI_MinimalMotion0[type], a1->mItemDescFrame);
		}

		break;
	case ChaoItemCategory_Fruit:
		ProjectToScreen(390, 212, -22.0f / a1->mItemDescScl);
		//njScale(1.0, 1.0, 1);
		RotateX(a1->mItemDescAngX);
		RotateY(a1->mItemDescAngY);
		njTranslate(NULL, 0, -0.4f, 0);
		ObjectRegistry::DrawModel<njCnkDrawModel>(ChaoItemCategory_Fruit, type);
		break;
	case ChaoItemCategory_Seed:
		ProjectToScreen(390, 212, -11.0f / a1->mItemDescScl);
		njScale(NULL, 1, 1, *(float*)0x173B43C);
		RotateX(a1->mItemDescAngX);
		RotateY(a1->mItemDescAngY);
		njTranslate(NULL, 0, -0.8f, 0);
		njSetTexture(ModAPI_SeedTexlists[type]);
		njCnkDrawModel(ModAPI_SeedModels[type]->chunkmodel);
		break;
	case ChaoItemCategory_Hat:
		if (type >= 16 && type <= 84)
		{
			ProjectToScreen(390, 212, -26.0f / a1->mItemDescScl);
			RotateX(a1->mItemDescAngX);
			RotateY(a1->mItemDescAngY);
			njTranslate(NULL, 0, -1.4f, 0);
			njSetTexture(&AL_BODY);
			ColorEggModel((((NJS_OBJECT**)GetDllData("MaskObjObjectList"))[16]->chunkmodel), type - 16);
		}
		else
		{
			if (type == 15)
				ProjectToScreen(390.0f, 212.0f, -13.0f / a1->mItemDescScl);
			else
				ProjectToScreen(390.0f, 212.0f, -26.0f / a1->mItemDescScl);

			RotateX(a1->mItemDescAngX);
			RotateY(a1->mItemDescAngY);
			switch (type)
			{
			case 4:
			case 6:
			case 7:
			case 8:
			case 9:
				njRotateX(NULL, 0x8000);
				break;
			default:
				break;
			}

			if (type == 15)
				njTranslate(NULL, 0, -0.15f, 0);
			else
				njTranslate(NULL, 0, -1.4f, 0);

			ALO_ObakeHeadDraw<false, false>(type);
		}
		break;
	case ChaoItemCategory_MenuTheme:
		ProjectToScreen(390, 212, -85.0f / a1->mItemDescScl);
		njScale(NULL, 1, 1, *(float*)0x173B43C);
		njSetTexture((NJS_TEXLIST*)0x11E13A8);

		njCnkDrawObject(((NJS_OBJECT**)0x011D291C)[type]);
		break;
	default:
		break;
	}

	njPopMatrixEx();

	DoLighting(LightIndexBackupMaybe);
}

BlackMarketItemAttributes* __cdecl AlItemGetInfo(SAlItem* a1)
{
	BlackMarketItemAttributes* result; // eax
	BlackMarketCategoryAttribute* v4; // edx
	
	static BlackMarketItemAttributes dummyAttrib = { 0,0,999,0,0,0 };

	result = &dummyAttrib;
	if (a1->mCategory <= ChaoItemCategory_Count)
	{
		v4 = &CategoryAttribs[a1->mCategory];
		if (v4)
		{
			if (a1->mType >= 0 && a1->mType < v4->Count)
			{
				result = &v4->attrib[a1->mType];
			}
		}
	}
	return result;
}
static void __declspec(naked) AlItemGetInfoHook()
{
	__asm
	{
		push ecx // a1

		// Call your __cdecl function here:
		call AlItemGetInfo

		pop ecx // a1
		retn
	}
}
const int GXSetBlendModeAlphaPtr = 0x0420480;
void GXSetBlendModeAlpha(int a1, int a2, int a3)
{
	__asm
	{
		push a3
		mov esi, a2
		mov edi, a1
		call GXSetBlendModeAlphaPtr
		add esp, 4
	}
}

const char* BlackMarket_GetItemMsg(BlackMarketData const* data, int const msgID)
{
	if (msgID < 0 || (size_t)msgID >= MsgAlItem.size() || !MsgAlItem[msgID])
		return "MISSING MSG";

	return (const char*)((char*)data->mItemMsgList + data->mItemMsgList[msgID]);
}

const char* BlackMarket_GetBlMsg(BlackMarketData const* data, int const msgID)
{
	return (const char*)((char*)data->mMsgList + data->mMsgList[msgID]);
}

#pragma optimize("gty", off)
void __cdecl FBuyListDispText(BlackMarketData const* a1)
{
	MessageFontThing messageBuffer;
	for (int i = 0; i < ITEMSINBUYLIST; i++)
	{
		int currItemIndex = a1->mBuyListScroll + i;
		if (currItemIndex >= cweSaveFile.marketInventoryCount[TabCategory]) break;
		
		BlackMarketItemAttributes* attrib = AlItemGetInfo(&cweSaveFile.marketInventory[TabCategory][currItemIndex]);
		if (!attrib) continue;
		if (!attrib->Name) continue;

		const char* nameStr = BlackMarket_GetItemMsg(a1, attrib->Name);
		if (!nameStr) continue;
			
		//msg color
		*(Uint32*)0x01A267D0 = 0xCECECEFF;
		if (a1->mBuyListScroll + i == a1->mBuyListCursor)
		{
			*(Uint32*)0x01A267D0 = 0xFFFF6AFF;
		}

		AlMsgFontCreateCStr(TextLanguage == 0, (int)nameStr, (int)&messageBuffer, 999);
		if (((Uint16*)&messageBuffer)[1] >= 190)
			AlMsgFontDrawRegionScale2(-1, &messageBuffer, SELECTION_TEXT_OX, SELECTION_TEXT_OY + DISTANCEBUYLIST * i, -1, 32, 0, (190.0f / ((Uint16*)&messageBuffer)[1]), 1);
		else
			AlMsgFontDrawRegionScale2(-1, &messageBuffer, SELECTION_TEXT_OX, SELECTION_TEXT_OY + DISTANCEBUYLIST * i, -1, 32, 0, 1, 1);
		AlMsgFontDelete(&messageBuffer);
	}
}

#pragma warning(push)
#pragma warning(disable: 4838)
#pragma warning(disable: 4244)
NJS_TEXANIM BuyListBasePanel[] = {
	{42 / 2,34 / 2,  0,0, 0,0, 42,             34,		12, NJD_SPRITE_ALPHA},//top left
	{20 / 2, 1,    0,0, 0,34,20,             34,		12, NJD_SPRITE_ALPHA},//middle left
	{42 / 2,34 / 2,  0,0, 0,53,42,             53 + 37,	12, NJD_SPRITE_ALPHA},//bottom left
	{46 / 2,37 / 2,  0,0, 70 + 1,55,70 + 46,       55 + 37, 12, NJD_SPRITE_ALPHA},//bottom right
	{46 / 2,1,     0,0, 70 + 1,56,70 + (46 / 2),	   56,		12, NJD_SPRITE_ALPHA},//middle right
	{22 / 2,1,     0,0, 70,17,70 + 22,       17,		12, NJD_SPRITE_ALPHA},//scroll space
	{18 / 2,57 / 2,  0,0, 132,12,132 + 18,     12 + 57,	12, NJD_SPRITE_ALPHA},//scroll wheel

	{1,20 / 2,   0,0, 41,0,41,			   20,		12, NJD_SPRITE_ALPHA},//middle top 
	{1,18 / 2,   0,0, 41,0,41,			   20,		12, NJD_SPRITE_ALPHA},//middle bottom

	//tabs (9)
	{49 / 1.5f,40 / 1.5f,   0,0, 148,212,198,			   255,		12, NJD_SPRITE_ALPHA},//L tab
	{53 / 1.5f,40 / 1.5f,   0,0, 205,212,255,			255,		12, NJD_SPRITE_ALPHA},//R tab
	{118 / 2.15f,49 / 2,   0,0, 0,154,118,			154 + 49,		12, NJD_SPRITE_ALPHA},//inactive tab
	{118 / 2.15f,49 / 2,   0,0, 0,213,118,		213 + 43,		12, NJD_SPRITE_ALPHA},//active tab

	{17 / 2, 15 / 2,  0,0,	185,162, 201,176, 12, NJD_SPRITE_ALPHA},//top right corner sprite
	{17 / 2, 15 / 2,  0,0,	185,178, 201,192, 12, NJD_SPRITE_ALPHA},//bottom right corner sprite

	{118 / 2.15f,49 / 2,   0,0, 0,99,118,		99 + 46,		12, NJD_SPRITE_ALPHA},//not available tab

	{118 / 2,49 / 2,   0,0, 0,0,256,			256,		12, NJD_SPRITE_ALPHA},//tab icon

	{22 / 2,2,     0,0, 70,10,70 + 22,       11,		12, NJD_SPRITE_ALPHA},//scroll space top
	{22 / 2,2,     0,0, 70,29,70 + 22,       30,		12, NJD_SPRITE_ALPHA},//scroll space bottom
};
NJS_SPRITE BuyListPanel = { {},1,1,0,&CWE_UI_TEXLIST, (NJS_TEXANIM*)BuyListBasePanel };

//ChaoHudThing kWinQuad_BuyListLines = { 0, SELECTION_BOX_SX, 3,11, (121 / 256.0f) * 4096.0f,(127 / 256.0f) * 4096.0f,(192 / 256.0f) * 4096.0f,	(192 / 256.0f) * 4096.0f };
const ChaoHudThing kWinQuad_BuyListLines = { 0, SELECTION_BOX_SX, 3,11, (120 / 256.0f) * 4096.0f,(120 / 256.0f) * 4096.0f,(197 / 256.0f) * 4096.0f,	(197 / 256.0f) * 4096.0f };
const ChaoHudThing kWinQuad_BuyListCursor_0_0 = { 4, SELECTION_BOX_SX - 4 ,  3, 11 ,  2632, 2632 ,  1032, 1144 };
const ChaoHudThing kWinQuad_BuyListCursor_0_1 = { 4, SELECTION_BOX_SX - 4 ,  3, 11 ,  2760, 2760 ,  1032, 1144 };

const ChaoHudThing stru_8A33E8[6] =
{
  {  0,   16 ,								   0, SELECTION_BOX_SY / 2,					2568, 2808 ,  1160, 1512  }, //top half left
  {  16,  SELECTION_BOX_SX - 16,			   0, 4 ,									2824, 2824 ,  1160, 1208  }, //top half middle
  {  SELECTION_BOX_SX - 16, SELECTION_BOX_SX,  0, SELECTION_BOX_SY / 2 ,				2952, 3192 ,  1160, 1512  }, //top half right
  {  0,   16 ,								   SELECTION_BOX_SY / 2, SELECTION_BOX_SY , 2568, 2808 ,  1432, 1784  }, //bottom half left
  {  16, SELECTION_BOX_SX - 16 ,			   SELECTION_BOX_SY - 4, SELECTION_BOX_SY , 2824, 2824 ,  1736, 1784  }, //bottom half middle
  {  SELECTION_BOX_SX - 16, SELECTION_BOX_SX , SELECTION_BOX_SY / 2, SELECTION_BOX_SY , 2952, 3192 ,  1432, 1784  }  //bottom half right
};

#pragma warning(pop)

void DrawTimer();
VoidFunc(sub_42C170, 0x42C170);
void __cdecl FBuyListDisp(BlackMarketData* a1)
{
	ChaoHudThing const* buyListCursor;

	if (!a1->mBuyListShow) {
		if (!a1->mSellListShow) {
			DrawTimer();
			int timer = (*(int*)0x1DBED8C - *(int*)0x0174B038);
			if (timer <= 0 && a1->mMode >= 5 && a1->mMode < 14)
				a1->mMode = 14;
		}
		return;
	}
	
	njSetTexture((NJS_TEXLIST*)0x011D2ACC);
	njSetTextureNum(0, 0, 0, 0);
	GXSetBlendModeAlpha(5, 4, 1);
	sub_583C60();

	buyListCursor = &kWinQuad_BuyListCursor_0_0;
	if (a1->mBuyListMode)
	{
		//is buying/selected item
		buyListCursor = &kWinQuad_BuyListCursor_0_1;
	}

	AlgKinderOrthoQuadDrawArrayTileOffset(&kWinQuad_BuyListLines, 1, 0x808080FF, 1, 26, SELECTION_BOX_OX, SELECTION_BOX_OY - 3);

	const int selectBarY = DISTANCEBUYLIST * (a1->mBuyListCursor - a1->mBuyListScroll) + SELECTION_BOX_OY;
	AlgKinderOrthoQuadDrawArrayTileOffset(buyListCursor, 1, 0xFFFFFF80, 1, 5, SELECTION_BOX_OX, selectBarY);
	AlgKinderOrthoQuadDrawArrayOffset(stru_8A33E8, 6, a1->mBuyListMode != 0 ? 0xC5FF31FF : 0xFFCC33FF, SELECTION_BOX_OX, selectBarY);

	const float basePosX = 30;
	const float basePosY = 125;
	const float panelSizeX = 268;
	const float panelSizeY = 190;

	//top left
	BuyListPanel.p = { basePosX, basePosY };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 0, -1, NJD_SPRITE_ALPHA);

	//middle left
	BuyListPanel.p = { basePosX, basePosY + BuyListBasePanel[0].sy };
	BuyListPanel.sy = panelSizeY;
	njDrawSprite2D(&BuyListPanel, 1, -1, NJD_SPRITE_ALPHA);

	//bottom left
	BuyListPanel.p = { basePosX, basePosY + BuyListBasePanel[0].sy + panelSizeY };
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 2, -1, NJD_SPRITE_ALPHA);

	//bottom right
	BuyListPanel.p = { basePosX + panelSizeX, basePosY + BuyListBasePanel[0].sy + panelSizeY - 1 };
	njDrawSprite2D(&BuyListPanel, 3, -1, NJD_SPRITE_ALPHA);

	//bottom right corner
	BuyListPanel.p = { basePosX + panelSizeX - 8, basePosY + BuyListBasePanel[0].sy + panelSizeY + 1 };
	njDrawSprite2D(&BuyListPanel, 14, -1, NJD_SPRITE_ALPHA);

	//top right corner
	BuyListPanel.p = { basePosX + panelSizeX - 8, basePosY + BuyListBasePanel[0].sy - 7.75f };
	njDrawSprite2D(&BuyListPanel, 13, -1, NJD_SPRITE_ALPHA);

	//middle top
	BuyListPanel.p = { basePosX + BuyListBasePanel[0].sx - 1, basePosY };
	BuyListPanel.sx = panelSizeX + 3;
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 7, -1, NJD_SPRITE_ALPHA);

	//middle right
	BuyListPanel.p = { basePosX + panelSizeX, basePosY + BuyListBasePanel[7].sy - 6 };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = panelSizeY + BuyListBasePanel[7].sy + 3;
	njDrawSprite2D(&BuyListPanel, 4, -1, NJD_SPRITE_ALPHA);

	//middle bottom
	BuyListPanel.p = { basePosX + BuyListBasePanel[2].sx - 1, basePosY + BuyListBasePanel[0].sy + panelSizeY + (197 - 179 - 2) / 2 };
	BuyListPanel.sx = panelSizeX - BuyListBasePanel[3].sx + 3;
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 8, -1, NJD_SPRITE_ALPHA);

	//scroll space
	BuyListPanel.p = { basePosX + panelSizeX + 5, basePosY + BuyListBasePanel[7].sy - 6 + 5.25f + 0.25f };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = panelSizeY - 0.25f;
	njDrawSprite2D(&BuyListPanel, 5, -1, NJD_SPRITE_ALPHA);

	//scroll wheel
	BuyListPanel.p = { basePosX + panelSizeX + 6, basePosY + BuyListBasePanel[7].sy + ((float)a1->mBuyListCursor / (cweSaveFile.marketInventoryCount[TabCategory] - 1)) * (panelSizeY - BuyListBasePanel[6].sy) };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 6, -1, NJD_SPRITE_ALPHA);


	//L tab
	BuyListPanel.p = { basePosX - 0.45f + 0.09f + 0.3f, basePosY - BuyListBasePanel[9].sy + 2 };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 9, -1, NJD_SPRITE_ALPHA);

	//R tab
	BuyListPanel.p = { basePosX + panelSizeX - (BuyListBasePanel[10].sx / 2.5f) + 1.7f + 0.09f + 0.45f, basePosY - BuyListBasePanel[9].sy + 2.5f };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = 1;
	njDrawSprite2D(&BuyListPanel, 10, -1, NJD_SPRITE_ALPHA);

	BuyListPanel.p = { basePosX + BuyListBasePanel[9].sx + 4.5f, basePosY - BuyListBasePanel[9].sy + 4.5f };
	BuyListPanel.sx = 1;
	BuyListPanel.sy = 1;
	//tabs
	for (int i = 0; i < 4; i++)
	{
		BuyListPanel.p.x = basePosX + BuyListBasePanel[9].sx + 4.5f + (BuyListBasePanel[11].sx * i);
		if (i + (a1->currentTab / 4) * 4 >= MarketTabCount)
			break;

		if (cweSaveFile.marketInventoryCount[MarketTabIndices[i + (a1->currentTab / 4) * 4]] == 0) 
			njDrawSprite2D(&BuyListPanel, 15, -1, NJD_SPRITE_ALPHA); //empty tab
		else if (i == (a1->currentTab % 4))
			njDrawSprite2D(&BuyListPanel, 12, -1, NJD_SPRITE_ALPHA); //selected
		else
			njDrawSprite2D(&BuyListPanel, 11, -1, NJD_SPRITE_ALPHA); //not selected

		BuyListBasePanel[16].texid = 13 + (i + (a1->currentTab / 4) * 4);
		BuyListPanel.p.x -= 2;
		njDrawSprite2D(&BuyListPanel, 16, -1, NJD_SPRITE_ALPHA);
	}
	FBuyListDispText(a1);
	FBuyListItemDisp(a1);
}
#pragma optimize("gty", on)
const int JumpBackHere_ = 0x005897C3;
void __declspec(naked) FItemDescDispHook()
{
	_asm
	{
		push[esp + 0x2C]
		call FItemDescDisp
		add esp, 4
		jmp JumpBackHere_
	}
}

//garden object count
signed int __cdecl AL_GetMaxItemNum(const int a1)
{
	if (a1 == ChaoItemCategory_Fruit)
	{
		return 40;
	}
	if (a1 == ChaoItemCategory_Seed)
	{
		return 12;
	}
	if (a1 == ChaoItemCategory_Hat || a1 == ChaoItemCategory_Accessory)
	{
		return 24;
	}
	if (a1 == ChaoItemCategory_Special)
	{
		return 30;
	}
	return 0;
}
int edxBackup;
static void __declspec(naked) AL_GetMaxItemNum_hook()
{
	__asm
	{
		mov edxBackup, edx
		push eax // a1

		// Call your __cdecl function here:
		call AL_GetMaxItemNum

		add esp, 4 // a1<eax> is also used for return value
		mov edx, edxBackup
		retn
	}
}

const int sub_52F4F0Ptr = 0x52F4F0;
int sub_52F4F0(int a1)
{
	int retval;
	__asm
	{
		mov ecx, a1
		call sub_52F4F0Ptr
		mov retval, eax
	}
	return retval;
}

int __cdecl AL_GetExistItemNum(const int a1)
{
	return 0;
	if (a1 == ChaoItemCategory_Special)
	{
		int ret = 0;
		for (int i = 0; i < 30; i++)
			if (cweSaveFile.specialItems[i].Type != -1) ret++;
		return ret;
	}
	else
		return sub_52F4F0(a1);
}

const int sub_5A64B0Ptr = 0x5A64B0;
void sub_5A64B0(signed int a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		call sub_5A64B0Ptr
	}
}
const int sub_5A6450Ptr = 0x5A6450;
void sub_5A6450(int a1, float a2)
{
	__asm
	{
		push a2
		mov eax, a1
		call sub_5A6450Ptr
		add esp, 4
	}
}
#pragma pack(push, 8)
struct __declspec(align(4)) SomeUI
{
	float left;
	float top;
	float right;
	float bottom;
	float u1;
	float v1;
	float u2;
	float v2;
};
#pragma pack(pop)

void DrawTimer()
{
	sub_781CB0(1);
	float uibuff[10];
	njSetTexture((NJS_TEXLIST*)&CWE_UI_TEXLIST);
	//sub_781CB0(1);
	*(int*)0x1A54FF0 = 0xFFFFFFFF;

	const float posX = 50;
	const float posY = 115;
	njSetTextureNum(20, 0, 0, 0);
	SomeUI* a2 = (SomeUI*)uibuff;
	a2->left = posX;
	a2->right = posX + 32;
	a2->top = posY;
	a2->bottom = posY + 40;
	a2->u1 = 0;
	a2->v1 = 0;
	a2->u2 = 32 / 56.0f;
	a2->v2 = 1;
	sub_5A6450((int)a2, 0.99f);
	a2->left = posX + 32;
	a2->right = posX + 150;
	a2->u1 = 34 / 56.0f;
	a2->u2 = 35 / 56.0f;
	sub_5A6450((int)a2, 0.99f);
	a2->left = posX + 150;
	a2->right = posX + 150 + 16;
	a2->u1 = 43 / 56.0f;
	a2->u2 = 1;
	sub_5A6450((int)a2, 0.99f);
	njSetTexture((NJS_TEXLIST*)0x01366ABC);
	njSetTextureNum(1, 0, 0, 0);

	int timerDiff = *(int*)0x1DBED8C - *(int*)0x0174B038;
	if (timerDiff < 0)
		timerDiff = 0;
	const int minutes = timerDiff / 60 / 60;
	const int seconds = (timerDiff / 60) % 60;
	uibuff[0] = posX + 32;
	uibuff[2] = posX + 32 + 22;
	uibuff[1] = posY + 10;
	uibuff[3] = posY + 10 + 22;

	sub_5A64B0(minutes / 10, (int)uibuff);
	sub_5A6450((int)uibuff, 1);
	uibuff[0] += 25;
	uibuff[2] += 25;
	sub_5A64B0(minutes % 10, (int)uibuff);
	sub_5A6450((int)uibuff, 1);

	uibuff[0] += 25;
	uibuff[2] += 25;
	sub_5A64B0(10, (int)uibuff);
	sub_5A6450((int)uibuff, 1);

	uibuff[0] += 25;
	uibuff[2] += 25;
	sub_5A64B0(seconds / 10, (int)uibuff);
	sub_5A6450((int)uibuff, 1);
	uibuff[0] += 25;
	uibuff[2] += 25;
	sub_5A64B0(seconds % 10, (int)uibuff);
	sub_5A6450((int)uibuff, 1);
}

Light BM_MenuLight = { {  0.1f, -0.7f, -0.7f },  1,  0.5f, {  1,  1,  1 } };

#define Translate(x,y,z) OrthoScreenTranslate(x,y,(-26.0f)/z * scl)
extern "C" __declspec(dllexport) void DrawItem(const float x, const float y, const float scl, const Rotation& rot, const SAlItem& mItemDescItem) {
	njPushMatrixEx();
	njUnitMatrix(0);

	Light backupLight = Lights[10];
	Lights[10] = BM_MenuLight;
	DoLighting(10);

	OrthoDrawBegin();
	int type = mItemDescItem.mType;
	if (type < BlackMarketCategories[mItemDescItem.mCategory].Count) {
		switch (mItemDescItem.mCategory)
		{
		case ChaoItemCategory_Accessory:
			Translate(x, y, -26.0f);
			RotateX(rot.x);
			RotateY(rot.y);
			njTranslate(NULL, 0, -1.4f, 0);

			njSetTexture(&CWE_OBJECT_TEXLIST);
			if (ALO_IsAccessoryGeneric(type))
			{
				njTranslate(NULL, 0, -0.5f, 0);
				njScale(NULL, 0.978f, 0.978f, 0.978f);
				chCnkDrawObject(&object_ala_full_mannequin);
			}
			else
				chCnkDrawObject(&object_alo_mannequin);

			ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Accessory, type);
			break;
		case ChaoItemCategory_Special:
			Translate(x, y, -26.0f);
			RotateX(rot.x);
			RotateY(rot.y);
			njTranslate(NULL, 0, -1.4f, 0);
			ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Special, type);
			break;
		case ChaoItemCategory_Egg:
			Translate(x, y, -34.0f);
			RotateX(rot.x);
			RotateY(rot.y);
			njTranslate(NULL, 0, -2.8f, 0);
			njSetTexture((NJS_TEXLIST*)0x013669FC);
			ColorEggModel((NJS_CNK_MODEL*)0x125D31C, type);
			break;
		case 2:
			Translate(x, y, -26.0f);
			njScale(NULL, 1, 1, *(float*)0x173B43C);
			RotateX(rot.x);
			RotateY(rot.y);
			if (type >= 21 && type <= 24)
			{
				njScale(NULL, 1.2f, 1.2f, 1.2f);
				njSetTexture((NJS_TEXLIST*)0x01717DAC);
				njCnkDrawObject(((NJS_OBJECT**)0x0171A294)[type - 21]);
			}
			else
			{
				njTranslate(NULL, 0, -2.1f, 0);

				//check here to make size_t cast "safe"
				if (type >= 0) {
					if ((size_t)type < ModAPI_MinimalTexlists.size())
						njSetTexture(ModAPI_MinimalTexlists[type]);

					NJS_OBJECT* obj = ModAPI_MinimalModels[type];
					NJS_MOTION* mot = ModAPI_MinimalMotion0[type];
					if ((size_t)type < ModAPI_MinimalModels.size() && obj && mot)
						njCnkMotion(obj, mot, 0);
				}
			}

			break;
		case ChaoItemCategory_Fruit:
			Translate(x, y, -22.0f);
			//njScale(1.0, 1.0, 1);
			RotateX(rot.x);
			RotateY(rot.y);
			njTranslate(NULL, 0, -0.4f, 0);
			ObjectRegistry::DrawModel<njCnkDrawModel>(ChaoItemCategory_Fruit, type);
			break;
		case ChaoItemCategory_Seed:
			Translate(x, y, -11.0f);
			njScale(NULL, 1, 1, *(float*)0x173B43C);
			RotateX(rot.x);
			RotateY(rot.y);
			njTranslate(NULL, 0, -0.8f, 0);
			njSetTexture(ModAPI_SeedTexlists[type]);
			njCnkDrawModel(ModAPI_SeedModels[type]->chunkmodel);
			break;
		case ChaoItemCategory_Hat:
			if (type >= 16 && type <= 84)
			{
				Translate(x, y, -26.0f);
				RotateX(rot.x);
				RotateY(rot.y);
				njTranslate(NULL, 0, -1.4f, 0);
				njSetTexture(&AL_BODY);
				ColorEggModel((((NJS_OBJECT**)GetDllData("MaskObjObjectList"))[16]->chunkmodel), type - 16);
			}
			else
			{
				if (type == 15)
					Translate(x, y, -13.0f);
				else
					Translate(x, y, -26.0f);

				RotateX(rot.x);
				RotateY(rot.y);
				switch (type)
				{
				case 4:
				case 6:
				case 7:
				case 8:
				case 9:
					njRotateX(NULL, 0x8000);
					break;
				default:
					break;
				}

				if (type == 15)
					njTranslate(NULL, 0, -0.15f, 0);
				else
					njTranslate(NULL, 0, -1.4f, 0);

				ALO_ObakeHeadDraw<false, false>(type);
			}
			break;
		case ChaoItemCategory_MenuTheme:
			Translate(x, y, -85.0f);
			njScale(NULL, 1, 1, *(float*)0x173B43C);
			njSetTexture((NJS_TEXLIST*)0x11E13A8);

			njCnkDrawObject(((NJS_OBJECT**)0x011D291C)[type]);
			break;
		default:
			break;
		}
	}
	OrthoDrawEnd();
	njPopMatrixEx();
	Lights[10] = backupLight;
}

void DrawPurchasedItem() {
	if (cweSaveFile.purchasedItemCount <= 0) return;

	NJS_QUAD_TEXTURE v9;
	int v2 = 0;
	switch (AL_GetStageNumber())
	{
	case CHAO_STG_NEUT:
	case CHAO_STG_HERO:
	case CHAO_STG_DARK:
	case CHAO_STG_KINDER:
	case CHAO_STG_LOBBY:
		//old display
		if (!gConfigVal.NewInventory) {
			v9.u1 = 0.0;
			v9.u2 = 1.0;
			v9.v2 = 1.0;
			v9.v1 = 0.0;
			njSetTexture(&AL_ITEM_TEXLIST);
			sub_781CB0(1);
			for (int i = 0; i < cweSaveFile.purchasedItemCount; i++)
			{
				*(int*)0x1A54FF0 = 0xFFFFFFFF;

				if (save::CWE_PurchasedItems[i].mCategory < 0)
					njSetTextureNum(0, 0, 0, 0);
				else
					njSetTextureNum(save::CWE_PurchasedItems[i].mCategory, 0, 0, 0);

				float v4 = i * *(float*)0x1A13BE0 + *(float*)0x1A13BE8;
				v9.x1 = v4 - *(float*)0x1A13BE4;
				v9.x2 = v4 + *(float*)0x1A13BE4;
				float v5 = i * *(float*)0x1A13BF0 + *(float*)0x1A13BF4;
				v9.y1 = 480.0f - (*(float*)0x1A13BEC + v5);
				v9.y2 = 480.0f - (v5 - *(float*)0x1A13BEC);
				float v6 = -1.0f / *(float*)0x1A13BD8;
				DrawQuadTexture((int)&v9, v6);
			}
			return;
		}

		const float size = 32;
		const ChaoHudThingB hud = { 1,size,size,0,0,1,1,&CWE_UI_TEXLIST, 27 };

		for (int i = 0; i < cweSaveFile.purchasedItemCount; i++) {
			SetShaders(1);
			DoLighting(0);
			Rotation rot = { 0,0,0 };
			DrawItem(
				i * *(float*)0x1A13BE0 + *(float*)0x1A13BE8,
				i * *(float*)0x1A13BF0 + *(float*)0x1A13BF4,
				0.75f,
				rot,
				save::CWE_PurchasedItems[i]
			);
		}
			
			
		for (int i = 0; i < cweSaveFile.purchasedItemCount; i++)
		{
			float opacity = 0.5f;
			SetChaoHUDThingBColor(opacity, 1, 1, 1);
			DrawChaoHudThingB(
				(ChaoHudThingB*) & hud,
				i * *(float*)0x1A13BE0 + *(float*)0x1A13BE8,
				i * *(float*)0x1A13BF0 + *(float*)0x1A13BF4,
				-1,
				2, 2,
				0, 0
			);
			SetChaoHUDThingBColor(1, 1, 1, 1);
		}				
		break;
	}
}

void __cdecl FixFontScale(__int16* a1, float a2, float a3, float a4)
{
	//351 - longest
	//270 just fits
	if (a1[1] >= 270)
	{
		AlMsgFontDrawRegionScale2(-1, (MessageFontThing*)a1, a2, a3, a4, 32.0, 0.0, 270.0f / (float)a1[1], 1.0);
	}
	else
	{
		DrawFontThing(a1, a2, a3, a4);
	}
}
static void __declspec(naked) FixFontScaleHook()
{
	__asm
	{
		push[esp + 0Ch] // a4
		push[esp + 0Ch] // a3
		push[esp + 0Ch] // a2
		push ebx // a1

		// Call your __cdecl function here:
		call FixFontScale

		pop ebx // a1
		add esp, 4 // a2
		add esp, 4 // a3
		add esp, 4 // a4
		retn
	}
}

const char* BlackMarketGetNameMsg(BlackMarketData const* a1, BlackMarketItemAttributes const* a2) {
	if (!a2) return "GetNameMsg ITEM NULL";
	if (a2->Name < 0) return "GetNameMsg Name invalid";

	return BlackMarket_GetItemMsg(a1, a2->Name);
}

const char* BlackMarketGetDescMsg(BlackMarketData const* a1, BlackMarketItemAttributes const* a2) {
	if (!a2) return "GetDescMsg ITEM NULL";
	if (a2->Descriptions < 0) "GetDescMsg Desc invalid";

	return BlackMarket_GetItemMsg(a1, a2->Descriptions);
}

void __cdecl FItemDescSet(SAlItem* a1, BlackMarketData* a2) {	
	a2->mItemDescItem = *a1;
	a2->mItemDescInfo = AlItemGetInfo(a1);
	a2->mItemDescScl = 1;
	a2->mItemDescAngX = 0;
	a2->mItemDescAngY = 0;
	a2->mItemDescFrame = 0;

	if (a2->mItemDescWinExplOn) {
		AlMsgWinDelete(&a2->mItemDescWinExpl);
		a2->mItemDescWinExplOn = 0;
	}
	
	if (BlackMarketGetDescMsg(a2, a2->mItemDescInfo)) {	
		KinderCoMessageThing& msg = a2->mItemDescWinExpl;
		AlMsgWinCreate(&msg);
		AlMsgWinSetPos(&msg, 352, 274, -1);
		AlMsgWinSetSize(&msg, 240, 50);
		AlMsgWinOpen(&msg);
		*(Uint32*)0x01A267D0 = 255;
		AlMsgWinAddLineC(&msg, BlackMarketGetDescMsg(a2, a2->mItemDescInfo), TextLanguage == 0);
		a2->mItemDescWinExplOn = 1;
	}
}
static void __declspec(naked) FItemDescSetHook()
{
	__asm
	{
		push edi // a2
		push eax // a1

		// Call your __cdecl function here:
		call FItemDescSet

		pop eax // a1
		pop edi // a2
		retn
	}
}

void __cdecl FSellListStart(BlackMarketData* a1)
{
	SAlItem item; // [esp+10h] [ebp-C4h] BYREF
	ALK_SWINDOW_TABLE swindow; // [esp+14h] [ebp-C0h] BYREF
	AL_KinderPMessage v19; // [esp+1Ch] [ebp-B8h] BYREF
	int v20[33]; // [esp+48h] [ebp-8Ch] BYREF

	a1->mItemDescShow = 1;
	a1->mItemDescWinExplOn = 0;
	a1->mItemDescSell = 1;
	a1->mSellListShow = 1;

	sub_58B120(&item);
	FItemDescSet(&item, a1);

	swindow.select_color = 0xFFEE6400;
	swindow.checkID = &a1->mSellListSelect;
	a1->mSellListSelect = -1;
	a1->SellKinder[0] = ALO_Kinder_Window_Load(a1->mTask, 0, (ALK_WINDOW_TABLE*)0x8A3558, (int)&swindow);
	a1->SellKinder[1] = ALO_Kinder_Window_Load(a1->mTask, 1, (ALK_WINDOW_TABLE*)0x8A35C4, (int)&swindow);

	v19.field_20 = 1;
	v19.fontSize = 115;
	v19.posX = 525;
	v19.posZ = -4.7f;
	v19.posY = 383;
	v19.a = 0;
	v19.flags = 15;
	v19.color = 0xFF000000;
	v19.field_8 = -1;
	v19.msgID = 0;
	v19.field_10 = (int)&a1->mSellListSelect;
	v19.field_2 = 0;
	v19.string = BlackMarket_GetBlMsg(a1, 9);
	a1->SellKinder[2] = AL_KinderPMessageExec_Load(a1->mTask, &v19);

	v19.posY = 416;
	v19.field_2 = 1;
	v19.string = BlackMarket_GetBlMsg(a1, 10);
	a1->SellKinder[3] = AL_KinderPMessageExec_Load(a1->mTask, &v19);

	*(Uint32*)0x01A267D0 = -1;
	AlMsgWinAddLineC(a1->mMainWin, BlackMarket_GetBlMsg(a1, 26), TextLanguage == 0);

	sprintf((char*)v20, BlackMarket_GetBlMsg(a1, 24), a1->mItemDescInfo->SalePrice);
	*(Uint32*)0x01A267D0 = -1;
	AlMsgWinAddLineC(a1->mMainWin, (const char*)v20, TextLanguage == 0);
}
int _ebxBackup;
static void __declspec(naked) FSellListStartHook()
{
	__asm
	{
		push ecx // a1

		// Call your __cdecl function here:
		call FSellListStart

		pop ecx // a1
		retn
	}
}

void FBuyListSetItemDesc(BlackMarketData* a1)
{
	FItemDescSet(BM_GetSelectedItem(a1), a1);
}

const int ALO_RingWinAdd_Ptr = 0x005A6B30;
void ALO_RingWinAdd_(int a1)
{
	__asm
	{
		mov eax, a1
		call ALO_RingWinAdd_Ptr
	}
}

DataPointer(int, RingDisplayFieldC, 0x01A2787C);
//normally RingWinAdd adds it to num inside it, but for some reason sa2 optimized it to be outside
void ALO_RingWinAdd(int num) {
	ALO_RingWinAdd_(RingDisplayFieldC + num);
}

const int FMainWinWaitClosePtr = 0x588C00;
void FMainWinWaitClose(BlackMarketData* a1)
{
	__asm
	{
		mov eax, a1
		call FMainWinWaitClosePtr
	}
}

const int FMainWinAddLineStrPtr = 0x588BA0;
void FMainWinAddLineStr(BlackMarketData* a1, char* a2)
{
	__asm
	{
		mov ecx, a1
		mov ebx, a2
		call FMainWinAddLineStrPtr
	}
}

FastcallFunctionPointer(void, FMainWinAddLineId, (BlackMarketData* a1, int a2), 0x588BD0);
void __cdecl FBuyListExec(BlackMarketData* a1)
{
	char buff[MAX_PATH];
	
	if (!a1->mBuyListShow) return;
	
	a1->mBuyListAngY += 256;

	if (ControllerPointers[0]->on & Buttons_Y) {
		return;
	}
		
	auto IsTabNotEmpty = [](int i) {
		return cweSaveFile.marketInventoryCount[MarketTabIndices.at(i)] != 0;
	};

	auto SetTabIfItHasItem = [&](int i) {
		if (IsTabNotEmpty(i)) {
			PlaySoundProbably(0x8000, 0, 0, 0);
			a1->currentTab = i;
			a1->mBuyListCursor = 0;
			a1->mBuyListScroll = 0;
			FBuyListSetItemDesc(a1);
			return true;
		}

		return false;
	};

	switch (a1->mBuyListMode) {
	case 0:
		if (MenuButtons_Pressed[0] & Buttons_L) {
			for (int i = a1->currentTab - 1; i >= 0; i--) {
				if (SetTabIfItHasItem(i))
					break;
			}
		}

		if (MenuButtons_Pressed[0] & Buttons_R) {
			for (int i = a1->currentTab + 1; i < MarketTabCount; i++) {
				if (SetTabIfItHasItem(i))
					break;
			}
		}

		if (MenuButtons_Pressed[0] & Buttons_Up)
		{
			if (a1->mBuyListCursor > 0)
			{
				a1->mBuyListCursor--;
				a1->mBuyListAngY = 0;
				if (a1->mBuyListScroll > a1->mBuyListCursor)
				{
					a1->mBuyListScroll = a1->mBuyListCursor;
				}
				FBuyListSetItemDesc(a1);
				PlaySoundProbably(0x8000, 0, 0, 0);
			}
		}
		if (MenuButtons_Pressed[0] & Buttons_Down)
		{
			if (a1->mBuyListCursor < cweSaveFile.marketInventoryCount[TabCategory] - 1) {
				a1->mBuyListCursor++;
				int v6 = a1->mBuyListCursor - (ITEMSINBUYLIST - 1);
				a1->mBuyListAngY = 0;
				if (a1->mBuyListScroll < v6) {
					a1->mBuyListScroll = v6;
				}
				FBuyListSetItemDesc(a1);
				PlaySoundProbably(0x8000, 0, 0, 0);
			}
		}

		if (ControllerPointers[0]->press & Buttons_A)
		{
			if (TotalRings >= a1->mItemDescInfo->PurchasePrice && a1->mItemDescItem.mType < BlackMarketCategories[a1->mItemDescItem.mCategory].Count)
			{
				ALK_SWINDOW_TABLE select;
				AL_KinderPMessage a2;

				select.select_color = 0xFFEE6400;
				select.checkID = &a1->mBuyListSelect;

				a1->mBuyListMode = 1;
				a1->mBuyListSelect = -1;
				a1->BuylistKinder[0] = ALO_Kinder_Window_Load(a1->mTask, 0, (ALK_WINDOW_TABLE*)0x8A3468, (int)&select);
				a1->BuylistKinder[1] = ALO_Kinder_Window_Load(a1->mTask, 1, ((ALK_WINDOW_TABLE*)0x8A3468) + 1, (int)&select);

				*(Uint32*)0x01A267D0 = 0xFFFFFFFF;

				sprintf(buff, BlackMarket_GetBlMsg(a1, 17), BlackMarketGetNameMsg(a1, a1->mItemDescInfo));
				FMainWinAddLineStr(a1, buff);
				sprintf(buff, BlackMarket_GetBlMsg(a1, 16), a1->mItemDescInfo->PurchasePrice);
				FMainWinAddLineStr(a1, buff);
				a2.field_20 = 1;
				a2.fontSize = 115;
				a2.posX = 525;
				a2.posZ = -4.7f;
				a2.a = 0;
				a2.posY = 383;
				a2.flags = PMSG_FLAG_SELECTION | PMSG_FLAG_CENTER | PMSG_FLAG_USE_STRING | PMSG_FLAG_SCALE_THING;
				a2.color = -16777216;
				a2.field_8 = -1;
				a2.msgID = 0;
				a2.field_10 = (int)&a1->mBuyListSelect;
				a2.field_2 = 0;
				a2.string = BlackMarket_GetBlMsg(a1, 7);
				a1->BuylistKinder[2] = AL_KinderPMessageExec_Load(a1->mTask, &a2);
				a2.field_2 = 1;
				a2.posY = 416.0;
				a2.string = BlackMarket_GetBlMsg(a1, 10);
				a1->BuylistKinder[3] = AL_KinderPMessageExec_Load(a1->mTask, &a2);
			}
			else
			{
				//not enough money, print the text
				FMainWinAddLineId(a1, 19);
				FMainWinWaitClose(a1);
				//once the text is done mode 3 goes back to mode 0
				a1->mBuyListMode = 3;
				PlaySoundProbably(32777, 0, 0, 0);
			}
		}
		else if (ControllerPointers[0]->press & Buttons_B)
		{
			a1->currentTab = 0;
			a1->mMode = 9;
			PlaySoundProbably(4106, 0, 0, 0);
		}
		break;
	case 1:
		if (a1->mBuyListSelect < 0)
		{
			a1->mBuyListSelect = 0;
		}
		if ((MenuButtons_Pressed[0] & 0x10) && a1->mBuyListSelect == 1)
		{
			a1->mBuyListSelect = 0;
			PlaySoundProbably(0x8000, 0, 0, 0);
		}
		if ((MenuButtons_Pressed[0] & 0x20) && !a1->mBuyListSelect)
		{
			a1->mBuyListSelect = 1;
			PlaySoundProbably(0x8000, 0, 0, 0);
		}
			
		if ((ControllerPointers[0]->press & 0x406) == 0) {
			return;
		}

		if ((ControllerPointers[0]->press & 0x402) != 0 && a1->mBuyListSelect != 1) {
			a1->mBuyListSelect = 1;
			PlaySoundProbably(4106, 0, 0, 0);
		}
		else {
			if (a1->mBuyListSelect)
			{
				if (a1->mBuyListSelect != 1)
				{
					goto LABEL_57;
				}
				a1->BuylistKinder[0]->MainSub = DeleteObject_;
				a1->BuylistKinder[1]->MainSub = DeleteObject_;
				a1->BuylistKinder[2]->MainSub = DeleteObject_;
				a1->BuylistKinder[3]->MainSub = DeleteObject_;
				goto LABEL_56;
			}
			if (a1->mItemDescItem.mCategory == 16)
			{
				((OtherItemPtr)OtherItemFuncs[a1->mItemDescItem.mType])(a1->mItemDescItem.mType);
			}
			else
			{
				bool saveFull;
				if (a1->mItemDescItem.mCategory == ChaoItemCategory_Egg)
				{
					saveFull = AL_GetAllChaoCount() >= AL_GetMaxChao();
				}
				else
				{
					int existingItems = AL_GetExistItemNum(a1->mItemDescItem.mCategory);
					int maxItem = AL_GetMaxItemNum(a1->mItemDescItem.mCategory);
					saveFull = existingItems >= maxItem;
				}
				if (saveFull)
				{
					FMainWinAddLineId(a1, 21);
					FMainWinWaitClose(a1);
					a1->BuylistKinder[0]->MainSub = DeleteObject_;
					a1->BuylistKinder[1]->MainSub = DeleteObject_;
					a1->BuylistKinder[2]->MainSub = DeleteObject_;
					a1->BuylistKinder[3]->MainSub = DeleteObject_;

					goto LABEL_56;
				}
				if (cweSaveFile.purchasedItemCount >= (int)save::CWE_PurchasedItems.size())
				{
					sprintf(
						buff,
						BlackMarket_GetBlMsg(a1, 20),
						save::CWE_PurchasedItems.size());
					FMainWinAddLineStr(a1, buff);
					FMainWinWaitClose(a1);
					a1->BuylistKinder[0]->MainSub = DeleteObject_;
					a1->BuylistKinder[1]->MainSub = DeleteObject_;
					a1->BuylistKinder[2]->MainSub = DeleteObject_;
					a1->BuylistKinder[3]->MainSub = DeleteObject_;

					goto LABEL_56;
				}

				if (BM_GetSelectedItem(a1)->mCategory == ChaoItemCategory_TransporterUnlock)
				{
					cweSaveFile.transporterFlag |= (1 << BM_GetSelectedItem(a1)->mType);
				}
				else
				{
					save::CWE_PurchasedItems[cweSaveFile.purchasedItemCount] = *BM_GetSelectedItem(a1);
					cweSaveFile.purchasedItemCount++;
				}
			}
			FMainWinAddLineId(a1, 18);
			FMainWinWaitClose(a1);
			ALO_RingWinAdd(-a1->mItemDescInfo->PurchasePrice);

			a1->BuylistKinder[0]->MainSub = DeleteObject_;
			a1->BuylistKinder[1]->MainSub = DeleteObject_;
			a1->BuylistKinder[2]->MainSub = DeleteObject_;
			a1->BuylistKinder[3]->MainSub = DeleteObject_;

			if (!AlItemIsRebuyable(a1->mItemDescItem))
			{
				//cant rebuy, set buy mode to code that removes the selected slot from inventory
				a1->mBuyListMode = 2;
			}
			else
			{
			LABEL_56:
				a1->mBuyListMode = 3;
			}
		}
	LABEL_57:
		if ((ControllerPointers[0]->press & 0x402) != 0)
		{
			PlaySoundProbably(4106, 0, 0, 0);
		}
		else
		{
			PlaySoundProbably(4103, 0, 0, 0);
		}
		break;
	case 2:
		if (a1->mMainWin->wordsLeftMaybe <= 0)
		{
			int v29 = a1->mBuyListCursor;
			int v30 = BM_GetInvSize(a1) - 1;
			--cweSaveFile.marketInventoryCount[TabCategory];

			if (v29 < cweSaveFile.marketInventoryCount[TabCategory])
			{
				SAlItem* v31 = &cweSaveFile.marketInventory[TabCategory][v29];
				SAlItem* v32 = &cweSaveFile.marketInventory[TabCategory][v29 + 1];
				for (int i = v30 - v29; i; --i)
				{
					*v31++ = *v32++;
				}
			}

			if (a1->mBuyListScroll > v30 - ITEMSINBUYLIST)
			{
				a1->mBuyListScroll = v30 - ITEMSINBUYLIST;
				if (a1->mBuyListScroll < 0)
				{
					a1->mBuyListScroll = 0;
				}
			}

			if (v30)
			{
				if (a1->mBuyListCursor == v30)
				{
					a1->mBuyListCursor--;
				}
				FBuyListSetItemDesc(a1);
				a1->mBuyListMode = 3;
			}
			else
			{
				a1->mMode = 9;
			}
		}
		return;
	case 3:
		if (a1->mMainWin->wordsLeftMaybe <= 0)
		{
			a1->mBuyListMode = 0;
		}
		return;
	default:
		return;
	}
}

bool NoText = false;
void __cdecl sub_58A170(BlackMarketData* a1)
{
	a1->mBuyListShow = 1;
	a1->mItemDescShow = 1;
	a1->mBuyListMode = 0;
	a1->mBuyListCursor = 0;
	a1->mBuyListScroll = 0;
	a1->mBuyListAngY = 0;
	a1->mItemDescWinExplOn = 0;
	a1->mItemDescSell = 0;

	int firstTab = -1;
	for (int i = 0; i < MarketTabCount; i++)
	{
		if (cweSaveFile.marketInventoryCount[MarketTabIndices[i]])
		{
			firstTab = i;
			break;
		}
	}

	if (firstTab < 0) {
		throw std::runtime_error(std::string(__FILE__) + ":" + std::to_string(__LINE__) + " couldn't find tab to open, why is market empty");
	}

	a1->currentTab = firstTab;
	FItemDescSet(cweSaveFile.marketInventory[MarketTabIndices[firstTab]], a1);

	*(Uint32*)0x01A267D0 = 0xFFFFFFFF;
	if (!NoText)
		AlMsgWinAddLineC(a1->mMainWin, BlackMarket_GetBlMsg(a1, 15), TextLanguage == 0);
}
static void __declspec(naked) sub_58A170Hook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call sub_58A170

		pop eax // a1
		retn
	}
}

float ringDisplayY = 80.0f;
FunctionPointer(void, LoadRingDisplay, (float x, float y), 0x005A6AC0);
void LoadRingDisplayHook()
{
	LoadRingDisplay(640, 80);
	
	bool empty = true;
	//todo: std::all_of
	for (int i = 0; i < ChaoItemCategory_Count; i++)
		if (cweSaveFile.marketInventoryCount[i])
			empty = false;

	if (empty)
		*(int*)0x1DBED8C = 0;
}

void CreateMenuThemeItem()
{
	BlackMarketAddInventory(ChaoItemCategory_MenuTheme, 2);
}

void __cdecl sub_589850(BlackMarketData* data)
{
	if (!data->mItemDescShow) {
		return;
	}
	
	if ((ControllerPointers[0]->on & 0x200) != 0) {
		data->mItemDescAngX += ControllerPointers[0]->y1;
		data->mItemDescAngY += ControllerPointers[0]->x1;

		data->mItemDescScl = (ControllerPointers[0]->l - ControllerPointers[0]->r) * 0.00013f + data->mItemDescScl;;

		if (data->mItemDescScl < 1) {
			data->mItemDescScl = 1;
		}

		if (data->mItemDescScl > 3) {
			data->mItemDescScl = 3;
		}
	}

	if (data->mItemDescItem.mCategory == 2) {
		auto type = data->mItemDescItem.mType;

		if (type >= 0 && type < ModAPI_MinimalMotion0.size() && ModAPI_MinimalMotion0[type]) {
			data->mItemDescFrame += 0.07f;

			if (ModAPI_MinimalMotion0[type]->nbFrame <= data->mItemDescFrame) {
				data->mItemDescFrame = 0;
			}
		}
	}

	if (data->mItemDescWinExplOn) {
		MsgDialog(&data->mItemDescWinExpl);
	}
}
static void __declspec(naked) sub_589850Hook()
{
	__asm
	{
		push ecx // data

		// Call your __cdecl function here:
		call sub_589850

		pop ecx // data
		retn
	}
}

static void SellHeldItem() {
	task* pHeld = MainCharObj2[0]->HeldObject;
	if (pHeld)
	{
		pHeld->MainSub = DeleteObject_;
		if (MainCharObj1[0]) {
			sub_46E5E0(0, (int)MainCharObj1[0]);
		}
		MainCharObj2[0]->HeldObject = 0;
	}

	void* pSave = AL_GetHoldingItemSaveInfo();
	if (pSave) {
		if (CWE_IsCustomItemSaveInfoCategory(AL_GetHoldingItemCategory())) {
			AL_ClearItemSaveInfo((ItemSaveInfoBase*)pSave);
		}
		else {
			AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)pSave);
		}
	}

	AL_ClearHoldingItemInfo();
}
float NewInvDisplayPosX = 230; //384 = original
void alg_kinder_bl_Init()
{
	RareFruitMarket.push_back({ SA2BFruit_Mushroom, 70 });

	WriteData((float**)0x0058B67F, &NewInvDisplayPosX);

	//fix grey button check
	WriteData((int*)0x58BACD, (int)&cweSaveFile.marketInventoryCount[ChaoItemCategory_Fruit]);
	WriteData((int*)0x58BD8B, (int)&cweSaveFile.marketInventoryCount[ChaoItemCategory_Fruit]);

	//chaos drive crash fix i think
	WriteData((short*)0x0058936C, (short)0x9090);

	//display sale/purchase price even if 0
	WriteData<4>((char*)0x005890AB, (char)0x90);

	//draw inventory
	WriteJump((void*)0x005341D0, DrawPurchasedItem);
	WriteData((char*)0x0052AE5B, (char)0x1C); //set purchased item displayer from DisplaySub to field_1C
	//if cwe inventory is empty, force a black market reload
	//also move ring display position
	WriteCall((void*)0x0058B810, LoadRingDisplayHook);
	WriteData((float**)0x00588CAA, &ringDisplayY);

	WriteJump((void*)0x00589A00, FBuyListUpdate);
	WriteJump((void*)0x0058A210, FBuyListDisp);
	WriteJump((void*)0x58AB40, FBuyListExec);
	WriteJump((void*)0x58A170, sub_58A170Hook); //buylist "init", fixes starting on the correct tab and showing the correct item

	//item desc
	WriteJump((void*)0x005891E8, FItemDescDispHook);
	WriteJump((void*)0x52F650, AL_GetMaxItemNum_hook);

	//selling code
	WriteJump((void*)0x52F470, SellHeldItem);
	WriteCall((void*)0x0058BDF1, AlItemGetInfoHook);
	WriteCall((void*)0x0058BE2D, FItemDescSetHook);
	WriteCall((void*)0x0058BDEA, sub_58B120Hook);
	WriteJump((void*)0x58B140, FSellListStartHook);

	WriteCall((void*)0x005891C4, FixFontScaleHook);

	//fixing animal motion thing
	WriteJump((void*)0x589850, sub_589850Hook);

	//memory size for bm exec
	WriteData((int*)0x0058BFDF, (int)(sizeof(BlackMarketData) + 4));
	WriteData((int*)0x0058BFE6, (int)sizeof(BlackMarketData));

	//menu theme bs
	WriteData((int*)0x0058B924, (int)&(cweSaveFile.marketInventory[ChaoItemCategory_MenuTheme]->mCategory));
	WriteData((int*)0x0058B92E, (int)&(cweSaveFile.marketInventory[ChaoItemCategory_MenuTheme]->mType));
	WriteData((int*)0x0058B919, (int)&cweSaveFile.marketInventoryCount[ChaoItemCategory_MenuTheme]);
	WriteData((int*)0x0058B937, (int)&cweSaveFile.marketInventoryCount[ChaoItemCategory_MenuTheme]);
	WriteData<0x1B>((char*)0x0058B97B, (char)0x90);
	WriteCall((void*)0x0058B97B, CreateMenuThemeItem);

	if (gConfigVal.BlackMarketShort)
	{
		NoText = true;

		//exit text
		WriteJump((void*)0x0058BCD4, (void*)0x0058BD21); //text about "getting more emblems"
		WriteCall((void*)0x0058BD23, nullsub_1);

		WriteData<2>((char*)0x0058BE88, (char)0x90);//kills the message box check in case 14 (exit text)
		WriteData<6>((char*)0x0058BA06, (char)0x90); //kills the message box check in case 3 (entry text)

		//i think this might be the "you cant sell this item" no reason to shorten
		//WriteData<2>((char*)0x0058BE6E, (char)0x90);//kills the message box check in case 13 (sell text)
		//WriteCall((void*)0x58BE39, nullsub_1);
		//WriteCall((void*)0x58BE40, nullsub_1);

		//"you dont have anything to sell"
		WriteCall((void*)0x0058BD44, nullsub_1);
		WriteCall((void*)0x0058BD57, nullsub_1);
		WriteCall((void*)0x0058BD63, nullsub_1);
		WriteCall((void*)0x0058BD6A, nullsub_1);

		//kills the msg calls in case 0
		WriteCall((void*)0x0058B82E, nullsub_1);
		WriteCall((void*)0x0058B835, nullsub_1);
		WriteCall((void*)0x0058B848, nullsub_1);
		WriteCall((void*)0x0058B841, nullsub_1);
		WriteCall((void*)0x0058B854, nullsub_1);
	}
}