#include "stdafx.h"
#include "alg_kinder_bl.h"
#include "al_odekake.h"
const int sub_579EF0Ptr = 0x579EF0;
void sub_579EF0(int a1, KinderCoMessageThing* a2, int a3, int a4, float a5, float a6, float a7, float a8)
{
	__asm
	{
		mov ebx, a1
		mov esi, a2
		push a8
		push a7
		push a6
		push a5
		push a4
		push a3
		call sub_579EF0Ptr
		add esp, 6 * 4
	}
}

#pragma pack(push, 8)
struct al_stg_kinder_co_data
{
	int mode;
	int timer;
	int enteringRoom;
	int lastLoadedMSGId;
	int* msgLoaded;
	int dword14;
	int gap18;
	MessageFontThing* dword1C;
	char byte20;
	char field_21;
	char field_22;
	char field_23;
	KinderCoMessageThing* field_24;
	KinderCoMessageThing* field_28;
};
#pragma pack(pop)

DataPointer(ObjectMaster*, pKinderChao, 0x01AED248);
static bool IsValidRoom(int room) {
	if (!pKinderChao) return true;

	if (pKinderChao->Data1.Chao->pParamGC->field_19 == 1 && room == 4)
		return false;

	return true;
}

char BlockedString[200];
DataArray(int, dword_8A1AF8, 0x8A1AF8, 14);
void __cdecl KindergartenText(al_stg_kinder_co_data* pCoData)
{
	if (pCoData->enteringRoom == 6)
		AlMsgFontCreateCStr(
			TextLanguage == 0,
			(int)"Credits",
			(int)pCoData->dword1C,
			640);
	else {
		char* titleString = (char*)((int)pCoData->msgLoaded + pCoData->msgLoaded[dword_8A1AF8[2 * pCoData->enteringRoom]]);
		const char* suffix = "";
		if (!IsValidRoom(pCoData->enteringRoom))
			suffix = " (blocked)";

		sprintf(BlockedString, "%s %s", titleString, suffix);

		AlMsgFontCreateCStr(
			TextLanguage == 0,
			(int)BlockedString,
			(int)pCoData->dword1C,
			640);
	}
}

static void __declspec(naked)  KindergartenTextHook()
{
	__asm {
		push edi
		call KindergartenText
		pop edi
		retn
	}
}

void __cdecl CorridorText1(const char* a1, KinderCoMessageThing* a2, signed int a3)
{
	AlMsgWinAddLineC(a2, "This Mod is made by the \x0E\xFF\x11 Chao Modders Team \xFF\x10\x0F:", a3);
}
static void __declspec(naked) CorridorText1Hook()
{
	__asm
	{
		push[esp + 04h] // int a3
		push esi // a2
		push ebx // a1

		// Call your __cdecl function here:
		call CorridorText1

		pop ebx // a1
		pop esi // a2
		add esp, 4 // int a3
		retn
	}
}

void __cdecl CorridorHeader(int a1, KinderCoMessageThing* a2, int a3, int a4, float a5, float a6, float a7, float a8)
{
	sub_579EF0(a1, a2, (int)"Chao World Extended Credit & Info", a4, a5, a6, a7, a8);
}
static void __declspec(naked) CorridorHeaderHook()
{
	__asm
	{
		push[esp + 18h] // a8
		push[esp + 18h] // a7
		push[esp + 18h] // a6
		push[esp + 18h] // a5
		push[esp + 18h] // a4
		push[esp + 18h] // a3
		push esi // a2
		push ebx // a1

		// Call your __cdecl function here:
		call CorridorHeader

		pop ebx // a1
		pop esi // a2
		add esp, 4 // a3
		add esp, 4 // a4
		add esp, 4 // a5
		add esp, 4 // a6
		add esp, 4 // a7
		add esp, 4 // a8
		retn
	}
}

static const char* CorridorText2Str = "Darkybenji, Exant, Mindacos, CGBuster, Nostalgia Ninja, AWildDayDreamer, krzys2, Erubbu, Chao Professor, Roaxes, SSF1991, Runasutaru, Justin113D and Shaddatic. Check out the website \x0E\xFF\x11 Chao Island \xFF\x10\x0F for more information and join our community for tourney event and mods to download!";
void __cdecl CorridorText2(const char* a1, KinderCoMessageThing* a2, signed int a3)
{
	AlMsgWinAddLineC(a2, CorridorText2Str,  a3);
}
static void __declspec(naked) CorridorText2Hook()
{
	__asm
	{
		push[esp + 04h] // int a3
		push esi // a2
		push ebx // a1

		// Call your __cdecl function here:
		call CorridorText2

		pop ebx // a1
		pop esi // a2
		add esp, 4 // int a3
		retn
	}
}



DataArray(ObjectMaster*, doorObjectArray, 0x01A27850, 6);
void __cdecl EnteringRoom(al_stg_kinder_co_data* pCoData, int room) {
	pCoData->enteringRoom = room;
	if (IsValidRoom(room) && ControllerPointers[0]->press & (Buttons_A | Buttons_B))
	{
		*(char*)0x0174AFFE = 0;
		*(char*)0x0174AFD6 = 1;
		doorObjectArray[room]->Data1.Entity->NextAction = 1;
		pCoData->timer = 30;
		pCoData->mode = 2;
		pCoData->dword14 = 1024;
	}
}

static void __declspec(naked) EnteringRoomHook()
{
	__asm {
		push esi
		push edi
		call EnteringRoom
		pop edi
		pop esi
		retn
	}
}

void alg_kinder_co_Init() {

	WriteCall((void*)0x00590283, EnteringRoomHook);
	WriteJump((void*)0x00590288, (void*)0x5902BF);

	//corridor CWE credits
	WriteCall((void*)0x00590416, KindergartenTextHook);
	WriteCall((void*)0x0058FC31, CorridorText1Hook);
	WriteCall((void*)0x0058FC58, CorridorText2Hook);
	WriteCall((void*)0x0058FD34, CorridorHeaderHook);
}