#include "stdafx.h"
#include <Chao.h>
#include <ninja_functions.h>
#include <al_texlist.h>
#include <d3d9.h>
#include "../BrightFix/BrightFix/structs.h"
#include <usercall.h>
#include <ALifeSDK_Functions.h>
#include <d3d9.h>
#include "ChaoMain.h"
#include <al_modelcontainer.h>
#include <al_sandhole.h>
#include <ui/al_tween.h>
#include <al_speechbubble.h>
#include <al_draw.h>
#include <ui/al_ortho.h>
#include <al_parts.h>
#include <brightfixapi.h>
#include <rendertarget.h>

enum {
	SPEECH_MD_SPAWN = 0,
	SPEECH_MD_TIMER,
	SPEECH_MD_DIE,
};

struct speechwk {
	Uint8 mode;
	task* pChao;
	float scl;
	Uint32 entryType;
	Uint32 entryID;
	int spawnTimer;
	int aliveTimer;
	eSpeechPos position;
};

#define GET_SPEECHWK(tp) ((speechwk*)(tp->Data2.Undefined))

enum {
	SPEECH_BUBBLE_TRANSLATE = BIT_0,
	SPEECH_BUBBLE_ROTATION = BIT_1,
	SPEECH_BUBBLE_SCALE = BIT_2,
	SPEECH_BUBBLE_SKIP_PER_INDEX = BIT_3, // 2 * index
	SPEECH_BUBBLE_USE_TEXPAIR = BIT_4
};

struct speech_bubble_entry {
	NJS_OBJECT* pObj;
	NJS_TEXLIST* pTexlist;
	ChaoItemCategory category;
	Uint32 type;

	Uint32 flags;
	NJS_POINT3 translate;
	Rotation rotation;
	NJS_POINT3 scale;
};

// bit hacked on, but if some entries require a different texlist i use this pair thing
struct speech_bubble_entry_objtex_pair {
	NJS_OBJECT* pObj;
	NJS_TEXLIST* pTex;
};

struct speech_bubble_entry_new {
	Uint32 category;

	// if these are not specified, use ObjectRegistry (fruits, hats, accessories, etc)
	NJS_OBJECT** pObjList; // optional
	NJS_TEXLIST* pTexlist; // optional

	speech_bubble_entry_objtex_pair* pPairs;

	Uint32 flags;
	float offsetY;
	float scl;
	NJS_POINT3 translate;
	Rotation rotation;
	NJS_POINT3 scale;
};

static NJS_OBJECT DrumFix = {
	NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL,
	((NJS_OBJECT*)0x11C62C4)->chunkmodel
};

extern NJS_OBJECT object_alo_accordion;
extern NJS_OBJECT object_alo_guitar;
static NJS_OBJECT* MusicObjList[] = {
	(NJS_OBJECT*)0x11C5B3C, // bell
	(NJS_OBJECT*)0x11C7A9C, // castanas or whatever they're called
	(NJS_OBJECT*)0x11C8964, // cymbal
	&DrumFix,				// drum
	(NJS_OBJECT*)0x11C73A4, // flute
	(NJS_OBJECT*)0x11C505C, // maracas
	(NJS_OBJECT*)0x11C830C, // trumpet
	(NJS_OBJECT*)0x11C9AE4, // 
};

static speech_bubble_entry_objtex_pair MusicObjListCWE[] = {
	{&object_alo_guitar, &CWE_OBJECT_TEXLIST},
	{&object_alo_accordion, &AL_TOY_TEXLIST}
};

static speech_bubble_entry_new entries_new[NB_SPEECH_ENTRY] = {
	{ALW_CATEGORY_FRUIT, NULL, NULL, NULL, 0, 0, 1},
	{ALW_CATEGORY_ACCESSORY, NULL, NULL, NULL, 0, 0, 1},
	{ALW_CATEGORY_MASK, NULL, NULL, NULL, 0, 0, 1},
	{ALW_CATEGORY_SPECIAL, NULL, NULL, NULL, 0, 0, 1},
	{-1, (NJS_OBJECT**)0x012E58FC, (NJS_TEXLIST*)0x1366AD4, NULL, 0, -75, 1.5f}, // MEDAL (big medals, offset by 1, ids start at 0)
	{-1, (NJS_OBJECT**)0x0136184C, &AL_TOY_TEXLIST, NULL, 0, 0, 0.85f}, // LTOY (race LToyRewards list)
	//{-1, (NJS_OBJECT**)0x0136184C, &AL_TOY_TEXLIST, 0, 0, 0.85f}, // LTOY_CWE
	{-1, (NJS_OBJECT**)0x1361888, &AL_TOY_TEXLIST, NULL, SPEECH_BUBBLE_SKIP_PER_INDEX, 0, 0.85f}, // STOY
	{-1, MusicObjList, &AL_TOY_TEXLIST, NULL, 0, -100, 3}, // Music
	{-1, NULL, NULL, MusicObjListCWE, 0, -100, 3}, // MusicCWE
	{-1, (NJS_OBJECT**)GetDllData("MayuObjectList"), &AL_BODY, NULL, 0, 75, 1} // Cocoon
};

static speech_bubble_entry entries[NB_SPEECH_ENTRY] = {
	{NULL, NULL, ChaoItemCategory_Fruit, SA2BFruit_SquareFruit, 0},
	{NULL, NULL, ChaoItemCategory_Fruit, SA2BFruit_RoundFruit, 0},
	{NULL, NULL, ChaoItemCategory_Fruit, SA2BFruit_TriangleFruit, 0},

	{(NJS_OBJECT*)0x0130111C, (NJS_TEXLIST*)0x01366AFC, ChaoItemCategory_Fruit, SA2BFruit_TriangleFruit, SPEECH_BUBBLE_TRANSLATE, {0, -2.547241f, -0.154359f}},
};

#pragma region Mock Chao

// This is a very "minimal" chao task, set up to have just enough to render a chao without issues

FunctionPointer(void, AL_ShapeInit, (task* tp), 0x0056C9D0);

const int sub_539F90Ptr = 0x539F90;
static void AL_FaceInit(task* tp) {
	__asm {
		mov ebx, tp
		call sub_539F90Ptr
	}
}

const int AL_MotionInitPtr = 0x0055C370;
static void AL_MotionInit(task* tp) {
	__asm {
		mov eax, tp
		call AL_MotionInitPtr
	}
}

const int MotionControlPtr = 0x007938D0;
static void MotionControl(MotionTableData* motion) {
	__asm {
		mov esi, motion
		call MotionControlPtr
	}
}

static task* AL_SpeechBubble_CreateMockChao(ChaoData* chaoData) {
	task* pChao = LoadObject(4, "MockChao", [](task*) {}, (LoadObj)0);

	pChao->DeleteSub = Chao_Delete;

	chaowk* work = ALLOC(ChaoData1);
	memset(work, 0, sizeof(ChaoData1));

	pChao->Data1.Undefined = work;

	work->pParamGC = (ChaoDataBase*)ALLOC(ChaoData);
	work->isParamCopy = TRUE;
	memcpy(work->pParamGC, chaoData, sizeof(ChaoData));

	AL_MotionInit(pChao);
	AL_ShapeInit(pChao);
	AL_FaceInit(pChao);
	sub_566B80(pChao);

	AL_SetMotionLink(pChao, 0);
	MotionControl(&work->MotionTable);

	return pChao;
}
#pragma endregion

// utility function to find the first model inside a hierarchy
// use it to (very) roughly estimate the distance needed from the camera to render an item
static NJS_CNK_MODEL* FindFirstModel(NJS_OBJECT* obj) {
	while (obj) {
		if (obj->chunkmodel) return obj->chunkmodel;

		if (obj->child) {
			const auto mdl = FindFirstModel(obj->child);
			if (mdl) return mdl;
		}

		obj = obj->sibling;
	}
	return NULL;
}

static void AL_SpeechBubble_RenderChao(task* pChao) {
	njPushMatrixEx();
	njUnitMatrix(0);
	DoLighting(10);

	LoadNewShaders();
	OrthoDrawBegin();
	OrthoScreenTranslate(320, 240 + 50, 7.5f);
	OnlyDrawHeadChao(pChao);
	OrthoDrawEnd();
	CancelNewShaders();

	njPopMatrixEx();
}

static void AL_SpeechBubble_RenderItem(speech_bubble_entry_new& entry, Uint32 entryID) {
	if (entry.flags & SPEECH_BUBBLE_SKIP_PER_INDEX) entryID *= 2;

	const auto& obj = entry.pObjList ? entry.pObjList[entryID] : ObjectRegistry::Get(ChaoItemCategory(entry.category))->GetObj(entryID);
	const auto& texlist = entry.pTexlist ? entry.pTexlist : ObjectRegistry::Get(ChaoItemCategory(entry.category))->GetTex(entryID);
	const auto& mdl = FindFirstModel(obj);

	njPushMatrixEx();
	njUnitMatrix(0);
	DoLighting(10);
	LoadNewShaders();
	OrthoDrawBegin();
	OrthoScreenTranslate(320, 240 + entry.offsetY, 4.5f);

	static int ang = 0;
	ang += 0x100;
	njRotateY(NULL, ang);

	njScale(NULL, entry.scl, entry.scl, entry.scl);

	njSetTexture(texlist);
	chCnkDrawObject(obj);

	OrthoDrawEnd();
	CancelNewShaders();
	njPopMatrixEx();
}

// the 2D rendering function I use for the speechbox sets the rendering texture inside of it
// so this is a hacky way to set the necessary parameter to a dummy struct
// the TextureData is a MagicSDK texture, the only thing read by the rendering function is the directx texture pointer
static void SetTextureRenderTargetHack() {
	static NJS2_TEXSURFACE texsurface = { 0 };
	static TextureData texData = { 0 };

	texData.D3DTexture = GetRenderTarget();
	texsurface.Type = 0x0;
	texsurface.pSurface = (Uint32*)&texData;

	_nj_curr_ctx_->texsurface = &texsurface;
}

static NJS_POINT3 AL_SpeechBubble_GetPosition(task* pChao, eSpeechPos position) {
	const size_t iconPositionOffset = 0x6EC + 0x2C;
	NJS_POINT3 viewPosition;

	switch (position) {
		case SPEECH_POS_LEFT:
			sub_426CC0(_nj_current_matrix_ptr_, &viewPosition, &GET_CHAOWK(pChao)->HeadTranslationPos, 0);
			viewPosition.x += -3;
			break;
		case SPEECH_POS_RIGHT:
			sub_426CC0(_nj_current_matrix_ptr_, &viewPosition, &GET_CHAOWK(pChao)->HeadTranslationPos, 0);
			viewPosition.x += 3;
			break;
		case SPEECH_POS_TOP:
			sub_426CC0(_nj_current_matrix_ptr_, &viewPosition, (NJS_VECTOR*)((char*)GET_CHAOWK(pChao) + iconPositionOffset), 0);
			break;
	}
	
	viewPosition.y += 1.5f;


	return viewPosition;
}

static void AL_SpeechBubbleDisplayRenderTarget(task *tp) {
	auto work = GET_SPEECHWK(tp);

	auto backupLight = LightsGC[10];
	LightsGC[10].SomeFlag = 1;
	LightsGC[10].ambientReg = { 0.75f, 0.75f, 0.75f };
	LightsGC[10].lightColor = { 1, 1, 1 };
	LightsGC[10].direction = { 0,0,-1 };

	SetShaders(1);
	SetMaterial(1, 1, 1, 1);

	StartRenderTarget();
	if (work->pChao) {
		AL_SpeechBubble_RenderChao(work->pChao);
	}
	else {
		AL_SpeechBubble_RenderItem(entries_new[work->entryType], work->entryID);
	}
	StopRenderTarget();

	LightsGC[10] = backupLight;
}

const static NJS_TEXTURE_VTX SpeechBubbleTex[4] = {
	{ -1,  1.0, 0, 0, 0, 0xFFFFFFFF },
	{ -1, -1.0, 0, 0, 1, 0xFFFFFFFF },
	{  1,  1.0, 0, 1, 0, 0xFFFFFFFF },
	{  1, -1.0, 0, 1, 1, 0xFFFFFFFF }
};

static void AL_SpeechBubbleDisplayer(task* tp) {
	auto work = GET_SPEECHWK(tp);
	const float scl = work->scl;
	NJS_POINT3 position = AL_SpeechBubble_GetPosition(tp->Parent, work->position);

	AL_SpeechBubbleDisplayRenderTarget(tp);

	SetShaders(0);

	njPushMatrix((NJS_MATRIX_PTR)0x025F02A0);
	njSetTexture(&CWE_UI_TEXLIST);

	njTranslateEx(&position);
	njScale(NULL, scl, scl, 1);

	njPushMatrixEx();
	float flip = 1;
	if (work->position == SPEECH_POS_RIGHT) {
		flip = -1;
	}
	njScale(NULL, flip * 1.1f, 1.1f, 1.1f);
	njSetTextureNum(35);
	njDrawTexture3DExSetData(SpeechBubbleTex, 4);
	njPopMatrixEx();

	SetTextureRenderTargetHack();

	// preserve aspect ratio of rendertarget + extra downscaling
	const float sizeMul = 1.25f;
	njScale(NULL, sizeMul, sizeMul * VerticalResolution / HorizontalResolution, 1);
	njDrawTexture3DExSetData(SpeechBubbleTex, 4);

	njPopMatrixEx();
}

static void AL_SpeechBubbleExecutor(task* tp) {
	auto work = GET_SPEECHWK(tp);

	switch (work->mode) {
		case SPEECH_MD_TIMER:
			if (--work->aliveTimer <= 0) {
				CreateTween(tp, EASE_OUT, INTERP_EXPO, &work->scl, 0.0f, work->spawnTimer, [](task* pParent) {
					DeleteObject_(pParent);
				});

				work->mode = SPEECH_MD_DIE;
			}
			break;
	}
}

void AL_SpeechBubbleDestructor(task* tp) {
	task* chao = GET_SPEECHWK(tp)->pChao;
	if (chao) {
		DeleteObject_(chao);
	}
}

task* AL_SpeechBubbleCreate(task* pChao, ChaoData* pChaoData, Uint32 entryType, Uint32 entry, eSpeechPos position, Uint32 spawnTimer, Uint32 aliveTimer) {
	task* obj = LoadChildObject(LoadObj(0), AL_SpeechBubbleExecutor, pChao);

	obj->field_1C = AL_SpeechBubbleDisplayer;
	obj->DeleteSub = AL_SpeechBubbleDestructor;

	speechwk* work = ALLOC(speechwk);
	obj->Data2.Undefined = work;

	work->mode = SPEECH_MD_SPAWN;
	work->pChao = NULL;
	work->scl = 0.f;
	work->entryType = entryType;
	work->entryID = entry;
	work->position = position;
	work->spawnTimer = spawnTimer;
	work->aliveTimer = aliveTimer;

	if (pChaoData) {
		work->pChao = AL_SpeechBubble_CreateMockChao(pChaoData);
	}

	CreateTween(obj, EASE_IN, INTERP_EXPO, &work->scl, 1.35f, spawnTimer, [](task* pParent) {
		GET_SPEECHWK(pParent)->mode = SPEECH_MD_TIMER;
	});

	return obj;
}