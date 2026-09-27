#include "stdafx.h"
#include <Chao.h>
#include "al_parts.h"
#include "api/api_customchao.h"
#include "ChaoMain.h"
#include "asmutil.h"

void AL_ShapeExpandElementToParam(AL_SHAPE_ELEMENT* pElement, CHAO_PARAM_GC* pParam) {
	if (pElement->type == 254) {
		pParam->type = TYPE_CHILD;
		pParam->body.FormNum = AL_FORM_OMOCHAO;
	}
	else if (pElement->type == 255) {
		pParam->type = TYPE_CHILD;
		pParam->body.FormNum = AL_FORM_EGG_FOOT;
	}
	else {
		pParam->type = pElement->type;
		pParam->body.FormNum = AL_FORM_NORMAL;
	}

	pParam->body.DefaultEyeNum = pElement->DefaultEyeNum;
	pParam->body.DefaultMouthNum = pElement->DefaultMouthNum;
	pParam->body.HonbuNum = pElement->HonbuNum;
	pParam->body.ObakeHead = pElement->ObakeHead;
	pParam->body.ObakeBody = pElement->ObakeBody;
	pParam->body.MedalNum = pElement->MedalNum;
	pParam->body.ColorNum = pElement->ColorNum;
	pParam->body.NonTex = pElement->NonTex;
	pParam->body.JewelNum = pElement->JewelNum;
	pParam->body.MultiNum = pElement->MultiNum;

	memcpy(pParam->PartsBTL.MinimalParts, pElement->MinimalParts, sizeof(pElement->MinimalParts));

 	pParam->body.HPos = pElement->HPos / 10000.f;
    pParam->body.VPos = pElement->VPos / 10000.f;
    pParam->body.APos = pElement->APos / 10000.f;
    pParam->body.growth = pElement->Growth / 10000.f;

	memcpy(GET_CWEPARAM(pParam)->Name, pElement->name, sizeof(pElement->name));
	GET_CWEPARAM(pParam)->Name[7] = 0;
	memcpy(pParam->name, pElement->name, sizeof(pParam->name));

	memcpy(pParam->Skill, pElement->Skill, sizeof(pParam->Skill));
}

static void ASM_FUNC AL_ShapeExpandElementToParam_Hook() {
	ASM_PUSH(ecx); // data
	ASM_PUSH(eax); // KarateOpponentData

	// Call your __cdecl function here:
	ASM_CALL (AL_ShapeExpandElementToParam);

	ASM_POP(eax); // KarateOpponentData
	ASM_POP(ecx); // data
	ASM_RET(0);
}

static ASM_FUNC int AL_ShapeChangeType(task* a1, int typevalue) {
    // arguments
    ASM_MOVE( eax, ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( ecx, ASM_ESP(1+0+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x56CF40 );

    // return
    ASM_RET( 0 );
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
		case TYPE_N_FLY:
		case TYPE_H_FLY:
		case TYPE_D_FLY:
			type = TYPE_TAILS;
			break;

		case TYPE_N_POWER:
		case TYPE_H_POWER:
		case TYPE_D_POWER:
			type = TYPE_KNUCKLES;
			break;

		case TYPE_N_RUN:
		case TYPE_H_RUN:
		case TYPE_D_RUN:
			type = TYPE_AMY;
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

static void ASM_FUNC AL_ShapeChangeType_Hook() {
	ASM_PUSH(eax); // int a2
	ASM_PUSH(ecx); // a1

	// Call your __cdecl function here:
	ASM_CALL (AL_ShapeChangeType_Hack);

	ASM_POP(ecx); // a1
	ASM_ESP_ADD( 1 ); // int a2
	ASM_RET(0);
}


void AL_Shape_Init() {
	WriteJump((void*)0x536550, (void*)AL_ShapeExpandElementToParam_Hook);
	//Custom Character Chao
	WriteCall((void*)0x00568E10, (void*)AL_ShapeChangeType_Hook);
}