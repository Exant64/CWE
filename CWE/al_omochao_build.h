#pragma once
#include "al_save.h"

extern NJS_POINT3 OmoPositions[];
extern uint32_t OmoRot[];

void ALO_OmoBuildCreate(NJS_POINT3* pPos, Angle ang);

OMOCHAO_INFO* GetOmoData();

FunctionPointer(double, CalculateFalloffPosition_, (float x, float y, float z, Rotation* a4), 0x00494C30);