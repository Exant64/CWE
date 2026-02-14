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

extern NJS_OBJECT object_alo_mannequin;
void __cdecl ALO_Special_Display(ObjectMaster* a1)
{
	DoLighting(LightIndex);
	njPushMatrixEx();

	njTranslateEx(&a1->Data1.Entity->Position);
	njRotateY(NULL, a1->Data1.Entity->Rotation.y);

	//ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Special, a1->Data1.Entity->Rotation.x);
	ObjectRegistry::DrawObject(ChaoItemCategory_Special, a1->Data1.Entity->Rotation.x);

	if (RenderFix_IsEnabled() && a1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&a1->UnknownA_ptr->field_30) {
		njTranslate(NULL, 0, 0.4f, 0);
		njScale(NULL, 1, 0.7f, 1);

		rfapi_core->pDraw->AL_ShadowDraw();
	}
	njPopMatrixEx();

}

extern "C" __declspec(dllexport) ObjectMaster* ALO_Special_Load(int ID, NJS_VECTOR* position, int rotY, NJS_VECTOR* velocity, short* savedata)
{
	ObjectMaster* obj = ALO_ObakeHeadExecutor_Load(ID, position, rotY, velocity, (int)savedata);
	*(unsigned char*)&obj->Data1.Entity->Collision->CollisionArray[2].field_0 = CI_KIND_AL_SPECIAL;
	obj->Data1.Entity->Rotation.x = ID;
	obj->DisplaySub = ALO_Special_Display;
	((ChaoSomeUnknownA*)obj->UnknownA_ptr)->index = ChaoItemCategory_Special;
	return obj;
}
