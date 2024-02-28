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

std::map<int, EAccessoryType> AccessoryTypeMap;
extern NJS_OBJECT object_ala_full_mannequin;
extern NJS_OBJECT object_alo_mannequin;

bool ALO_IsAccessoryGeneric(int type)
{
	return AccessoryTypeMap[type] == EAccessoryType::Generic1 || AccessoryTypeMap[type] == EAccessoryType::Generic2;
}

void Accessory_Display(ObjectMaster* a1)
{
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
CollisionData ALO_Accessory_collision[3] =
{
  { 256, 112, 9216, {  0,  1.5f,  0 },  15,  3,  0, 0, 0, 0, 0 },
  { 0, 3078, 0, {  0,  1.2f,  0 },  2,  0,  0, 0, 0, 0, 0 },
  { 0x194, 3184, 0, {  0, -2,  0 },  4.2f, 5, 0, 0, 0, 0, 0 }
};
ObjectMaster* Accessory_Load(int ID, NJS_VECTOR* position, int rotY, NJS_VECTOR* velocity, short* savedata)
{
	ObjectMaster* obj = ALO_ObakeHeadExecutor_Load(ID, position, rotY, velocity, (int)savedata);
	if (ID >= 256) {
		//is accessory
		*(unsigned char*)&obj->Data1.Entity->Collision->CollisionArray[2].field_0 = CI_KIND_AL_ACCESSORY;
		obj->Data1.Entity->Rotation.x = ID - 256;
		obj->DisplaySub = Accessory_Display;
		((ChaoSomeUnknownA*)obj->UnknownA_ptr)->index = ChaoItemCategory_Accessory;
	}
	return obj;
}
