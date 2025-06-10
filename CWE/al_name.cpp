#include "stdafx.h"
#include <Chao.h>
#include <util.h>
#include <alg_kinder_ortho.h>
#include "data/al_namefontspace.h"
#include <al_texlist.h>
#include <FunctionHook.h>
#include <ninja_functions.h>

enum DrawAnchorH
{
	DrawAncorV_Left = 0xFFFFFFFF,
	DrawAncorV_Center = 0x0,
	DrawAncorV_Right = 0x1,
};

enum DrawAnchorV
{
	DrawAncorV_Top = 0xFFFFFFFF,
	DrawAncorV_Middle = 0x0,
	DrawAncorV_Bottom = 0x1,
};

VoidFunc(sub_583C60, 0x583C60);

static const int DisplayChaoNamePtr = 0x00536BE0;
static void DisplayChaoName(const char* a1, float a2, float a3, float a4, float a5, int a6, int a7, int a8)
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

// used to calculate scaling if size is above the usual chao name size
static Float CalculateLastLetterXPos(const Float x, const float xsize, const char* name, const size_t length, const float sizeRatio = 1.0f) {
	Float xpos = x;

	for (size_t c = 0; c < length; c++) {
		const size_t character = name[c];

		if (character != 0 && character != 95 && character != 255)
		{
			Uint8 index;
			if (character >= 95)
			{
				index = character - 2;
			}
			else
			{
				index = character - 1;
			}

			const size_t spacing = ChaoNameFontWidth[index];
			const float spacingRatio = float(spacing) / 44.f;
			const float width = xsize * spacingRatio;
			xpos += width * sizeRatio + xsize / 22.0f;
		}
		else {
			xpos += sizeRatio * xsize / 2.f;
		}
	}

	return xpos;
}

// used by name menu to find the position of the cursor
// yes, this stinks of dupe code
static Float CalculateStringXPos(char* pName, float xpos, float xsize, size_t length, size_t selectLen = 999) {
	Float sizeRatio = 1;

	if (length > 7) {
		// we calculate the start position of the 8th letter, and calculate the size in a manner
		// that the last letter ends up there
		const float letterAtEnd = CalculateLastLetterXPos(xpos, xsize, pName, 8 - 1);
		const float letterRealAtEnd = CalculateLastLetterXPos(xpos, xsize, pName, length - 1);
		sizeRatio = (letterAtEnd - xpos) / (letterRealAtEnd - xpos);
	}

	return CalculateLastLetterXPos(xpos, xsize, pName, min(selectLen, length), sizeRatio);
}

static void DisplayChaoName_NewFont(char* name, float xpos, float ypos, float xsize, float ysize, NJS_COLOR col, int FreeStrlen, DrawAnchorH ancH) {
	const char* pName = (const char*)(name + (offsetof(ChaoDataBase, Name) - 0x12));
	const size_t length = strlen(pName);

	SetChaoHUDThingBColor(
		col.argb.a / 255.f, 
		col.argb.b / 255.f,
		col.argb.g / 255.f,
		col.argb.r / 255.f
	);

	Float xisze_ = length * (xsize - 1.0f);

	Float anchorMul = 1.0f;

	Float x;
	switch (ancH)
	{
	default:
	case DrawAncorV_Left:
		x = xpos;
		break;
	case DrawAncorV_Center:
		anchorMul = 0.5f;
		[[fallthrough]];
	case DrawAncorV_Right:
		x = xpos - (CalculateStringXPos(name, xpos, xsize, length) - xpos) * anchorMul;
		break;
	}

	Float sizeRatio = 1;

	if (length > 7) {
		// we calculate the start position of the 8th letter, and calculate the size in a manner
		// that the last letter ends up there
		const float letterAtEnd = CalculateLastLetterXPos(x, xsize, pName, 8 - 1);
		const float letterRealAtEnd = CalculateLastLetterXPos(x, xsize, pName, length - 1);
		sizeRatio = (letterAtEnd - x) / (letterRealAtEnd - x);

		// vertically center it
		ypos += (ysize / 2.f) * (1.f - sizeRatio);
	}

	ChaoHudThingB bbi;
	bbi.pTexlist = &CWE_UI_TEXLIST;
	bbi.TexNum = 35;
	bbi.adjust = 1;

	for (size_t c = 0; c < length; c++) {
		const size_t character = pName[c];

		if (character != 0 && character != 95 && character != 255)
		{
			Uint8 index;
			if (character >= 95)
			{
				index = character - 2;
			}
			else
			{
				index = character - 1;
			}

			const size_t spacing = ChaoNameFontWidth[index];
			const float spacingRatio = float(spacing) / 44.f;

			bbi.wd = xsize * spacingRatio;
			bbi.ht = ysize;

			const Float loc_x = (22 * (index % 23));
			const Float loc_y = (22 * (index / 23));
			bbi.s0 = (loc_x + 0.1f) / 512.0f;
			bbi.t0 = (loc_y + 0.35f) / 256.0f;
			bbi.s1 = (loc_x + 22.0f * spacingRatio) / 512.0f;
			bbi.t1 = (loc_y + 22.0f - .2f) / 256.0f;

			DrawChaoHudThingB(
				&bbi,
				x,
				ypos,
				-1.2,
				sizeRatio * xsize / 22.f,
				sizeRatio * ysize / 22.0f,
				DrawAncorV_Left,
				DrawAncorV_Top
			);

			x += sizeRatio * bbi.wd + xsize * (1/22.f);
		}
		else {
			x += sizeRatio * xsize / 2.f;
		}
	}
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
		call DisplayChaoName_NewFont

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

FastcallFunctionPointer(void, sub_57A6F0, (char* a1, int a2), 0x57A6F0);
static void __cdecl sub_58DA30(int a1, int a2)
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
static void __cdecl FortuneTeller_SetName(char* a1, char* a2, unsigned int a3)
{
	char* name = (char*)((int)a1 + (offsetof(ChaoDataBase, Name) - 0x12));
	char* menuStr = (char*)((int)a2 + (0x60 - 0x48));
	memset(name, 0, sizeof(AL_NAME));
	memcpy(name, menuStr, strlen(menuStr));
}

static void __cdecl OpenNameMenu(char* NazukeyaBuff)
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


AL_NAME FortuneTellerNameBuffer;

static void __fastcall sub_57A6F0_(char* a1, int a2)
{
	sub_57A6F0(a1, (int)FortuneTellerNameBuffer);
}

static const int WcConvFromCStrPtr = 0x0057A680;
static __declspec(naked) void FoNameWcConvFromCStrHook()
{
	__asm
	{
		mov edx, offset FortuneTellerNameBuffer
		jmp WcConvFromCStrPtr
	}
}
static void __cdecl sub_536BA0(const char* v6, float a1, float a2, float a3, float a4, int a5, int a6)
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

static const int KarateOpponentNameHookTrampoline = 0x543280;
static __declspec(naked) void KarateOpponentNameHook() {
	__asm {
		add edi, -0x12
		add edi, 0x624
		jmp KarateOpponentNameHookTrampoline
	}
}

static FunctionHook<void, char*> NameMenuDisplayTrampoline(0x5827A0);
static void NameMenuDisplayHook(char* work) {
	NameMenuDisplayTrampoline.Original(work);

	if (!work[0x50]) return; // is name menu open

	char* const namePointer = work + 0x60;
	const int nameLength = work[0x53];

	// blinking bar draw
	if (work[0x5C] < 20) {
		njSetTexture((NJS_TEXLIST*)0x011D2ACC); // al_stg_kinder_ad_tex
		njSetTextureNum(1, 0, 0, 0);
		ChaoHudThing bar;
		bar.x0 = CalculateStringXPos(namePointer, 431, 22.f, nameLength, work[0x52]) + 1; // work[0x52] is the selection with the arrows
		bar.x1 = bar.x0 + 2;
		bar.y0 = 130;
		bar.y1 = 156;
		bar.u1 = 1160;
		bar.u0 = 1160;
		bar.v1 = 1416;
		bar.v0 = 1416;
		AlgKinderOrthoQuadDrawArray(&bar, 1, -1);
	}

	// name draw
	if (nameLength <= 0) return;
	
	// the name menu has an arbitrary name length, so this is a hack to terminate it at the specified index
	char backupChar = namePointer[nameLength];
	namePointer[nameLength] = 0;
	// the function expects a vanilla chaodata name offset as input so we undo the hack done to it using this subtraction/addition
	DisplayChaoName_NewFont(namePointer - offsetof(ChaoDataBase, Name) + 0x12, 433, 131, 22, 22, (NJS_COLOR)-1, 0, DrawAncorV_Left);
	namePointer[nameLength] = backupChar;
}

void AL_Name_Init() {
	//fortune teller name conversion new buffer
	WriteCall((void*)0x005824BF, sub_57A6F0_);
	WriteCall((void*)0x05824D9, FoNameWcConvFromCStrHook);

	//name menu name draw
	NameMenuDisplayTrampoline.Hook(NameMenuDisplayHook);
	WriteCall((void*)0x00582EAF, nullsub_1); // kill original name sprite draw
	WriteCall((void*)0x00582F45, nullsub_1); // kill blinking cursor bar

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

	//new name limits in name menu
	WriteData((Uint8*)(0x005832E6), Uint8(sizeof(AL_NAME) - 1));
	WriteData((Uint8*)(0x00583134), Uint8(sizeof(AL_NAME) - 1));

	//karate name fix offset
	WriteCall((void*)0x0057904F, KarateOpponentNameHook);

	//dont delete name object if empty (i cant fix the check, because its name offset is a byte)
	WriteData<2>((char*)0x005719FF, (char)0x90);
	WriteData((char*)0x00571A01, (char)0xEB);

	WriteCall((void*)0x0565986, DisplayChaoName_Hook); //stat panel
	WriteCall((void*)0x0058832D, DisplayChaoName_Hook); //classroom

	WriteCall((void*)0x00536BCF, DisplayChaoName_Hook); //multiple things (4 calls to this function)

	// this uses the race record shape element name and the offset hack in our hook breaks it
	// we manually redirect the call to the original draw since it cannot exceed 7 characters anyways
	WriteCall((void*)0x00556A26, sub_536BA0_Hook);

	WriteCall((void*)0x00593122, DisplayChaoName_Hook); //entrance chaodata panel todo: test race panel
	WriteJump((void*)0x58DA30, sub_58DA30Hook);			//health center
	WriteCall((void*)0x00597C35, DisplayChaoName_Hook); //might be too small, cant test
	WriteCall((void*)0x00571994, DisplayChaoName_Hook); //karate1
	WriteCall((void*)0x005719A2, DisplayChaoName_Hook); //karate2
	WriteCall((void*)0x00597C35, DisplayChaoName_Hook); //stamina manager
	WriteCall((void*)0x005AD604, DisplayChaoName_Hook); //goodbye menu param window
}