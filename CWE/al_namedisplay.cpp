#pragma once
#include "stdafx.h"

#include "ninja_functions.h"
#include "al_world.h"
#include "Chao.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "ChaoMain.h"
#include <brightfixapi.h>
#include "al_daynight.h"
#include "al_save.h"
#include "al_draw.h"
#include "usercall.h"
#include "ui/al_ortho.h"
#include "al_minimal.h"

auto DrawFVF_H = GenerateUsercallWrapper<void(*)(NJS_TEXTURE_VTX* a1, signed int vertexCount)>(noret, 0x781370,rEAX,rECX);
auto sub_41FDE0 = GenerateUsercallWrapper<void(*)(float* a1, int a2)>(noret, 0x41FDE0, rECX, stack4);
/*
static const void* const DrawFVF_HPtr = (void*)0x781370;
static inline void DrawFVF_H(NJS_TEXTURE_VTX* a1, signed int vertexCount)
{
	__asm
	{
		mov ecx, [vertexCount]
		mov eax, [a1]
		call DrawFVF_HPtr
	}
}

static const void* const sub_41FDE0Ptr = (void*)0x41FDE0;
static inline void sub_41FDE0(float* a1, int a2)
{
	__asm
	{
		push[a2]
		mov ecx, [a1]
		call sub_41FDE0Ptr
		add esp, 4
	}
}*/
VoidFunc(sub_4293B0, 0x4293B0);
VoidFunc(sub_4292E0, 0x4292E0);
ThiscallFunctionPointer(void, Renderer_SetRenderState, (void* renderer, int a1, int a2), 0x00869070);

DataPointer(char*, rendererthing, 0x1A557C0);
void __cdecl RESETSAMPLERSTATE(int a1)
{
	int device = *(int*)(rendererthing + 0x38);
	StdcallFunctionPointer(void, SetSamplerStateD3D, (int a1, int Sampler, int Type, int Value), (void*) * (int*)(*(int*)device + 0x114));
	SetSamplerStateD3D(device, 0, 5, a1);
	SetSamplerStateD3D(device, 0, 6, a1);
	SetSamplerStateD3D(device, 0, 7, a1);
	//SetSamplerState(a1, *(void**)0x1A557C0, 0, 0);
	//SetSamplerState(a1, *(void**)0x1A557C0, 0, 1);
	//SetSamplerState(a1, *(void**)0x1A557C0, 0, 2);

}
void __cdecl njDrawTexture3DExSetData(NJS_TEXTURE_VTX* a1, int vertexCount)
{
	sub_41FDE0(_nj_current_matrix_ptr_, 0);
	SetShaders(1);
	sub_4293B0();
	sub_4292E0();

	RESETSAMPLERSTATE(1);
	//Renderer_SetRenderState(*(void**)0x1A557C0, 4, 7);
	DrawFVF_H(a1, vertexCount);
	RESETSAMPLERSTATE(2);
}
void __cdecl njDrawTexture3DExSetData_Linear(NJS_TEXTURE_VTX* a1, int vertexCount)
{
	sub_41FDE0(_nj_current_matrix_ptr_, 0);
	SetShaders(1);
	sub_4293B0();
	sub_4292E0();

	//RESETSAMPLERSTATE(1);
	DrawFVF_H(a1, vertexCount);
	//RESETSAMPLERSTATE(2);
}
NJS_TEXTURE_VTX stru_C5ACCB0[4] =
{
	{ -0.5, -1.0, 0, 0, 0.5, 0xFFFFFFFF },
	{ -0.5,  1.0, 0, 0,  1,   0xFFFFFFFF },
	{ 0.5, -1.0, 0, 0.25, 0.5, 0xFFFFFFFF },
	{ 0.5, 1.0, 0, 0.25, 1.0, 0xFFFFFFFF }
};
NJS_TEXTURE_VTX stru_C5ACD70[4] =
{
	{ -0.5, -1.0, 0, 0.5, 0.5, 0xFFFFFFFF },
	{ -0.5,  1.0, 0, 0.5,  1,   0xFFFFFFFF },
	{ 0.5, -1.0, 0, 0.75, 0.5, 0xFFFFFFFF },
	{ 0.5, 1.0, 0, 0.75, 1.0, 0xFFFFFFFF }
};

NJS_TEXTURE_VTX stru_C5ACDD0[4] = {
{ -0.5, -0.5, 0, 0.75, 0.5, 0xFFFFFFFF},
{ -0.5, 0.5, 0, 0.75, 0.75, 0xFFFFFFFF},
{ 0.5, -0.5, 0, 1.0, 0.5, 0xFFFFFFFF},
{ 0.5, 0.5, 0, 1.0, 0.75, 0xFFFFFFFF}
};

NJS_TEXTURE_VTX stru_C5ACD10[4] = {
{ -0.5, -1.0, 0, 0.25, 0.5, 0xFFFFFFFF},
{ -0.5, 1.0, 0, 0.25, 1.0, 0xFFFFFFFF},
{ 0.5, -1.0, 0, 0.5, 0.5, 0xFFFFFFFF},
{ 0.5, 1.0, 0, 0.5, 1.0, 0xFFFFFFFF}
};

NJS_TEXTURE_VTX stru_C5ACC50[4] = {
{ -0.5, -0.5, 0, 0.75, 0, 0xFFFFFFFF},
{ -0.5, 0.5, 0, 0.75, 0.25, 0xFFFFFFFF},
{ 0.5, -0.5, 0, 1.0, 0, 0xFFFFFFFF},
{ 0.5, 0.5, 0, 1.0, 0.25, 0xFFFFFFFF}
};
NJS_TEXTURE_VTX stru_C5ACB30[4] =
{
	{ -0.5, -1.0, 0, 0, 0, 0xFFFFFFFF },
	{ -0.5,  1.0, 0, 0,  0.5,   0xFFFFFFFF },
	{ 0.5, -1.0, 0, 0.25, 0, 0xFFFFFFFF },
	{ 0.5, 1.0, 0, 0.25, 0.5, 0xFFFFFFFF }
};
NJS_TEXTURE_VTX stru_C5ACBF0[4] =
{
	{ -0.5, -1.0, 0, 0.5, 0, 0xFFFFFFFF },
	{ -0.5,  1.0, 0, 0.5,  0.5,   0xFFFFFFFF },
	{ 0.5, -1.0, 0, 0.75, 0, 0xFFFFFFFF },
	{ 0.5, 1.0, 0, 0.75, 0.5, 0xFFFFFFFF }
};
NJS_TEXTURE_VTX stru_C5ACB90[4] = {
{ -0.5, -1.0, 0, 0.25, 0, 0xFFFFFFFF},
{ -0.5, 1.0, 0, 0.25, 0.5, 0xFFFFFFFF},
{ 0.5, -1.0, 0, 0.5, 0, 0xFFFFFFFF},
{ 0.5, 1.0, 0, 0.5, 0.5, 0xFFFFFFFF}
};
void AL_NameDisplay_Panel(int length, Uint32 color)
{
	float fr13 = (length - 1) * 0.5f;
	njPushMatrixEx();
	//	njScale(0, 1, -1, 1);
	
	stru_C5ACDD0[0].col = color;
	stru_C5ACDD0[1].col = color;
	stru_C5ACDD0[2].col = color;
	stru_C5ACDD0[3].col = color;
	stru_C5ACCB0[0].col = color;
	stru_C5ACCB0[1].col = color;
	stru_C5ACCB0[2].col = color;
	stru_C5ACCB0[3].col = color;
	stru_C5ACD70[0].col = color;
	stru_C5ACD70[1].col = color;
	stru_C5ACD70[2].col = color;
	stru_C5ACD70[3].col = color;
	stru_C5ACD10[0].col = color;
	stru_C5ACD10[1].col = color;
	stru_C5ACD10[2].col = color;
	stru_C5ACD10[3].col = color;
	
	//middle part
	njPushMatrixEx();
	njTranslate(NULL, -fr13, 0, -0.005f);
	njDrawTexture3DExSetData(stru_C5ACB30, 4);
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslate(NULL, fr13, 0, -0.005f);
	njDrawTexture3DExSetData(stru_C5ACBF0, 4);
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslate(NULL, 0, 0, -0.005f);
	njScale(NULL, (float)(length - 1), 1, 1);
	njDrawTexture3DExSetData(stru_C5ACB90, 4);
	njPopMatrixEx();


	//outline part
	njPushMatrixEx();
	njTranslate(NULL, -fr13, 0, -0.005f);
	njDrawTexture3DExSetData(stru_C5ACCB0, 4);
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslate(NULL, fr13, 0, -0.005f);
	njDrawTexture3DExSetData(stru_C5ACD70, 4);
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslate(NULL, 0, 0, -0.005f);
	njScale(NULL, (float)(length - 1), 1, 1);
	njDrawTexture3DExSetData(stru_C5ACD10, 4);
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslate(NULL, 0, 1, -0.015f); //-1?
	njDrawTexture3DExSetData(stru_C5ACC50, 4);
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslate(NULL, 0, 1, -0.015f); //-1?
	njDrawTexture3DExSetData(stru_C5ACDD0, 4);
	njPopMatrixEx();

	njPopMatrixEx();
}
struct RenderInfoThing
{
	char gap0[8];
	int texparplus4;
	int Thing;
	int unknown2;
	int texparbuf;
	int unknown3;
	int unknown4;
	NJS_TEXLIST* CurrentTexlist;
	int unknown;
	int CurrentTexid;
};

DataPointer(RenderInfoThing*, RenderInfo, 0x2670544);

void njSetTextureNum_mainmemory(int texid)
{
	NJS_TEXMEMLIST* tex = (NJS_TEXMEMLIST*)RenderInfo->CurrentTexlist->textures[texid].texaddr;
	RenderInfo->texparplus4 = tex->texparambuffer + 4;
	RenderInfo->CurrentTexid = texid;
	RenderInfo->texparbuf = *(int*)tex->texparambuffer;
}
NJS_TEXTURE_VTX stru_C5ACAD0[4] = {
{ -0.5, -0.5, 0, 0, 0, 0xFFFFFFFF},
{ -0.5, 0.5, 0, 0, 1.0, 0xFFFFFFFF},
{ 0.5, -0.5, 0, 1.0, 0, 0xFFFFFFFF},
{ 0.5, 0.5, 0, 1.0, 1.0, 0xFFFFFFFF}
};
void AL_NameDisplay_Char(char c, int color)
{
	stru_C5ACAD0[0].col = color;
	stru_C5ACAD0[1].col = color;
	stru_C5ACAD0[2].col = color;
	stru_C5ACAD0[3].col = color;

	float v4 = (c & 0xF) * 0.0625f;
	float v5 = (c >> 4) * 0.0625f;
	stru_C5ACAD0[0].u = v4;
	stru_C5ACAD0[0].v = v5;

	stru_C5ACAD0[1].u = v4;
	stru_C5ACAD0[1].v = v5 + 0.0625f;

	stru_C5ACAD0[2].u = v4 + 0.0625f;
	stru_C5ACAD0[2].v = v5;
	stru_C5ACAD0[3].u = v4 + 0.0625f;
	stru_C5ACAD0[3].v = v5 + 0.0625f;
	njDrawTexture3DExSetData(stru_C5ACAD0, 4);
}

extern "C" __declspec(dllexport) void AL_NameDisplay_(char* a, NJS_VECTOR * v, float scale = 1, bool flip = false) {
	int textLength = 0;
	char* ptr = a;
	while (*ptr != 0)
	{
		textLength++;
		ptr++;
	}
	//check for spaces at the end of the name
	for (int i = textLength-1; i >= 0; i--)
	{
		if (a[i] == 0x5F) textLength--;
		else break;
	}
	if (textLength)
	{
		textLength++;
		//textLength++;
		NJS_VECTOR asd;
		sub_426CC0(_nj_current_matrix_ptr_, &asd, v, 0);

		njPushMatrixEx();
		njUnitMatrix(0);//njPushMatrix((NJS_MATRIX_PTR)0x025F02A0);

		njSetTexture((NJS_TEXLIST*)0x1366AB4);
		njSetTextureNum_mainmemory(1);

		njTranslateEx(&asd);
		njTranslate(NULL, 0, 3 + (2.5) * 2, 0);
		njScale(NULL, scale, flip ? 1 : -1, scale);
		AL_NameDisplay_Panel(textLength, 0xFF6699FF);

		njTranslate(NULL, -((textLength - 2) * 0.5f), 0, -0.0099999998f);

		njSetTextureNum_mainmemory(0);

		if (flip) {
			njScale(NULL, 1, -1, 1);
		}

		ptr = a;
		while (*ptr != 0)
		{
			AL_NameDisplay_Char(*ptr, 0xFF6699FF);
			ptr++;
			njTranslate(NULL, 1, 0, 0);
		}
		njPopMatrixEx();
	}
}
NJS_TEXANIM texanim[] = {
{128,20,64,10,0,0,  256,64, 3,0},
{128,20,64,10,0,64, 256,128,3,0},
{128,20,64,10,0,128,256,192,3,0},
{128,20,64,10,0,192,256,256,3,0}
};
NJS_SPRITE selectMenu = { {320,220,0}, 1,1, 0, &CWE_UI_TEXLIST, texanim };

NJS_TEXANIM cloudyTex[] = {
	{128,128,64,64,0,0,256,256,9,0},
	{128,128,64,64,0,0,256,256,10,0},
};
NJS_SPRITE cloudySpr = { {317,290,0}, 0.5f,0.5f, 0, &CWE_UI_TEXLIST, cloudyTex };

NJS_TEXANIM texanim2[] = {
	{256,256,128,128,0,0,256,256,0,0},
	{256,256,128,128,0,0,256,256,1,0},
	{256,256,128,128,0,0,256,256,2,0}
};
NJS_SPRITE dayNightUI = { {320,220,0}, 0.5f,0.5f, 0, &CWE_UI_TEXLIST, texanim2 };
DataArray(ChaoSomeUnknownA, stru_1DC0FC0, 0x1DC0FC0, 32);
NJS_TEXTURE_VTX emoIcon[4] =
{
	{ -1, -1.0, 0, 0, 0, 0xFFFFFFFF },
	{ -1,  1.0, 0, 0,  1,   0xFFFFFFFF },
	{ 1, -1.0, 0, 1, 0, 0xFFFFFFFF },
	{ 1, 1.0, 0, 1, 1, 0xFFFFFFFF }
};

enum {
	EMOTION_ICON_MATE = 0,
	EMOTION_ICON_HUNGER = 1,
	EMOTION_ICON_FEAR = 2,
	EMOTION_ICON_ILLNESS = 3,
	EMOTION_ICON_COUNT
};
bool AL_EmoteIconRequirement(ObjectMaster* a1, int index)
{
	ChaoDataBase* data = a1->Data1.Chao->pParamGC;

	if (!data)
		return false;

	switch (index)
	{
	case EMOTION_ICON_MATE:
		return AL_EmotionGetValue(a1, EM_ST_BREED) >= 10000;
	case EMOTION_ICON_HUNGER:
		return AL_EmotionGetValue(a1, EM_ST_HUNGER) >= 8000;
	case EMOTION_ICON_FEAR:
		return AL_EmotionGetValue(a1, EM_MD_FEAR) >= 100;
	case EMOTION_ICON_ILLNESS:
		char* sickness = &data->Emotion.CoughLevel;
		for (int i = 0; i < 6; i++)
		{
			if (sickness[i] < 0)
				return true;
		}
		break;
	}
	return false;
}

void RenderAnimals() {
	SetShaders(1);
	njPushMatrixEx();
	
	OrthoDrawBegin();
	for (int i = 0; i < 10; i++) {
		njUnitMatrix(0);
		OrthoScreenTranslate(100 + (float)(40 * i), 450);
		njRotateY(NULL, NJM_DEG_ANG(-25));
		njSetTexture(ModAPI_MinimalTexlists[i]);
		njCnkDrawObject(ModAPI_MinimalModels[i]);
	}
	OrthoDrawEnd();
	njPopMatrixEx();
}

void __cdecl AL_NameDisplayer(ObjectMaster* a1)
{
	//RenderAnimals();
	//int backup = *(int*)0x01A5579C;
	//njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	//njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	if (a1->Data1.Entity->Action == 0 && gConfigVal.EmotionDisplay)
	{
		for (int i = 0; i < 32; i++) {
			ObjectMaster* chao = stru_1DC0FC0[i].pointerToOwner;
			if (chao && 
				chao->EntityData2 && 
				chao->Data1.Entity->Position.y + 2 >= chao->EntityData2->field_DC)
			{
				NJS_VECTOR asd;
				sub_426CC0(_nj_current_matrix_ptr_, &asd, &chao->Data1.Entity->Position, 0);

				njPushMatrix((NJS_MATRIX_PTR)0x025F02A0);

				njSetTexture((NJS_TEXLIST*)&CWE_UI_TEXLIST);
				//njSetTexture((NJS_TEXLIST*)0x1366AB4);
				njTranslateEx(&asd);
				njTranslate(NULL, 3, 2.5f, 0);
				njScale(NULL, 1, -1, 1);
				for (int j = 0; j < EMOTION_ICON_COUNT; j++)
				{
					if (AL_EmoteIconRequirement(chao, j))
					{
						Uint32 color = *(Uint32*)(((int)chao->Data1.Undefined) + 0x6EC + 0x14);
						emoIcon[0].col = color;
						emoIcon[1].col = color;
						emoIcon[2].col = color;
						emoIcon[3].col = color;
						njSetTextureNum_mainmemory(j + 21);
						//njSetTextureNum_mainmemory(1);
						njDrawTexture3DExSetData_Linear(emoIcon, 4);
						njTranslate(NULL, 0, -2.5f, 0);
					}
				}
				njPopMatrixEx();
			}
		}
	}
	if (a1->Data1.Entity->Action == 1)
	{
		RESETSAMPLERSTATE(1);
		*(char*)0x25EFFCC = 0;
		//njDrawSprite2D(&chaoCamEdge_sprite, 1, 0, 0x60);
		selectMenu.p = { 320,220, 0 };
		njDrawSprite2D(&selectMenu, (a1->Data1.Entity->NextAction == 0) ? 1 : 0, 1, 0x60);
		selectMenu.p.y += 40;
		njDrawSprite2D(&selectMenu, (a1->Data1.Entity->NextAction == 1) ? 3 : 2, 1, 0x60);
		*(char*)0x25EFFCC = 1;
		RESETSAMPLERSTATE(2);
	}
	else if (a1->Data1.Entity->Action == 2)
	{
		for (int i = 0; i < 32; i++) {
			ObjectMaster* chao = stru_1DC0FC0[i].pointerToOwner;
			if (chao)
				AL_NameDisplay_(chao->Data1.Chao->pParamGC->Name, &chao->Data1.Entity->Position);
		}
	}
	else if (a1->Data1.Entity->Action == 3)
	{
		*(char*)0x25EFFCC = 0;
		
		bool isDay = gDayNightCheatPhase == PHASE_CLD || gDayNightCheatPhase == PHASE_DAY;
		auto phase = (gDayNightCheatPhase == PHASE_CLD) ? PHASE_DAY : gDayNightCheatPhase;
		njDrawSprite2D(&dayNightUI, phase, 1, 0x60);
		if(isDay)
			njDrawSprite2D(&cloudySpr, gDayNightCheatPhase != PHASE_CLD, 1, 0x60);
		
		*(char*)0x25EFFCC = 1;
	}
	//SetShaders(backup);
}
void AL_NameDisplay_Main(ObjectMaster* a1)
{
	switch (a1->Data1.Entity->Action)
	{
	case 0:
		if ((MenuButtons_Held[0] & Buttons_X) &&
			(MenuButtons_Held[0] & Buttons_Y) &&
			MainCharObj2[0] && !MainCharObj2[0]->HeldObject)
		{
			if (gConfigVal.DayNightCheat)
				a1->Data1.Entity->Action = 1;
			else
				a1->Data1.Entity->Action = 2;
		}
		break;
	case 1:
		ControlEnabled = 0;
		
		if (MenuButtons_Pressed[0] & Buttons_Up)
			a1->Data1.Entity->NextAction = 0;
		else if (MenuButtons_Pressed[0] & Buttons_Down)
			a1->Data1.Entity->NextAction = 1;


		if (MenuButtons_Pressed[0] & Buttons_A)
		{
			ControlEnabled = 1;
			a1->Data1.Entity->Action = 2 + a1->Data1.Entity->NextAction;
			a1->Data1.Entity->NextAction = 0;
		}
		break;
	
	case 2:
		a1->Data1.Entity->field_6++;
		if (a1->Data1.Entity->field_6 > 60 * 10)
		{
			a1->Data1.Entity->field_6 = 0;
			a1->Data1.Entity->Action = 0;
		}
		break;
	case 3:
		ControlEnabled = 0;

		if (MenuButtons_Pressed[0] & Buttons_Left)
			gDayNightCheatPhase = PHASE_DAY;
		else if (MenuButtons_Pressed[0] & Buttons_Right)
			gDayNightCheatPhase = PHASE_EVE;
		else if (MenuButtons_Pressed[0] & Buttons_Down)
			gDayNightCheatPhase = PHASE_NGT;

		bool isDay = gDayNightCheatPhase == PHASE_CLD || gDayNightCheatPhase == PHASE_DAY;
		if (isDay && (MenuButtons_Pressed[0] & Buttons_Y))
			gDayNightCheatPhase = (gDayNightCheatPhase == PHASE_CLD) ? PHASE_DAY : PHASE_CLD;

		if (MenuButtons_Pressed[0] & Buttons_A)
		{
			a1->Data1.Entity->field_6 = 0;
			a1->Data1.Entity->Action = 0;

			ControlEnabled = 1;
		}

		break;
	}
}

void AL_NameDisplayCreate() {
	ObjectMaster* p = LoadObject(2, "AL_NameDisplay", AL_NameDisplay_Main, LoadObj_Data1);
	p->field_28 = AL_NameDisplayer;
}