#pragma once
#include "al_texlist.h"

extern const HelperFunctions* g_HelperFunctions;
extern void(__cdecl* DrawChaoWorldShadow)();

void AL_CustomizationInit();
DataArray(ChaoData*, ChaoSelectData, 0x01DBEE80, 48);

DataPointer(char, ControlEnabled, 0x0174AFFE);

void AL_NameDisplayCreate();
ObjectMaster* AL_LargeTitleBar_Create();
DataPointer(int, ChaoSaveTimer, 0x019F6474);
VoidFunc(sub_583C60, 0x583C60);
FunctionPointer(void, ProjectToScreen, (float screenX, float screenY, float worldSpaceZ), 0x005848F0);
ThiscallFunctionPointer(DWORD*, sub_52F9E0, (int a1), 0x52F9E0);
DataArray(int, nbWorldEntry, 0x1DC0F80, 5); //length is not actually 5 but good for now
DataArray(int, nbMaxEntry, 0x008AB838, 5);

struct ConfigValues {
	bool BrightFixEnabled;
	bool KeepAnimalParts;
	bool EnergyCap;
	bool ForceShinyTT;
	bool EmotionDisplay;
	bool TransporterColor;
	bool ChaoCounter = false;
	bool MoreSound;
	bool MoreAnimation;
	bool ClassroomTimerDisplay;

	bool DayNightCycle;
	bool DayNightCycleNeutralGardenSkybox;
	bool DayNightCycleHeroGardenSkybox;
	bool DayNightCycleDarkGardenSkybox;
	bool DayNightCycleNeutralGarden;
	bool DayNightCycleHeroGarden;
	bool DayNightCycleDarkGarden;
	bool DayNightCycleRace;
	bool DayNightCycleKarate;
	int DayNightCycleHourFrame;
	bool DayNightDebug;
	bool DayNightCheat = true;

	float GlobalPriceMultiplier;

	bool OmochaoParts = false;
	bool UnusedToys = false;
	bool SpawnOmochao = true;
	bool ToyMove, ToyReset;
	bool NewInventory;
	bool DoctorChaoInfo;
	bool BlackMarketShort;
	bool HDHoodie;
	bool WinJewelChaoInJewelRace;
	bool MedalChaoCanMakeColorChao;
	bool DisableArmsLegs;
	bool DisableAllParts;
	bool FixMonsterEvo;
};
extern ConfigValues gConfigVal;

FunctionPointer(ChaoData*, AL_GetNewChaoSaveInfo, (), 0x00531AA0);
ThiscallFunctionPointer(void*, AL_GetNewItemSaveInfo, (int a1), 0x52F9E0);

extern ObjectMaster* pBirthdayChao;

#pragma pack(push, 8)
struct TreeData
{
	char treeType;
	unsigned __int8 lifeTimeStatus;
	unsigned __int8 growth;
	unsigned __int8 lifeSpanMaybe;
	char fruitSize1;
	char FruitSize2;
	char fruitSize3;
	char rotation;
};
#pragma pack(pop)
void sub_46E5E0(int a1, int a2);

#pragma pack(push, 8)
struct RaceTimeData
{
	int set;
	KarateOpponent Data;
};
#pragma pack(pop)


#pragma pack(push, 8)
struct RaceData
{
	char UnlockedRaces[6];
	char field_6[15];
	char field_15[11];
	RaceTimeData RaceTimeData[10];
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) BlackMarketSaveData
{
	char gap0[28];
	int dword1C;
	int ItemCount;
	int dword24;
	BlackMarketItem Items[20];
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	__int16 field_68;
	__int16 word6A;
	__int16 word6C;
	__int16 word6E;
	__int16 word70;
};
#pragma pack(pop)
void JoyCarry_Init();

struct  ChaoObjectData
{
	__int16 Type;
	__int16 Garden;
	__int16 Size;
	__int16 Age;
	NJS_VECTOR position;
};

struct  ALFSave
{
	int ChaoSaveStart;
	int unk_1;
	int Seed;
	int ChaoGardensUnlocked;
	int ChaoToysUnlocked;
	int field_14;
	int field_18;
	int TotalResetTriggered;
	TreeData ChaoTreeSlots[21];
	ChaoObjectData ChaoFruitSlots[24];
	ChaoObjectData ChaoUnknownSlots[16];
	ChaoObjectData ChaoSeedSlots[12];
	ChaoObjectData ChaoHatSlots[24];
	ChaoObjectData ChaoAnimalSlots[10];
	RaceData RaceData;
	char field_9D0[32];
	BlackMarketSaveData BlackMarketData;
	ChaoData ChaoSlots[24];
	int Checksum[2];
};

DataArray(ALFSave, ChaoSave, 0x019F6460, 2);

extern int HyperSwimFruitID;
extern int HyperFlyFruitID;
extern int HyperRunFruitID;
extern int HyperPowerFruitID;
extern int ShinyFruitID;
extern int CakeSliceID;
extern int OrangeID;
extern int BeeID;
extern int MirrorID;
void ApplyWSwitch(int a1);
bool GetWSwitch(int a1);
void ShinyJewelSpace_Init();
void ResetChaoObjectData(ChaoObjectData *result);
VoidFunc(AL_PlayerControlManager_Load, 0x0052BA00);
void ChaoMain_Init();
void LoadChaoTexlist(const char* a2, NJS_TEXLIST* texlist, int a1);