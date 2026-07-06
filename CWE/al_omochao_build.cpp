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

const std::array<std::pair<int, NJS_CNK_OBJECT*>, 6> OmochaoPartModels = {{
	{HANDS, &object_omobuild_arms},
	{MOUTH, &object_omobuild_mouth},
	{LEGS, &object_omobuild_legs},
	{EYE_L, &object_omobuild_eye_l},
	{EYE_R, &object_omobuild_eye_r},
	{PROPELLER, &object_omobuild_prop}
}};

void __cdecl AL_OmoBuild_Display(task* a1)
{
	EggStartShaderHook();
	njPushMatrixEx();
	njSetTexture(&AL_BODY);
	njTranslateEx(&a1->twp->pos);
	njRotateY(NULL, a1->twp->ang.y);
	
	OMOCHAO_INFO* omoinf = GetOmoData();
	
	if (omoinf->phase & MANNEQUIN) 
	{
		if (!(omoinf->phase & LEGS))
			njTranslate(NULL, 0, -0.575f, 0);

		ChaoColoring(omoinf->jewel, omoinf->color, omoinf->shiny, omoinf->monotone, 0, object_omobuild_base.model);
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
void __cdecl AL_OmoBuild_Main(task* a1)
{
	if (GetOmoData()->phase == 255)
	{
		CHAO_SAVE_INFO* v11 = (CHAO_SAVE_INFO*)AL_GetNewChaoSaveInfo();
		if (v11) 
		{
			v11->data.type = ChaoType_Child;
			v11->data.BodyType = SADXBodyType_Omochao;
			v11->data.place = AL_GetStageNumber();
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

			CreateChao(v11, 0, 0, &a1->twp->pos, a1->twp->ang.y);
			memset(GetOmoData(), 0, sizeof(OMOCHAO_INFO));
			a1->exec = DeleteObject_;
			return;
		}
		
	}

	if (GetOmoData()->phase & MANNEQUIN) 
	{
		CCL_Enable(a1, 0);
	}
	else 
		CCL_Disable(a1,0);

	CCL_Entry(a1);
	if (GetOmoData()->phase & MANNEQUIN)
		sub_540FD0(&a1->twp->pos, 0.235f);
}


CollisionData omoColli = { 0, 3191, 32768, {  0.0,  1.0,  0.0 },  2.0,  0.0,  0.0, 0, 0, 0, 0 };
void ALO_OmoBuildCreate(NJS_POINT3* pPos, Angle ang) {
	task* obj = CreateElementalTask(LoadObj_Data1, 4, AL_OmoBuild_Main, "AL_OmoBuild");
	InitCollision(obj, (CollisionData*)&omoColli, 1, 5);
	obj->twp->pos = *pPos;
	obj->twp->ang.y = ang;
	obj->disp = AL_OmoBuild_Display;
}