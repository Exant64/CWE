#pragma once
#include "SA2ModLoader.h"

void AL_NameSet(char* lval, char* rval);
bool AL_IsChild(task* tp);
bool AL_IsHero(unsigned __int8 a1);
bool AL_IsHero2(task* a1);
bool AL_IsDark(unsigned __int8 a1);
bool AL_IsNegative(task* a1);
bool AL_IsCustomChao(task* tp);

void AL_ParameterClearAccessory(task* tp, int slot);

void AL_ParameterAddAPos(task* a1, float a2);

void sub_535E90(ChaoDataBase *a1, int a2);

void AL_ParameterAddUserLike(task* tp, int add, float AddAttr);
void AL_ParameterGrow(task* a1, unsigned __int16 a2, unsigned __int16 a3, int a4);

void IncrementFlySwim(task* a1, float a2);
void IncrementPowerRun(task* a1, float a2);

void AL_ParameterInit();