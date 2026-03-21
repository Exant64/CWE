#pragma once

#include "stdafx.h"

struct ChaoItemStats {
	__int16 Mood;
	__int16 Belly;
	__int16 Swim;
	__int16 Fly;
	__int16 Run;
	__int16 Power;
	__int16 Stamina;
	__int16 Luck;
	__int16 Intelligence;
	__int16 anonymous_7;
};

#pragma pack(push, 1)
typedef char AL_NAME[13];
struct AL_GUEST {
	Uint8 Type;
	float Magnitude;
	float FlySwim;
	float RunPower;
	float Alignment;
	AL_NAME Name;
	Uint16 Flags;
};

struct AL_PARAM_ACCESSORY_INFO {
	char ID[21];
	Uint32 Flags;
	Uint32 ColorFlags;
	Uint32 ColorSlots[8];
};

struct CHAO_PARAM_CWE {
	char Padding1[114];
	char ShinyFruitValue_obsolete;
	char ShinyJewelMonotone;
	char FreeSpace;
	char GiftChaoValue; //unused for a while, we might pick this up again some time
	char FreeSpace_;
	char LobbyTextureValue; //same here
	char FreeSpace__;
	char EyeAlignment;
	char EyeColor;
	char AltMouthPatch;
	char UpgradeCounter;
	char freeSpace3;
	char XGradeValue;
	AL_SHAPE_ELEMENT motherData;
	AL_SHAPE_ELEMENT fatherData;
	unsigned char IsInitializedAccessory;
	unsigned char Accessories_old[4];
	bool DCWings;
	bool Negative;
	char padding_cwedna[2];
	short Birthday;
	bool ForceReincarnate;
	unsigned short Accessories_[4];
	int Flags;
	int Version;
	AL_NAME Name;
	AL_NAME MotherName;
	AL_NAME FatherName;
	AL_NAME MGroundMotherName;
	AL_NAME MGroundFatherName;
	AL_NAME FGroundMotherName;
	AL_NAME FGroundFatherName;
	AL_GUEST Guest;
	char TypeID[16];
	//we can't make a knowledge struct cuz we don't really know what we'll put in it in the future, I don't want it to end up like CWEDNA
	Uint8 MusicFlag_CWE;
	Uint8 DanceFlag_CWE;
	AL_PARAM_ACCESSORY_INFO Accessories[4];
};
#pragma pack(pop)

static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, Padding1) == 0x520);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, LobbyTextureValue) == 0x597);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, fatherData) == 0x5D4);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, padding_cwedna) == 0x60F);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, Birthday) == 0x611);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, MGroundMotherName) == 0x64B);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, Accessories) == 0x6B1);
static_assert(sizeof(CHAO_PARAM_GC) + offsetof(CHAO_PARAM_CWE, XGradeValue) == 0x59e);

void AL_NameSet(char* lval, char* rval);
bool AL_IsChild(task* tp);
bool AL_IsHero(unsigned __int8 a1);
bool AL_IsHero2(task* a1);
bool AL_IsDark(unsigned __int8 a1);
bool AL_IsNegative(task* tp);
bool AL_IsCustomChao(task* tp);

void AL_ParameterClearAccessory(task* tp, int slot);

void AL_ParameterAddAPos(task* a1, float a2);

void sub_535E90(CHAO_PARAM_GC *a1, int a2);

void AL_ParameterAddUserLike(task* tp, int add, float AddAttr);
void AL_ParameterGrow(task* a1, unsigned __int16 a2, unsigned __int16 a3, int a4);

void IncrementFlySwim(task* a1, float a2);
void IncrementPowerRun(task* a1, float a2);

void AL_ParameterInit();

int AL_ParameterGetSkill(task* tp, Uint16 SkillKind);