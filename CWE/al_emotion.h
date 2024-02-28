#pragma once
#include "SA2ModLoader.h"

enum 
{
	EM_MD_PLEASURE = 0x0,
	EM_MD_ANGER = 0x1,
	EM_MD_SORROW = 0x2,
	EM_MD_FEAR = 0x3,
	EM_MD_SURPRISE = 0x4,
	EM_MD_PAIN = 0x5,
	EM_MD_RELAX = 0x6,
	EM_MD_TOTAL = 0x7,
	EM_ST_SLEEPY = 0x8,
	EM_ST_SLEEP_DEPTH = 0x9,
	EM_ST_HUNGER = 0xA,
	EM_ST_BREED = 0xB,
	EM_ST_TEDIOUS = 0xC,
	EM_ST_LONELY = 0xD,
	EM_ST_TIRE = 0xE,
	EM_ST_STRESS = 0xF,
	EM_ST_NOURISHMENT = 0x10,
	EM_ST_CONDITION = 0x11,
	EM_ST_THIRSTY = 0x12,
	EM_PER_CURIOSITY = 0x13,
	EM_PER_KINDNESS = 0x14,
	EM_PER_AGRESSIVE = 0x15,
	EM_PER_SLEEPY_HEAD = 0x16,
	EM_PER_SOLITUDE = 0x17,
	EM_PER_VITALITY = 0x18,
	EM_PER_GLUTTON = 0x19,
	EM_PER_REGAIN = 0x1A,
	EM_PER_SKILLFUL = 0x1B,
	EM_PER_CHARM = 0x1C,
	EM_PER_CHATTY = 0x1D,
	EM_PER_CALM = 0x1E,
	EM_PER_FICKLE = 0x1F,
	NB_EMOTION = 0x20,
};

void AL_EmotionSet(ObjectMaster* a1, int a2, int a3);
void AL_EmotionAdd(ObjectMaster* a1, int a2, int a3);
extern int(*AL_EmotionGetValue)(ObjectMaster* tp, int emotion);