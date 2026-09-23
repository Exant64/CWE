#include "al_parameter.h"
#include "stdafx.h"
#include "alg_kinder_bl.h"
#include "al_odekake.h"
#include "ChaoMain.h"
#include "asmutil.h"

static ASM_FUNC void sub_579EF0(int a1, KinderCoMessageThing* a2, int a3, int a4, float a5, float a6, float a7, float a8) {
    // save regs
    ASM_PUSH( ebx );
    ASM_PUSH( esi );

    // arguments
    ASM_PUSH(      ASM_ESP(8+0+2) ); // a8
    ASM_PUSH(      ASM_ESP(7+1+2) ); // a7
    ASM_PUSH(      ASM_ESP(6+2+2) ); // a6
    ASM_PUSH(      ASM_ESP(5+3+2) ); // a5
    ASM_PUSH(      ASM_ESP(4+4+2) ); // a4
    ASM_PUSH(      ASM_ESP(3+5+2) ); // a3
    ASM_MOVE( esi, ASM_ESP(2+6+2) ); // a2
    ASM_MOVE( ebx, ASM_ESP(1+6+2) ); // a1

    // call
    ASM_CALL_R( edx, 0x579EF0 );

    // end arguments
    ASM_ESP_ADD( 6 );

    // restore regs
    ASM_POP( esi );
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
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

static bool IsValidRoom(int doorEntry) {
	struct KinderDoorThing {
		int room;
		int spawnPos;
		int doorIndex;
		NJS_VECTOR position;
		int rotation;
	};
	DataArray(KinderDoorThing, DoorInfo, 0x8A1A50, 6);

	const int room = DoorInfo[doorEntry].room;

	DataPointer(task*, pKinderChaoTask, 0x1AED248);
	if(!pKinderChaoTask) {
		return true;
	}

	if(AL_ParameterIsGuest(pKinderChaoTask)) {
		switch(room) {
			case 3: // classroom
				return false;
			case 8: // fortune teller
				return !gConfigVal.GuestBlockNameChange;
		}
	}

	return true;
}

DataArray(int, dword_8A1AF8, 0x8A1AF8, 14);
static void KindergartenText(al_stg_kinder_co_data* pCoData) {
	static char BlockedString[200];

	if (pCoData->enteringRoom == 6) {
		AlMsgFontCreateCStr(
			Language == 0,
			(int)"Credits",
			(int)pCoData->dword1C,
			640
		);
	}
	else {
		char* titleString = (char*)((int)pCoData->msgLoaded + pCoData->msgLoaded[dword_8A1AF8[2 * pCoData->enteringRoom]]);
		const char* suffix = "";
		if (!IsValidRoom(pCoData->enteringRoom))
			suffix = " (blocked)";

		sprintf(BlockedString, "%s %s", titleString, suffix);

		AlMsgFontCreateCStr(
			Language == 0,
			(int)BlockedString,
			(int)pCoData->dword1C,
			640);
	}
}

static void ASM_FUNC KindergartenTextHook() {
	ASM_PUSH(edi);

	ASM_PUSH(edi);
	ASM_CALL (KindergartenText);
	ASM_ESP_ADD(1);

	ASM_POP(edi);
	ASM_RET(0);
}

void __cdecl CorridorText1(const char* a1, KinderCoMessageThing* a2, signed int a3)
{
	AlMsgWinAddLineC(a2, "This Mod is made by the \x0E\xFF\x11 Chao Modders Team \xFF\x10\x0F:", a3);
}

static void ASM_FUNC CorridorText1Hook() {
	ASM_PUSH(esi); // a2
	ASM_PUSH(ebx); // a1

	ASM_PUSH(ASM_ESP(3)); // int a3
	ASM_PUSH(esi); // a2
	ASM_PUSH(ebx); // a1

	// Call your __cdecl function here:
	ASM_CALL (CorridorText1);

	ASM_ESP_ADD( 1 ); // int a1
	ASM_ESP_ADD( 1 ); // int a2
	ASM_ESP_ADD( 1 ); // int a3

	ASM_POP(ebx); // a1
	ASM_POP(esi); // a2
	ASM_RET(0);
}

void __cdecl CorridorHeader(int a1, KinderCoMessageThing* a2, int a3, int a4, float a5, float a6, float a7, float a8)
{
	sub_579EF0(a1, a2, (int)"Chao World Extended Credit & Info", a4, a5, a6, a7, a8);
}

static void ASM_FUNC CorridorHeaderHook() {
	ASM_PUSH(esi); // a2
	ASM_PUSH(ebx); // a1

	ASM_PUSH(ASM_ESP(8)); // a8
	ASM_PUSH(ASM_ESP(8)); // a7
	ASM_PUSH(ASM_ESP(8)); // a6
	ASM_PUSH(ASM_ESP(8)); // a5
	ASM_PUSH(ASM_ESP(8)); // a4
	ASM_PUSH(ASM_ESP(8)); // a3
	ASM_PUSH(esi); // a2
	ASM_PUSH(ebx); // a1

	// Call your __cdecl function here:
	ASM_CALL (CorridorHeader);

	ASM_ESP_ADD( 1 ); // a3
	ASM_ESP_ADD( 1 ); // a3
	ASM_ESP_ADD( 1 ); // a3
	ASM_ESP_ADD( 1 ); // a4
	ASM_ESP_ADD( 1 ); // a5
	ASM_ESP_ADD( 1 ); // a6
	ASM_ESP_ADD( 1 ); // a7
	ASM_ESP_ADD( 1 ); // a8

	ASM_POP(ebx);
	ASM_POP(esi);
	ASM_RET(0);
}

static const char* CorridorText2Str = "Darkybenji, Exant, Mindacos, CGBuster, Nostalgia Ninja, AWildDayDreamer, krzys2, Erubbu, Chao Professor, Roaxes, SSF1991, Runasutaru, Justin113D and Shaddatic. Check out the website \x0E\xFF\x11 Chao Island \xFF\x10\x0F for more information and join our community for tourney event and mods to download!";
void __cdecl CorridorText2(const char* a1, KinderCoMessageThing* a2, signed int a3)
{
	AlMsgWinAddLineC(a2, CorridorText2Str,  a3);
}

static void ASM_FUNC CorridorText2Hook() {
	ASM_PUSH(esi); // a2
	ASM_PUSH(ebx); // a1

	ASM_PUSH(ASM_ESP(3)); // int a3
	ASM_PUSH(esi); // a2
	ASM_PUSH(ebx); // a1
	// Call your __cdecl function here:
	ASM_CALL (CorridorText2);
	ASM_ESP_ADD( 3 ); // int a3

	ASM_POP(ebx); // a1
	ASM_POP(esi); // a2
	ASM_RET(0);
}

DataArray(task*, doorObjectArray, 0x01A27850, 6);
void __cdecl EnteringRoom(al_stg_kinder_co_data* pCoData, int room) {
	pCoData->enteringRoom = room;
	if (IsValidRoom(room) && per[0]->press & (BTN_A | BTN_B))
	{
		*(char*)0x0174AFFE = 0;
		*(char*)0x0174AFD6 = 1;
		doorObjectArray[room]->twp->smode = 1;
		pCoData->timer = 30;
		pCoData->mode = 2;
		pCoData->dword14 = 1024;
	}
}

static void ASM_FUNC EnteringRoomHook() {
	ASM_PUSH(esi);
	ASM_PUSH(edi);

	ASM_PUSH(esi);
	ASM_PUSH(edi);
	ASM_CALL (EnteringRoom);
	ASM_ESP_ADD(2);

	ASM_POP(edi);
	ASM_POP(esi);
	ASM_RET(0);
}

void alg_kinder_co_Init() {

	WriteCall((void*)0x00590283, (void*)EnteringRoomHook);
	WriteJump((void*)0x00590288, (void*)0x5902BF);

	//corridor CWE credits
	WriteCall((void*)0x00590416, (void*)KindergartenTextHook);
	WriteCall((void*)0x0058FC31, (void*)CorridorText1Hook);
	WriteCall((void*)0x0058FC58, (void*)CorridorText2Hook);
	WriteCall((void*)0x0058FD34, (void*)CorridorHeaderHook);
}