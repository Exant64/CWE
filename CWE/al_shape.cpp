#include "stdafx.h"
#include <Chao.h>
#include "al_parts.h"
#include "api/api_customchao.h"
#include "ChaoMain.h"

void AL_ShapeExpandElementToParam(AL_SHAPE_ELEMENT* pElement, CHAO_PARAM_GC* pParam) {
	if (pElement->type == 254) {
		pParam->type = ChaoType_Child;
		pParam->BodyType = SADXBodyType_Omochao;
	}
	else if (pElement->type == 255) {
		pParam->type = ChaoType_Child;
		pParam->BodyType = SADXBodyType_EggChao;
	}
	else {
		pParam->type = pElement->type;
		pParam->BodyType = SADXBodyType_Normal;
	}

	pParam->EyeType = pElement->DefaultEyeNum;
	pParam->MouthType = pElement->DefaultMouthNum;
	pParam->BallType = pElement->HonbuNum;
	pParam->Headgear = pElement->ObakeHead;
	pParam->HideFeet = pElement->ObakeBody;
	pParam->Medal = pElement->MedalNum;
	pParam->Color = pElement->ColorNum;
	pParam->MonotoneHighlights = pElement->NonTex;
	pParam->Texture = pElement->JewelNum;
	pParam->Shiny = pElement->MultiNum;

	memcpy(pParam->PartsBTL.MinimalParts, pElement->MinimalParts, sizeof(pElement->MinimalParts));

 	pParam->PowerRun = pElement->HPos / 10000.f;
    pParam->FlySwim = pElement->VPos / 10000.f;
    pParam->Alignment = pElement->APos / 10000.f;
    pParam->EvolutionProgress = pElement->Growth / 10000.f;

	memcpy(GET_CWEPARAM(pParam)->Name, pElement->name, sizeof(pElement->name));
	GET_CWEPARAM(pParam)->Name[7] = 0;
	memcpy(pParam->name, pElement->name, sizeof(pParam->name));

	memcpy(pParam->Skill, pElement->Skill, sizeof(pParam->Skill));
}
static void __declspec(naked) AL_ShapeExpandElementToParam_Hook()
{
	__asm
	{
		push ecx // data
		push eax // KarateOpponentData

		// Call your __cdecl function here:
		call AL_ShapeExpandElementToParam

		pop eax // KarateOpponentData
		pop ecx // data
		retn
	}
}

const int sub_56CF40Ptr = 0x56CF40;
static int AL_ShapeChangeType(task* a1, int typevalue)
{
	int retval;
	__asm
	{
		mov ecx, a1
		mov eax, typevalue
		call sub_56CF40Ptr
		mov retval, eax
	}
	return retval;
}

int __cdecl AL_ShapeChangeType_Hack(task* tp, int type) {
	CHAO_PARAM_GC* pParam = GET_CHAOPARAM(tp);
	int mini = 1;
	int minicount = 21;

	for (int i = 0; i < minicount; i++) {
		if (!AL_PartsIsMinimalFlagOn(tp, i)) {
			mini = 0;
			break;
		}
	}

	if (gConfigVal.CharacterChaoEvo && mini && pParam->like > 80 && 
		pParam->nbSucceed >= 2)
	{
		switch (type) {
		case ChaoType_Neutral_Fly:
		case ChaoType_Hero_Fly:
		case ChaoType_Dark_Fly:
			type = ChaoType_Tails;
			break;

		case ChaoType_Neutral_Power:
		case ChaoType_Hero_Power:
		case ChaoType_Dark_Power:
			type = ChaoType_Knuckles;
			break;

		case ChaoType_Neutral_Run:
		case ChaoType_Hero_Run:
		case ChaoType_Dark_Run:
			type = ChaoType_Amy;
			break;
		}
	}

	if(gConfigVal.CustomChaoEvo) {
		for (size_t i = 0; i < CustomChaoTypeEntries.size(); i++) {
			CustomChaoEntry& entry = CustomChaoTypeEntries[i];

			if (entry.Data.pEvolveFunc && entry.Data.pEvolveFunc(tp)) {
				auto pParam = GET_CWEPARAM(tp);

				//set type
				type = 26;
				memcpy(pParam->TypeID, entry.Data.ID, sizeof(pParam->TypeID));

				break;
			}
		}
	}

	return AL_ShapeChangeType(tp, type);
}

static void __declspec(naked) AL_ShapeChangeType_Hook()
{
	__asm
	{
		push eax // int a2
		push ecx // a1

		// Call your __cdecl function here:
		call AL_ShapeChangeType_Hack

		pop ecx // a1
		add esp, 4 // int a2
		retn
	}
}


void AL_Shape_Init() {
	WriteJump((void*)0x536550, AL_ShapeExpandElementToParam_Hook);
	//Custom Character Chao
	WriteCall((void*)0x00568E10, AL_ShapeChangeType_Hook);
}