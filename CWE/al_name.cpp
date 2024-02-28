#include "stdafx.h"
#include <Chao.h>
#include <util.h>
#include <alg_kinder_ortho.h>

VoidFunc(sub_583C60, 0x583C60);

const int DisplayChaoNamePtr = 0x00536BE0;
void DisplayChaoName(const char* a1, float a2, float a3, float a4, float a5, int a6, int a7, int a8)
{
	__asm
	{
		push a7
		push a6
		push a5
		push a4
		push a3
		push a2
		push a1
		mov ecx, a8
		call DisplayChaoNamePtr
		add esp, 4 * 7
	}
}

float textSizeMul = 13;
float charSizeMul = 1; //1.75 for right pic
bool enableCharWidthMul = false;
float posYOffset = 0;

void __cdecl DisplayChaoName_ScaleLength(int a1, float a2, float a3, float a4, float a5, int a6, int a7, int a8)
{
	const char* name = (const char*)(a1 + (offsetof(ChaoDataBase, Name) - 0x12));
	int length = strlen(name);
	posYOffset = 0;
	if (length > 7)
	{
		enableCharWidthMul = true;
		float t = (length - 7) / 5.0f;
		a4 = lerp(a4 - 2, textSizeMul, t);
		a5 = lerp(a5 - 2, textSizeMul, t);
		charSizeMul = lerp(1, 2, t);
		posYOffset = 3.0f * t;
	}
	DisplayChaoName(name, a2, a3, a4, a5, a6, 1, a8);
	enableCharWidthMul = false;
}
static void __declspec(naked) DisplayChaoName_Hook()
{
	__asm
	{
		push ecx // a8
		push[esp + 20h] // a7
		push[esp + 20h] // a6
		push[esp + 20h] // a5
		push[esp + 20h] // a4
		push[esp + 20h] // a3
		push[esp + 20h] // a2
		push[esp + 20h] // a1

		// Call your __cdecl function here:
		call DisplayChaoName_ScaleLength

		add esp, 4 // a1
		add esp, 4 // a2
		add esp, 4 // a3
		add esp, 4 // a4
		add esp, 4 // a5
		add esp, 4 // a6
		add esp, 4 // a7
		pop ecx // a8
		retn
	}
}

void __cdecl DrawChar(ChaoHudThingB* a1, float a2, float a3, float a4, float a5, float a6, int a7, int a8)
{
	if (enableCharWidthMul)
	{
		a1->wd *= charSizeMul;
		a1->ht *= charSizeMul;
	}
	int fetchOriginalU = (int)((a1->s0 / 0.001953125f) - 0.1f);//(v25 + v14) * v15;
	int fetchOriginalV = (int)((a1->t0 / 0.00390625f) - 0.1f);//(v25 + v14) * v15;
	if (fetchOriginalU == 418 && fetchOriginalV == 66)
		DrawChaoHudThingB(a1, a2 - 2.5f, a3 + posYOffset, a4, a5, a6, a7, a8);
	else
		DrawChaoHudThingB(a1, a2, a3 + posYOffset, a4, a5, a6, a7, a8);
}
static void __declspec(naked) DrawChar_Hook()
{
	__asm
	{
		push[esp + 1Ch] // a8
		push[esp + 1Ch] // a7
		push[esp + 1Ch] // a6
		push[esp + 1Ch] // a5
		push[esp + 1Ch] // a4
		push[esp + 1Ch] // a3
		push[esp + 1Ch] // a2
		push eax // a1

		// Call your __cdecl function here:
		call DrawChar

		pop eax // a1
		add esp, 4 // a2
		add esp, 4 // a3
		add esp, 4 // a4
		add esp, 4 // a5
		add esp, 4 // a6
		add esp, 4 // a7
		add esp, 4 // a8
		retn
	}
}
FastcallFunctionPointer(void, sub_57A6F0, (char* a1, int a2), 0x57A6F0);
void __cdecl sub_58DA30(int a1, int a2)
{
	ChaoDataBase* v2; // eax
	AL_NAME a1a; // [esp+0h] [ebp-1Ch] BYREF
	__int16 a2a[sizeof(AL_NAME)]; // [esp+8h] [ebp-14h] BYREF

	v2 = *(ChaoDataBase**)(*(int*)(*(int*)(a2 + 8) + 52) + 92);
	memcpy(a1a, v2->Name, sizeof(AL_NAME));
	sub_57A6F0(a1a, (int)a2a);
	WcConvFromCStr((int)a1, (int)a2a, TextLanguage == 0);
}
static void __declspec(naked) sub_58DA30Hook()
{
	__asm
	{
		push[esp + 04h] // a2
		push esi // a1

		// Call your __cdecl function here:
		call sub_58DA30

		pop esi // a1
		add esp, 4 // a2
		retn
	}
}
void __cdecl FortuneTeller_SetName(char* a1, char* a2, unsigned int a3)
{
	char* name = (char*)((int)a1 + (offsetof(ChaoDataBase, Name) - 0x12));
	char* menuStr = (char*)((int)a2 + (0x60 - 0x48));
	memset(name, 0, sizeof(AL_NAME));
	memcpy(name, menuStr, strlen(menuStr));
}

void __cdecl OpenNameMenu(char* NazukeyaBuff)
{
	NazukeyaBuff[80] = 1;
	NazukeyaBuff[81] = 0;
	int ptr = (int)NazukeyaBuff;
	ObjectMaster* chao = *(ObjectMaster**)(ptr + 0x1C);
	memcpy(&NazukeyaBuff[0x60], chao->Data1.Chao->pParamGC->Name, sizeof(AL_NAME));
	NazukeyaBuff[0x4F] = 0;
	int v4 = strlen((const char*)(&NazukeyaBuff[0x60]));
	NazukeyaBuff[82] = v4;
	NazukeyaBuff[83] = v4;
	*(short*)&NazukeyaBuff[84] = 2;
	*(short*)&NazukeyaBuff[86] = 7;
	*(short*)&NazukeyaBuff[88] = 16;
	*(short*)&NazukeyaBuff[90] = 0;
	*(short*)&NazukeyaBuff[92] = 0;
}
static void __declspec(naked) OpenNameMenuHook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call OpenNameMenu

		pop eax // a1
		retn
	}
}

void __cdecl AlgKinderOrthoQuadDraw_Name(ChaoHudThing* a1, int a2)
{
	int i = (a1->x0 - 433) / 22;
	a1->x0 = 13 * i + 433;
	a1->x1 = 13 * i + 450;
	a1->y0 = (short)(a1->y0 * 0.75f);
	a1->y1 = (short)(a1->y1 * 0.75f);
	a1->y0 += 35;
	a1->y1 += 35;
	//AlgKinderOrthoQuadDrawArray(a1, 1, 0xFFFFFFFF);
	ChaoHudThingB hudreplacement;
	hudreplacement.adjust = 1;
	hudreplacement.pTexlist = (NJS_TEXLIST*)0x01366ABC;
	hudreplacement.TexNum = 1;
	hudreplacement.s0 = a1->u0 / 4096.f;
	hudreplacement.s1 = a1->u1 / 4096.f;
	hudreplacement.t0 = a1->v0 / 4096.f;
	hudreplacement.t1 = a1->v1 / 4096.f;
	hudreplacement.wd = (float)(a1->x1 - a1->x0);
	hudreplacement.ht = (float)(a1->y1 - a1->y0);
	if (a1->u0 == 3348 && a1->v0 == 1064)
		DrawChaoHudThingB(&hudreplacement, a1->x0 - 2.5f, a1->y0, -1, 1, 1, -1, -1);
	else
		DrawChaoHudThingB(&hudreplacement, a1->x0, a1->y0, -1, 1, 1, -1, -1);
	*(int*)(&a1->y0) = 0x990083;
	sub_583C60();
}

static void __declspec(naked) AlgKinderOrthoQuadDraw_NameHook()
{
	__asm
	{
		push ecx // a2
		push eax // a1

		// Call your __cdecl function here:
		call AlgKinderOrthoQuadDraw_Name

		pop eax // a1
		pop ecx // a2
		retn
	}
}

AL_NAME FortuneTellerNameBuffer;

void __fastcall sub_57A6F0_(char* a1, int a2)
{
	sub_57A6F0(a1, (int)FortuneTellerNameBuffer);
}

const int WcConvFromCStrPtr = 0x0057A680;
__declspec(naked) void FoNameWcConvFromCStrHook()
{
	__asm
	{
		mov edx, offset FortuneTellerNameBuffer
		jmp WcConvFromCStrPtr
	}
}
void __cdecl sub_536BA0(const char* v6, float a1, float a2, float a3, float a4, int a5, int a6)
{
	DisplayChaoName(v6, a1, a2, a3, a4, a6, 0, a5);
}
static void __declspec(naked) sub_536BA0_Hook()
{
	__asm
	{
		push[esp + 18h] // a6
		push[esp + 18h] // a5
		push[esp + 18h] // a4
		push[esp + 18h] // a3
		push[esp + 18h] // a2
		push[esp + 18h] // a1
		push ecx // v6

		// Call your __cdecl function here:
		call sub_536BA0

		pop ecx // v6
		add esp, 4 // a1
		add esp, 4 // a2
		add esp, 4 // a3
		add esp, 4 // a4
		add esp, 4 // a5
		add esp, 4 // a6
		retn
	}
}

const int KarateOpponentNameHookTrampoline = 0x543280;
__declspec(naked) void KarateOpponentNameHook() {
	__asm {
		add edi, -0x12
		add edi, 0x624
		jmp KarateOpponentNameHookTrampoline
	}
}


void AL_Name_Init() {
	//fortune teller name conversion new buffer
	WriteCall((void*)0x005824BF, sub_57A6F0_);
	WriteCall((void*)0x05824D9, FoNameWcConvFromCStrHook);
	//name menu name draw
	WriteCall((void*)0x00582EAF, AlgKinderOrthoQuadDraw_NameHook);
	WriteData((Uint8*)(0x00582EEC - 1), (Uint8)13);
	WriteData((Uint32*)(0x00582EF1 - 4), (Uint32)435);
	//fortune teller copy name to chao
	WriteCall((void*)0x0058242E, FortuneTeller_SetName);
	WriteJump((void*)0x00582730, OpenNameMenuHook);
	//fortune teller allocation strings
	WriteData((Uint8*)(0x00583C10 - 1), Uint8(0x60 + 4 + sizeof(AL_NAME) + 4));
	WriteData((Uint8*)(0x00583C13), Uint8(0x60 + 4 + sizeof(AL_NAME)));
	//fortune teller new name offset
	WriteData((Uint8*)(0x005837F8 - 1), (Uint8)0x60);
	WriteData((Uint8*)(0x005832FD - 1), (Uint8)0x60);

	WriteData((Uint8*)(0x00583358 - 1), (Uint8)0x60);
	WriteData((Uint8*)(0x00583354 - 1), (Uint8)0x61);

	WriteData((Uint8*)(0x00582E85 - 1), (Uint8)0x60);
	WriteData((Uint8*)(0x00583142 - 1), (Uint8)0x60);
	WriteData((Uint8*)(0x0058326E - 2), (Uint8)0x60);
	WriteData((Uint8*)(0x005824BB - 1), (Uint8)0x60);

	WriteData((Uint8*)(0x0058325A - 1), (Uint8)0x5F);
	WriteData((Uint8*)(0x005831C9 - 1), (Uint8)0x5F);
	WriteData((Uint8*)(0x0058317F - 1), (Uint8)0x5F);
	WriteData((Uint8*)(0x0058317B - 1), (Uint8)0x5F);

	//new name limits
	WriteData((Uint8*)(0x005832E6), Uint8(sizeof(AL_NAME) - 1));
	WriteData((Uint8*)(0x00583134), Uint8(sizeof(AL_NAME) - 1));

	//karate name fix offset
	WriteCall((void*)0x0057904F, KarateOpponentNameHook);

	//dont delete name object if empty (i cant fix the check, because its name offset is a byte)
	WriteData<2>((char*)0x005719FF, (char)0x90);
	WriteData((char*)0x00571A01, (char)0xEB);

	WriteCall((void*)0x00536E33, DrawChar_Hook);
	WriteCall((void*)0x0565986, DisplayChaoName_Hook); //stat panel
	WriteCall((void*)0x0058832D, DisplayChaoName_Hook); //classroom

	WriteCall((void*)0x00536BCF, DisplayChaoName_Hook); //multiple things (4 calls to this function)
	//theres a single function that doesnt use Name as input and the offset hack in our hook breaks it 
	//so we have to manually redirect that one to the original one
	WriteCall((void*)0x00556A26, sub_536BA0_Hook);


	WriteCall((void*)0x00593122, DisplayChaoName_Hook); //entrance chaodata panel todo: test race panel
	WriteJump((void*)0x58DA30, sub_58DA30Hook);		//health center
	WriteCall((void*)0x00597C35, DisplayChaoName_Hook); //might be too small, cant test
	WriteCall((void*)0x00571994, DisplayChaoName_Hook); //karate1
	WriteCall((void*)0x005719A2, DisplayChaoName_Hook); //karate2
	WriteCall((void*)0x00597C35, DisplayChaoName_Hook); //stamina manager
	WriteCall((void*)0x005AD604, DisplayChaoName_Hook); //goodbye menu param window
}