#include "stdafx.h"
#include "al_save.h"
#include "ninja_functions.h"
#include "ChaoMain.h"

#include "data/omochao/omobuild_all.h"

#include "al_draw.h"
#include <brightfixapi.h>
#include "ALifeSDK_Functions.h"
#include "ALifeSDK_Functions.h"
#include "Chao.h"
#include "al_omochao_build.h"
#include <array>
#include "al_stage.h"
#include "al_stage.h"

NJS_POINT3 OmoPositions[] = {
	{-33, 0, -153},
	{51.750145f, 0, 96.482094f},
	{-35, 0.543546f, -108.602592f}
};

uint32_t OmoRot[] = { 0x2000,0x76c9,0x1b00 };

OMOCHAO_INFO* GetOmoData()
{
	static OMOCHAO_INFO DummyOmochaoSave = { 0 };

	if (!AL_IsGarden())
		return &DummyOmochaoSave;

	return &cweSaveFile.omochao[AL_GetStageNumber() - 1];
}
extern "C"
{
	extern void EggStartShaderHook();
	extern void EggEndHook();
}

const std::array<std::pair<int, NJS_OBJECT*>, 6> OmochaoPartModels = {{
	{HANDS, &object_omobuild_arms},
	{MOUTH, &object_omobuild_mouth},
	{LEGS, &object_omobuild_legs},
	{EYE_L, &object_omobuild_eye_l},
	{EYE_R, &object_omobuild_eye_r},
	{PROPELLER, &object_omobuild_prop}
}};

void __cdecl AL_OmoBuild_Display(ObjectMaster* a1)
{
	EggStartShaderHook();
	njPushMatrixEx();
	njSetTexture(&AL_BODY);
	njTranslateEx(&a1->Data1.Entity->Position);
	njRotateY(NULL, a1->Data1.Entity->Rotation.y);
	
	OMOCHAO_INFO* omoinf = GetOmoData();
	
	if (omoinf->phase & MANNEQUIN) 
	{
		if (!(omoinf->phase & LEGS))
			njTranslate(NULL, 0, -0.575f, 0);

		ChaoColoring(omoinf->jewel, omoinf->color, omoinf->shiny, omoinf->monotone, 0, object_omobuild_base.chunkmodel);
		chCnkDrawObject(&object_omobuild_base);
		SetChunkTexIndexNull(2);
		ChunkMatEnable = 0;
		ChunkMatFlag = 0;
	}

	for (size_t i = 0; i < OmochaoPartModels.size(); i++)
	{
		if (omoinf->phase & OmochaoPartModels[i].first)
			chCnkDrawObject(OmochaoPartModels[i].second);
	}

	njPopMatrixEx();
	EggEndHook();
}

const int sub_540FD0Ptr = 0x540FD0;
void sub_540FD0(NJS_VECTOR* v, float f)
{
	__asm
	{
		mov eax, v
		push f
		call sub_540FD0Ptr
		add esp, 4
	}
}

//FunctionPointer(ChaoData*, AL_GetNewChaoSaveInfo, (), 0x00531AA0);
void __cdecl AL_OmoBuild_Main(ObjectMaster* a1)
{
	if (GetOmoData()->phase == 255)
	{
		ChaoData* v11 = (ChaoData*)AL_GetNewChaoSaveInfo();
		if (v11) 
		{
			v11->data.Type = ChaoType_Child;
			v11->data.BodyType = SADXBodyType_Omochao;
			v11->data.Garden = AL_GetStageNumber();
			InitChaoDNA(&v11->data.Gene);
			v11->data.Gene.Color[0] = GetOmoData()->color;
			v11->data.Gene.Color[1] = GetOmoData()->color;
			v11->data.Color			= GetOmoData()->color;

			v11->data.Gene.Multi[0] = GetOmoData()->shiny;
			v11->data.Gene.Multi[1] = GetOmoData()->shiny;
			v11->data.Shiny			= GetOmoData()->shiny;

			v11->data.Gene.NonTex[0]		= GetOmoData()->monotone;
			v11->data.Gene.NonTex[1]		= GetOmoData()->monotone;
			v11->data.MonotoneHighlights	= GetOmoData()->monotone;

			v11->data.Gene.Jewel[0] = GetOmoData()->jewel;
			v11->data.Gene.Jewel[1] = GetOmoData()->jewel;
			v11->data.Texture		= GetOmoData()->jewel;

			CreateChao(v11, 0, 0, &a1->Data1.Entity->Position, a1->Data1.Entity->Rotation.y);
			memset(GetOmoData(), 0, sizeof(OMOCHAO_INFO));
			a1->MainSub = DeleteObject_;
			return;
		}
		
	}

	if (GetOmoData()->phase & MANNEQUIN) 
	{
		CCL_Enable(a1, 0);
	}
	else 
		CCL_Disable(a1,0);

	AddToCollisionList(a1);
	if (GetOmoData()->phase & MANNEQUIN)
		sub_540FD0(&a1->Data1.Entity->Position, 0.235f);
}


CollisionData omoColli = { 0, 3191, 32768, {  0.0,  1.0,  0.0 },  2.0,  0.0,  0.0, 0, 0, 0, 0 };
void ALO_OmoBuildCreate(NJS_POINT3* pPos, Angle ang) {
	ObjectMaster* obj = LoadObject(4, "AL_OmoBuild", AL_OmoBuild_Main, LoadObj_Data1);
	InitCollision(obj, (CollisionData*)&omoColli, 1, 5);
	obj->Data1.Entity->Position = *pPos;
	obj->Data1.Entity->Rotation.y = ang;
	obj->DisplaySub = AL_OmoBuild_Display;
}