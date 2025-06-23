#include "stdafx.h"

#include "ninja_functions.h"
#include "al_draw.h"
#include <brightfixapi.h>
#include "alo_accessory.h"
#include "AL_ModAPI.h"
#include "ChaoMain.h"

#include "al_palette.h"
#include "al_face.h"
#include "al_parts.h"
#include "al_look.h"
#include "al_modelcontainer.h"
#include "alo_obakehead.h"

#include <FunctionHook.h>
#include <UsercallFunctionHandler.h>

#pragma warning(push)
#pragma warning( disable: 4838 )
#include "data/al_model/al_baldroot.nja"
#include "data/al_model/al_egg_chao.nja"
#include "data/al_model/al_omochao.nja"
#include <al_daynight.h>
#include <data/debugsphere.h>
#pragma warning(pop)

extern NJS_OBJECT object_alo_missing;

const int AnimateChaoPtr = 0x0056EF80;
void AnimateChao(int a1)
{
	__asm
	{
		mov eax, a1
		call AnimateChaoPtr
	}
}

#define MASK_OBJ_STR(...) {##__VA_ARGS__, nullptr} 
const char* MaskObjObjectListDllNames[OBAKE_HEAD_PARTS_EGG][4] = {
	{nullptr},
	MASK_OBJ_STR("object_al_pumpkinhead_pumpkinhead"),
	MASK_OBJ_STR("object_al_skullhead_skullhead"),
	MASK_OBJ_STR("object_al_mask_apple_mask_apple"),
	MASK_OBJ_STR("object_al_mask_bucket_mask_bucket"),
	MASK_OBJ_STR("object_al_mask_can_mask_can"),
	MASK_OBJ_STR("object_al_mask_cdbox_mask_cdbox"),
	MASK_OBJ_STR("object_al_mask_flowerpot_mask_flowerpot"),
	MASK_OBJ_STR("object_al_mask_paperbag_mask_paperbag"),
	MASK_OBJ_STR(
		"object_al_mask_stewpan_mask_stewpan",
		"object_al_mask_stewpan_eye_mask_stewpan_eye",
		"object_al_mask_stewpan_jaw_mask_stewpan_jaw"
	),
	MASK_OBJ_STR("object_al_mask_stump_mask_stump"),
	MASK_OBJ_STR("object_al_mask_wmelon_mask_wmelon"),
	MASK_OBJ_STR("object_al_mask_wool_a_mask_wool_a"),
	MASK_OBJ_STR("object_al_mask_wool_b_mask_wool_b"),
	MASK_OBJ_STR("object_al_mask_wool_c_mask_wool_c"),
	MASK_OBJ_STR("object_al_mzsk_teethingring_mzsk_teethingring")
};
static NJS_OBJECT* VanillaMaskObjObjectList[OBAKE_HEAD_PARTS_EGG][4];
static NJS_OBJECT* object_al_egghead_egghead;
static NJS_OBJECT* object_al_egghead_eye_egghead_eye;
static NJS_OBJECT* object_al_egghead_jaw_egghead_jaw;

static void AL_Mask_Init() {
	object_al_egghead_egghead = (NJS_OBJECT*)GetDllData("object_al_egghead_egghead");
	object_al_egghead_eye_egghead_eye = (NJS_OBJECT*)GetDllData("object_al_egghead_eye_egghead_eye");
	object_al_egghead_jaw_egghead_jaw = (NJS_OBJECT*)GetDllData("object_al_egghead_jaw_egghead_jaw");

	for (size_t i = 0; i < LengthOfArray(MaskObjObjectListDllNames); i++) {
		NJS_OBJECT** pObjList = VanillaMaskObjObjectList[i];
		const char** namesPtr = MaskObjObjectListDllNames[i];

		while (*namesPtr != nullptr) {
			*pObjList = (NJS_OBJECT*)GetDllData(*namesPtr);

			namesPtr++;
			pObjList++;
		}
	}
}
void AL_DrawMaskSub(int iMask, DrawObjectFunc pDrawFunc) {
	NJS_OBJECT** pObjList = VanillaMaskObjObjectList[iMask];
	while (*pObjList != nullptr) {
		pDrawFunc(*pObjList);
		pObjList++;
	}
}

void AL_DrawMask(int iMask) {
	switch (iMask)
	{
	case OBAKE_HEAD_PARTS_NONE:
		break;

	case OBAKE_HEAD_PARTS_PUMPKIN:
	case OBAKE_HEAD_PARTS_APPLE:
	case OBAKE_HEAD_PARTS_BUCKET:
	case OBAKE_HEAD_PARTS_CAN:
	case OBAKE_HEAD_PARTS_FLOWERPOT:
	case OBAKE_HEAD_PARTS_STEWPAN:
	case OBAKE_HEAD_PARTS_STUMP:
	case OBAKE_HEAD_PARTS_WMELON:
	case OBAKE_HEAD_PARTS_WOOL_A:
	case OBAKE_HEAD_PARTS_WOOL_B:
	case OBAKE_HEAD_PARTS_WOOL_C:
	case OBAKE_HEAD_PARTS_TEETHINGRING:
		AL_DrawMaskSub(iMask, njCnkDrawObject);
		break;

	case OBAKE_HEAD_PARTS_SKULL:
	case OBAKE_HEAD_PARTS_CDBOX:
	case OBAKE_HEAD_PARTS_PAPERBAG:
		AL_DrawMaskSub(iMask, chCnkDrawObject);
		break;

	default:

		if (iMask >= 16 && iMask <= 0x54)
		{
			NJS_OBJECT* eggModel = object_al_egghead_egghead;
			njPushMatrixEx();
			njTranslate(NULL, 0.0, eggModel->pos[1], 0.0);
			ColorEggModel(eggModel->chunkmodel, iMask - 16);
			njPopMatrixEx();
			chCnkDrawObject(object_al_egghead_eye_egghead_eye);
			chCnkDrawObject(object_al_egghead_jaw_egghead_jaw);
		}
		else
		{
			//hideHat = HideHatVec[a1->Data1.Chao->pParamGC->Headgear - 85];
			njSetTexture(MaskObjObjectList[iMask - 85].second);
			chCnkDrawObject(MaskObjObjectList[iMask - 85].first);
		}
		break;
	}
}

void AL_DrawToy(ChunkObjectPointer* chunkObjectPointer)
{
	if (chunkObjectPointer->toy.model)
	{
		njSetTexture(chunkObjectPointer->toy.texlist);
		ChunkMatFlag = SecondTextureEnvironmentMap;
		njPushMatrixEx();
		if (chunkObjectPointer->useTransform)
		{
			njTranslateEx(&chunkObjectPointer->position);
			RotateZ(chunkObjectPointer->rotation.z);
			RotateY(chunkObjectPointer->rotation.y);
			RotateX(chunkObjectPointer->rotation.x);
		}
		njScale(NULL, chunkObjectPointer->toy.scale, chunkObjectPointer->toy.scale, chunkObjectPointer->toy.scale);
		chCnkDrawObject(chunkObjectPointer->toy.model);
		njPopMatrixEx();
		ChunkMatFlag = 0;
	}
}

void AL_SetMotionMatrix(ObjectMaster* a1, ChunkObjectPointer* ptr)
{
	AnimateChao((int)ptr);

	AL_LOOK* look = AL_GetLook(a1);
	if (Chao_NodeIndex == 14)
	{
		//njRotateX(NULL, look->AngX);
		
		if (look->AngY != 0) 
		{		
			//njRotateY(NULL, -a1->Data1.Entity->Rotation.y);
			//njRotateY(NULL, NJM_DEG_ANG(180) + look->AngY);
			/*
			if (a1->Data1.Entity->Rotation.y > look->AngY)
				njRotateY(NULL, a1->Data1.Entity->Rotation.y - look->AngY);
			else
				njRotateY(NULL, look->AngY - a1->Data1.Entity->Rotation.y);
				*/
		}
	}
}

void CWE_ShinyCheck(int shiny)
{
	if(gConfigVal.ForceShinyTT && shiny)
		BrightFixPlus_ShinyCheck(2);
	else 
		BrightFixPlus_ShinyCheck(shiny);
}

void ChaoColoring(int texture, int color, int shiny, int monotone, int shinyJewelMonotone, NJS_CNK_MODEL* model)
{
	// if the chao is shiny twotone, or shiny monotone apply the lerp
	if (!shinyJewelMonotone && (shiny == 2 || (shiny && monotone))) {
		AL_DayNightCycle_SetLerpShinyTexture();
	}

	if (shinyJewelMonotone && shiny && texture && monotone)
	{
		int flag = SecondTextureEnvironmentMap | HasSecondTexture | DontUseTexture;
		ChunkMatEnable = 1;
		SetChunkTexIndexPrimary(17 + texture, 1, 1);
		SetChunkTexIndexSecondary(34, 0, 1);
		SetPixelShaderFloat(78, 1);
		
		if (color)
		{
			flag |= UseChunkObjectColor;
			ChunkObjectColor = dword_1298448[color]; //dword_1298448 = chaoColors
		}
		ChunkMatFlag = flag;

		CWE_ShinyCheck(shiny);

		return;
	}

	CWE_ShinyCheck(shiny);

	int flag = 0;
	if (!model)
	{
		return;
	}
	if (shiny)
	{
		flag = SecondTextureEnvironmentMap | HasSecondTexture;
		SetChunkTexIndexPrimary(GetChunkTexIndex(model), 1, 1);

		//HACK: !texture is to fix a problem regarding Shiny jewel monotones
		//todo: check if this hack is actually still needed lol
		if (monotone && !texture)
			SetChunkTexIndexPrimary(34, 1, 1);
		else
			SetChunkTexIndexSecondary(34, 1, 1);
	}
	else if (texture > 0)
	{
		ChunkObjectColor = 0xFFFFFFFF;
		flag = SecondTextureEnvironmentMap | UseChunkObjectColor;
		SetChunkTexIndexPrimary(texture + 17, 1, 1);
	}

	if (texture == 0)
	{
		if (color > 0)
		{
			flag |= UseChunkObjectColor;
			ChunkObjectColor = dword_1298448[color];
		}
		if (monotone)
		{
			flag |= DontUseTexture;
		}
		else
		{
			SetChunkTexIndexPrimary(GetChunkTexIndex(model), 1, 1);
		}
	}
	if (flag)
	{
		ChunkMatEnable = 1;
		ChunkMatFlag = flag;
	}
	else
	{
		ChunkMatEnable = 0;
		ChunkMatFlag = 0;
	}
}

void AL_SetRareMaterial(ObjectMaster* a1, NJS_CNK_MODEL* model)
{
	ChaoColoring(
		a1->Data1.Chao->pParamGC->Texture,
		a1->Data1.Chao->pParamGC->Color,
		a1->Data1.Chao->pParamGC->Shiny,
		a1->Data1.Chao->pParamGC->MonotoneHighlights,
		a1->Data1.Chao->pParamGC->ShinyJewelMonotone,
		model);
}
int AccessoryNodeIndex = 0;
FunctionPointer(int, sub_42E660, (NJS_CNK_MODEL* a1), 0x42E660);
void DrawAccessory(ObjectMaster* a1, ChunkObjectPointer* chunkObjectPointer, NJS_OBJECT* traverseAccessory)
{
	while (chunkObjectPointer)
	{
		NJS_VECTOR backup = chunkObjectPointer->position;

		njPushMatrixEx();
		
		//????
		//ill be completely honest, i don't remember why we do this?
		if (AccessoryNodeIndex == AL_PART_HORN_L)
		{
			njTranslate(NULL, 0.016813f, 1.55422f, -1.506664f);
			RotateZ(chunkObjectPointer->base.ang[2]);
			RotateY(chunkObjectPointer->base.ang[1]);
			RotateX(chunkObjectPointer->base.ang[0]);
			if ((chunkObjectPointer->base.evalflags & 4) == 0)
				njScale(NULL, chunkObjectPointer->base.scl[0], chunkObjectPointer->base.scl[1], chunkObjectPointer->base.scl[2]);
		}
		else
			AnimateChao((int)chunkObjectPointer);

		if (AccessoryNodeIndex == 33) {
			njTranslate(NULL, 0, 0.75, 0);
		}
		
		
		if (traverseAccessory->chunkmodel)
		{
			traverseAccessory->chunkmodel->r = 0.0f;
			sub_42E660(traverseAccessory->chunkmodel);
		}
		chunkObjectPointer->position = backup;
		AccessoryNodeIndex++;
		if (chunkObjectPointer->base.child)
		{
			DrawAccessory(a1, (ChunkObjectPointer*)chunkObjectPointer->base.child, traverseAccessory->child);
		}
		njPopMatrixEx();
		
		traverseAccessory = traverseAccessory->sibling;
		chunkObjectPointer = (ChunkObjectPointer*)chunkObjectPointer->base.sibling;
	}
}

bool AL_CanRenderAccessory(ObjectMaster* a1, EAccessoryType type)
{
	return a1->Data1.Chao->pParamGC->Accessories[type] // has accessory
		&& a1->Data1.Chao->pParamGC->Accessories[type] - 1 < ObjectRegistry::Get(ChaoItemCategory_Accessory)->Size(); //valid accessory
}

void AL_RenderAccessory(ObjectMaster* a1, EAccessoryType type)
{
	if (!AL_CanRenderAccessory(a1, type)) {
		return;
	}

	ObjectRegistry::DrawObject(ChaoItemCategory_Accessory, a1->Data1.Chao->pParamGC->Accessories[type] - 1);
}

void AL_ValidateAccessory(ObjectMaster* a1, EAccessoryType type)
{
	if (a1->Data1.Chao->pParamGC->Accessories[type] && AccessoryTypeMap[a1->Data1.Chao->pParamGC->Accessories[type] - 1] >= EAccessoryType::Generic1)
		a1->Data1.Chao->pParamGC->Accessories[type] = 0;
}

bool AL_CanRenderRigAccessory(ObjectMaster* a1, EAccessoryType type)
{
	int id = a1->Data1.Chao->pParamGC->Accessories[type];
	return id &&//chao has accessory
		 AccessoryTypeMap[id - 1] == type &&//accessory matches its type
		 ObjectRegistry::Get(ChaoItemCategory_Accessory)->GetObj(id - 1); //has model
}

void AL_RenderRigAccessory(ObjectMaster* a1, ChunkObjectPointer* chunkObjectPointer, EAccessoryType type)
{
	if (AL_CanRenderRigAccessory(a1, type))
	{
		int id = a1->Data1.Chao->pParamGC->Accessories[type] - 1;

		if (ModAPI_DisableJiggle.contains(id))
			a1->Data1.Chao->field_B0 &= ~0x1000;
		else
			a1->Data1.Chao->field_B0 |= 0x1000;

		AccessoryNodeIndex = 0;

		auto registry = ObjectRegistry::Get(ChaoItemCategory_Accessory);
		njSetTexture(registry->GetTex(id));
		DrawAccessory(a1, chunkObjectPointer, registry->GetObj(id));
		sub_56E9C0(a1);
	}
}

bool IsOmochao(ObjectMaster* a1) {
	return a1->Data1.Chao->pParamGC->BodyType == 2;
}

bool IsEggChao(ObjectMaster* a1) {
	return a1->Data1.Chao->pParamGC->BodyType == 1;
}

bool EggChaoFeetCheck(ObjectMaster* a1) {
	//if not egg chao stop
	//if doesnt have hidefeet stop
	//but if it does, make sure youre not rendering arm_l and arm_r
	return !IsEggChao(a1) ||
		(!a1->Data1.Chao->pParamGC->HideFeet || (Chao_NodeIndex != AL_PART_REG_L && Chao_NodeIndex != AL_PART_REG_R));
}

void DrawOtherChao(ObjectMaster* a1, ChunkObjectPointer* chunkObjectPointer, NJS_OBJECT* model)
{
	signed int v36; // esi
	int v44; // [esp+28h] [ebp-38h]

	if (IsOmochao(a1))
	{
		if (Chao_NodeIndex == 0)
		{
			AL_ValidateAccessory(a1, EAccessoryType::Head);
			AL_ValidateAccessory(a1, EAccessoryType::Face);

			njPushMatrixEx();
			njScale(NULL, 1.1f, 1.1f, 1.1f);
			AL_RenderRigAccessory(a1, chunkObjectPointer, EAccessoryType::Generic1);
			AL_RenderRigAccessory(a1, chunkObjectPointer, EAccessoryType::Generic2);
			njPopMatrixEx();
		}
	}
	else if (IsEggChao(a1)) {
		switch (Chao_NodeIndex) {
			case AL_PART_FEELER_L:
			case AL_PART_FEELER_R:
			case AL_PART_MOUTH:
				model->chunkmodel = nullptr;
				break;
		}
	}

	while (1)
	{
		if (Chao_NodeIndex == 34) {
			model->ang[1] = 0;
		}
		else if (Chao_NodeIndex == 35) {
			model->ang[1] = 0;
			model->pos[0] = -0.028161f;
			model->pos[1] = 0.1f;
			model->pos[2] = 1.10046f;
		}

		njPushMatrixEx();
		chunkObjectPointer->differenceOrigOther = { 0,0,0 };
		*(NJS_VECTOR*)chunkObjectPointer->base.pos = *(NJS_VECTOR*)model->pos;
		//propeller and eyes
		if (Chao_NodeIndex == 29 || Chao_NodeIndex == 17 || Chao_NodeIndex == 21 || Chao_NodeIndex == 20)
		{
			njTranslateEx((NJS_VECTOR*)model->pos);
			RotateZ (model->ang[2]);
			RotateY (model->ang[1]);
			RotateX (model->ang[0]);
			if ((model->evalflags & NJD_EVAL_UNIT_SCL) == 0)
				njScale(NULL, model->scl[0], model->scl[1], model->scl[2]);
		}
		else
			AnimateChao((int)chunkObjectPointer);

		AL_DrawToy(chunkObjectPointer);

		if (IsOmochao(a1) && !a1->Data1.Chao->pParamGC->Headgear && Chao_NodeIndex == 27 && a1->Data1.Chao->Face.MouthCurrNum != 0) {
			njSetTexture(&AL_BODY);
			chCnkDrawObject(&omochao_mouth);
		}

		if (IsOmochao(a1) && a1->Data1.Chao->pParamGC->Headgear && (Chao_NodeIndex == 29 || Chao_NodeIndex == 18 || Chao_NodeIndex == 21))
			goto LABEL_98;

		//render accessory to head on omochao
		//and to the body on egg chao
		if ((Chao_NodeIndex == 16 && IsOmochao(a1)) || (Chao_NodeIndex == 1 && IsEggChao(a1)))
		{
			njPushMatrixEx();
			if (IsEggChao(a1))
			{
				njTranslate(NULL, 0, 0.6f, 0);
				njScale(NULL, 1.2f, 1.2f, 1.2f);
			}
			AL_RenderAccessory(a1, EAccessoryType::Head);
			AL_RenderAccessory(a1, EAccessoryType::Face);
			njPopMatrixEx();	

			if (IsOmochao(a1) && a1->Data1.Chao->pParamGC->Headgear) {
				njSetTexture(&AL_BODY);
				AL_DrawMask(a1->Data1.Chao->pParamGC->Headgear);
				goto LABEL_98;
			}
		}
		else if (Chao_NodeIndex == 35 && a1->Data1.Chao->pParamGC->Medal)
		{
			njPushMatrixEx();
			//njScale(2, 2, 2);
			njSetTexture((NJS_TEXLIST*)0x1366AD4);
			njCnkDrawObject(dword_12E58B8[a1->Data1.Chao->pParamGC->Medal]);
			njPopMatrixEx();
		}

		njSetTexture(&AL_BODY);
		if (IsOmochao(a1) && !a1->Data1.Chao->pParamGC->Headgear)
		{
			v36 = a1->Data1.Chao->Face.EyeLidExpressionCurrSlopeAng;
			v44 = a1->Data1.Chao->Face.EyeLidExpressionCurrCloseAng + a1->Data1.Chao->Face.EyeLidBlinkAng - 0x4000;
			switch (Chao_NodeIndex)
			{
			case 22:
				v36 = -v36;
			case 19:
				AL_SetRareMaterial(a1, chunkObjectPointer->base.chunkmodel);

				RotateZ(v36);
				RotateX(v44);

				if ((Chao_NodeIndex == 19 || Chao_NodeIndex == 22)
					|| a1->Data1.Chao->Face.EyeLidExpressionCurrCloseAng + a1->Data1.Chao->Face.EyeLidBlinkAng)
				{
					chCnkDrawObject(&omochao_eyelid);
				}

				SetPixelShaderFloat(78, 0);
				SetChunkTexIndexNull(2);
				ChunkMatEnable = 0;
				ChunkMatFlag = 0;
				break;
			}
		}
		if (IsOmochao(a1) && gConfigVal.OmochaoParts && chunkObjectPointer->animalPart && a1->Data1.Chao->pParamGC->Type < CharacterIndex)
		{
			njSetTexture((NJS_TEXLIST*)chunkObjectPointer->field_CC);
			chCnkDrawObject(chunkObjectPointer->animalPart);
		}
		else if (model->chunkmodel && EggChaoFeetCheck(a1))
		{
			if (IsOmochao(a1) && (Chao_NodeIndex == 18 || Chao_NodeIndex == 21))
			{
				njSetTexture(&OMO_EYE_TEXLIST);
				
				Sint16* texchunk = FindChunk(chunkObjectPointer->base.chunkmodel->plist, NJD_CT_TID);
				model->chunkmodel->plist[7] = texchunk[1]; //copy eye ID
			}

			if (IsEggChao(a1)) {
				//copy shiny jewel (material color)
				if (Chao_NodeIndex == 1 && a1->Data1.Chao->pParamGC->Color > 0)
					*(int*)((__int16*)model->chunkmodel->plist + 2) = *(int*)((__int16*)chunkObjectPointer->base.chunkmodel->plist + 2);

				//copy feet color and texture onto egg chao feet
				if (Chao_NodeIndex == 6 || Chao_NodeIndex == 13)
				{
					model->chunkmodel->plist[7] = chunkObjectPointer->base.chunkmodel->plist[7];
					*(int*)((__int16*)model->chunkmodel->plist + 2) = *(int*)((__int16*)chunkObjectPointer->base.chunkmodel->plist + 2);
				}
			}

			if ((IsEggChao(a1) && (Chao_NodeIndex == 1 || Chao_NodeIndex == 6 || Chao_NodeIndex == 13)) ||
				(IsOmochao(a1) && (Chao_NodeIndex == 1 || Chao_NodeIndex == 16)))
				AL_SetRareMaterial(a1, model->chunkmodel);

			chCnkDrawModel(model->chunkmodel);

			//reset shaders
			SetPixelShaderFloat(78, 0);
			SetChunkTexIndexNull(2);
			ChunkMatEnable = 0;
			ChunkMatFlag = 0;

			//the eyes
			if (IsEggChao(a1) && Chao_NodeIndex == 1)
				chCnkDrawModel(&cnk_attach_0023CBCC_002);

			//reset material color on eggchao model
			if (IsEggChao(a1) && (Chao_NodeIndex == 1 || Chao_NodeIndex == 6 || Chao_NodeIndex == 13))
				*(int*)((__int16*)model->chunkmodel->plist + 2) = 0xFFFFFFFF;
		}

		LABEL_98:
		Chao_NodeIndex++;
		if (model->child && chunkObjectPointer->base.child)
		{
			DrawOtherChao(a1, (ChunkObjectPointer*)chunkObjectPointer->base.child, model->child);
		}
		njPopMatrixEx();
		if (!model->sibling || !chunkObjectPointer->base.sibling)
		{
			break;
		}
		chunkObjectPointer = (ChunkObjectPointer*)chunkObjectPointer->base.sibling;
		model = model->sibling;
	}
}
const int sub_56E9C0Ptr = 0x56E9C0;
void sub_56E9C0(ObjectMaster* a1)
{
	__asm
	{
		mov eax, a1
		call sub_56E9C0Ptr
	}
}
void __cdecl DrawEggChao(ObjectMaster* a1)
{
	ChaoData1* data1 = a1->Data1.Chao;

	if (!(data1->field_B0 & 0x200)) return;

	njPushMatrixEx();
	njTranslateEx(&data1->entity.Position);
	RotateZ (data1->entity.Rotation.z);
	RotateX (data1->entity.Rotation.x);
	RotateY (data1->entity.Rotation.y);

	if (GameState != GameStates_Pause && a1->Data1.Chao->pParamGC->BodyType == 1)
	{
		if (a1->Data1.Chao->entity.Scale.y < 1.0f)	
			a1->Data1.Chao->entity.Scale.y += 0.06f;

		float a1a = (1.0f - a1->Data1.Chao->entity.Scale.x) * 0.15f + a1->Data1.Chao->entity.Scale.z;
		a1->Data1.Chao->entity.Scale.z = a1a * 0.9f;
		a1->Data1.Chao->entity.Scale.x += a1a;
	}

	njSetTexture(&AL_BODY);
	sub_56E9C0(a1);
	alpalSetBank(a1, a1->Data1.Entity->Index);
	Chao_NodeIndex = 0;
	if (a1->Data1.Chao->pParamGC->BodyType == 1)
		DrawOtherChao(a1, data1->field_510, &object_0023CC1C);
	else if (a1->Data1.Chao->pParamGC->BodyType == 2)
		DrawOtherChao(a1, data1->field_510, &object_00254A0C);
	njPopMatrixEx();
	if (data1->pLeftHandItemObject)
	{
		njSetTexture(data1->pLeftHandItemTexlist);
		njPushMatrixEx();
		njTranslateEx(&data1->LeftHandTranslationPos);
		RotateY(data1->entity.Rotation.y);
		njScale(NULL, data1->LeftHandItemScale, data1->LeftHandItemScale, data1->LeftHandItemScale);
		njCnkDrawObject(data1->pLeftHandItemObject);
		njPopMatrixEx();
	}
	if (data1->pRightHandItemObject)
	{
		njSetTexture(data1->pRightHandItemTexlist);
		njPushMatrixEx();
		njTranslateEx(&data1->RightHandTranslationPos);
		RotateY(data1->entity.Rotation.y);
		njScale(NULL, data1->RightHandItemScale, data1->RightHandItemScale, data1->RightHandItemScale);
		njCnkDrawObject(data1->pRightHandItemObject);
		njPopMatrixEx();
	}
	njPopMatrixEx();
}
const int loc_55016E = 0x55016E;
static void __declspec(naked) DrawEggChaoHook()
{
	__asm
	{
		push ebx
		call DrawEggChao
		add    esp, 4
		jmp loc_55016E
	}
}

const int ColorEggModelPtr = 0x0056D540;
void ColorEggModel(NJS_CNK_MODEL* a1, int a2)
{
	__asm
	{
		push a2
		mov edi, a1
		call ColorEggModelPtr
		add esp, 4
	}
}

NJS_OBJECT* FirstEvoEyes[4 * 24];

bool AL_CanBeBald(ObjectMaster* a1)
{
	return AL_IsChild(a1) || 
		(a1->Data1.Chao->pParamGC->Type < ChaoType_Neutral_Chaos && 
			a1->Data1.Chao->pParamGC->Type >= 5 &&
			AL_BaldRoot[(a1->Data1.Chao->pParamGC->Type - 5) + 1]);
}

bool AL_CheckBald(ObjectMaster* a1, EAccessoryType type) {
	int id = a1->Data1.Chao->pParamGC->Accessories[type];
	return (id && AccessoryTypeMap[id - 1] == type && ModAPI_BaldAccessory.contains(id - 1));
}

bool AL_CheckAllBald(ObjectMaster* a1)
{
	return AL_CheckBald(a1, EAccessoryType::Head) || AL_CheckBald(a1, EAccessoryType::Generic1) || AL_CheckBald(a1, EAccessoryType::Generic2);
}

void AL_SetBodyTexture(ObjectMaster* a1)
{
	if (a1->Data1.Chao->pParamGC->Type == 26) {
		njSetTexture(&AL_BODY);
		return;
	}

	if (AL_IsNegative(a1))
		njSetTexture(&XL_BODY_TEXLIST);
	else
		njSetTexture(&AL_BODY);
}

static bool BaldFlag = false;
static bool CanBeBald = false;
void DrawChao(ObjectMaster* a1, ChunkObjectPointer* chunkObjectPointer)
{
	int v36, v44;

	if (Chao_NodeIndex == 0)
	{
		BaldFlag = AL_CheckAllBald(a1);
		CanBeBald = AL_CanBeBald(a1);

		AL_ValidateAccessory(a1, EAccessoryType::Head);
		AL_ValidateAccessory(a1, EAccessoryType::Face);

		AL_RenderRigAccessory(a1, chunkObjectPointer, EAccessoryType::Generic1);
		AL_RenderRigAccessory(a1, chunkObjectPointer, EAccessoryType::Generic2);
	}

	njPushMatrixEx();

	if (BaldFlag && CanBeBald)
	{
		NJS_OBJECT* eye = nullptr;

		if (Chao_NodeIndex == 17)
			eye = FirstEvoEyes[4 * (a1->Data1.Chao->pParamGC->Type - 2)];
		else if (Chao_NodeIndex == 20)
			eye = FirstEvoEyes[4 * (a1->Data1.Chao->pParamGC->Type - 2) + 1];
		else if (Chao_NodeIndex == 27)
			eye = FirstEvoEyes[4 * (a1->Data1.Chao->pParamGC->Type - 2) + 3];
		else if (Chao_NodeIndex == 28)
			eye = FirstEvoEyes[4 * (a1->Data1.Chao->pParamGC->Type - 2) + 2];

		NJS_VECTOR backupPos = *(NJS_VECTOR*)chunkObjectPointer->base.pos;

		if (eye)
			*(NJS_VECTOR*)chunkObjectPointer->base.pos = *(NJS_VECTOR*)eye->pos;

		AnimateChao((int)chunkObjectPointer);
		*(NJS_VECTOR*)chunkObjectPointer->base.pos = backupPos;
	}
	else
		AL_SetMotionMatrix(a1, chunkObjectPointer);
	//chibi chao lol
	//if (Chao_NodeIndex == 0)
		//njScale(0.5, 0.5, 0.5);
	//if (Chao_NodeIndex == 17 ||
		//Chao_NodeIndex == 20)
		//njScale(1.5, 1.5, 1.5);
	AL_DrawToy(chunkObjectPointer);

	bool hideHat = false;
	if (a1->Data1.Chao->pParamGC->Headgear)
	{
		//REMOVE WRONG HAT
		if (a1->Data1.Chao->pParamGC->Headgear >= 85)
		{
			int index = a1->Data1.Chao->pParamGC->Headgear - 85;
			if (MaskObjObjectList[index].first == &object_alo_missing)
				a1->Data1.Chao->pParamGC->Headgear = 0;
		}

		hideHat = true;
	}

	if (Chao_NodeIndex == 16)
	{
		AL_RenderAccessory(a1, EAccessoryType::Head);
		AL_RenderAccessory(a1, EAccessoryType::Face);

		if (a1->Data1.Chao->pParamGC->Headgear)
		{
			njSetTexture(&AL_BODY);
			AL_DrawMask(a1->Data1.Chao->pParamGC->Headgear);

			goto LABEL_98;
		}
	}
	else if (Chao_NodeIndex == 1)
	{
		if (a1->Data1.Chao->pParamGC->HideFeet && a1->Data1.Chao->pParamGC->Type >= 0 && a1->Data1.Chao->pParamGC->Type < (unsigned __int8)ChaoType_Neutral_Chaos)
		{
			AL_SetBodyTexture(a1);

			//alpalSetBank(a1, (unsigned __int8)a1->Data1.Entity->Index);
			*(int*)(*(int*)(*(int*)(*((int*)GetDllData("ObakeBodyObjectList")
				+ (unsigned __int8)a1->Data1.Chao->pParamGC->Type)
				+ 4)
				+ 4)
				+ 4) = *(int*)((__int16*)chunkObjectPointer->base.chunkmodel->plist + 2); //copies mat color onto the ghost model
			AL_SetRareMaterial(a1, chunkObjectPointer->base.chunkmodel);
			chCnkDrawObject(((NJS_OBJECT**)GetDllData("ObakeBodyObjectList"))[a1->Data1.Chao->pParamGC->Type]);
			SetMaterial(0, 0, 0, 0);
			goto LABEL_95;
		}
	}
	else if (Chao_NodeIndex == 35 && a1->Data1.Chao->pParamGC->Medal)
	{
		njSetTexture(&stru_1366AD4);
		njCnkDrawObject(dword_12E58B8[a1->Data1.Chao->pParamGC->Medal]);
		goto LABEL_98;
	}
	if (((!hideHat)
		|| Chao_NodeIndex != 18
		&& Chao_NodeIndex != 21
		&& Chao_NodeIndex != 19
		&& Chao_NodeIndex != 22
		&& Chao_NodeIndex != 23
		&& Chao_NodeIndex != 25
		&& Chao_NodeIndex != 24
		&& Chao_NodeIndex != 26
		&& Chao_NodeIndex != 30
		&& Chao_NodeIndex != 31
		&& Chao_NodeIndex != 27
		&& Chao_NodeIndex != 29)
		&& (!a1->Data1.Chao->pParamGC->HideFeet
			|| a1->Data1.Chao->pParamGC->Type >= (unsigned __int8)CharacterIndex
			|| (a1->Data1.Chao->pParamGC->DCWings || (Chao_NodeIndex != 37 && Chao_NodeIndex != 39)) && Chao_NodeIndex != 6 && Chao_NodeIndex != 13 && Chao_NodeIndex != 8))
	{
		if (chunkObjectPointer->animalPart && a1->Data1.Chao->pParamGC->Type < CharacterIndex)
		{
#ifdef COLORED_PARTS
			if (a1->Data1.Chao->pParamGC->Texture)
				Has_texlist_batadvPlayerChara_in_it[8] = (int)&AL_BODY;
			else
#endif
			njSetTexture((NJS_TEXLIST*)chunkObjectPointer->field_CC);
#ifdef COLORED_PARTS
			AL_SetRareMaterial(a1, chunkObjectPointer->animalPart->chunkmodel);
#endif
			chCnkDrawObject(chunkObjectPointer->animalPart);
#ifdef COLORED_PARTS
			goto LABEL_96;
#endif
		}
		else if (chunkObjectPointer->base.chunkmodel)
		{
			v36 = a1->Data1.Chao->Face.EyeLidExpressionCurrSlopeAng;
			v44 = a1->Data1.Chao->Face.EyeLidExpressionCurrCloseAng + a1->Data1.Chao->Face.EyeLidBlinkAng - 0x4000;
			switch (Chao_NodeIndex)
			{
			case 19:
				AL_SetBodyTexture(a1);
				AL_SetRareMaterial(a1, chunkObjectPointer->base.chunkmodel);
				//alpalSetBank(a1, a1->Data1.Chao->entity.Index);
				goto LABEL_86;
			case 22:
				AL_SetBodyTexture(a1);
				AL_SetRareMaterial(a1, chunkObjectPointer->base.chunkmodel);
				//alpalSetBank(a1, a1->Data1.Chao->entity.Index);
				v36 = -v36;
			LABEL_86:

				RotateZ(v36);
				RotateX(v44);

			LABEL_91:
				if (Chao_NodeIndex != 19 && Chao_NodeIndex != 22
					|| a1->Data1.Chao->Face.EyeLidExpressionCurrCloseAng + a1->Data1.Chao->Face.EyeLidBlinkAng)
				{
					break;
				}
			LABEL_96:
				SetPixelShaderFloat(78, 0);
				SetChunkTexIndexNull(2);
				ChunkMatEnable = 0;
				ChunkMatFlag = 0;
				goto LABEL_98;
			case 27:
				njSetTexture(&AL_MOUTH_TEXLIST);
				break;
			case 18:
			case 21:
				if (a1->Data1.Chao->Face.Flag == 0 || a1->Data1.Chao->pParamGC->EyeColor == 0 || (size_t)a1->Data1.Chao->pParamGC->EyeColor > ModAPI_EyeColors.size())
					njSetTexture(&AL_EYE_TEXLIST);
				else
					njSetTexture(ModAPI_EyeColors[a1->Data1.Chao->pParamGC->EyeColor - 1]);
				if (Chao_NodeIndex != 18)
				{
					//njTranslate(NULL, -a1->Data1.Chao->Face.EyePosX, a1->Data1.Chao->Face.EyePosY, 0);
					goto LABEL_91;
				}
				//njTranslate(NULL, a1->Data1.Chao->Face.EyePosX, a1->Data1.Chao->Face.EyePosY, 0);
				break;
			default:
				AL_SetBodyTexture(a1);

				if (a1->Data1.Chao->pParamGC->DCWings || (Chao_NodeIndex != 37 && Chao_NodeIndex != 39))
					AL_SetRareMaterial(a1, chunkObjectPointer->base.chunkmodel);

				//alpalSetBank(a1, a1->Data1.Chao->entity.Index);
				goto LABEL_91;
			}

			if (BaldFlag && Chao_NodeIndex == 16 && CanBeBald)
			{
				if (AL_IsChild(a1))
				{
					*(int*)(&(AL_BaldRoot[0]->chunkmodel->plist[2])) = *(int*)(&(chunkObjectPointer->base.chunkmodel->plist[2]));
					chCnkDrawModel(AL_BaldRoot[0]->chunkmodel);
				}
				else
				{
					*(int*)(&(AL_BaldRoot[(a1->Data1.Chao->pParamGC->Type - 5) + 1]->chunkmodel->plist[2])) = *(int*)(&(chunkObjectPointer->base.chunkmodel->plist[2]));
					chCnkDrawModel(AL_BaldRoot[(a1->Data1.Chao->pParamGC->Type - 5) + 1]->chunkmodel);
				}
			}
			else if (!BaldFlag || (!CanBeBald || (Chao_NodeIndex != 23 && Chao_NodeIndex != 25)))
				chCnkDrawModel(chunkObjectPointer->base.chunkmodel);
		LABEL_95:
			goto LABEL_96;
		}
	}
LABEL_98:
	Chao_NodeIndex++;
	if (chunkObjectPointer->base.child)
	{
		DrawChao(a1, (ChunkObjectPointer*)chunkObjectPointer->base.child);
	}
	njPopMatrixEx();

	if (chunkObjectPointer->base.sibling)
		DrawChao(a1, (ChunkObjectPointer*)chunkObjectPointer->base.sibling);
}

#ifdef IMGUIDEBUG
	task* ChaoDebugDistSelected = NULL;
	float ChaoDebugDist = 1.f;
#endif

static FunctionHook<void, task*> Chao_Display_t(0x0054FF80);
static void Chao_Display_r(task* tp) {
#ifdef IMGUIDEBUG
	if (ChaoDebugDistSelected == tp) {
		njPushMatrixEx();
		njTranslateEx(&GET_CHAOWK(tp)->entity.Position);
		njScale(NULL, ChaoDebugDist * 0.1f, ChaoDebugDist * 0.1f, ChaoDebugDist * 0.1f);
		SaveControl3D();
		OnControl3D(NJD_CONTROL_3D_CONSTANT_MATERIAL);
		SetMaterial(1, 1, 1, 1);
		njCnkDrawObject(&DebugSphere);
		SetMaterial(0, 0, 0, 0);
		LoadControl3D();
		njPopMatrixEx();
	}
#endif

	if (!gConfigVal.DayNightCycle) {
		Chao_Display_t.Original(tp);
		return;
	}

	const auto backupTex = AL_DayNightCycle_PreDrawSetupShinyTexture();
	Chao_Display_t.Original(tp);
	AL_DayNightCycle_PostDrawSetupShinyTexture(backupTex);
}

UsercallFuncVoid(ColorEggModel_t, (NJS_CNK_MODEL* model, int eggType), (model, eggType), 0x0056D540, rEDI, stack4);
static void ColorEggModel_r(NJS_CNK_MODEL* model, int eggType) {
	if (!gConfigVal.DayNightCycle) {
		ColorEggModel_t.Original(model, eggType);
		return;
	}

	const auto backupTex = AL_DayNightCycle_PreDrawSetupShinyTexture();

	// if the egg is shiny
	if (eggType >= SA2BEggColour_NormalShiny && eggType <= SA2BEggColour_BlackShiny_TwoTone) {
		AL_DayNightCycle_SetLerpShinyTexture();
	}

	ColorEggModel_t.Original(model, eggType);
	AL_DayNightCycle_PostDrawSetupShinyTexture(backupTex);
}

static FunctionHook<void, task*> EggDisplayer_t(0x0057B640);
static void EggDisplayer_r(task* tp) {
	if (!gConfigVal.DayNightCycle) {
		EggDisplayer_t.Original(tp);
		return;
	}

	const auto backupTex = AL_DayNightCycle_PreDrawSetupShinyTexture();

	// if the egg is shiny
	const auto eggType = GET_CHAOPARAM(tp)->EggColor;
	if (eggType >= SA2BEggColour_NormalShiny && eggType <= SA2BEggColour_BlackShiny_TwoTone) {
		AL_DayNightCycle_SetLerpShinyTexture();
	}

	EggDisplayer_t.Original(tp);
	AL_DayNightCycle_PostDrawSetupShinyTexture(backupTex);
}


void AL_Draw_Init() {
	// daynight cycle hooks
	Chao_Display_t.Hook(Chao_Display_r);
	EggDisplayer_t.Hook(EggDisplayer_r);
	ColorEggModel_t.Hook(ColorEggModel_r);

	//draw chao
	WriteJump((void*)0x0053FCF0, DrawChao);

	//draw other bodytype
	WriteJump((void*)0x0054FFEB, DrawEggChaoHook);
	WriteJump((void*)0x00550074, DrawEggChaoHook);

	AL_Mask_Init();

	//bald crap, i hate this 
	//preferably it should check the bald type and use the base model for those inside the AL_Deform stuff
	NJS_OBJECT** RootObj = (NJS_OBJECT**)GetDllData("AL_RootObject");
	if (RootObj)
	{
		//RootObj[18] = &object_al_chao_nnz;
		for (int i = 0; i < 22; i++)
		{
			//todo clean this up to use eachother (less child->child bs)
			FirstEvoEyes[4 * i] = RootObj[6 * i]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->child;
			FirstEvoEyes[4 * i + 1] = RootObj[6 * i]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling;
			FirstEvoEyes[4 * i + 2] = RootObj[6 * i]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling->sibling->child;
			FirstEvoEyes[4 * i + 3] = RootObj[6 * i]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling->sibling;
		}
	}
}