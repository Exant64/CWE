#include "stdafx.h"
#include <Chao.h>
#include <util.h>
#include <alg_kinder_ortho.h>
#include "data/al_namefontspace.h"
#include <al_texlist.h>
#include <FunctionHook.h>
#include <ninja_functions.h>
#include <al_name.h>
#include "ChaoMain.h"
#include "asmutil.h"

static ASM_FUNC void AL_ParameterDrawName(const char* a1, float a2, float a3, float a4, float a5, int a6, int a7, int a8) {
    // arguments
    ASM_PUSH(      ASM_ESP(7+0+0) ); // a7
    ASM_PUSH(      ASM_ESP(6+1+0) ); // a6
    ASM_PUSH(      ASM_ESP(5+2+0) ); // a5
    ASM_PUSH(      ASM_ESP(4+3+0) ); // a4
    ASM_PUSH(      ASM_ESP(3+4+0) ); // a3
    ASM_PUSH(      ASM_ESP(2+5+0) ); // a2
    ASM_PUSH(      ASM_ESP(1+6+0) ); // a1
    ASM_MOVE( ecx, ASM_ESP(8+7+0) ); // a8

    // call
    ASM_CALL_R( edx, 0x00536BE0 );

    // end arguments
    ASM_ESP_ADD( 7 );

    // return
    ASM_RET( 0 );
}

static float GetSpacingRatio(const size_t index) {
	if(gConfigVal.OldName) {
		return 1.f;
	}

	// a single character is 44x44 pixels
	// or atleast the table stores it as such
	return float(ChaoNameFontWidth[index]) / 44.f;
}

// finds the x position of the last letter, taking into account sizeRatio and everything
static Float CalculateLastLetterXPos(const Float x, const float xsize, const char* name, const size_t length, const float sizeRatio = 1.0f) {
	Float xpos = x;

	if(gConfigVal.OldName) {
		return xpos + length * sizeRatio * (xsize - 1);
	}
	
	for (size_t c = 0; c < length; c++) {
		const uint8_t character = name[c];

		if (character != 0 && character != 95 && character != 255)
		{
			size_t index;
			if (character >= 95)
			{
				index = character - 2;
			}
			else
			{
				index = character - 1;
			}

			const float spacingRatio = GetSpacingRatio(index);
			const float width = xsize * spacingRatio;
			xpos += width * sizeRatio + xsize / 22.0f;
		}
		else {
			xpos += sizeRatio * xsize / 2.f;
		}
	}

	return xpos;
}

// calculates the ratio to scale down by for the string to fit into the space of 8 characters
static Float CalculateStringSizeRatio(const char* pName, float xpos, float xsize, size_t length) {
	if (length > 7) {
		// we calculate the start position of the 8th letter, and calculate the size in a manner
		// that the last letter ends up there
		const float letterAtEnd = CalculateLastLetterXPos(xpos, xsize, pName, 8 - 1);
		const float letterRealAtEnd = CalculateLastLetterXPos(
			xpos, 
			xsize, 
			pName, 
			gConfigVal.OldName ? length : (length - 1)
		);
		return (letterAtEnd - xpos) / (letterRealAtEnd - xpos);
	}

	return 1.0f;
}

// used by name menu to find the position of the cursor, it's basically CalculateLastLetterXPos but with sizeRatio calculated
static Float CalculateStringXPos(const char* pName, float xpos, float xsize, size_t length, size_t selectLen = 999) {
	Float sizeRatio = CalculateStringSizeRatio(pName, xpos, xsize, length);
	return CalculateLastLetterXPos(xpos, xsize, pName, min(selectLen, length), sizeRatio);
}

void DisplayChaoName_NewFont(const char* pName, float xpos, float ypos, float xsize, float ysize, NJS_COLOR col, int FreeStrlen, DrawAnchorH ancH) {
	const size_t length = strlen(pName);

	chSetBillboardColor(
		col.argb.a / 255.f, 
		col.argb.b / 255.f,
		col.argb.g / 255.f,
		col.argb.r / 255.f
	);

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
		x = xpos - (CalculateStringXPos(pName, xpos, xsize, length) - xpos) * anchorMul;
		break;
	}

	Float sizeRatio = CalculateStringSizeRatio(pName, xpos, xsize, length);

	// vertically center when scaled down by sizeRatio
	if (sizeRatio < 1) {
		ypos += (ysize / 2.f) * (1.f - sizeRatio);
	}

	CHS_BILL_INFO bbi;

	if(gConfigVal.OldName) {
		bbi.pTexlist = (NJS_TEXLIST*)0x01366ABC;
		bbi.TexNum = 1;
	}
	else {
		bbi.pTexlist = &CWE_UI_TEXLIST;
		bbi.TexNum = 35;
	}

	bbi.adjust = 1;

	for (size_t c = 0; c < length; c++) {
		const uint8_t character = pName[c];

		const bool isSpace = character == 0 || character == 95 || character == 255;

		if (!isSpace) {
			size_t index;
			if (character >= 95)
			{
				index = character - 2;
			}
			else
			{
				index = character - 1;
			}

			const float spacingRatio = GetSpacingRatio(index);

			bbi.wd = xsize * spacingRatio;
			bbi.ht = ysize;

			// those small floats added to it are to prevent the font from bilinear interpolating towards a letter below or above it
			const Float loc_x = float(22 * (index % 23));
			const Float loc_y = float(22 * (index / 23));
			bbi.s0 = (loc_x + 0.1f) / 512.0f;
			bbi.t0 = (loc_y + 0.35f) / 256.0f;
			bbi.s1 = (loc_x + 22.0f * spacingRatio) / 512.0f;
			bbi.t1 = (loc_y + 22.0f - .2f) / 256.0f;

			chDrawBillboardSR(
				&bbi,
				x,
				ypos,
				-1.2f,
				sizeRatio * xsize / 22.f,
				sizeRatio * ysize / 22.0f,
				DrawAncorV_Left,
				DrawAncorV_Top
			);
		}

		if(gConfigVal.OldName) {
			x += sizeRatio * (xsize - 1);
			continue;
		}

		if(!isSpace) {
			x += sizeRatio * bbi.wd + xsize * (1/22.f);
		}
		else {
			x += sizeRatio * xsize / 2.f;
		}
	}
}

// hacky func that offsets the og name pointer to our new one, used for

static char* GetNewChaoDataNameLocation(char* pOriginalName) {
	return GET_CWEPARAM(pOriginalName - offsetof(CHAO_PARAM_GC, name))->Name;
}

static const char* GetNewChaoDataNameLocation(const char* pOriginalName) {
	return GetNewChaoDataNameLocation((char*)pOriginalName);
}

// this is a function that sits inbetween the draw func to 
static void DisplayChaoName_GetNewPtr(char* pName, float xpos, float ypos, float xsize, float ysize, NJS_COLOR col, int FreeStrlen, DrawAnchorH ancH) {
	DisplayChaoName_NewFont(
		GetNewChaoDataNameLocation(pName),
		xpos,
		ypos,
		xsize,
		ysize,
		col,
		FreeStrlen,
		ancH
	);
}

static void ASM_FUNC DisplayChaoName_Hook()
{
	ASM_PUSH(ecx); // a8
	ASM_PUSH(ASM_ESP(8));
	ASM_PUSH(ASM_ESP(8));
	ASM_PUSH(ASM_ESP(8));
	ASM_PUSH(ASM_ESP(8));
	ASM_PUSH(ASM_ESP(8));
	ASM_PUSH(ASM_ESP(8));
	ASM_PUSH(ASM_ESP(8));

	// Call your __cdecl function here:
	ASM_CALL (DisplayChaoName_GetNewPtr);

	ASM_ESP_ADD( 1 ); // a1
	ASM_ESP_ADD( 1 ); // a2
	ASM_ESP_ADD( 1 ); // a3
	ASM_ESP_ADD( 1 ); // a4
	ASM_ESP_ADD( 1 ); // a5
	ASM_ESP_ADD( 1 ); // a6
	ASM_ESP_ADD( 1 ); // a7
	ASM_POP(ecx); // a8
	ASM_RET(0);
}

FastcallFunctionPointer(void, sub_57A6F0, (char* a1, int a2), 0x57A6F0);
static void __cdecl sub_58DA30(int a1, int a2) {
	CHAO_PARAM_CWE* pParamCwe = GET_CWEPARAM(*(task**)(a2 + 8));

	AL_NAME name;
	__int16 nameConv[sizeof(AL_NAME)];

	memcpy(&name, pParamCwe->Name, sizeof(AL_NAME));
	sub_57A6F0(name, (int)nameConv);
	WcConvFromCStr((int)a1, (int)nameConv, Language == 0);
}
static void ASM_FUNC sub_58DA30Hook()
{
	ASM_PUSH(ASM_ESP(1)); // a2
	ASM_PUSH(esi); // a1

	// Call your __cdecl function here:
	ASM_CALL(sub_58DA30);

	ASM_POP(esi); // a1
	ASM_ESP_ADD( 1 ); // a2
	ASM_RET(0);
}
static void __cdecl FortuneTeller_SetName(char* a1, char* a2, unsigned int a3)
{
	char* name = GetNewChaoDataNameLocation(a1);
	char* menuStr = (char*)((int)a2 + (0x60 - 0x48));
	memset(name, 0, sizeof(AL_NAME));
	memcpy(name, menuStr, strlen(menuStr));
}

static void __cdecl OpenNameMenu(char* NazukeyaBuff)
{
	NazukeyaBuff[80] = 1;
	NazukeyaBuff[81] = 0;
	int ptr = (int)NazukeyaBuff;
	task* chao = *(task**)(ptr + 0x1C);
	memcpy(&NazukeyaBuff[0x60], GET_CWEPARAM(chao)->Name, sizeof(AL_NAME));
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

static void ASM_FUNC OpenNameMenuHook() {
	ASM_PUSH(eax); // a1

	// Call your __cdecl function here:
	ASM_CALL (OpenNameMenu);

	ASM_POP(eax); // a1
	ASM_RET(0);
}

AL_NAME FortuneTellerNameBuffer;

static void __fastcall sub_57A6F0_(char* a1, int a2)
{
	sub_57A6F0(a1, (int)FortuneTellerNameBuffer);
}

static void FoNameWcConvFromCStr_r(int result, int a2, signed int a3) {
	WcConvFromCStr(result, (int)FortuneTellerNameBuffer, a3);
}

static ASM_FUNC void FoNameWcConvFromCStrHook() {
	ASM_PUSH (ecx); // int a3
	ASM_PUSH (edx); // a2
	ASM_PUSH (eax); // result

	// Call your __cdecl function here:
	ASM_CALL (FoNameWcConvFromCStr_r);

	ASM_POP (eax); // result
	ASM_POP (edx); // a2
	ASM_POP (ecx); // int a3
	ASM_RET(0);
}

static void __cdecl sub_536BA0(const char* v6, float a1, float a2, float a3, float a4, int a5, int a6)
{
	AL_ParameterDrawName(v6, a1, a2, a3, a4, a6, 0, a5);
}

static void ASM_FUNC sub_536BA0_Hook() {
	ASM_PUSH(ASM_ESP(6));
	ASM_PUSH(ASM_ESP(6));
	ASM_PUSH(ASM_ESP(6));
	ASM_PUSH(ASM_ESP(6));
	ASM_PUSH(ASM_ESP(6));
	ASM_PUSH(ASM_ESP(6));
	ASM_PUSH(ecx); // v6

	// Call your __cdecl function here:
	ASM_CALL(sub_536BA0);

	ASM_POP(ecx); // v6
	ASM_ESP_ADD( 1 ); // a1
	ASM_ESP_ADD( 1 ); // a2
	ASM_ESP_ADD( 1 ); // a3
	ASM_ESP_ADD( 1 ); // a4
	ASM_ESP_ADD( 1 ); // a5
	ASM_ESP_ADD( 1 ); // a6
	ASM_RET(0);
}

static ASM_FUNC void KarateOpponentNameHook() {
	ASM_ADD(edi, -0x12);
	ASM_ADD(edi, 0x624);

	ASM_JUMP(eax, 0x543280);
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
		SAlgKinderOrthoQuad bar;
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
	const char backupChar = namePointer[nameLength];
	namePointer[nameLength] = 0;

	DisplayChaoName_NewFont(namePointer, 433, 131, 22, 22, (NJS_COLOR)-1, 0, DrawAncorV_Left);

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

	WriteCall((void*)0x00593122, DisplayChaoName_Hook); //entrance chaodata panel
	WriteJump((void*)0x58DA30, sub_58DA30Hook);			//health center
	WriteCall((void*)0x00597C35, DisplayChaoName_Hook); //might be too small, cant test
	WriteCall((void*)0x00571994, DisplayChaoName_Hook); //karate1
	WriteCall((void*)0x005719A2, DisplayChaoName_Hook); //karate2
	WriteCall((void*)0x00597C35, DisplayChaoName_Hook); //stamina manager
	WriteCall((void*)0x005AD604, DisplayChaoName_Hook); //goodbye menu param window
}