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

static const std::array<std::pair<int, NJS_CNK_OBJECT*>, 6> OmochaoPartModels = {{
	{HANDS, &object_omobuild_arms},
	{MOUTH, &object_omobuild_mouth},
	{LEGS, &object_omobuild_legs},
	{EYE_L, &object_omobuild_eye_l},
	{EYE_R, &object_omobuild_eye_r},
	{PROPELLER, &object_omobuild_prop}
}};

static void ALO_OmoBuildDisplayer(task* tp) {
	EggStartShaderHook();
	njPushMatrixEx();
	njSetTexture(&AL_BODY);
	njTranslateEx(&tp->twp->pos);
	njRotateY(NULL, tp->twp->ang.y);
	
	OMOCHAO_INFO* omoinf = GetOmoData();
	
	if (omoinf->phase & MANNEQUIN) {
		if (!(omoinf->phase & LEGS)) {
			njTranslate(NULL, 0, -0.575f, 0);
		}

		ChaoColoring(omoinf->jewel, omoinf->color, omoinf->shiny, omoinf->monotone, 0, object_omobuild_base.model);
		chCnkDrawObject(&object_omobuild_base);
		SetChunkTexIndexNull(2);
		ChunkMatEnable = 0;
		ChunkMatFlag = 0;
	}

	for (size_t i = 0; i < OmochaoPartModels.size(); i++) {
		if (omoinf->phase & OmochaoPartModels[i].first) {
			chCnkDrawObject(OmochaoPartModels[i].second);
		}
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

static void ALO_OmoBuildExecutor(task* tp) {
	if (GetOmoData()->phase == 255) {
		CHAO_SAVE_INFO* pChaoInfo = AL_GetNewChaoSaveInfo();
		if (pChaoInfo) {
			pChaoInfo->param.type = TYPE_CHILD;
			pChaoInfo->param.body.FormNum = AL_FORM_OMOCHAO;
			pChaoInfo->param.place = AL_GetStageNumber();
			AL_GeneCreate(&pChaoInfo->param.gene);
			pChaoInfo->param.gene.Color[0] = GetOmoData()->color;
			pChaoInfo->param.gene.Color[1] = GetOmoData()->color;
			pChaoInfo->param.body.ColorNum	= GetOmoData()->color;

			pChaoInfo->param.gene.Multi[0] = GetOmoData()->shiny;
			pChaoInfo->param.gene.Multi[1] = GetOmoData()->shiny;
			pChaoInfo->param.body.MultiNum	= GetOmoData()->shiny;

			pChaoInfo->param.gene.NonTex[0]		= GetOmoData()->monotone;
			pChaoInfo->param.gene.NonTex[1]		= GetOmoData()->monotone;
			pChaoInfo->param.body.NonTex	= GetOmoData()->monotone;

			pChaoInfo->param.gene.Jewel[0] = GetOmoData()->jewel;
			pChaoInfo->param.gene.Jewel[1] = GetOmoData()->jewel;
			pChaoInfo->param.body.JewelNum		= GetOmoData()->jewel;

			CreateChaoExtra(&pChaoInfo->param, 0, 0, &tp->twp->pos, tp->twp->ang.y);

			memset(GetOmoData(), 0, sizeof(OMOCHAO_INFO));
			tp->exec = DestroyTask;

			return;
		}
	}

	if (GetOmoData()->phase & MANNEQUIN) {
		CCL_Enable(tp, 0);
	}
	else {
		CCL_Disable(tp,0);
	}

	CCL_Entry(tp);

	if (GetOmoData()->phase & MANNEQUIN) {
		sub_540FD0(&tp->twp->pos, 0.235f);
	}
}

static CCL_INFO omoColli = { 0, 0, 0x77, 0xC, 32768, { 0.0,  1.0,  0.0 },  2.0,  0.0,  0.0, 0, 0, 0, 0 };
void ALO_OmoBuildCreate(NJS_POINT3* pPos, Angle ang) {
	task* obj = CreateElementalTask(IM_TWK, LEV_4, ALO_OmoBuildExecutor, "AL_OmoBuild");
	CCL_Init(obj, &omoColli, 1, 5);
	obj->twp->pos = *pPos;
	obj->twp->ang.y = ang;
	obj->disp = ALO_OmoBuildDisplayer;
}