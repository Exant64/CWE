#include "stdafx.h"
#include "data/parts/RaccoonArms.h"
#include "data/parts/SADXParts.h"
#include "ChaoMain.h"
#include "al_parts.h"
#include "al_minimal.h"
#include "Chao.h"
#include <vector>
#include <map>

void __cdecl AL_SetItem(ObjectMaster* a1, int a2, NJS_OBJECT* model, NJS_TEXLIST* texlist)
{
	ChaoData1* v4; // eax
	ChunkObjectPointer* v5; // edx

	v4 = a1->Data1.Chao;
	v5 = v4->field_524[a2];
	if (v5->toy.model != model)
	{
		v5->toy.scale = 0.0;
		v4->field_524[a2]->toy.model = model;
	}
	v4->field_524[a2]->toy.texlist = texlist;
	v4->field_524[a2]->toy.exists = 1;
	v4->field_524[a2]->useTransform = 0;
}

void  AL_PartsMinimalFlagOn(ObjectMaster* tp, int MinimalType)
{
	chaowk* work = CHAOWK(tp);
	work->pParamGC->PartsBTL.MinimalFlag |= 1 << MinimalType;
}
void  AL_PartsMinimalFlagOff(ObjectMaster* tp, int MinimalType)
{
	chaowk* work = CHAOWK(tp);
	work->pParamGC->PartsBTL.MinimalFlag = 0;
}
int  AL_PartsIsMinimalFlagOn(ObjectMaster* tp, int MinimalType)
{
    chaowk* work = CHAOWK(tp);
    if (work->pParamGC->PartsBTL.MinimalFlag & (1 << MinimalType))
        return 1;
    else
        return 0;
}

void AL_PartsMinimalFlagOn_DX(ObjectMaster* tp, int MinimalType)
{
	chaowk* work = CHAOWK(tp);
	work->pParamGC->partsDX.MinimalFlag |= 1 << MinimalType;
}
void AL_PartsMinimalFlagOff_DX(ObjectMaster* tp, int MinimalType)
{
	chaowk* work = CHAOWK(tp);
	work->pParamGC->partsDX.MinimalFlag = 0;
}
int AL_PartsIsMinimalFlagOn_DX(ObjectMaster* tp, int MinimalType)
{
	chaowk* work = CHAOWK(tp);
	if (work->pParamGC->partsDX.MinimalFlag & (1 << MinimalType))
		return 1;
	else
		return 0;
}

bool AL_IsExistPartsChild(int MinimalNum, int PartsKind)
{
	return ModAPI_MiniParts[MinimalNum][0].objects[PartsObjectListNumber[PartsKind][0]];
}
bool AL_IsExistPartsAdult(int MinimalNum, int PartsKind)
{
	return ModAPI_MiniParts[MinimalNum][1].objects[PartsObjectListNumber[PartsKind][0]];
}

int AL_GetMinimalPart(ObjectMaster* tp, int PartsKind)
{
	chaowk* work = CHAOWK(tp);
	return work->pParamGC->PartsBTL.MinimalParts[PartsKind];
}

std::vector<std::array<CWE_PARTS,2>> ModAPI_MiniParts;

NJS_OBJECT emptyNJSOBJECT = { NJD_EVAL_HIDE | NJD_EVAL_BREAK , NULL, 0,0,0, 0,0,0, 0,0,0, NULL, NULL };
NJS_OBJECT* emptyArray[13];

int PartsObjectTreeNumber[8][2] = {
	{3, 10},
	{24,26},
	{29, -1},
	{30, 31},
	{6, 13},
	{8, -1},
	{37,  39},
	{28, -1}
};

int PartsObjectListNumber[8][2] = {
	{0, 1},
	{2, 3},
	{4, -1},
	{5, 6},
	{7, 8},
	{9, -1},
	{10, 11},
	{12, -1}
};
void AL_SetMinimalParts(ObjectMaster* tp, int PartsKind, int MinimalType) {
	chaowk* work = tp->Data1.Chao;
	int i;

	if (gConfigVal.DisableAllParts) return;
	if (gConfigVal.DisableArmsLegs) {
		if (PartsKind == PARTS_KIND_ARM || PartsKind == PARTS_KIND_REG) return;
	}

	if (MinimalType >= (int)ModAPI_MiniParts.size() || (MinimalType >= SA2BAnimal_YellowChaosDrive && MinimalType <= SA2BAnimal_PurpleChaosDrive))
		MinimalType = -1;

	if (MinimalType < 0) {
		work->pParamGC->PartsBTL.MinimalParts[PartsKind] = -1;
	}
	else {
		work->pParamGC->PartsBTL.MinimalParts[PartsKind] = MinimalType;
	}

	for (i = 0; i < 2; i++)
	{
		int tree = PartsObjectTreeNumber[PartsKind][i];
		int listNumber = PartsObjectListNumber[PartsKind][i];
		if (tree != -1) {
			if (MinimalType >= 0) {
				//we're using field_CC (pDisplayList, unused field) as a texlist field
				//first element of CWE_PARTS[2] array is child parts, second is adult parts
				if (work->pParamGC->Type == 2) {
					work->field_524[tree]->animalPart = ModAPI_MiniParts[MinimalType][0].objects[listNumber];
					work->field_524[tree]->field_CC = (int)ModAPI_MiniParts[MinimalType][0].tex;
				}
				else {
					work->field_524[tree]->animalPart = ModAPI_MiniParts[MinimalType][1].objects[listNumber];
					work->field_524[tree]->field_CC = (int)ModAPI_MiniParts[MinimalType][1].tex;
				}
			}
			else
				work->field_524[tree]->animalPart = 0;
		}
	}
}

void AL_RemoveMinimalParts(ObjectMaster* tp, int PartsType) {
	AL_SetMinimalParts(tp, PartsType, -1);
}

FunctionPointer(void, sub_566B30, (ChunkObjectPointer* a1), 0x566B30);

static void AL_PartsConversion(ObjectMaster* a1) {
	ChaoData1* wk = CHAOWK(a1);
	ChaoDataBase* pParam = wk->pParamGC;

	for (size_t i = 0; i < NB_PARTS_KIND; i++) {
		if (wk->pParamGC->partsDX.MinimalParts[i] > 0) {
			wk->pParamGC->PartsBTL.MinimalParts[i] = wk->pParamGC->partsDX.MinimalParts[i] - 1 + 25;
		}
	}
}

void sub_566B80(ObjectMaster* tp) {
	ChaoData1* wk = CHAOWK(tp);
	ChaoDataBase* pParam = GET_CHAOPARAM(tp);

	if (!(pParam->Flags & AL_PARAM_FLAG_PARTS_CONVERSION)) {
		AL_PartsConversion(tp);

		pParam->Flags |= AL_PARAM_FLAG_PARTS_CONVERSION;
	}

	for (int i = 0; i < NB_PARTS_KIND; i++) {
		AL_SetMinimalParts(tp, i, wk->pParamGC->PartsBTL.MinimalParts[i]);
	}
	sub_566B30(wk->field_510);
}

static void __declspec(naked) sub_566B80Hook()
{
	__asm
	{
		push edi // a1

		// Call your __cdecl function here:
		call sub_566B80

		pop edi // a1
		retn
	}
}

void AL_Parts_Init()
{
	//no hands for bats, done to replace the raccoon arms arm removal functionality from a while ago
	emptyArray[0] = &emptyNJSOBJECT;
	emptyArray[1] = &emptyNJSOBJECT;
	ChildAnimalParts[SA2BAnimal_Bat] = emptyArray;
	AdultAnimalParts[SA2BAnimal_Bat] = emptyArray;

	//arms cuz colors overwrite it
	AdultAnimalParts[SA2BAnimal_Raccoon][0] = &RaccoonArmsRight;
	AdultAnimalParts[SA2BAnimal_Raccoon][1] = &RaccoonArmsLeft;

	ChildAnimalParts[SA2BAnimal_Raccoon][9] = (NJS_OBJECT*)0x012BB6F4;//raccoon tail
	AdultAnimalParts[SA2BAnimal_Raccoon][9] = (NJS_OBJECT*)0x0129A16C;

	//sa2 parts
	for (int i = 0; i < 21; i++) {
		CWE_PARTS sa2bparts_child = { ChildAnimalParts[i], &al_mini_parts_tex_texlist };
		CWE_PARTS sa2bparts_adult = { AdultAnimalParts[i], &al_mini_parts_tex_texlist };
		ModAPI_MiniParts.push_back({ sa2bparts_child, sa2bparts_adult });
	}

	//chaos drives
	for (int i = 0; i < 4; i++) {
		CWE_PARTS nullpart = { nullptr, nullptr };
		ModAPI_MiniParts.push_back({ nullpart, nullpart });
	}
	
	//sadx parts
	for (int i = 0; i < 15; i++) {
		CWE_PARTS sadxparts_child = { SADXChildAnimalParts[i], &AL_DX_PARTS_TEX_TEXLIST };
		CWE_PARTS sadxparts_adult = { SADXAdultAnimalParts[i], &AL_DX_PARTS_TEX_TEXLIST };
		ModAPI_MiniParts.push_back({ sadxparts_child, sadxparts_adult });
	}

	WriteJump((void*)0x566B80, sub_566B80Hook);
}
