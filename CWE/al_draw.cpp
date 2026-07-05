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
#include "al_modelcontainer.h"
#include "alo_obakehead.h"

#include <FunctionHook.h>
#include <UsercallFunctionHandler.h>

#pragma warning(push)
#pragma warning( disable: 4838 )
#include "data/al_model/al_egg_chao.nja"
#include "data/al_model/al_omochao.nja"
#include <data/debugsphere.h>
#pragma warning(pop)

#include <al_daynight.h>
#include <api/api_accessory.h>
#include <optional>
#include <renderfix.h>

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
			njSetTexture(MaskObjObjectList[iMask - 85].second);
			chCnkDrawObject(MaskObjObjectList[iMask - 85].first);
		}
		break;
	}
}

static void AL_DrawToy(AL_OBJECT* pObject) {
	if (!pObject->pItemObject) {
		return;
	}

	ChunkMatFlag = SecondTextureEnvironmentMap;
	njSetTexture(pObject->pItemTexlist);

	njPushMatrixEx();

	if (pObject->ItemOffsetFlag) {
		njTranslateEx(&pObject->ItemOffsetPos);
		RotateZ(pObject->ItemOffsetAng.z);
		RotateY(pObject->ItemOffsetAng.y);
		RotateX(pObject->ItemOffsetAng.x);
	}

	njScale(NULL, pObject->ItemScale, pObject->ItemScale, pObject->ItemScale);
	chCnkDrawObject((NJS_OBJECT*)pObject->pItemObject);

	njPopMatrixEx();

	ChunkMatFlag = 0;
}

static bool IsOmochao(const task* tp) {
	return GET_CHAOPARAM(tp)->BodyType == 2;
}

static bool IsEggChao(const task* tp) {
	return GET_CHAOPARAM(tp)->BodyType == 1;
}

void AL_SetMotionMatrix(AL_OBJECT* pObject) {
	AnimateChao((int)pObject);
}

void CWE_ShinyCheck(int shiny)
{
	if (gConfigVal.ForceShinyTT && shiny)
		BrightFixPlus_ShinyCheck(2);
	else
		BrightFixPlus_ShinyCheck(shiny);
}

// this stupid bs is a hack to fix negative chao not working properly with the daynight cycle shiny lerping
static NJS_TEXLIST* BackupShinyTexlist = NULL;
static void BackupAndSetShinyTexture() {
	BackupShinyTexlist = _nj_curr_ctx_->texlist;
	njSetTexture(&AL_BODY);
}

static void RestoreShinyTexture() {
	njSetTexture(BackupShinyTexlist);
}

void ChaoColoring(int texture, int color, int shiny, int monotone, int shinyJewelMonotone, NJS_CNK_MODEL* model)
{
	// if the chao is shiny twotone, or shiny monotone apply the lerp
	if ((gConfigVal.ForceShinyTT && shiny) || shiny == 2 || (shiny && monotone)) {
		AL_DayNightCycle_SetLerpShinyTexture();
	}

	if (shinyJewelMonotone && shiny && texture && monotone)
	{
		int flag = SecondTextureEnvironmentMap | HasSecondTexture | DontUseTexture;
		ChunkMatEnable = 1;
		SetChunkTexIndexPrimary(17 + texture, 1, 1);

		BackupAndSetShinyTexture();
		SetChunkTexIndexSecondary(34, 0, 1);
		RestoreShinyTexture();

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
		BackupAndSetShinyTexture();
		if (monotone && !texture)
			SetChunkTexIndexPrimary(34, 1, 1);
		else
			SetChunkTexIndexSecondary(34, 1, 1);
		RestoreShinyTexture();
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

void AL_SetRareMaterial(task* a1, NJS_CNK_MODEL* model)
{
	ChaoColoring(
		GET_CHAOWK(a1)->pParamGC->Texture,
		GET_CHAOWK(a1)->pParamGC->Color,
		GET_CHAOWK(a1)->pParamGC->Shiny,
		GET_CHAOWK(a1)->pParamGC->MonotoneHighlights,
		GET_CWEPARAM(a1)->ShinyJewelMonotone,
		model);
}

static const int C_MTXConcatPtr = 0x00426E40;
static void C_MTXConcat(float* a1, float* a2, float* a3) {
	__asm {
		mov eax, a1
		mov edx, a2
		mov ecx, a3
		call C_MTXConcatPtr
	}
}

static float StartViewMatrix[12];
static float DrawMatrices[40][12];
static size_t DrawMatrixNodeCount = 0;
static void AL_DrawGetChaoNodeMatricesSub(task* tp, AL_OBJECT* pObject) {
	njPushMatrixEx();

	AL_SetMotionMatrix(pObject);
	memcpy(DrawMatrices[DrawMatrixNodeCount], _nj_current_matrix_ptr_, sizeof(DrawMatrices[DrawMatrixNodeCount]));

	DrawMatrixNodeCount++;

	if (pObject->pChild) {
		AL_DrawGetChaoNodeMatricesSub(tp, pObject->pChild);
	}

	njPopMatrixEx();

	if (pObject->pSibling) {
		AL_DrawGetChaoNodeMatricesSub(tp, pObject->pSibling);
	}
}

static void AL_DrawGetOtherChaoNodeMatricesSub(task* tp, AL_OBJECT* pObject, NJS_OBJECT* pOriginalObj) {
	njPushMatrixEx();

	if (DrawMatrixNodeCount == 34) {
		pOriginalObj->ang[1] = 0;
	}
	else if (DrawMatrixNodeCount == 35) {
		pOriginalObj->ang[1] = 0;
		pOriginalObj->pos[0] = -0.028161f;
		pOriginalObj->pos[1] = 0.1f;
		pOriginalObj->pos[2] = 1.10046f;
	}

	pObject->diff = { 0,0,0 };
	*(NJS_VECTOR*)pObject->Pos = *(NJS_VECTOR*)pOriginalObj->pos;

	//propeller and eyes
	if (DrawMatrixNodeCount == 29 || DrawMatrixNodeCount == 17 || DrawMatrixNodeCount == 21 || DrawMatrixNodeCount == 20)
	{
		njTranslateEx((NJS_VECTOR*)pOriginalObj->pos);
		RotateZ(pOriginalObj->ang[2]);
		RotateY(pOriginalObj->ang[1]);
		RotateX(pOriginalObj->ang[0]);
		if ((pOriginalObj->evalflags & NJD_EVAL_UNIT_SCL) == 0)
			njScale(NULL, pOriginalObj->scl[0], pOriginalObj->scl[1], pOriginalObj->scl[2]);
	}
	else {
		AL_SetMotionMatrix(pObject);
	}
	
	memcpy(DrawMatrices[DrawMatrixNodeCount], _nj_current_matrix_ptr_, sizeof(DrawMatrices[DrawMatrixNodeCount]));

	DrawMatrixNodeCount++;

	if (pObject->pChild && pOriginalObj->child) {
		AL_DrawGetOtherChaoNodeMatricesSub(tp, pObject->pChild, pOriginalObj->child);
	}

	njPopMatrixEx();

	if (pObject->pSibling && pOriginalObj->sibling) {
		AL_DrawGetOtherChaoNodeMatricesSub(tp, pObject->pSibling, pOriginalObj->sibling);
	}
}

static void AL_DrawGetChaoNodeMatrices(task* tp, AL_OBJECT* pObject) {
	memcpy(StartViewMatrix, _nj_current_matrix_ptr_, sizeof(StartViewMatrix));

	DrawMatrixNodeCount = 0;
	njPushUnitMatrix();
	AL_DrawGetChaoNodeMatricesSub(tp, pObject);
	njPopMatrixEx();
}

static void AL_DrawGetOtherChaoNodeMatrices(task* tp, AL_OBJECT* pObject, NJS_OBJECT* pOriginalObj) {
	memcpy(StartViewMatrix, _nj_current_matrix_ptr_, sizeof(StartViewMatrix));

	DrawMatrixNodeCount = 0;
	njPushUnitMatrix();
	AL_DrawGetOtherChaoNodeMatricesSub(tp, pObject, pOriginalObj);
	njPopMatrixEx();
}

static void AL_PushChaoNodeMatrix(size_t nodeIndex) {
	DataPointer(float*, MaxMatrixStackPointer, 0x02670588);

	float* pNextMatrix = _nj_current_matrix_ptr_ + 12;
	if (pNextMatrix > MaxMatrixStackPointer) {
		return;
	}

	_nj_current_matrix_ptr_ = pNextMatrix;
	C_MTXConcat(StartViewMatrix, _nj_current_matrix_ptr_, DrawMatrices[nodeIndex]);
}


static bool AL_HasAccessoryInSlot(const task* tp, const EAccessoryType slot) {
	const chaowk* work = GET_CHAOWK(tp);
	return work->AccessoryIndices[slot] != -1;
}

static void AL_ValidateAccessory(task* tp, const EAccessoryType slot) {
	chaowk* work = GET_CHAOWK(tp);
	auto& accessoryIndex = work->AccessoryIndices[slot];

	if (accessoryIndex != -1 && GetAccessoryType(accessoryIndex) >= EAccessoryType::Generic1) {
		// empty id means no accessory
		// false id won't get cleared, just missing object
		GET_CWEPARAM(tp)->Accessories[slot].ID[0] = 0;
	}
}

static bool AL_CanRenderRigAccessory(const task* tp, const EAccessoryType slot) {
	const chaowk* work = GET_CHAOWK(tp);
	const int accessoryIndex = work->AccessoryIndices[slot];

	return accessoryIndex != -1 && // chao has accessory
		GetAccessoryType(accessoryIndex) >= EAccessoryType::Generic1 && // is of rigged accessory type (we dont enforce it to be the same type as the slot intentionally)
		ObjectRegistry::Get(ChaoItemCategory_Accessory)->GetObj(accessoryIndex); // has model
}

static size_t AccessoryNodeIndex = 0;
static void AL_DrawRigAccessorySub(NJS_OBJECT* pObject, bool isOmoChao, bool rfNormalDraw) {
	if (pObject->chunkmodel) {
		pObject->chunkmodel->r = 0.0f;
		// hack i added, if its an omochao it needs scaling
		// matrix multiplication is associative, so in theory this is equivalent to the scale + SetMotionMatrix-ing
		if (isOmoChao) {
			njPushMatrix(StartViewMatrix);
			njScale(NULL, 1.1f, 1.1f, 1.1f);
			C_MTXConcat(_nj_current_matrix_ptr_, _nj_current_matrix_ptr_, DrawMatrices[AccessoryNodeIndex]);
		}
		else {
			AL_PushChaoNodeMatrix(AccessoryNodeIndex);
		}

		if(!rfNormalDraw) {
			RenderFixBackwardsCompatibilityDrawModel(pObject->chunkmodel);
		}
		else {
			rfCnkNormalDrawModel(pObject->chunkmodel);
		}
		njPopMatrixEx();
	}

	AccessoryNodeIndex++;

	if (pObject->child) {
		AL_DrawRigAccessorySub(pObject->child, isOmoChao, rfNormalDraw);
	}

	if (pObject->sibling) {
		AL_DrawRigAccessorySub(pObject->sibling, isOmoChao, rfNormalDraw);
	}
}

static void AL_DrawRigAccessory(task* tp, const EAccessoryType slot) {
	if (!AL_CanRenderRigAccessory(tp, slot)) {
		return;
	}

	chaowk* work = GET_CHAOWK(tp);
	auto pParam = GET_CWEPARAM(tp);
	const int accessoryIndex = work->AccessoryIndices[slot];
	const auto& accessory_data = GetAccessoryData(accessoryIndex);

	AccessoryNodeIndex = 0;

	auto registry = ObjectRegistry::Get(ChaoItemCategory_Accessory);
	njSetTexture(registry->GetTex(accessoryIndex));
	AccessorySetupDraw(accessoryIndex, pParam->Accessories[slot].ColorSlots, pParam->Accessories[slot].ColorFlags);

	Control3D ctrl(0, NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL | NJD_CONTROL_3D_OFFSET_MATERIAL);
	AL_DrawRigAccessorySub(registry->GetObj(accessoryIndex), IsOmochao(tp), IsAccessoryRFSupported(accessoryIndex));
}

static void AL_DrawAccessory(const task* tp, const EAccessoryType slot) {
	const chaowk* work = GET_CHAOWK(tp);
	auto pParam = GET_CWEPARAM(tp);

	if (!AL_HasAccessoryInSlot(tp, slot)) {
		return;
	}

	// for egg chao the hats are on the body, otherwise on the head
	AL_PushChaoNodeMatrix(IsEggChao(tp) ? 1 : 16);

	if (IsEggChao(tp)) {
		njTranslate(NULL, 0, 0.6f, 0);
		njScale(NULL, 1.2f, 1.2f, 1.2f);
	}

	Control3D ctrl(0, NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL | NJD_CONTROL_3D_OFFSET_MATERIAL);
	AccessorySetupDraw(work->AccessoryIndices[slot], pParam->Accessories[slot].ColorSlots, pParam->Accessories[slot].ColorFlags);

	if(!IsAccessoryRFSupported(work->AccessoryIndices[slot])) {
		ObjectRegistry::DrawObject<RenderFixBackwardsCompatibilityDrawObject>(ChaoItemCategory_Accessory, work->AccessoryIndices[slot]);
	}
	else {
		ObjectRegistry::DrawObject<rfCnkNormalDrawObject>(ChaoItemCategory_Accessory, work->AccessoryIndices[slot]);
	}
	njPopMatrixEx();
}

static void AL_DrawAccessories(task* tp) {
	AL_ValidateAccessory(tp, EAccessoryType::Head);
	AL_ValidateAccessory(tp, EAccessoryType::Face);

	if (!IsEggChao(tp)) {
		AL_DrawRigAccessory(tp, EAccessoryType::Generic1);
		AL_DrawRigAccessory(tp, EAccessoryType::Generic2);
	}

	AL_DrawAccessory(tp, EAccessoryType::Head);
	AL_DrawAccessory(tp, EAccessoryType::Face);
}

bool EggChaoFeetCheck(task* a1) {
	//if not egg chao stop
	//if doesnt have hidefeet stop
	//but if it does, make sure youre not rendering arm_l and arm_r
	return !IsEggChao(a1) ||
		(!GET_CHAOPARAM(a1)->HideFeet || (Chao_NodeIndex != AL_PART_REG_L && Chao_NodeIndex != AL_PART_REG_R));
}

void DrawOtherChao(task* tp, AL_OBJECT* pObject, NJS_OBJECT* pOriginalObj)
{
	signed int v36; // esi
	int v44; // [esp+28h] [ebp-38h]

	if (Chao_NodeIndex == 0) {
		DrawMatrixNodeCount = 0;
		AL_DrawGetOtherChaoNodeMatrices(tp, pObject, pOriginalObj);

		AL_DrawAccessories(tp);
	}

	if (IsEggChao(tp)) {
		switch (Chao_NodeIndex) {
		case AL_PART_FEELER_L:
		case AL_PART_FEELER_R:
		case AL_PART_MOUTH:
			pOriginalObj->chunkmodel = nullptr;
			break;
		}
	}

	while (1) {
		AL_PushChaoNodeMatrix(Chao_NodeIndex);

		AL_DrawToy(pObject);

		if (IsOmochao(tp) && !GET_CHAOPARAM(tp)->Headgear && Chao_NodeIndex == 27 && GET_CHAOWK(tp)->Face.MouthCurrNum != 0) {
			njSetTexture(&AL_BODY);
			chCnkDrawObject(&omochao_mouth);
		}

		if (IsOmochao(tp) && GET_CHAOPARAM(tp)->Headgear && (Chao_NodeIndex == 29 || Chao_NodeIndex == 18 || Chao_NodeIndex == 21))
			goto LABEL_98;

		if ((Chao_NodeIndex == 16 && IsOmochao(tp)) || (Chao_NodeIndex == 1 && IsEggChao(tp)))
		{
			if (IsOmochao(tp) && GET_CHAOPARAM(tp)->Headgear) {
				njSetTexture(&AL_BODY);
				AL_DrawMask(GET_CHAOPARAM(tp)->Headgear);
				goto LABEL_98;
			}
		}
		else if (Chao_NodeIndex == 35 && GET_CHAOPARAM(tp)->Medal)
		{
			njPushMatrixEx();
			//njScale(2, 2, 2);
			njSetTexture((NJS_TEXLIST*)0x1366AD4);
			njCnkDrawObject(dword_12E58B8[GET_CHAOPARAM(tp)->Medal]);
			njPopMatrixEx();
		}

		njSetTexture(&AL_BODY);
		if (IsOmochao(tp) && !GET_CHAOPARAM(tp)->Headgear)
		{
			v36 = GET_CHAOWK(tp)->Face.EyeLidExpressionCurrSlopeAng;
			v44 = GET_CHAOWK(tp)->Face.EyeLidExpressionCurrCloseAng + GET_CHAOWK(tp)->Face.EyeLidBlinkAng - 0x4000;
			switch (Chao_NodeIndex)
			{
			case 22:
				v36 = -v36;
			case 19:
				AL_SetRareMaterial(tp, (NJS_CNK_MODEL*)pObject->pModel);

				RotateZ(v36);
				RotateX(v44);

				if ((Chao_NodeIndex == 19 || Chao_NodeIndex == 22)
					|| GET_CHAOWK(tp)->Face.EyeLidExpressionCurrCloseAng + GET_CHAOWK(tp)->Face.EyeLidBlinkAng)
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
		if (IsOmochao(tp) && gConfigVal.OmochaoParts && pObject->pPartsObject && GET_CHAOPARAM(tp)->type < CharacterIndex)
		{
			njSetTexture((NJS_TEXLIST*)pObject->DisplayList);
			chCnkDrawObject(pObject->pPartsObject);
		}
		else if (pOriginalObj->chunkmodel && EggChaoFeetCheck(tp))
		{
			if (IsOmochao(tp) && (Chao_NodeIndex == 18 || Chao_NodeIndex == 21))
			{
				njSetTexture(&OMO_EYE_TEXLIST);

				Sint16* texchunk = FindChunk(pObject->pModel->PList, NJD_CT_TID);
				pOriginalObj->chunkmodel->plist[7] = texchunk[1]; //copy eye ID
			}

			if (IsEggChao(tp)) {
				//copy shiny jewel (material color)
				if (Chao_NodeIndex == 1 && GET_CHAOPARAM(tp)->Color > 0)
					*(int*)((__int16*)pOriginalObj->chunkmodel->plist + 2) = *(int*)((__int16*)pObject->pModel->PList + 2);

				//copy feet color and texture onto egg chao feet
				if (Chao_NodeIndex == 6 || Chao_NodeIndex == 13)
				{
					pOriginalObj->chunkmodel->plist[7] = pObject->pModel->PList[7];
					*(int*)((__int16*)pOriginalObj->chunkmodel->plist + 2) = *(int*)((__int16*)pObject->pModel->PList + 2);
				}
			}

			if ((IsEggChao(tp) && (Chao_NodeIndex == 1 || Chao_NodeIndex == 6 || Chao_NodeIndex == 13)) ||
				(IsOmochao(tp) && (Chao_NodeIndex == 1 || Chao_NodeIndex == 16)))
				AL_SetRareMaterial(tp, pOriginalObj->chunkmodel);

			chCnkDrawModel(pOriginalObj->chunkmodel);

			//reset shaders
			SetPixelShaderFloat(78, 0);
			SetChunkTexIndexNull(2);
			ChunkMatEnable = 0;
			ChunkMatFlag = 0;

			//the eyes
			if (IsEggChao(tp) && Chao_NodeIndex == 1)
				chCnkDrawModel(&cnk_attach_0023CBCC_002);

			//reset material color on eggchao model
			if (IsEggChao(tp) && (Chao_NodeIndex == 1 || Chao_NodeIndex == 6 || Chao_NodeIndex == 13))
				*(int*)((__int16*)pOriginalObj->chunkmodel->plist + 2) = 0xFFFFFFFF;
		}

	LABEL_98:
		Chao_NodeIndex++;
		if (pOriginalObj->child && pObject->pChild)
		{
			DrawOtherChao(tp, pObject->pChild, pOriginalObj->child);
		}
		njPopMatrixEx();
		if (!pOriginalObj->sibling || !pObject->pSibling)
		{
			break;
		}
		pObject = pObject->pSibling;
		pOriginalObj = pOriginalObj->sibling;
	}
}
const int sub_56E9C0Ptr = 0x56E9C0;
void sub_56E9C0(task* a1)
{
	__asm
	{
		mov eax, a1
		call sub_56E9C0Ptr
	}
}
void __cdecl DrawEggChao(task* a1)
{
	chaowk* data1 = GET_CHAOWK(a1);

	if (!(data1->ChaoFlag & 0x200)) return;

	njPushMatrixEx();
	njTranslateEx(&data1->pos);
	RotateZ(data1->ang.z);
	RotateX(data1->ang.x);
	RotateY(data1->ang.y);

	if (GameState != GameStates_Pause && GET_CHAOPARAM(a1)->BodyType == 1)
	{
		if (GET_CHAOWK(a1)->scl.y < 1.0f)
			GET_CHAOWK(a1)->scl.y += 0.06f;

		float a1a = (1.0f - GET_CHAOWK(a1)->scl.x) * 0.15f + GET_CHAOWK(a1)->scl.z;
		GET_CHAOWK(a1)->scl.z = a1a * 0.9f;
		GET_CHAOWK(a1)->scl.x += a1a;
	}

	njSetTexture(&AL_BODY);
	sub_56E9C0(a1);
	alpalSetBank(a1, a1->twp->btimer);
	SaveControl3D();

	OffControl3D(NJD_CONTROL_3D_CONSTANT_ATTR);
	OffControl3D(NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);

	Chao_NodeIndex = 0;
	if (GET_CHAOPARAM(a1)->BodyType == 1)
		DrawOtherChao(a1, data1->Shape.pObject, &object_0023CC1C);
	else if (GET_CHAOPARAM(a1)->BodyType == 2)
		DrawOtherChao(a1, data1->Shape.pObject, &object_00254A0C);

	LoadControl3D();

	njPopMatrixEx();
	if (data1->Shape.pLeftHandItemObject)
	{
		njSetTexture(data1->Shape.pLeftHandItemTexlist);
		njPushMatrixEx();
		njTranslateEx(&data1->Shape.LeftHandPos);
		RotateY(data1->ang.y);
		njScale(NULL, data1->Shape.LeftHandItemScale, data1->Shape.LeftHandItemScale, data1->Shape.LeftHandItemScale);
		njCnkDrawObject(data1->Shape.pLeftHandItemObject);
		njPopMatrixEx();
	}
	if (data1->Shape.pRightHandItemObject)
	{
		njSetTexture(data1->Shape.pRightHandItemTexlist);
		njPushMatrixEx();
		njTranslateEx(&data1->Shape.RightHandPos);
		RotateY(data1->ang.y);
		njScale(NULL, data1->Shape.RightHandItemScale, data1->Shape.RightHandItemScale, data1->Shape.RightHandItemScale);
		njCnkDrawObject(data1->Shape.pRightHandItemObject);
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

void AL_SetBodyTexture(task* a1)
{
	if (GET_CHAOPARAM(a1)->type == 26) {
		njSetTexture(&AL_BODY);
		return;
	}

	if (AL_IsNegative(a1))
		njSetTexture(&XL_BODY_TEXLIST);
	else
		njSetTexture(&AL_BODY);
}

static bool BaldFlag = false;
static CWE_API_ACCESSORY_BALD_DATA DrawBaldData;
static uint64_t DrawHideNodes = 0;

static void AL_DrawSetupParams(task* tp, AL_OBJECT* pObject) {
	chaowk* work = GET_CHAOWK(tp);
	const auto* pParam = GET_CWEPARAM(tp);

	DrawHideNodes = 0;
	BaldFlag = false;

	DrawBaldData = {
		.Center = {0, 1.1, -0.1},
		.Influence = {0.9, 0.9, 0.9},
		.Radius = 1.15f,
		.ClipFace = true
	};

	std::optional<CWE_API_ACCESSORY_BALD_DATA> baldData;

	bool presets[3] = { false, false, false };
	bool dontKeepHeadParts = false;
	bool disableJiggle = false;

	for (size_t i = 0; i < _countof(pParam->Accessories); ++i) {
		if (work->AccessoryIndices[i] == -1) continue;
		const auto& data = GetAccessoryData(work->AccessoryIndices[i]);

		DrawHideNodes |= data.HideNodes;

		if (data.Flags & CWE_API_ACCESSORY_FLAGS_NO_JIGGLE) disableJiggle = true;

		if (data.Flags & CWE_API_ACCESSORY_FLAGS_BALD_PRESET_X) {
			presets[0] = true;
		}
		if (data.Flags & CWE_API_ACCESSORY_FLAGS_BALD_PRESET_Y) {
			presets[1] = true;
		}
		if (data.Flags & CWE_API_ACCESSORY_FLAGS_BALD_PRESET_Z) {
			presets[2] = true;
		}

		if (!(data.Flags & CWE_API_ACCESSORY_FLAGS_BALD_KEEP_HEAD_PARTS)) {
			dontKeepHeadParts = true;
		}

		if (!baldData) {
			baldData = GetAccessoryBaldData(work->AccessoryIndices[i]);
		}

		if (data.Flags & CWE_API_ACCESSORY_FLAGS_LEGACY_BALD) {
			presets[1] = true;

			if(!gConfigVal.LegacyBaldYOnly) {
				presets[0] = true;
				presets[2] = true;
			}
		}
	}

	const uint32_t prevFlag = (work->ChaoFlag & 0x20);
	if (disableJiggle) {
		work->ChaoFlag &= ~0x20;
	}
	else {
		work->ChaoFlag |= 0x20;
	}

	if((work->ChaoFlag & 0x20) != prevFlag) {
		work->JiggleFlagChanged = true;
	}

	work->BaldHideHead = false;

	if (!baldData) {
		for (size_t i = 0; i < 3; ++i) {
			if (presets[i]) {
				BaldFlag = true;

				if (dontKeepHeadParts) {
					DrawHideNodes |= uint64_t(1) << 23;
					DrawHideNodes |= uint64_t(1) << 25;
					work->BaldHideHead = true;
				}
				continue;
			}

			switch (i) {
			case 0:
				DrawBaldData.Influence.x = 0.f;
				break;
			case 1:
				DrawBaldData.Influence.y = 0.f;
				break;
			case 2:
				DrawBaldData.Influence.z = 0.f;
				break;
			}
		}
	}
	else {
		if (dontKeepHeadParts) {
			DrawHideNodes |= uint64_t(1) << 23;
			DrawHideNodes |= uint64_t(1) << 25;
			work->BaldHideHead = true;
		}

		BaldFlag = true;
		DrawBaldData = *baldData;
	}
}

static void AL_SetupBald(task* tp) {
	float vector[4] = { DrawBaldData.Center.x, DrawBaldData.Center.y, DrawBaldData.Center.z, DrawBaldData.Radius };
	cwe_device->SetVertexShaderConstantF(148, vector, 1);

	bool canbald = false;
	switch (Chao_NodeIndex) {
	case 14:
	case 15:
	case 16:
		vector[0] = 1;
		cwe_device->SetVertexShaderConstantF(152, vector, 1);
		canbald = BaldFlag;
		break;
	}

	if (canbald) {
		float vector[4] = { 1, DrawBaldData.Influence.x, DrawBaldData.Influence.y, DrawBaldData.Influence.z };
		cwe_device->SetVertexShaderConstantF(144, vector, 1);
	}
}
void DrawChao(task* tp, AL_OBJECT* pObject)
{
	int v36, v44;

	if (Chao_NodeIndex == 0) {
		DrawMatrixNodeCount = 0;
		AL_DrawGetChaoNodeMatrices(tp, pObject);

		AL_DrawAccessories(tp);
		AL_DrawSetupParams(tp, pObject);
	}

	AL_PushChaoNodeMatrix(Chao_NodeIndex);

	AL_DrawToy(pObject);

	bool hideHat = false;
	if (GET_CHAOPARAM(tp)->Headgear)
	{
		//REMOVE WRONG HAT
		if (GET_CHAOPARAM(tp)->Headgear >= 85)
		{
			int index = GET_CHAOPARAM(tp)->Headgear - 85;
			if (MaskObjObjectList[index].first == &object_alo_missing)
				GET_CHAOPARAM(tp)->Headgear = 0;
		}

		hideHat = true;
	}

	if (Chao_NodeIndex == 16) {
		if (GET_CHAOPARAM(tp)->Headgear)
		{
			njSetTexture(&AL_BODY);
			AL_DrawMask(GET_CHAOPARAM(tp)->Headgear);

			goto LABEL_98;
		}
	}
	else if (Chao_NodeIndex == 1)
	{
		if (GET_CHAOPARAM(tp)->HideFeet && GET_CHAOPARAM(tp)->type >= 0 && GET_CHAOPARAM(tp)->type < (unsigned __int8)ChaoType_Neutral_Chaos)
		{
			AL_SetBodyTexture(tp);

			//alpalSetBank(a1, (unsigned __int8)a1->twp->Index);
			*(int*)(*(int*)(*(int*)(*((int*)GetDllData("ObakeBodyObjectList")
				+ (unsigned __int8)GET_CHAOPARAM(tp)->type)
				+ 4)
				+ 4)
				+ 4) = *(int*)((__int16*)pObject->pModel->PList + 2); //copies mat color onto the ghost model
			AL_SetRareMaterial(tp, (NJS_CNK_MODEL*)pObject->pModel);
			chCnkDrawObject(((NJS_OBJECT**)GetDllData("ObakeBodyObjectList"))[GET_CHAOPARAM(tp)->type]);
			SetMaterial(0, 0, 0, 0);
			goto LABEL_95;
		}
	}
	else if (Chao_NodeIndex == 35 && GET_CHAOPARAM(tp)->Medal)
	{
		njSetTexture(&stru_1366AD4);
		njCnkDrawObject(dword_12E58B8[GET_CHAOPARAM(tp)->Medal]);
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
		&& (!GET_CHAOPARAM(tp)->HideFeet
			|| GET_CHAOPARAM(tp)->type >= (unsigned __int8)CharacterIndex
			|| (GET_CWEPARAM(tp)->DCWings || (Chao_NodeIndex != 37 && Chao_NodeIndex != 39)) && Chao_NodeIndex != 6 && Chao_NodeIndex != 13 && Chao_NodeIndex != 8))
	{
		if (pObject->pPartsObject && GET_CHAOPARAM(tp)->type < CharacterIndex) {
			if (!(DrawHideNodes & (uint64_t(1) << uint64_t(Chao_NodeIndex)))) {
				njSetTexture((NJS_TEXLIST*)pObject->DisplayList);
				chCnkDrawObject(pObject->pPartsObject);
			}
		}
		else if (pObject->pModel)
		{
			v36 = GET_CHAOWK(tp)->Face.EyeLidExpressionCurrSlopeAng;
			v44 = GET_CHAOWK(tp)->Face.EyeLidExpressionCurrCloseAng + GET_CHAOWK(tp)->Face.EyeLidBlinkAng - 0x4000;
			switch (Chao_NodeIndex)
			{
			case 19:
				AL_SetBodyTexture(tp);
				AL_SetRareMaterial(tp, (NJS_CNK_MODEL*)pObject->pModel);
				//alpalSetBank(a1, GET_CHAOWK(a1)->Index);
				goto LABEL_86;
			case 22:
				AL_SetBodyTexture(tp);
				AL_SetRareMaterial(tp, (NJS_CNK_MODEL*)pObject->pModel);
				//alpalSetBank(a1, GET_CHAOWK(a1)->Index);
				v36 = -v36;
			LABEL_86:

				RotateZ(v36);
				RotateX(v44);

			LABEL_91:
				if (Chao_NodeIndex != 19 && Chao_NodeIndex != 22
					|| GET_CHAOWK(tp)->Face.EyeLidExpressionCurrCloseAng + GET_CHAOWK(tp)->Face.EyeLidBlinkAng)
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
				if (GET_CHAOWK(tp)->Face.Flag == 0 || GET_CWEPARAM(tp)->EyeColor == 0 || (size_t)GET_CWEPARAM(tp)->EyeColor > ModAPI_EyeColors.size())
					njSetTexture(&AL_EYE_TEXLIST);
				else
					njSetTexture(ModAPI_EyeColors[GET_CWEPARAM(tp)->EyeColor - 1]);
				if (Chao_NodeIndex != 18)
				{
					//njTranslate(NULL, -GET_CHAOWK(a1)->Face.EyePosX, GET_CHAOWK(a1)->Face.EyePosY, 0);
					goto LABEL_91;
				}
				//njTranslate(NULL, GET_CHAOWK(a1)->Face.EyePosX, GET_CHAOWK(a1)->Face.EyePosY, 0);
				break;
			default:
				AL_SetBodyTexture(tp);

				if (GET_CWEPARAM(tp)->DCWings || (Chao_NodeIndex != 37 && Chao_NodeIndex != 39))
					AL_SetRareMaterial(tp, (NJS_CNK_MODEL*)pObject->pModel);

				//alpalSetBank(a1, GET_CHAOWK(a1)->Index);
				goto LABEL_91;
			}

			AL_SetupBald(tp);

			if (!(DrawHideNodes & (uint64_t(1) << uint64_t(Chao_NodeIndex)))) {
				chCnkDrawModel((NJS_CNK_MODEL*)pObject->pModel);
			}
		LABEL_95:
			goto LABEL_96;
		}
	}
LABEL_98:
	float vector[4] = { 0,0,0,0 };
	cwe_device->SetVertexShaderConstantF(144, vector, 1);
	Chao_NodeIndex++;
	if (pObject->pChild)
	{
		DrawChao(tp, (AL_OBJECT*)pObject->pChild);
	}
	njPopMatrixEx();

	if (pObject->pSibling)
		DrawChao(tp, (AL_OBJECT*)pObject->pSibling);
}

#ifdef IMGUIDEBUG
	task* ChaoDebugDistSelected = NULL;
	float ChaoDebugDist = 1.f;
#endif

static FunctionHook<void, task*> Chao_Display_t(0x0054FF80);
static void Chao_Display_r(task* tp) {
	if(gConfigVal.AccUpdateDraw) {
		AL_ChaoAccessoryMainCheck(tp);
	}

#ifdef IMGUIDEBUG
	if (ChaoDebugDistSelected == tp) {
		njPushMatrixEx();
		njTranslateEx(&GET_CHAOWK(tp)->pos);
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

	if (!gConfigVal.DayNightCycle || !gConfigVal.DayNightShinyTex) {
		Chao_Display_t.Original(tp);
		return;
	}

	const auto backupTex = AL_DayNightCycle_PreDrawSetupShinyTexture();
	Chao_Display_t.Original(tp);
	AL_DayNightCycle_PostDrawSetupShinyTexture(backupTex);
}

UsercallFuncVoid(ColorEggModel_t, (NJS_CNK_MODEL* model, int eggType), (model, eggType), 0x0056D540, rEDI, stack4);
static void ColorEggModel_r(NJS_CNK_MODEL* model, int eggType) {
	if (!gConfigVal.DayNightCycle || !gConfigVal.DayNightShinyTex) {
		ColorEggModel_t.Original(model, eggType);
		return;
	}

	const auto backupTex = AL_DayNightCycle_PreDrawSetupShinyTexture();

	// if the egg is shiny
	if (eggType >= SA2BEggColour_NormalShiny && eggType <= SA2BEggColour_BlackShiny_MonoTone) {
		AL_DayNightCycle_SetLerpShinyTexture();
	}

	ColorEggModel_t.Original(model, eggType);
	AL_DayNightCycle_PostDrawSetupShinyTexture(backupTex);
}

static FunctionHook<void, task*> EggDisplayer_t(0x0057B640);
static void EggDisplayer_r(task* tp) {
	if (!gConfigVal.DayNightCycle || !gConfigVal.DayNightShinyTex) {
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
}