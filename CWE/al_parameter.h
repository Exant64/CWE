#pragma once
#include "SA2ModLoader.h"

void AL_NameSet(char* lval, char* rval);
bool AL_IsChild(ObjectMaster* tp);
bool AL_IsHero(unsigned __int8 a1);
bool AL_IsDark(unsigned __int8 a1);
bool AL_IsNegative(ObjectMaster* a1);
bool AL_IsCustomChao(ObjectMaster* tp);
void AL_ParameterAddAPos(ObjectMaster* a1, float a2);

void AL_ParameterAddUserLike(ObjectMaster* tp, int add, float AddAttr);
void AL_ParameterGrow(ObjectMaster* a1, unsigned __int16 a2, unsigned __int16 a3, int a4);

void IncrementFlySwim(ObjectMaster* a1, float a2);
void IncrementPowerRun(ObjectMaster* a1, float a2);