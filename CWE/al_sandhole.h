#pragma once
#include "stdafx.h"

#include <random>
#include "ninja_functions.h"

DataPointer(char, LightIndex, 0x01DE4664);
DataPointer(char, LightIndexBackupMaybe, 0x01DE4400);
FunctionPointer(double, CalculateFalloffPosition, (float x, float y, float z, Rotation *a4), 0x00494C30);

const int DoLightingPtr = 0x00487060; //TODO: move to new thing
void DoLighting(int a1);

ObjectMaster*  SandHole_Load(NJS_VECTOR * pos);