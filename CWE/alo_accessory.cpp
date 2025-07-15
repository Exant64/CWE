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

extern NJS_OBJECT object_ala_full_mannequin;
extern NJS_OBJECT object_alo_mannequin;

bool ALO_IsAccessoryGeneric(int index) {
	const auto& type = ModAPI_AccessoryDataList[index].SlotType;
	return type == EAccessoryType::Generic1 || type == EAccessoryType::Generic2;
}

void Accessory_Display(ObjectMaster* a1) {
	DoLighting(LightIndex);
	njPushMatrixEx();

	njTranslateEx(&a1->Data1.Entity->Position);
	njRotateY(NULL, a1->Data1.Entity->Rotation.y);

	njSetTexture(&CWE_OBJECT_TEXLIST);

	if (ALO_IsAccessoryGeneric(a1->Data1.Entity->Rotation.x))
		chCnkDrawObject(&object_ala_full_mannequin);
	else
		chCnkDrawObject(&object_alo_mannequin);

	ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Accessory, a1->Data1.Entity->Rotation.x);

	if (a1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&a1->UnknownA_ptr->field_30)
	{
		njTranslate(NULL, 0, 0.4f, 0);
	
		njScale(NULL, 1.5f, 0.7f, 1.5f);
		DrawChaoWorldShadow();
	}

	njPopMatrixEx();
}

DataArray(CollisionData, ALO_ObakeHeadExecutor_collision, 0x008A6F68, 3);

static void ALO_AccessoryDelete(task* tp) {
	CWE_ALW_CancelEntry(tp);
}

task* Accessory_Load(const int ID, const NJS_POINT3* pPos, const int AngY, const NJS_VECTOR* pVelo, AccessorySaveInfo* savedata) {
	task* tp = LoadObject(4, "ALO_Accessory", ALO_ObakeHeadExecutor_Main, LoadObj_Data1);
	EntityData1* work = tp->Data1.Entity;
	UnknownData2* move = AllocateUnknownData2(tp);

	work->Position = *pPos;
	work->Rotation.y = AngY;

	if (pVelo) {
		move->velocity = *pVelo;
	}

	work->Rotation.x = ID;

	move->gravity = -0.05f;
	move->field_AC = 3; // Offset.y
	move->field_C8 = 0.8f; // BoundFloor
	move->field_D0 = 0.7f; // BoundFriction

	move->field_30 = (int)sub_57A7A0(4.0);// rad

	InitCollision(tp, ALO_ObakeHeadExecutor_collision, 3, 5u);
	ObjectMovableInitialize(work, 10);

	*(unsigned char*)&work->Collision->CollisionArray[2].field_0 = CI_KIND_AL_ACCESSORY;

	tp->DisplaySub = Accessory_Display;
	tp->DeleteSub = ALO_AccessoryDelete;

	work->Action = 0;
	work->NextAction = 0;

	CWE_ALW_Entry(ChaoItemCategory_Accessory, tp, 0, savedata);
	ALW_SetHeldOffset(tp, 0.0f);
	ALW_SetHeldRadius(tp, 1.4f);

	if (savedata) {
		savedata->IndexID = ID;

		// clear ID first, so that it's invalid accessory if it doesn't exist
		savedata->ID[0] = 0;

		char id[20];
		if (ItemMetadata::Get()->GetID(ChaoItemCategory_Accessory, ID, id)) {
			memcpy(savedata->ID, id, sizeof(savedata->ID));
		}
	}

	return tp;
}
