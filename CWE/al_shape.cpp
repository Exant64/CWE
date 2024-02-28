#include "stdafx.h"
#include <Chao.h>
#include "al_parts.h"
#include "api/api_customchao.h"

void AL_ShapeExpandElementToParam(KarateOpponent* KarateOpponentData, ChaoDataBase* data)
{
	signed int v2; // ST00_4
	__int16* v3; // eax
	char* v4; // ecx

	SADXBodyType storage = SADXBodyType_Normal;

	if ((unsigned char)(KarateOpponentData->ChaoType) == 254)
	{
		data->Type = ChaoType_Child;
		storage = SADXBodyType_Omochao;
	}
	else if ((unsigned char)(KarateOpponentData->ChaoType) == 255)
	{
		data->Type = ChaoType_Child;
		storage = SADXBodyType_EggChao;
	}
	else
		data->Type = KarateOpponentData->ChaoType;
	data->BodyType = storage;

	data->EyeType = KarateOpponentData->EyeType;
	data->MouthType = KarateOpponentData->MouthType;
	data->BallType = KarateOpponentData->BallType;
	data->Headgear = KarateOpponentData->Headgear;
	data->HideFeet = KarateOpponentData->HideFeet;
	data->Medal = KarateOpponentData->Medal;
	data->Color = KarateOpponentData->Color;
	data->MonotoneHighlights = KarateOpponentData->Monotone;
	data->Texture = KarateOpponentData->Texture;
	data->Shiny = KarateOpponentData->Shiny;
	*(int*)&data->PartsBTL.MinimalParts[0] = *(int*)&KarateOpponentData->SA2BArmType;
	*(int*)&data->PartsBTL.MinimalParts[4] = *(int*)&KarateOpponentData->SA2BLegType;
	v2 = KarateOpponentData->PowerRun;
	v3 = KarateOpponentData->StatPoints;
	v4 = (char*)data->StatPoints;
	*((float*)v4 + 0x1C) = v2 * 0.00009999999747378752f;
	*((float*)v4 + 0x1D) = *(v3 - 7) * 0.00009999999747378752f;
	*((float*)v4 + 0x1E) = *(v3 - 6) * 0.00009999999747378752f;
	*((float*)v4 + 0x22) = 0.00009999999747378752f * *(v3 - 5);
	memcpy(data->Name, KarateOpponentData->Name, sizeof(KarateOpponentData->Name));
	data->Name[7] = 0;
	memcpy(data->Name_, KarateOpponentData->Name, sizeof(KarateOpponentData->Name));

	*(int*)v4 = *(int*)v3;
	*((int*)v4 + 1) = *((int*)v3 + 1);
	*((int*)v4 + 2) = *((int*)v3 + 2);
	*((short*)v4 + 6) = v3[6];


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
static int AL_ShapeChangeType(ObjectMaster* a1, int typevalue)
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

int __cdecl AL_ShapeChangeType_Hack(ObjectMaster* tp, int type) {
	ChaoDataBase* pParam = tp->Data1.Chao->pParamGC;
	int mini = 1;
	int minicount = 21;

	for (int i = 0; i < minicount; i++) {
		if (!AL_PartsIsMinimalFlagOn(tp, i)) {
			mini = 0;
			break;
		}
	}

	if (mini && pParam->Happiness > 80 && pParam->Reincarnations >= 2)
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

	for (size_t i = 0; i < CustomChaoTypeEntries.size(); i++) {
		CustomChaoEntry& entry = CustomChaoTypeEntries[i];

		if (entry.Data.pEvolveFunc && entry.Data.pEvolveFunc(tp)) {
			//set type
			type = 26;
			memcpy(pParam->TypeID, entry.Data.ID, sizeof(pParam->TypeID));

			break;
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