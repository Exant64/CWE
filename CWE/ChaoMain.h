#pragma once
#include "al_texlist.h"
#include <d3d9.h>

extern const HelperFunctions* g_HelperFunctions;

extern IDirect3DDevice9* cwe_device;

void AL_CustomizationInit();
DataArray(CHAO_SAVE_INFO*, ChaoSelectData, 0x01DBEE80, 48);

DataPointer(char, ControlEnabled, 0x0174AFFE);
DataPointer(unsigned int, ChaoSaveText, 0x0136606E);

void AL_NameDisplayCreate();
task* AL_LargeTitleBar_Create();
DataPointer(int, ChaoSaveTimer, 0x019F6474);
VoidFunc(sub_583C60, 0x583C60);
FunctionPointer(void, ProjectToScreen, (float screenX, float screenY, float worldSpaceZ), 0x005848F0);
ThiscallFunctionPointer(DWORD*, sub_52F9E0, (int a1), 0x52F9E0);
DataArray(int, nbWorldEntry, 0x1DC0F80, 5); //length is not actually 5 but good for now
DataArray(int, nbMaxEntry, 0x008AB838, 5);

enum {
	CFG_MORE_FACE_NONE,
	CFG_MORE_FACE_PERSONALITY,
	CFG_MORE_FACE_RANDOM
};

struct ConfigValues {
	bool BrightFixEnabled;
	bool CharacterChaoEvo;
	bool CustomChaoEvo;
	bool KeepAnimalParts;
	bool EnergyCap;
	bool ForceShinyTT;
	bool EmotionDisplay;
	bool TransporterColor;
	bool ChaoCounter = false;
	bool MoreSound;
	bool MoreAnimation;
	bool ClassroomTimerDisplay;
	bool OldName;

	bool AutoGenerateStats;
	bool LuckyChao;

	bool ChaoAttention;
	bool AnnoyingMeanChao;
	bool BullyChao;

	bool ColorMixing;
	bool NewbornVaryingShades;
	bool NormalChaoMakeColorChao;
	bool EyeColorsForNewbornChao;

	uint32_t MoreFaces;

	bool DayNightCycle;
	bool DayNightShinyTex;
	bool DayNightRain;
	bool DayNightHeroRain;
	bool DayNightRainSounds;
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

	bool PathfindingEnabled;
	bool PathfindingVanilla;
	bool PathfindingLog;

	float GlobalPriceMultiplier;

	bool OmochaoParts = false;
	bool UnusedToys = false;
	bool SpawnOmochao = true;

	bool NeutSet, HeroSet, DarkSet;

	bool LegacyBaldYOnly;
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
	bool AccUpdateDraw;

	bool FixHeroSky;

	bool StageAnimals;
	float StageAnimalChance;
	size_t StageAnimalMinCount;
	size_t StageAnimalMaxCount;
	bool StageAnimalIncludeSADX;

	bool NeutGrayscale;
	bool HeroGrayscale;
	bool DarkGrayscale;
};
extern ConfigValues gConfigVal;

FunctionPointer(CHAO_SAVE_INFO*, AL_GetNewChaoSaveInfo, (), 0x00531AA0);
ThiscallFunctionPointer(ITEM_SAVE_INFO*, AL_GetNewItemSaveInfo, (int a1), 0x52F9E0);

extern task* pBirthdayChao;

void sub_46E5E0(int a1, int a2);

extern uint32_t CWE_ModIndex;

extern int HyperSwimFruitID;
extern int HyperFlyFruitID;
extern int HyperRunFruitID;
extern int HyperPowerFruitID;
extern int ShinyFruitID;
extern int CakeSliceID;
extern int OrangeID;
extern int BeeID;
extern int MirrorID;

void ShinyJewelSpace_Init();
VoidFunc(AL_PlayerControlManager_Load, 0x0052BA00);
void ChaoMain_Init();
void AL_LoadTex(const char* a2, NJS_TEXLIST* texlist, int a1);