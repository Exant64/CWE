#pragma once

#include "ninja_functions.h"
#include <random>

enum ePIANOTYPE {
	PIANOTYPE_PIANO = 0,
	PIANOTYPE_ORGAN = 1
};

DataPointer(NJS_OBJECT, Piano, 0x01304754);
DataPointer(NJS_OBJECT, Organ, 0x0135E3CC);
bool __cdecl SetPianoWaypoint(ObjectMaster* a2, NJS_VECTOR* a1);
void ALO_PianoCreate(int index, NJS_POINT3* pPos, Angle ang);