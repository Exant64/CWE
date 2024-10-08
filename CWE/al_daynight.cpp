#include "stdafx.h"
#include <array>
#include "ChaoMain.h"
#include "al_daynight.h"
#include "al_save.h"
#include <stdexcept>
#include "al_stage.h"
#include "memory.h"
#include <util.h>
#include <ninja_functions.h>
#include <al_sandhole.h>
#include "renderfix_api.h"

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

static const std::unordered_map<std::string, int> JsonNameToApi = { {
	{"neut", CHAO_STG_NEUT},
	{"hero", CHAO_STG_HERO},
	{"dark", CHAO_STG_DARK}
}};

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

NJS_TEXNAME HeroSkyTestTex[15];
NJS_TEXLIST HeroSkyTest = { HeroSkyTestTex, 15 };

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

enum {
	LIGHT_DEF = 0,
	LIGHT_EVE = 1,
	LIGHT_NGT = 2,
	LIGHT_CLOUD = 3
};

struct DAYNIGHT_SKYBOX {
	uint32_t origTexID; // tex ID to look for to find mesh
	uint32_t defaultTexID;
	uint32_t eveningTexID;
	uint32_t nightTexID;
	uint32_t cloudyTexID;
};

#define PLACEHOLDER_SKYBOX(orig, replace, number) {orig, replace, replace + number, replace + number * 2, replace},

static DAYNIGHT_SKYBOX placeholderSkyboxList[] = {
	PLACEHOLDER_SKYBOX(27, 0, 5)
	PLACEHOLDER_SKYBOX(28, 1, 5)
	PLACEHOLDER_SKYBOX(29, 2, 5)
	PLACEHOLDER_SKYBOX(30, 3, 5)
	PLACEHOLDER_SKYBOX(31, 4, 5)
};

struct DAYNIGHT_SKYBOX_TEXMAP_TABLE {
	uint32_t* pTexID;
	DAYNIGHT_SKYBOX skybox;
};

struct DAYNIGHT_SKYBOX_TABLE {
	COL* pCOL;
	DAYNIGHT_SKYBOX_TEXMAP_TABLE* pTexMap;
	size_t texMapCount;
};

struct DAYNIGHT_WORK {
	int mode;
	LandTable* pNewLandtable;
	LandTable* pOldLandtable;
	SA2B_VertexData** pColorTable; // pair of src and dst vertexcolor entries (ChunkModelCount * 2 amount of pointers)
	
	int timer;
	int phase;
	NJS_ARGB appliedColor;

	// 4 for the phases, 4 for the lights in the file
	Light lights[4][4];
	LightGC lightsGC[4][4];

	DAYNIGHT_SKYBOX_TABLE* pSkyboxTable;
	int skyboxCount;
};

static DAYNIGHT_WORK& GetWork(task* tp) {
	return *reinterpret_cast<DAYNIGHT_WORK*>(tp->Data2.Undefined);
}

// Frees the copied object, the sa2bmodel, and the vertex colors it copies
static void AL_DayNightCycle_FreeObject(NJS_OBJECT* pObj, SA2B_VertexData* pDstColors) {
	if (!pObj) return;

	if (pObj->sa2bmodel && pDstColors) {
		FREE(pDstColors->Data);
		FREE(pObj->sa2bmodel->Vertices);
		FREE(pObj->sa2bmodel);
	}

	FREE(pObj);
}

// Frees the LandTable, its COLs and all their related data
static void AL_DayNightCycle_FreeLandTableCOLAndObjects(task* tp) {
	auto& work = GetWork(tp);
	LandTable* pLand = work.pNewLandtable;

	for (size_t i = 0; i < pLand->ChunkModelCount; i++) {
		AL_DayNightCycle_FreeObject(pLand->COLList[i].Model, work.pColorTable[1]);
	}

	FREE(pLand->COLList);

	FREE(pLand);
}

// Finds the original texture ID in the skybox texture map table, and retrieves the texture IDs to map it to
static bool AL_DayNightCycle_FindSkyboxTexID(uint32_t texID, DAYNIGHT_SKYBOX** pSkybox) {
	for (uint32_t i = 0; i < _countof(placeholderSkyboxList); i++) {
		if (texID == placeholderSkyboxList[i].origTexID) { 
			*pSkybox = &placeholderSkyboxList[i];
			return true; 
		}
	}
	return false;
}

// Checks if the COL's model has any skybox textures, if yes then it fills the list of texture IDs to change
// (and what to change it to)
static bool AL_DayNightCycle_CheckSkybox(const COL* pSrc, DAYNIGHT_SKYBOX_TABLE& entry) {
	const SA2B_Model* pModel = pSrc->Model->sa2bmodel;
	const size_t sumGeoCount = pModel->OpaqueGeometryCount + pModel->TranslucentGeometryCount;

	std::unique_ptr<DAYNIGHT_SKYBOX_TEXMAP_TABLE[]> texMapArray(new DAYNIGHT_SKYBOX_TEXMAP_TABLE[sumGeoCount]);
	size_t texMapCount = 0;

	const auto checkAndAddGeo = [&](const SA2B_GeometryData* pGeo) {
		for (size_t i = 0; i < pGeo->ParameterCount; i++) {
			uint32_t* para = reinterpret_cast<uint32_t*>(&pGeo->ParameterOffset[i * 2]);
			const auto type = reinterpret_cast<const uint8_t*>(para)[0];

			if (type == 8) { // "texture" parameter type
				const auto texID = para[1] & 0xFFFF;

				DAYNIGHT_SKYBOX* pSkyboxEntry;
				if (AL_DayNightCycle_FindSkyboxTexID(texID, &pSkyboxEntry)) {
					auto& texMap = texMapArray[texMapCount++];

					texMap.pTexID = &para[1];
					texMap.skybox = *pSkyboxEntry;
				}
			}
		}
	};

	for (size_t i = 0; i < pModel->OpaqueGeometryCount; i++) {
		checkAndAddGeo(&pModel->OpaqueGeoData[i]);
	}

	for (size_t i = 0; i < pModel->TranslucentGeometryCount; i++) {
		checkAndAddGeo(&pModel->TranslucentGeoData[i]);
	}

	if (texMapCount) {
		entry.pTexMap = ALLOC_ARRAY(texMapCount, DAYNIGHT_SKYBOX_TEXMAP_TABLE);
		entry.texMapCount = texMapCount;

		memcpy(entry.pTexMap, texMapArray.get(), texMapCount * sizeof(DAYNIGHT_SKYBOX_TEXMAP_TABLE));
	}

	return texMapCount > 0;
}

// Copies the GC model, it's vertex data (for the colors) and gets the pointer to the vertex colors to not have to look for them again
static void AL_DayNightCycle_CopyGCModel(SA2B_VertexData** pVertexColorTable, const SA2B_Model* pSrc, SA2B_Model* pDst) {
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

	pVertexColorTable[0] = pSrc->Vertices + vertexColorIndex;
	pVertexColorTable[1] = pDst->Vertices + vertexColorIndex;
}

// Copies the COL and its vertex colors if not skybox
// if it's a skybox, add it to the list of skyboxes to interpolate the textures on
static void AL_DayNightCycle_CopyCOLObject(const COL* pSrc, COL* pDst, SA2B_VertexData** pVertexColorTable, std::unique_ptr<DAYNIGHT_SKYBOX_TABLE []>& pSkyboxColList, uint32_t& skyboxCount) {
	if (AL_DayNightCycle_CheckSkybox(pSrc, pSkyboxColList[skyboxCount])) {
		pSkyboxColList[skyboxCount++].pCOL = pDst;
		pDst->Flags &= ~0x80000000; // remove visibility col flag, we'll handle rendering
		return;
	}

	// !!! we assume there are no children or siblings, DrawLandtable doesn't support hierarchies (thanks shad) !!!
	pDst->Model = ALLOC(NJS_OBJECT);
	*pDst->Model = *pSrc->Model;
	
	// little disorienting naming scheme, the COL's Model refers to NJS_OBJECT, not an actual model
	const SA2B_Model* pSrcModel = pSrc->Model->sa2bmodel;
	if (pSrcModel) {
		SA2B_Model* pDstModel = ALLOC(SA2B_Model);
		pDst->Model->sa2bmodel = pDstModel;

		*pDstModel = *pSrcModel;
		
		AL_DayNightCycle_CopyGCModel(pVertexColorTable, pSrcModel, pDstModel);	
	}
}

// Initializes the LandTable copy, and the skybox entries
static void AL_DayNightCycle_InitNewLandTable(task* tp) {
	auto& work = GetWork(tp);

	const LandTable* pSrc = CurrentLandTable;
	LandTable* pDst = ALLOC(LandTable);
	work.pNewLandtable = pDst;

	// create the skybox table, this is a temporary pointer 
	// since we don't know ahead how many skybox objects we find let's assume all of them are skyboxes
	// and then later on in the code we only actually allocate the ones we found
	// and this will get automatically freed
	std::unique_ptr<DAYNIGHT_SKYBOX_TABLE[]> pSkyboxColList(new DAYNIGHT_SKYBOX_TABLE [pSrc->ChunkModelCount]);
	uint32_t skyboxColCount = 0;

	// copy the landtable
	*pDst = *pSrc;
	
	// create the COLList and copy it
	pDst->COLList = reinterpret_cast<COL*>(ALLOC_ARRAY(pSrc->COLCount, COL));
	memcpy(pDst->COLList, pSrc->COLList, pSrc->COLCount * sizeof(COL));

	// table of src and dst pointers for each col, to not have to find them every frame
	const size_t pointerCount = pSrc->ChunkModelCount * 2;
	work.pColorTable = ALLOC_ARRAY(pointerCount, SA2B_VertexData*);
	memset(work.pColorTable, 0, sizeof(SA2B_VertexData*) * pointerCount);

	// copy the visual COLs' models
	for (size_t i = 0; i < pSrc->ChunkModelCount; i++) {
		AL_DayNightCycle_CopyCOLObject(&pSrc->COLList[i], &pDst->COLList[i], &work.pColorTable[i * 2], pSkyboxColList, skyboxColCount);
	}

	// if we found skyboxes, let's allocate the amount we found and copy it all
	if (skyboxColCount > 0) {
		work.pSkyboxTable = ALLOC_ARRAY(skyboxColCount, DAYNIGHT_SKYBOX_TABLE);
		work.skyboxCount = skyboxColCount;

		memcpy(work.pSkyboxTable, pSkyboxColList.get(), sizeof(DAYNIGHT_SKYBOX_TABLE) * skyboxColCount);
	}
}

// Applies the color we want to the destination vertexdata, using the source vertexdata as the base color
static void AL_DayNightCycle_ApplyVertexColor(task* tp, const SA2B_VertexData* pSrcVertexColorData, SA2B_VertexData* pDstVertexColorData) {
	const auto& applyColor = GetWork(tp).appliedColor;

	// technically NJS_COLOR isn't good, because in the GC models it's ARGB not BGRA
	// but we're dirty little cheaters and use NJS_COLOR anyways (we write to GRA instead of RGB)
	NJS_COLOR* pSrcColors = reinterpret_cast<NJS_COLOR*>(pSrcVertexColorData->Data);
	NJS_COLOR* pDstColors = reinterpret_cast<NJS_COLOR*>(pDstVertexColorData->Data);

	for (size_t i = 0; i < pSrcVertexColorData->ElementCount; i++) {
		NJS_COLOR& color = pDstColors[i];
		color = pSrcColors[i];
		color.argb.g = (Uint8)(color.argb.g * applyColor.r);
		color.argb.r = (Uint8)(color.argb.r * applyColor.g);
		color.argb.a = (Uint8)(color.argb.a * applyColor.b);
	}
	
}

// Applies the color we want to all the landtable models
static void AL_DayNightCycle_ApplyColorToLandTable(task* tp) {
	SA2B_VertexData** pVertexColorTable = GetWork(tp).pColorTable;
	
	for (size_t colIndex = 0; colIndex < CurrentLandTable->ChunkModelCount; colIndex++) {	
		// we turn off visibility for skybox models since *we* handle rendering them
		// so we check for visibility to not lerp the color on those
		if (!(CurrentLandTable->COLList[colIndex].Flags & 0x80000000)) continue;

		const SA2B_VertexData* pSrcVertices = pVertexColorTable[colIndex * 2];
		SA2B_VertexData* pDstVertices = pVertexColorTable[colIndex * 2 + 1];

		if (!pDstVertices) continue;

		AL_DayNightCycle_ApplyVertexColor(tp, pSrcVertices, pDstVertices);
	}
}

// Lerp color for RGB only (without alpha) which we represent with an NJS_VECTOR for now
static void LerpColor(NJS_VECTOR& out, const NJS_VECTOR& a, const NJS_VECTOR& b, float t) {
	out.x = lerp(a.x, b.x, t);
	out.y = lerp(a.y, b.y, t);
	out.z = lerp(a.z, b.z, t);
}

// Lerp color for RGB part of an NJS_ARGB
static void LerpColor(NJS_ARGB& out, const NJS_ARGB& a, const NJS_ARGB& b, float t) {
	out.r = lerp(a.r, b.r, t);
	out.g = lerp(a.g, b.g, t);
	out.b = lerp(a.b, b.b, t);
}

// Lerp GC light
static void LerpLight(LightGC& out, const LightGC& a, const LightGC& b, float t) {
	LerpColor(out.lightColor, a.lightColor, b.lightColor, t);
	LerpColor(out.ambientReg, a.ambientReg, b.ambientReg, t);
}

// Lerp DC light
static void LerpLight(Light& out, const Light& a, const Light& b, float t) {
	LerpColor(out.color, a.color, b.color, t);
	out.ambient = lerp(a.ambient, b.ambient, t);
	out.intensity = lerp(a.intensity, b.intensity, t);
}

// Copy the game's lights into ours
static void CopyLights(task* tp, size_t index) {
	auto& work = GetWork(tp);
	memcpy(&work.lights[index], Lights, sizeof(work.lights[index]));
	memcpy(&work.lightsGC[index], LightsGC, sizeof(work.lightsGC[index]));
}

// Main update function for the DNC
static void AL_DayNightCycleExecutor(task* tp) {
	if (!gConfigVal.DayNightCycle) return;
	if (!AL_IsGarden()) return;

	auto& work = GetWork(tp);

	switch (work.mode) {
		case MODE_INIT_LT_COPY:
			// temp
			LoadChaoTexlist("AL_HERO_SKY", &HeroSkyTest, 1);

			// copy the landtable and make the game render our new one, and initialize the skyboxes
			AL_DayNightCycle_InitNewLandTable(tp);
			work.pOldLandtable = CurrentLandTable;
			CurrentLandTable = work.pNewLandtable;

			work.mode++;

			// load the lights we want and copy them (todo maybe this is too hacky)
			{
				// this daynightconfig thing is temp for now too
				const AL_DayNightConfig& config = gDayNightConfig.at(CurrentChaoArea);

				LoadStageLight(config.DefaultLight);
				CopyLights(tp, LIGHT_DEF);
				LoadStageLight(config.EveLight);
				CopyLights(tp, LIGHT_EVE);
				LoadStageLight(config.NightLight);
				CopyLights(tp, LIGHT_NGT);
				LoadStageLight(config.CloudLight);
				CopyLights(tp, LIGHT_CLOUD);
			}

			break;
		case MODE_LERP:
			// most of this is just temp to test the rendering and all that
			work.timer++;
			if (work.timer > 60 * 5) {
				work.timer = 0;

				work.phase++;
				work.phase = work.phase % 3;
			}

			const static NJS_ARGB ColorLerp[] = {
				{1,1,1,1},
				//{1,223/255.0f, 212/255.0f, 177/255.0f},
				{1,0.566252f,0.702886f,0.84448f},
				{1,1,200 / 255.0f, 152 / 255.0f}
				
			};

			int nextPhase = (work.phase + 1) % 3;
			
			LerpColor(work.appliedColor, ColorLerp[work.phase], ColorLerp[nextPhase], work.timer / (60 * 5.0f));
			for (size_t i = 0; i < 4; i++) {
				LerpLight(Lights[i], work.lights[work.phase][i], work.lights[nextPhase][i], work.timer / (60 * 5.0f));
				LerpLight(LightsGC[i], work.lightsGC[work.phase][i], work.lightsGC[nextPhase][i], work.timer / (60 * 5.0f));
			}
			
			AL_DayNightCycle_ApplyColorToLandTable(tp);
			break;
	}

	// old dnc
#if 0
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
#endif
}

// Frees all the allocated copied stuff and skybox stuff
static void AL_DayNightCycleDestructor(task* tp) {
	auto& work = GetWork(tp);

	AL_DayNightCycle_FreeLandTableCOLAndObjects(tp);

	FREE(work.pColorTable);

	if (work.skyboxCount) {
		for (size_t i = 0; i < work.skyboxCount; i++) {
			FREE(work.pSkyboxTable[i].pTexMap);
		}
		FREE(work.pSkyboxTable);
	}

	FREE(tp->Data2.Undefined);
}

const int sub_42B5A0Ptr = 0x42B5A0;
void sub_42B5A0(SA2B_Model* a1) {
	__asm {
		mov ebx, a1
		call sub_42B5A0Ptr
	}
}

VoidFunc(SaveControl3D, 0x446D00);
VoidFunc(LoadControl3D, 0x446D10);
VoidFunc(SaveConstantAttr, 0x446CB0);
VoidFunc(LoadConstantAttr, 0x446CD0);

static const void* const OnControl3DPtr = (void*)0x446D20;
static inline void OnControl3D(int flag)
{
	__asm
	{
		mov eax, [flag]
		call OnControl3DPtr
	}
}

static const void* const OnConstantAttrPtr = (void*)0x446CF0;
static inline void OnConstantAttr(int soc_and, int soc_or)
{
	__asm
	{
		mov eax, [soc_and]
		mov ecx, [soc_or]
		call OnConstantAttrPtr
	}
}

FunctionPointer(void, sub_4264D0, (int a1), 0x4264D0);
FunctionPointer(void, sub_42BA60, (unsigned int a1), 0x42BA60);
static RFAPI_CORE* rfapi_core;

static void ToggleLighting(SA2B_Model* pModel, bool toggle) {
	const auto changeGeoLightingFlag = [&](SA2B_GeometryData& geo) {
		for (int j = 0; j < geo.ParameterCount * 2; j += 2) {
			if (geo.ParameterOffset[j] == 2) {
				int& flag = geo.ParameterOffset[j + 1];

				if (!toggle) {
					flag &= ~2;
					flag |= (1 << 11);
				}
				else {
					flag |= 2;
					flag &= ~(1 << 11);
				}
			}
		}
	};

	for (int i = 0; i < pModel->OpaqueGeometryCount; i++) {
		changeGeoLightingFlag(pModel->OpaqueGeoData[i]);
	}

	for (int i = 0; i < pModel->TranslucentGeometryCount; i++) {
		changeGeoLightingFlag(pModel->TranslucentGeoData[i]);
	}
}

static void AL_DayNightCycleDisplayer(task* tp) {
	auto& work = GetWork(tp);
	// placeholder
	const float t = work.timer / float(5 * 60);

	// landtables use this light index for some reason
	DoLighting(2); 

	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(1, NJD_COLOR_BLENDING_INVSRCALPHA);

	uint32_t& blendModeGinjaFunc = *(uint32_t*)0x0174B344;
	uint32_t blendModeGinjaFuncBackup = blendModeGinjaFunc;
	blendModeGinjaFunc = (int)nullsub_1;

	rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_TRANSPARENT);

	const auto getPhaseTextureID = [](const DAYNIGHT_SKYBOX& skyboxEntry, int phase) {
		switch (phase) {
		case 1:
			return skyboxEntry.eveningTexID;
		case 2:
			return skyboxEntry.nightTexID;
		default:
			return skyboxEntry.defaultTexID;
		}
	};

	for (size_t i = 0; i < work.skyboxCount; i++) {
		auto& skybox = work.pSkyboxTable[i];

		COL* pCol = skybox.pCOL;

		const auto changeAllTexturesToPhase = [&](int phase) {
			for (size_t t = 0; t < skybox.texMapCount; t++) {
				const auto changeTex = [](uint32_t& data, uint32_t texID) {
					data &= 0xFFFF0000;
					data |= texID;
				};

				auto& texMap = skybox.pTexMap[t];
				changeTex(*texMap.pTexID, getPhaseTextureID(texMap.skybox, phase));
			}
		};

		njPushMatrixEx();

		njSetTexture(&HeroSkyTest);

		njTranslate(NULL, pCol->Model->pos[0], pCol->Model->pos[1], pCol->Model->pos[2]);

		RotateZ(pCol->Model->ang[2]);
		RotateY(pCol->Model->ang[1]);
		RotateX(pCol->Model->ang[0]);

		SaveControl3D();
		SaveConstantAttr();

		OnControl3D(NJD_CONTROL_3D_CONSTANT_MATERIAL);

		SA2B_Model* model = pCol->Model->sa2bmodel;

		ToggleLighting(model, 0);
		SetMaterial(1 - t, 1, 1, 1);
		sub_42BA60(0xFFFFFFFF);
		
		changeAllTexturesToPhase(work.phase);
		sub_42B5A0(model);
		
		sub_4264D0(1);
		SetMaterial(t, 1, 1, 1);
		sub_42BA60(0xFFFFFFFF);

		int nextPhase = (work.phase + 1) % 3;
		changeAllTexturesToPhase(nextPhase);

		sub_42B5A0(model);
		ToggleLighting(model, 1);

		changeAllTexturesToPhase(0);

		LoadControl3D();
		LoadConstantAttr();
	
		njPopMatrixEx();
	}

	blendModeGinjaFunc = blendModeGinjaFuncBackup;

	SetMaterial(1, 1, 1, 1);

	rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_END);

	njColorBlendingMode(0, 8);
	njColorBlendingMode(1, 6);
}

void AL_CreateDayNightCycle() {
	task* tp = LoadObject(4, "AL_DayNightCycle", AL_DayNightCycleExecutor, LoadObj_Data1);

	tp->Data2.Undefined = ALLOC(DAYNIGHT_WORK);

	memset(tp->Data2.Undefined, 0, sizeof(DAYNIGHT_WORK));

	tp->DisplaySub = AL_DayNightCycleDisplayer;
	tp->DeleteSub = AL_DayNightCycleDestructor;
}

void AL_DayNight_Init(const HelperFunctions& helper){
	auto* rf = helper.Mods->find("sa2-render-fix");
	if (!rf) return;
		
	rfapi_core = rf->GetDllExport<RFAPI_CORE*>("rfapi_core");
}