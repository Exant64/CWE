#include "stdafx.h"
#include <array>
#include "ChaoMain.h"
#include "al_daynight.h"
#include "al_save.h"
#include <stdexcept>
#include "al_stage.h"
#include "memory.h"
#include <util.h>

//struct per level
struct AL_DayNightConfig {
	const char* const DefaultObjTex;

	const char* const DayTex;
	const char* const RainTex;
	const char* const EveTex;
	const char* const NightTex;

	const char* const DefaultLight;
	const char* const CloudLight;
	const char* const EveLight;
	const char* const NightLight;
};

const std::unordered_map<int, AL_DayNightConfig> gDayNightConfig = { {
	{
		CHAO_STG_NEUT,
		{
			"AL_NEUT_OBJ_TEX",

			"al_stg_neut_tex",
			"al_neut_cld_tex",
			"al_neut_eve_tex",
			"al_neut_ngt_tex",

			"stg00_light.bin",
			"stg00_cld_light.bin",
			"stg00_eve_light.bin",
			"stg00_ngt_light.bin"
		}
	},
	{
		CHAO_STG_HERO,
		{
			"AL_HERO_OBJ_TEX",

			"al_stg_hero_tex",
			"al_hero_cld_tex",
			"al_hero_eve_tex",
			"al_hero_ngt_tex",

			"stg01_light.bin",
			"stg01_cld_light.bin",
			"stg01_eve_light.bin",
			"stg01_ngt_light.bin"
		}
	},
	{
		CHAO_STG_DARK,
		{
			"AL_DARK_OBJ_TEX",

			"al_stg_dark_tex",
			"al_dark_bld_tex",
			NULL,
			NULL,

			"stg02_light.bin",
			"stg02_cld_light.bin",
			NULL,
			NULL
		}
	},
	{
		CHAO_STG_RACE,
		{
			NULL,

			NULL,
			NULL,
			"AL_RACE_eve_N_TEX",
			"AL_RACE_ngt_N_TEX",

			NULL,
			NULL,
			NULL,
			NULL
		}
	},
	{
		CHAO_STG_ENTRANCE,
		{
			NULL,

			NULL,
			NULL,
			"AL_ENTRANCE_EVE",
			"AL_ENTRANCE_NGT",

			NULL,
			NULL,
			NULL,
			NULL
		}
	},
	{
		9999,
		{
			NULL,

			NULL,
			NULL,
			"al_karate_eve_tex",
			"al_karate_ngt_tex",

			NULL,
			NULL,
			NULL,
			NULL
		}
	}
} };

void AL_DayNightTexLoad(int stage)
{
	if (!gDayNightConfig.contains(stage)) return;
	
	const AL_DayNightConfig& config = gDayNightConfig.at(stage);

	switch (GetTimeOfDay()) {
	case AL_TIME_DAY:
		//if has all three important components, if not then its not worth loading anyways
		if (config.DayTex && config.DefaultObjTex && config.RainTex) {
			if (GetWeather() == AL_WEATHER_RAIN) {
				LoadTextureList((char*)config.RainTex, &timeofdayTexlistReplacer);
			}
			else {
				LoadTextureList((char*)config.DayTex, &timeofdayTexlistReplacer);
				LoadTextureList((char*)config.DefaultObjTex, (NJS_TEXLIST*)0x1366B04);
			}
		}
		break;
	case AL_TIME_EVENING:
		if (config.EveTex) {
			LoadTextureList((char*)config.EveTex, &timeofdayTexlistReplacer);
		}
		break;
	case AL_TIME_NIGHT:
		if (config.NightTex) {
			LoadTextureList((char*)config.NightTex, &timeofdayTexlistReplacer);
		}
		break;
	}
}

void AL_DayNightLightLoad(int stage)
{
	if (!gDayNightConfig.contains(stage)) return;

	const AL_DayNightConfig& config = gDayNightConfig.at(stage);

	//if theres no default light, theres no special light assigned to this garden
	if (!config.DefaultLight) return;

	LoadStageLight(config.DefaultLight);

	switch (GetTimeOfDay()) {
	case AL_TIME_DAY:
		if (config.CloudLight && GetWeather() == AL_WEATHER_RAIN) {
			LoadStageLight(config.CloudLight);
		}
		break;
	case AL_TIME_EVENING:
		if (config.EveLight) {
			LoadStageLight(config.EveLight);
		}
		break;
	case AL_TIME_NIGHT:
		if (config.NightLight) {
			LoadStageLight(config.NightLight);
		}
		break;
	}
}

const int sub_478790Ptr = 0x478790;
signed int sub_478790(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3, unsigned __int8 a4)
{
	int re;
	__asm
	{
		mov bl, a1
		push dword ptr[a4]
		push dword ptr[a3]
		push dword ptr[a2]
		call sub_478790Ptr
		mov re, eax
		add esp, 12
	}
	return re;
}

void DayNightTransition(ObjectMaster* a1)
{
	switch (a1->Data1.Entity->Action)
	{
	case 0: //init
		//something something store the previous time here
		a1->Data1.Entity->Action++;
		break;
	case 1: //fade to black, second argument is alpha (weird argument order because usercall) (guess is order is BARG)
		if (sub_478790(0, 255, 0, 0))
		{
			a1->Data1.Entity->Action++;
		}
		break;
	case 2: //counter, you would render the display on this mode
		a1->Data1.Entity->Rotation.x++;
		if (a1->Data1.Entity->Rotation.x > 60 * 2)
		{
			SetTimeOfDay( a1->Data1.Entity->Rotation.y);
			SetWeather( a1->Data1.Entity->Rotation.z); 

			AL_DayNightTexLoad(AL_GetStageNumber() - 1);
			AL_DayNightLightLoad(AL_GetStageNumber() - 1);

			a1->Data1.Entity->Action++;
		}
		break;
	case 3: //fade back
		if (sub_478790(0, 0, 0, 0))
		{
			a1->Data1.Entity->Action++;
		}
		break;
	case 4: //kill
		a1->MainSub = DeleteObject_;
		break;
	}
}

void DayNightTransition_Create(int time, int weather)
{
	if (gConfigVal.NoDayNightTransition)
	{
		SetTimeOfDay(time);
		SetWeather(weather);	
		AL_DayNightTexLoad(AL_GetStageNumber());
		AL_DayNightLightLoad(AL_GetStageNumber());
	}
	else 
	{
		ObjectMaster* ob = LoadObject(4, "Daynight", DayNightTransition, LoadObj_Data1);
		ob->Data1.Entity->Rotation.y = time;
		ob->Data1.Entity->Rotation.z = weather;
	}
}

const int sub_486E50 = 0x486E50;
void sub_486E50Thing()
{
	__asm
	{
		mov ecx, 0
		mov eax, 0
		call sub_486E50
	}
}
void LightingNeut()
{
	AL_DayNightLightLoad(0);
}
void LightingHero()
{
	sub_486E50Thing();
	AL_DayNightLightLoad(1);
}
void LightingDark()
{
	sub_486E50Thing();
	AL_DayNightLightLoad(2);
}

void AL_NEUT_OBJ_TEX_HOOK()
{
	LoadTextureList((char*)"AL_NEUT_OBJ_TEX", (NJS_TEXLIST*)0x1366B04);
	AL_DayNightTexLoad(AL_GetStageNumber());
}
void AL_HERO_OBJ_TEX_HOOK()
{
	LoadTextureList((char*)"AL_HERO_OBJ_TEX", (NJS_TEXLIST*)0x1366B0C);
	AL_DayNightTexLoad(AL_GetStageNumber());
}
void AL_DARK_OBJ_TEX_HOOK()
{
	LoadTextureList((char*)"AL_DARK_OBJ_TEX", (NJS_TEXLIST*)0x1366B14);
	AL_DayNightTexLoad(AL_GetStageNumber());
}

enum {
	MODE_INIT_LT_COPY = 0,
	MODE_LERP
};

static void AL_DayNightCycle_FreeModel(SA2B_Model* pModel) {
	SA2B_VertexData* pVertices = pModel->Vertices;
	while (pVertices->DataType != -1) {
		if (pVertices->DataType == 3) { // vertex color type
			FREE(pVertices->Data);
			break;
		}
		pVertices++;
	}

	FREE(pVertices);
}

static void AL_DayNightCycle_FreeObject(NJS_OBJECT* pObj) {
	if (!pObj) return;

	if (pObj->sa2bmodel) {
		AL_DayNightCycle_FreeModel(pObj->sa2bmodel);
		FREE(pObj->sa2bmodel);
	}
}

static void AL_DayNightCycle_FreeLandTableCOLAndObjects(LandTable* pLand) {
	for (size_t i = 0; i < pLand->ChunkModelCount; i++) {
		COL* pCol = &pLand->COLList[i];

	}

	FREE(pLand->COLList);
}

static void AL_DayNightCycle_CopyGCModel(const SA2B_Model* pSrc, SA2B_Model* pDst) {
	// we only need to clone the vertex colors, so we loop through the vertex types until we find it
	
	const SA2B_VertexData* pSrcVert = pSrc->Vertices;
	size_t vertexColorIndex = -1;

	// we first need the size of the whole thing
	while (pSrcVert->DataType != -1) {
		if (pSrcVert->DataType == 3) { // vertex color datatype == 3
			vertexColorIndex = pSrcVert - pSrc->Vertices;
		}
		pSrcVert++;
	}

	if (vertexColorIndex == -1) {
		// todo: how do we handle this
		PrintDebug(__FUNCTION__  ": One of the models don't have vertex colors!!");
		return;
	}

	const size_t vertexDataCount = (pSrcVert + 1 - pSrc->Vertices);
	pDst->Vertices = ALLOC_ARRAY(vertexDataCount, SA2B_VertexData);
	memcpy(pDst->Vertices, pSrc->Vertices, sizeof(SA2B_VertexData) * vertexDataCount);

	SA2B_VertexData& pVertexColorData = pDst->Vertices[vertexColorIndex];
	pVertexColorData.Data = ALLOC_ARRAY(pVertexColorData.DataSize, uint8_t);
}

static void AL_DayNightCycle_CopyCOLObject(const COL* pSrc, COL* pDst) {
	// !!! we assume there are no children or siblings, DrawLandtable doesn't support hierarchies (thanks shad) !!!
	pDst->Model = ALLOC(NJS_OBJECT);
	*pDst->Model = *pSrc->Model;
	
	// little disorienting naming scheme, the COL's Model refers to NJS_OBJECT, not an actual model
	const SA2B_Model* pSrcModel = pSrc->Model->sa2bmodel;
	if (pSrcModel) {
		SA2B_Model* pDstModel = ALLOC(SA2B_Model);
		pDst->Model->sa2bmodel = pDstModel;

		*pDstModel = *pSrcModel;
		AL_DayNightCycle_CopyGCModel(pSrcModel, pDstModel);
	}
}

static void AL_DayNightCycle_CopyLandTable(const LandTable* pSrc, LandTable* pDst) {
	*pDst = *pSrc;
	
	pDst->COLList = reinterpret_cast<COL*>(ALLOC_ARRAY(pSrc->COLCount, COL));
	memcpy(pDst->COLList, pSrc->COLList, pSrc->COLCount * sizeof(COL));

	for (size_t i = 0; i < pSrc->ChunkModelCount; i++) {
		AL_DayNightCycle_CopyCOLObject(&pSrc->COLList[i], &pDst->COLList[i]);
	}
}

static NJS_ARGB VertColor;

static void AL_DayNightCycle_ApplyVertexColor(const SA2B_VertexData* pSrcVertexColorData, SA2B_VertexData* pDstVertexColorData) {
	// technically NJS_COLOR isn't good, because its ARGB not BGRA
	// but we're dirty little cheaters and use NJS_COLOR anyways (we write to GRA instead of RGB)
	NJS_COLOR* pSrcColors = reinterpret_cast<NJS_COLOR*>(pSrcVertexColorData->Data);
	NJS_COLOR* pDstColors = reinterpret_cast<NJS_COLOR*>(pDstVertexColorData->Data);

	for (size_t i = 0; i < pSrcVertexColorData->ElementCount; i++) {
		NJS_COLOR& color = pDstColors[i];
		color = pSrcColors[i];
		color.argb.g = (Uint8)(color.argb.g * VertColor.r);
		color.argb.r = (Uint8)(color.argb.r * VertColor.g);
		color.argb.a = (Uint8)(color.argb.a * VertColor.b);
	}
	
}

static void AL_DayNightCycle_ApplyColorToLandTable(task* tp) {
	const LandTable* pSrcLand = reinterpret_cast<LandTable * >(tp->Data1.Entity->Rotation.z);
	LandTable* pDstLand = reinterpret_cast<LandTable*>(tp->Data2.Undefined);

	for (size_t colIndex = 0; colIndex < pSrcLand->ChunkModelCount; colIndex++) {
		const COL* pSrcCol = &pSrcLand->COLList[colIndex];
		COL* pDstCol = &pDstLand->COLList[colIndex];

		const NJS_OBJECT* pSrcObject = pSrcCol->Model;
		if (!pSrcObject->sa2bmodel) continue;

		const SA2B_Model* pSrcModel = pSrcCol->Model->sa2bmodel;
		SA2B_Model* pDstModel = pDstCol->Model->sa2bmodel;

		const SA2B_VertexData* pSrcVertices = pSrcModel->Vertices;
		while (pSrcVertices->DataType != 0xFF) {
			if (pSrcVertices->DataType == 3) {
				SA2B_VertexData* pDstVertices = pDstModel->Vertices + (pSrcVertices - pSrcModel->Vertices);
				AL_DayNightCycle_ApplyVertexColor(pSrcVertices, pDstVertices);
				break;
			}
			pSrcVertices++;
		}
	}
}

static void LerpColor(NJS_ARGB& out, const NJS_ARGB& a, const NJS_ARGB& b, float t) {
	out.r = lerp(a.r, b.r, t);
	out.g = lerp(a.g, b.g, t);
	out.b = lerp(a.b, b.b, t);
}

static void AL_DayNightCycleExecutor(task* tp) {
	if (!gConfigVal.DayNightCycle) return;
	if (!AL_IsGarden()) return;

	EntityData1* work = tp->Data1.Entity;

	switch (work->Action) {
		case MODE_INIT_LT_COPY:
			AL_DayNightCycle_CopyLandTable(CurrentLandTable, reinterpret_cast<LandTable*>(tp->Data2.Undefined));
			tp->Data1.Entity->Rotation.z = (int)CurrentLandTable;
			CurrentLandTable = reinterpret_cast<LandTable*>(tp->Data2.Undefined);
			work->Action++;
			break;
		case MODE_LERP:
			work->Rotation.x++;
			if (work->Rotation.x > 60 * 5) {
				work->Rotation.x = 0;
				work->Rotation.y++;
				work->Rotation.y = work->Rotation.y % 3;
			}

			const static NJS_ARGB ColorLerp[] = {
				{1,1,1,1},
				//{1,223/255.0f, 212/255.0f, 177/255.0f},
				{1,1,200 / 255.0f, 152 / 255.0f},
				{1,0.566252f,0.702886f,0.84448f}
			};

			int nextphase = (work->Rotation.y + 1) % 3;
			LerpColor(VertColor, ColorLerp[work->Rotation.y], ColorLerp[nextphase], work->Rotation.x / (60 * 5.0f));

			AL_DayNightCycle_ApplyColorToLandTable(tp);
			break;
	}

	return;

	if (work->Action == 1) {
		//AL_DayNightTexLoad(CurrentChaoArea);
		//AL_DayNightLightLoad(CurrentChaoArea);
	}

	if (AL_IsGarden()) {
		if (work->Rotation.x < 62)
			work->Rotation.x++;

		if (!gConfigVal.DayNightCheat && work->Rotation.x >= 60) {
			ChaoSaveTimer++; //(timer thing)
			if (ChaoSaveTimer >= (60 * 60 * 18))
			{
				ChaoSaveTimer = 0; //reset timer
				SetTimeOfDay(AL_TIME_DAY); //reset the time to day
				if (njRandom() < 0.4f)
					SetWeather(AL_WEATHER_RAIN);   //set weather to rain
				else
					SetWeather(AL_WEATHER_NONE);
				//DayNightTransition_Create(AL_TIME_DAY, GetWeather());
			}
			else if (ChaoSaveTimer % (60 * 60 * 6) == 0)
			{
				//DayNightTransition_Create(GetTimeOfDay() + 1, GetWeather());
			}
		}
	}
}


static void AL_DayNightCycleDestructor(task* tp) {
	LandTable* pLand = reinterpret_cast<LandTable*>(tp->Data2.Undefined);

	AL_DayNightCycle_FreeLandTableCOLAndObjects(pLand);
	FREE(pLand);
}

void AL_CreateDayNightCycle() {
	task* tp = LoadObject(4, "AL_DayNightCycle", AL_DayNightCycleExecutor, LoadObj_Data1);

	tp->Data2.Undefined = ALLOC(LandTable);
	tp->DeleteSub = AL_DayNightCycleDestructor;
}

void AL_DayNight_Init() {
}