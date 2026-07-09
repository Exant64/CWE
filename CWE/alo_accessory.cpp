#include "stdafx.h"
#include "ninja_functions.h"
#include "AL_ModAPI.h"
#include "Chao.h"
//#include "data/cwe/object_common_cnk/alo_mannequin.nja"
#include "ChaoMain.h"
#include "al_sandhole.h"
#include "al_modelcontainer.h"
#include "ALifeSDK_Functions.h"
#include "al_behavior/al_intention.h"
#include <api/api_accessory.h>
#include <save/save_item.h>
#include <al_toy_move.h>
#include <api/api_metadata.h>

#include <renderfix.h>

extern NJS_CNK_OBJECT object_ala_full_mannequin;
extern NJS_CNK_OBJECT object_alo_mannequin;

void Accessory_Display(task* a1) {
	const AccessorySaveInfo* save = (const AccessorySaveInfo*)AL_GetItemSaveInfo(a1);

	DoLighting(LightIndex);
	njPushMatrixEx();

	njTranslateEx(&a1->twp->pos);
	njRotateY(NULL, a1->twp->ang.y);

	njSetTexture(texlist_cwe_object);

	if (IsAccessoryGeneric(a1->twp->ang.x))
		chCnkDrawObject(&object_ala_full_mannequin);
	else
		chCnkDrawObject(&object_alo_mannequin);

	Control3D ctrl(0, NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);
	AccessorySetupDraw(a1->twp->ang.x, save->Colors, save->UsedColors);
	
	if(!IsAccessoryRFSupported(a1->twp->ang.x)) {
		ObjectRegistry::DrawObject<RenderFixBackwardsCompatibilityDrawObject>(ALW_CATEGORY_ACCESSORY, a1->twp->ang.x);
	}
	else {
		ObjectRegistry::DrawObject<rfCnkNormalDrawObject>(ALW_CATEGORY_ACCESSORY, a1->twp->ang.x);
	}

	if (RenderFix_IsEnabled() && a1->fwp && ChaoGlobal.CamDistShadowCutLev2 > GET_ALW_ENTRY_WORK(a1)->CamDist)
	{
		njTranslate(NULL, 0, 0.4f, 0);
	
		njScale(NULL, 1.5f, 0.7f, 1.5f);
		rfapi_core->pDraw->AL_ShadowDraw();
	}

	njPopMatrixEx();
}

DataArray(CCL_INFO, ALO_ObakeHeadExecutor_collision, 0x008A6F68, 3);

static void ALO_AccessoryDelete(task* tp) {
	CWE_ALW_CancelEntry(tp);
}

FunctionPointer(double, sub_57A7A0, (float a1), 0x57A7A0);

task* Accessory_Load(const int ID, const NJS_POINT3* pPos, const int AngY, const NJS_VECTOR* pVelo, AccessorySaveInfo* savedata) {
	task* tp = CreateElementalTask(IM_TWK, 4, ALO_ObakeHeadExecutor, "ALO_Accessory");
	taskwk* work = tp->twp;
	MOVE_WORK* move = MOV_Init(tp);

	work->pos = *pPos;
	work->ang.y = AngY;

	if (pVelo) {
		move->Velo = *pVelo;
	}

	work->ang.x = ID;

	move->Gravity = -0.05f;
	move->Offset.y = 3;
	move->BoundFloor = 0.8f;
	move->BoundFriction = 0.7f;

	move->unk = (int)sub_57A7A0(4.0);// rad

	CCL_Init(tp, ALO_ObakeHeadExecutor_collision, 3, 5u);
	ObjectMovableInitialize(work, 10);

	work->cwp->info[2].kind = CI_KIND_AL_ACCESSORY;

	tp->disp = Accessory_Display;
	tp->dest = ALO_AccessoryDelete;

	work->mode = 0;
	work->smode = 0;

	CWE_ALW_Entry(ALW_CATEGORY_ACCESSORY, tp, 0, savedata);
	ALW_SetHeldOffset(tp, 0.0f);
	ALW_SetHeldRadius(tp, 1.4f);

	if (savedata) {
		savedata->IndexID = ID;

		// clear ID first, so that it's invalid accessory if it doesn't exist
		savedata->ID[0] = 0;

		char id[METADATA_ID_SIZE];
		if (ItemMetadata::Get()->GetID(ALW_CATEGORY_ACCESSORY, ID, id)) {
			memcpy(savedata->ID, id, sizeof(savedata->ID));
		}
	}

	return tp;
}
