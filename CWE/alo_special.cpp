#include "stdafx.h"
#include "ninja_functions.h"
#include "AL_ModAPI.h"
#include "Chao.h"
#include "ChaoMain.h"
#include "alo_special.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "al_modelcontainer.h"
#include "al_behavior/al_intention.h"
#include "renderfix.h"

extern NJS_CNK_OBJECT object_alo_mannequin;
void __cdecl ALO_Special_Display(task* a1)
{
	DoLighting(LightIndex);
	njPushMatrixEx();

	njTranslateEx(&a1->twp->pos);
	njRotateY(NULL, a1->twp->ang.y);

	//ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Special, a1->twp->Rotation.x);
	ObjectRegistry::DrawObject(ALW_CATEGORY_SPECIAL, a1->twp->ang.x);

	if (RenderFix_IsEnabled() && a1->fwp && ChaoGlobal.CamDistShadowCutLev2 > GET_ALW_ENTRY_WORK(a1)->CamDist) {
		njTranslate(NULL, 0, 0.4f, 0);
		njScale(NULL, 1, 0.7f, 1);

		rfapi_core->pDraw->AL_ShadowDraw();
	}
	njPopMatrixEx();

}

extern "C" __declspec(dllexport) task* ALO_SpecialCreate(int ID, NJS_VECTOR* position, int rotY, NJS_VECTOR* velocity, ITEM_SAVE_INFO* savedata)
{
	task* obj = ALO_ObakeHeadCreate(ID, position, rotY, velocity, savedata);
	obj->twp->cwp->info[2].kind = CI_KIND_AL_SPECIAL;
	obj->twp->ang.x = ID;
	obj->disp = ALO_Special_Display;
	GET_ALW_ENTRY_WORK(obj)->category = ALW_CATEGORY_SPECIAL;
	return obj;
}
