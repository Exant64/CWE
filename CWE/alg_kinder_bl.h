#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "al_msg_win.h"
#include "al_save.h"

enum 
{
	MarketTabEgg,
	MarketTabFruit,
	MarketTabSeed,
	MarketTabHat,
	MarketTabAccessory,
	MarketTabSpecial,
	MarketTabOthers,
	MarketTabCount
};

struct BlackMarketItemAttributes
{
	int PurchasePrice;
	int SalePrice;
	__int16 RequiredEmblems;
	__int16 Name;
	__int16 Descriptions;
	__int16 Unknown;
};

#pragma pack(push, 8)
struct BlackMarketData
{
	ObjectMaster* mTask;
	Uint32 mMode;
	char mSelect;
	char field_9;
	Uint16 wordA;
	Uint16 wordC;
	Sint8 gapE[2];
	Uint32* mMsgList;
	ObjectMaster* KinderWindows[7];
	Sint8 byte30;
	Sint8 byte31;
	Sint8 byte32;
	Sint8 byte33;
	Sint8 byte34;
	Sint8 gap35[3];
	float float38;
	KinderCoMessageThing* mMainWin;
	Sint8 byte40;
	Sint8 gap41[3];
	Uint32 dword44;
	Uint32* mItemMsgList;
	Sint8 mItemIconShow;
	Sint8 mItemDescShow;
	Sint8 mItemDescSell;
	Sint8 mItemDescWinExplOn;
	SAlItem mItemDescItem;
	BlackMarketItemAttributes* mItemDescInfo;
	KinderCoMessageThing mItemDescWinExpl;
	int mItemDescAngX;
	int mItemDescAngY;
	float mItemDescScl;
	float mItemDescFrame;
	char mBuyListShow;
	char mBuyListMode;
	Sint8 mBuyListUpdated;
	char gap9B;
	int mBuyListCursor;
	int mBuyListScroll;
	int mBuyListAngY;
	char mBuyListSelect;
	char field_A9;
	char field_AA;
	char field_AB;
	ObjectMaster* BuylistKinder[4];
	char mSellListShow;
	char mSellListSelect;
	char field_BE;
	char field_BF;
	ObjectMaster* SellKinder[4];
	int byteD0;
	// new fields starting here
	int currentTab;
};
#pragma pack(pop)

struct ItemChance_old
{
	Sint8 item;
	Sint8 chance;
};
struct ItemChance 
{
	Uint16 item;
	Sint8 chance;
};

struct BlackMarketCategoryAttribute
{
	BlackMarketItemAttributes *attrib;
	int Count;
};
DataArray(BlackMarketCategoryAttribute, CategoryAttribs, 0x008A8728, 10);
DataArray(BlackMarketItemAttributes, FruitBMAttributes, 0x008A8028, 24);
DataArray(BlackMarketItemAttributes, HatBMAttributes, 0x8A81A8, 85);
DataArray(BlackMarketCategoryAttribute, BlackMarketCategories, 0x008A8728, 17);
DataArray(ItemChance_old, GeneralFruitChances, 0x008A3548, 4);

DataPointer(int, PurchasedItemCount, 0x01DBEDAC);
DataArray(BlackMarketItem, PurchasedItems, 0x01DBEDA0, 10);

extern std::vector<ItemChance> GeneralFruitMarket;
extern std::vector<ItemChance> RareFruitMarket;
extern std::array<std::unordered_set<int>, ChaoItemCategory_Count> AlItemRebuyable;
extern std::unordered_map<int, int> OtherItemFuncs;

void BlackMarketAddInventory(int cat, int item);
void alg_kinder_bl_Init();
extern "C" __declspec(dllexport) void DrawItem(float x, float y, float scl, const Rotation & rot, const SAlItem & CurrentItem);