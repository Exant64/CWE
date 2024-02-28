#pragma once
#include "al_world.h"
#include <vector>
#include "Chao.h"
#include <array>
struct SaveFileAPIEntry
{
	const char* suffix;
	void* pointer;
	int fileSize;
};
extern std::vector<SaveFileAPIEntry> ModAPI_SaveAPI;


enum eTRANSPORTER
{
	NAME = BIT_0,
	MOVE = BIT_1,
	CUSTOMIZATION = BIT_2,
};

enum eOMOCHAOPHASE
{
	NONE = 0,
	MANNEQUIN = BIT_0,
	EYE_L = BIT_1,
	EYE_R = BIT_2,
	MOUTH = BIT_3,
	BODY = BIT_4,
	HANDS = BIT_5,
	LEGS = BIT_6,
	PROPELLER = BIT_7
};

struct OMOCHAO_INFO
{
	unsigned char phase;
	NJS_VECTOR position;
	int rotY;
	unsigned char color;
	unsigned char monotone;
	unsigned char shiny;
	unsigned char jewel;
	CHAO_ID chaoID;
};

struct SAlItem
{
	Sint8 mCategory;
	Uint16 mType;
};

struct CWESaveFile
{
	ChaoData chaoParam[24];
	ITEM_SAVE_INFO specialItems[30];
	int TimeOfDay;
	int Weather;
	OMOCHAO_INFO omochao[3];
	int transporterFlag;
	int marketInventoryCount[ChaoItemCategory_Count];
	SAlItem marketInventory[ChaoItemCategory_Count][32];
	int purchasedItemCount;
	SAlItem PurchasedItems[5]; //deprecated, ALW_Control_Main moves it to save::CWE_PurchasedItems
	ITEM_SAVE_INFO cweToyInfo[NB_ALW_KIND * 3];
};

namespace save {
	extern std::array<SAlItem,10> CWE_PurchasedItems;
}

void ReadCWESaveFiles();
void SaveCWESaveFiles();

SAlItem* GetMarketInventory(int category);
int GetMarketInvSize(int category);

int GetTimeOfDay();
void SetTimeOfDay(int time);
int GetWeather();
void SetWeather(int weather);

extern CWESaveFile cweSaveFile;
void *AL_GetSpecialItemSave();
void AL_SaveInit();