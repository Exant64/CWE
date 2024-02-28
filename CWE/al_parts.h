#pragma once
#include <map>
#include <vector>
#include "al_minimal.h"
#include "cwe_api.h"

enum
{
	PARTS_KIND_ARM = 0x0,
	PARTS_KIND_EAR = 0x1,
	PARTS_KIND_FOREHEAD = 0x2,
	PARTS_KIND_HORN = 0x3,
	PARTS_KIND_REG = 0x4,
	PARTS_KIND_TAIL = 0x5,
	PARTS_KIND_WING = 0x6,
	PARTS_KIND_TONGUE = 0x7,
	NB_PARTS_KIND = 0x8,
};

extern std::vector<std::array<CWE_PARTS, 2>> ModAPI_MiniParts;
extern int PartsObjectListNumber[8][2];

void AL_PartsMinimalFlagOn(ObjectMaster* tp, int MinimalType);
void AL_PartsMinimalFlagOn_DX(ObjectMaster* tp, int MinimalType);

int  AL_PartsIsMinimalFlagOn(ObjectMaster* tp, int MinimalType);

void __cdecl AL_SetItem(ObjectMaster* a1, int a2, NJS_OBJECT* model, NJS_TEXLIST* texlist);


bool AL_IsExistPartsChild(int MinimalNum, int PartsKind);
bool AL_IsExistPartsAdult(int MinimalNum, int PartsKind);

int AL_GetMinimalPart(ObjectMaster* tp, int PartsKind);
void AL_SetMinimalParts(ObjectMaster* tp, int PartsKind, int MinimalType);
void AL_RemoveMinimalParts(ObjectMaster* tp, int PartsType);

void AL_Parts_Init();
void sub_566B80(ObjectMaster *a1);
DataArray(NJS_OBJECT**, ChildAnimalParts, 0x012E59A0, SA2BAnimal_Phoenix);
DataArray(NJS_OBJECT**, AdultAnimalParts, 0x012E5938, SA2BAnimal_Phoenix);
extern NJS_OBJECT** SADXChildAnimalParts[15];
extern NJS_OBJECT** SADXAdultAnimalParts[15];