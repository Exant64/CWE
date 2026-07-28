#pragma once

#include "stdafx.h"

typedef enum eLANDMARK_ATTR {
    LMA_NONE = 0x0,
    LMA_GROUND1 = 0x1,
    LMA_GROUND2 = 0x2,
    LMA_GROUND3 = 0x3,
    LMA_SPECIAL1 = 0x4,
    LMA_SPECIAL2 = 0x5,
    LMA_BRIDGE = 0x6,
    LMA_NONE7 = 0x7,
    LMA_WG = 0x8,
    LMA_WATER = 0x9,
} eLANDMARK_ATTR;

struct POS_XZ
{
	int x;
	int z;
};
struct LAND_ATTR_INDEX
{
	int nbIndex;
	POS_XZ pos[512];
};
DataArray(LAND_ATTR_INDEX, stru_1A15938, 0x1A15938, 15);

void AL_GetRandomAttrPos(eLANDMARK_ATTR attr, NJS_POINT3* pPos);
int AL_GetCurrLandAttr(NJS_POINT3* pPos);