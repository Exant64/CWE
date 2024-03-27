#include "stdafx.h"

#include "alg_kinder_he.h"
#include "al_odekake.h"
#include "ChaoMain.h"
#include "ninja_functions.h"
#include <vector>
#include "al_save.h"
#include "al_ode_guide.h"

#include "cwe_api.h"
#include <al_ode_menu.h>

// todo: move this maybe somewhere more appropriate
std::vector<CWE_API_ODEKAKE_ENTRY> odekakeMenuEntries;

void AL_OdekakeMove(ODE_MENU_MASTER_WORK* a1);
void AL_OdekakeName(ODE_MENU_MASTER_WORK* a1);
void AL_OdekakeStorage(ODE_MENU_MASTER_WORK* a1);
void __cdecl AL_OdekakeCustomization(ODE_MENU_MASTER_WORK* a1);

void AL_OdekakeGoodbye(ODE_MENU_MASTER_WORK* a1) {
	sub_5A6F50(a1);
}

void __cdecl AL_OdekakeMenuMaster_(ObjectMaster* a1) {
	ODE_MENU_MASTER_WORK* pOde = AL_OdekakeMenuMaster_Data_ptr;
	int stage = pOde->CurrStage;

	if (stage && odekakeMenuEntries[stage - 1].pMenuFunc)
	{
		odekakeMenuEntries[stage - 1].pMenuFunc(pOde);
	}
	else
	{
		sub_5A6C20(pOde);
	}
	pOde->state = 0;
}

ChaoDataBase* GBAManager_GetChaoDataPointer() {
	if (!AL_GBAManagerExecutor_ptr) {
		return nullptr;
	}
	
	return (ChaoDataBase*)*((int*)AL_GBAManagerExecutor_ptr + 5);
}

DataPointer(int, Odekake_EnabledButtons, 0x01DB1020);
FunctionPointer(void, sub_5AC390, (char a1, float a2, float a3, __int16 a4, int* a5), 0x5AC390);
int Odekake_EnabledButtonsCWE[255] = { 0 }; //big placeholder buffer, ill decide a max size at some point

bool AL_OdekakeIsGuest() {
	ChaoDataBase* pParam = GBAManager_GetChaoDataPointer();

	return Odekake_EnabledButtons && pParam && pParam->field_19 == 1;
}

void __cdecl AL_OdekakeButtons(char a1, float a2, float a3, __int16 a4, int* a5) {
	int isThereChao = Odekake_EnabledButtons;
	bool guest = AL_OdekakeIsGuest();

	for (size_t i = 0; i < odekakeMenuEntries.size(); i++)
	{
		const auto& entry = odekakeMenuEntries[i];

		if(entry.Flags & ODE_FLAGS_REQUIRE_CHAO)
			Odekake_EnabledButtonsCWE[i] = (isThereChao && !guest) ? 1 : 0;
		else if (entry.Flags & ODE_FLAGS_REQUIRE_NO_CHAO)
			Odekake_EnabledButtonsCWE[i] = isThereChao ? 0 : 1; //flipped
		else
			Odekake_EnabledButtonsCWE[i] = 1;

		sub_5AC390((Sint8)i, 320, 209 + (56 * ((float)i - 1)), 38 + (short)(8 * i), &Odekake_EnabledButtonsCWE[i]);
	}

	CreateButtonGuide(SELECT | CONFIRM | BACK);
}

ObjectFunc(sub_5AC010,0x5AC010);
void NewButtonDraw(ObjectMaster *a1)
{
	EntityData1* v1 = a1->Data1.Entity;
	SetChaoHUDThingBColor(*(float*)& v1->Rotation.z, 1, 1, 1);
	
	float v15, v23, v13, a1a, v22;
	a1a = *(float*)& v1->Rotation.x;
	v22 = *(float*)& v1->Rotation.y;
	ChaoHudThingB* v12 = odekakeMenuEntries[v1->field_2].ButtonText;
	if (!*(int*)(*(int*)(&v1->Scale.y)))
	{
		v12 = odekakeMenuEntries[v1->field_2].GreyButtonText;
	}
	v23 = v22 - 5;
	v13 = a1a - 44;
	DrawChaoHudThingB(v12, v13, v23, -100, 1, 1, -1, 1);
	
	if (!*(int*)(*(int*)(&v1->Scale.y)))
	{
		v15 = *(float*)0xB50FC4;
	}
	else
	{
		v15 = 0.08f;
	}
	float v16 = v15;
	float v17 = njSin(*(int*)(&v1->Position.y)) * (v1->Position.z * v16) + 1;
	float v18 = v17;
	float v19 = 2 - v17;
	float a4 = v18;
	float a1b = a1a - 72;
	DrawChaoHudThingB(odekakeMenuEntries[v1->field_2].ButtonIcon, a1b, v23, -100, a4, v19, 0, 1);
	SetChaoHUDThingBColor(1, 1, 1, 1);
}
void __cdecl ButtonDraw(ObjectMaster *a1)
{
	sub_5AC010(a1);
	NewButtonDraw(a1);
}
void sub_558BA0(int a1, int a2, int a3, float a4, float a5, float a6)
{
	DrawChaoHudThingB((ChaoHudThingB*)a3, a4, a5, a6, 1, 1, a2, a1);
}
void NewBarDraw(ObjectMaster *a1)
{

	EntityData1* v1 = a1->Data1.Entity;
	float a2 = *(float*)& v1->Rotation.x;
	float a3 = *(float*)& v1->Rotation.y;
	float  v11 = v1->Position.x;
	float a5 = v11 * 0.2f + 0.8f;
	
	if (v1->field_2 == 0)
	{
		SetChaoHUDThingBColor(1, 0, 0.5f, 0.7f);
	}
	else 
	{
		SetChaoHUDThingBColor(odekakeMenuEntries[v1->field_2 - 1].BarColorA, odekakeMenuEntries[v1->field_2 - 1].BarColorR, odekakeMenuEntries[v1->field_2 - 1].BarColorG, odekakeMenuEntries[v1->field_2 - 1].BarColorB);
	}
	float v12 = a2 - v11 * 18;
	DrawChaoHudThingB(&stru_11BA528[28], a2, a3, -99.5f, 2, 0.74375004f, -1, 0);
	a2 = *(float*)& v1->Rotation.x;
	v11 = v1->Position.x;
	a5 = v11 * 0.2f + 0.8f;
	float a6 = a5 * 0.85f;
	v12 = a2 - v11 * 18;
	a3 = *(float*)& v1->Rotation.y;
	DrawChaoHudThingB(&stru_11BA528[26], v12, a3, -99.5f, a6, a6, 0, 0);

	SetChaoHUDThingBColor(*(float*)& v1->Rotation.z, 1, 1, 1);
	int v5 = 2 * (unsigned __int8)v1->field_2 + 16;
	int v7;
	if (v5 != 16)
	{
		float v6 = a2 - 18 + 2;
		sub_558BA0(0, 0, (int)odekakeMenuEntries[v1->field_2 - 1].BarIcon, v6, a3, -99);
	}
	else
	{
		float v6 = a2 - 18 + 2;
		sub_558BA0(0, 0, (int)&stru_11BA528[v5], v6, a3, -99);
	}
	v7 = 2 * (unsigned __int8)v1->field_2 + 17;
	SetChaoHUDThingBColor(1, 1, 1, 1);
	if (v7 == 19)
	{
		v7 = 25;
	}
	float a4 = a3;
	float v15 = a2 + 32;
	if(v7 == 17)
		DrawChaoHudThingB(&stru_11BA528[v7], v15, a4, -99, 1, 1, -1, 0);
	else
		DrawChaoHudThingB(odekakeMenuEntries[v1->field_2 - 1].BarText, v15, a4, -99, 1, 1, -1, 0);
}
ObjectFunc(sub_5ABA70, 0x5ABA70);
void BarDraw(ObjectMaster* a1)
{
	sub_5ABA70(a1);
	NewBarDraw(a1);
}

// todo: maybe move these somewhere more findable?
static CWE_API_ODEKAKE_ENTRY OdekakeGoodbyeEntry = { AL_OdekakeGoodbye, nullptr, ODE_FLAGS_REQUIRE_CHAO, &stru_11BA528[33],&stru_11BA528[10],&stru_11BA528[11],&stru_11BA528[24],&stru_11BA528[25], 1.0, 0.0, 0.7265625, 0.74609375 };

// the quit entry is basically dummied out (except for the button sprites), because the menu is hardcoded to exit on the last button, so no functionality is needed for it
static CWE_API_ODEKAKE_ENTRY OdekakeQuitEntry = { nullptr, nullptr, ODE_FLAGS_NONE, &stru_11BA528[34],&stru_11BA528[12],&stru_11BA528[13],nullptr,nullptr };

void GoodbyeBar()
{
	CreateButtonGuide(SELECT | CONFIRM | BACK);
	LargeTitleBarExecutor_Load(AL_OdekakeMenuMaster_Data_ptr->CurrStage, 650.0, 66.0);
}

const int sub_582F60Ptr = 0x582F60;
void sub_582F60(char* a1)
{
	__asm
	{
		mov eax, a1
		call sub_582F60Ptr
	}
}

void __cdecl sub_582F60_CheckGuest(char* a1)
{
	ObjectMaster* chaoPtr = *(ObjectMaster**)(&a1[0x1c]);
	if (chaoPtr && chaoPtr->Data1.Chao->pParamGC->field_19 == 1 && a1[0x50])
	{
		if(*(void**)(&a1[0x44]))
			AlMsgWinAddLineC(*(KinderCoMessageThing**)(&a1[0x44]), "This is a Guest Chao.", TextLanguage == 0);
		a1[0x50] = 0;
		a1[0x51] = 2;
	}
	
	sub_582F60(a1);
}
static void __declspec(naked) sub_582F60Hook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call sub_582F60_CheckGuest

		pop eax // a1
		retn
	}
}

void AddOdekakeMenu(const CWE_API_ODEKAKE_ENTRY& entry) {
	odekakeMenuEntries.push_back(entry);
}

void AL_Odekake_Finalize() {
	// this should be the last button in the menu, always
	// it's unnecessary to have this in a separate function but i didn't know any other way to "categorize" these types of changes
	odekakeMenuEntries.push_back(OdekakeQuitEntry);
}

void AL_Odekake_Update() {
	

	unsigned char FirstMenuButton = 0;
	unsigned char LastMenuButton = (Uint8)odekakeMenuEntries.size() - 1;

	// todo: check if these are even needed
	WriteData((char*)0x005A6D81, (char)FirstMenuButton);
	WriteData((char*)0x005A6D6E, (char)FirstMenuButton);

	WriteData((char*)0x005A6E8D, (char)LastMenuButton); //"check button to exit from if pressing B" 
	WriteData((char*)0x005A6EC0, (char)LastMenuButton); //button to go to if pressing B

	WriteData((char*)0x5A6D76, (char)LastMenuButton); //selection
	WriteData((char*)0x005A6D7C, (char)LastMenuButton);
	WriteData((char*)0x005A6D9C, (char)LastMenuButton); //exit
}

void AL_Odekake_Init()
{
	//name guest check
	//WriteCall((void*)0x00583B2F, sub_582F60Hook);

	// this kills... something? in ChaoSelectMenuManager?? im assuming some weird patch related to the move menu
	WriteData<2>((char*)0x0055426D, (char)0x90);

	//disable "X" being a back button in chao select menu
	//X is used for the multisave selection in move menu
	WriteData((int*)0x005541EE, (int)Buttons_B); 

	// fixes id on bar (goodbye bar needs to use "CurrStage" instead of baked in "1") 
	WriteCall((void*)0x05A6F8E, GoodbyeBar);

	odekakeMenuEntries.clear();
	//if(cweSaveFile.transporterFlag & eTRANSPORTER::NAME)
		odekakeMenuEntries.push_back(OdekakeNameEntry);
	//if (cweSaveFile.transporterFlag & eTRANSPORTER::MOVE)
		odekakeMenuEntries.push_back(OdakakeMoveEntry);
	//if (cweSaveFile.transporterFlag & eTRANSPORTER::CUSTOMIZATION)
		odekakeMenuEntries.push_back(OdekakeCustomizationEntry);

	odekakeMenuEntries.push_back(OdekakeGuestEntry);
		
	odekakeMenuEntries.push_back(OdekakeGoodbyeEntry);

	WriteData((char*)0x005AC2BE, (char)30); //displayfixes
	WriteData((char*)0x005AC259, (char)4);
	
	WriteData<5>((char*)0x005A6CD3, (char)0x90); //kills first button so i can fully create my own

	//button new
	WriteData((int*)0x5ac3da, (int)ButtonDraw);
	WriteData<5>((char*)0x005AC2AD, (char)0x90);
	WriteData<5>((char*)0x005AC345, (char)0x90);
	//bar new
	WriteData((int*)0x5ABD75, (int)BarDraw);
	WriteData<5>((char*)0x005ABCA4, (char)0x90);
	WriteData<5>((char*)0x05ABD19, (char)0x90);
	WriteData<5>((char*)0x05ABBED, (char)0x90);
	WriteData<5>((char*)0x05ABC34, (char)0x90);

	//fix name??
	WriteData((char*)0x01314164, (char)0x4E);
	WriteData((char*)0x01314165, (char)0x45);

	//fix selection
	WriteData((int*)0x5A6DC6, (int)Odekake_EnabledButtonsCWE);

	//hook button creation
	WriteCall((void*)0x005A6CFA, AL_OdekakeButtons);

	//obviously hook the menumaster
	WriteJump((void*)AL_OdekakeMenuMaster, AL_OdekakeMenuMaster_);

	//kill the "select/confirm/back" drawing for every background tile, ported to a separate object (al_ode_guide)
	WriteCall((void*)0x005A7771, nullsub_1);
}