#include "stdafx.h"
#include <array>
#include "ChaoMain.h"
#include "al_daynight.h"
#include "al_save.h"
#include <stdexcept>
#include "al_stage.h"
#include <util.h>
#include <ninja_functions.h>
#include <al_sandhole.h>
#include "renderfix_api.h"
#include <api/api_util.h>
#include "FunctionHook.h"
#include <optional>
#include <set>

#include "rapidjson.h"
#include <error/en.h>
#include <filereadstream.h>
#include <document.h>


#include "memory.h"
#include <span>

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

// this is where the code for the new one starts

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

FunctionPointer(void, EnableAlpha, (int a1), 0x4264D0);

// NOT an official name
FunctionPointer(void, gjSetDiffuse, (unsigned int a1), 0x42BA60);

static RFAPI_CORE* rfapi_core;
static task* pDayNightTask;

static const char* AL_DayNightCycle_GetGardenID() {
	//todo: ecw function here when it's done

	switch (AL_GetStageNumber()) {
	default:
	case CHAO_STG_NEUT:
		return "neut";

	case CHAO_STG_HERO:
		return "hero";

	case CHAO_STG_DARK:
		return "dark";
	}
}

enum {
	PHASE_DAY = 0,
	PHASE_EVE = 1,
	PHASE_NGT = 2,
	PHASE_CLD = 3,
	NB_PHASE
};

struct DAYNIGHT_SKYBOX {
	uint32_t origTexID; // tex ID to look for to find mesh
	uint32_t dayTexID;
	uint32_t eveningTexID;
	uint32_t nightTexID;
	uint32_t cloudyTexID;
};

// Manages the loaded JSON data, contains vectors of said data so that I don't lose my sanity with writing memory management
struct DAYNIGHT_DATA_MANAGER {
private:
	const char* m_skyboxTextureName = nullptr;
	std::vector<DAYNIGHT_SKYBOX> m_skyboxEntries;
	NJS_ARGB m_phaseColors[NB_PHASE];
	LightGC m_lights[NB_PHASE];
	bool m_lightLoaded[NB_PHASE];

public:
	const char* const GetSkyboxTextureFileName() const {
		return m_skyboxTextureName;
	}

	const NJS_ARGB& GetColorForPhase(size_t phaseIndex) const {
		assert(phaseIndex < NB_PHASE);
		return m_phaseColors[phaseIndex];
	}

	const bool HasLightForPhase(size_t phaseIndex) const {
		assert(phaseIndex < NB_PHASE);
		return m_lightLoaded[phaseIndex];
	}

	const LightGC& GetLightForPhase(size_t phaseIndex) const {
		assert(phaseIndex < NB_PHASE);
		return m_lights[phaseIndex];
	}

	const std::span<const DAYNIGHT_SKYBOX> GetSkyboxEntries() const {
		return std::span<const DAYNIGHT_SKYBOX>(m_skyboxEntries.data(), m_skyboxEntries.size());
	}

	bool LoadConfig(const char* gardenID) {
		using namespace rapidjson;

		char pathBuffer[MAX_PATH];
		sprintf_s(pathBuffer, "./resource/gd_PC/DayNightCycle/daynight_%s.json", gardenID);

		APIErrorUtil error("Error loading daynight_%s.json: ", gardenID);

		FILE* fp = fopen(g_HelperFunctions->GetReplaceablePath(pathBuffer), "rb");
		if (!fp) {
			return false;
		}

		fseek(fp, 0L, SEEK_END);
		size_t sz = ftell(fp);
		std::unique_ptr<char[]> readBuffer(new char[sz]);

		fseek(fp, 0, SEEK_SET);
		FileReadStream is(fp, readBuffer.get(), sz);

		Document document;
		const auto& d = document.ParseStream(is);

		const auto parseFloat3 = [&](const auto & colorElement, NJS_VECTOR& outFloat3) {
			const size_t numOfElements = 3;
			const auto& phaseColorArray = colorElement.GetArray();

			if (phaseColorArray.Size() != numOfElements) {
				error.print("float array isn't in a valid length! (expected 3 elements of floats in 0 and 1 range)");
				return false;
			}

			for (size_t elemIndex = 0; elemIndex < numOfElements; elemIndex++) {
				const auto& elem = phaseColorArray[elemIndex];

				if (elem.GetType() != rapidjson::Type::kNumberType) {
					error.print("float array's element isn't a number! (expected number in 0 and 1 range)");
					return false;
				}

				const float f = elem.GetFloat();
				switch (elemIndex) {
				case 0:
					outFloat3.x = f;
					break;
				case 1:
					outFloat3.y = f;
					break;
				case 2:
					outFloat3.z = f;
					break;
				}
			}

			return true;
		};

		const auto parseColor = [&](const auto& colorElement, NJS_ARGB& outColor) {
			if (colorElement.IsString()) {
				// #AABBCC formatting
				const auto& colorString = colorElement.GetString();
				constexpr size_t hexFormatStringLen = std::char_traits<char>::length("#AABBCC");

				if (strlen(colorString) != hexFormatStringLen) {
					error.print("color string isn't in a valid formatting! (expected #AABBCC formatting)");
					return false;
				}

				bool checkFormatting = true;
				checkFormatting &= colorString[0] == '#';

				//# 0 A 1 A 2 B 3 B 4 C 5 C 6
				for (size_t strIndex = 1; checkFormatting && strIndex <= 6; strIndex++) {
					bool validHex = (colorString[strIndex] >= '0' && colorString[strIndex] <= '9');
					validHex |= (colorString[strIndex] >= 'A' && colorString[strIndex] <= 'F');
					checkFormatting &= validHex;
				}

				if (!checkFormatting) {
					error.print("phase color string isn't in a valid formatting! (expected #AABBCC formatting)");
					return false;
				}

				int r, g, b;
				sscanf(colorString + 1, "%02x%02x%02x", &r, &g, &b);

				outColor.r = r / 255.f;
				outColor.g = g / 255.f;
				outColor.b = b / 255.f;
			}
			else if (colorElement.IsArray()) {
				// hack to write to the RGB fields as a vector, we really need an NJS_RGB type or something similar
				if (!parseFloat3(colorElement, *(NJS_VECTOR*)&outColor.r)) {
					return false;
				}
			}
			else {
				error.print("color isn't in a valid formatting!");
				return false;
			}

			return true;
		};

		if (d.HasParseError()) {
			error.print("error parsing at %u: %s\n",
				(unsigned)document.GetErrorOffset(),
				GetParseError_En(d.GetParseError())
			);
			return false;
		}

		if (d.HasMember("skyboxTextureName")) {
			const auto& member = d["skyboxTextureName"];
			if (!member.IsString()) {
				error.print("\"skyboxTextureName\" is not a string!");
				return false;
			}
			else {
				m_skyboxTextureName = _strdup(member.GetString());
			}
		}

		// parse skybox materials (if there are any
		if (d.HasMember("skyboxMaterials")) {
			const auto& member = d["skyboxMaterials"];
			if (!member.IsArray()) {
				error.print("\"skyboxMaterials\" is not an array!");
				return false;
			}

			const auto& materials = member.GetArray();
			for (const auto& material : materials) {
				DAYNIGHT_SKYBOX skyboxEntry{};

				if (!material.HasMember("originalTextureID")) {
					error.print("originalTextureID is required in \"skyboxMaterials\"!");
					return false;
				}
				const auto& origTexID = material["originalTextureID"];

				if (!origTexID.IsNumber()) {
					error.print("originalTextureID needs to be a number!");
					return false;
				}

				skyboxEntry.origTexID = origTexID.GetUint();

				// if any of these are unset they will default to the original texture ID (incl. original texlist), they're optional
				skyboxEntry.dayTexID = -1;
				skyboxEntry.eveningTexID = -1;
				skyboxEntry.nightTexID = -1;
				skyboxEntry.cloudyTexID = -1;

				const char* const phaseTexIDKeys[NB_PHASE] = {
					 "dayTextureID",
					 "eveningTextureID",
					 "nightTextureID",
					 "cloudyTextureID"
				};
				
				for (size_t phaseIndex = 0; phaseIndex < _countof(phaseTexIDKeys); phaseIndex++) {
					const auto& key = phaseTexIDKeys[phaseIndex];

					if (!material.HasMember(key)) continue;

					const auto& texID = material[key];

					if (!texID.IsNumber()) {
						error.print("\"%s\" needs to be a number!", phaseTexIDKeys[phaseIndex]);
						return false;
					}

					uint32_t id = texID.GetUint();

					switch (phaseIndex) {
						case PHASE_DAY:
							skyboxEntry.dayTexID = id;
							break;
						case PHASE_EVE:
							skyboxEntry.eveningTexID = id;
							break;
						case PHASE_NGT:
							skyboxEntry.nightTexID = id;
							break;
						case PHASE_CLD:
							skyboxEntry.cloudyTexID = id;
							break;
					}
				}

				m_skyboxEntries.push_back(skyboxEntry);
			}
		}

		// parse phases
		{
			if (!d.HasMember("phases")) {
				error.print("\"phases\" doesn't exist!");
				return false;
			}

			auto& arr = d["phases"];
			if (!arr.IsObject()) {
				error.print("\"phases\" is not an array!");
				return false;
			}

			const auto& phases = d["phases"];

			const char* const phasesList[NB_PHASE] = {
				"day",
				"evening",
				"night",
				"cloudy"
			};

			for (size_t i = 0; i < _countof(phasesList); i++) {
				const auto& phaseKey = phasesList[i];
				if (!phases.HasMember(phaseKey)) continue;

				const auto& phase = phases[phaseKey];
				
				if (!phase.IsObject()) {
					error.print("\"%s\" is not an object!", phaseKey);
					return false;
				}

				// light member is optional, it will default to the loaded one if not specified
				if (phase.HasMember("light")) {
					const auto& lightElement = phase["light"];
					LightGC outLight;

					NJS_ARGB diffuse, ambient;

					if (!lightElement.HasMember("direction")) {
						error.print("light's \"direction\" doesn't exist!");
						return false;
					}

					if (!lightElement.HasMember("ambient")) {
						error.print("light's \"ambient\" doesn't exist!");
						return false;
					}

					if (!lightElement.HasMember("diffuse")) {
						error.print("light's \"diffuse\" doesn't exist!");
						return false;
					}

					if (!parseFloat3(lightElement["direction"], outLight.direction)) {
						return false;
					}

					if (!parseColor(lightElement["diffuse"], diffuse)) {
						return false;
					}

					if (!parseColor(lightElement["ambient"], ambient)) {
						return false;
					}

					outLight.ambientReg = { ambient.r, ambient.g, ambient.b };
					outLight.lightColor = { diffuse.r, diffuse.g, diffuse.b };

					m_lights[i] = outLight;
					m_lightLoaded[i] = true;
				}

				if (!phase.HasMember("color")) {
					error.print("\"%s\" phase doesn't have color attribute!", phaseKey);
					return false;
				}

				const auto& phaseColor = phase["color"];
				auto& outPhaseColor = m_phaseColors[i];

				if (!parseColor(phaseColor, outPhaseColor)) {
					return false;
				}
			};
		}

		fclose(fp);
		return true;
	}

	void Clear() {
		m_skyboxEntries.clear();
		memset(m_lightLoaded, false, sizeof(m_lightLoaded));
	}
} static gDayNightManager;


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

struct DAYNIGHT_SKYBOX_TEXMAP_TABLE {
	uint32_t* pTexID;

	uint32_t* pLightingParameter;
	uint32_t originalLightingParameter;

	DAYNIGHT_SKYBOX skybox;
};

struct DAYNIGHT_SKYBOX_TABLE {
	NJS_OBJECT* pObj;
	DAYNIGHT_SKYBOX_TEXMAP_TABLE* pTexMap;
	size_t texMapCount;
};

enum {
	VERTEX_COLOR_TABLE_SRC = 0,
	VERTEX_COLOR_TABLE_DST = 1,
	VERTEX_COLOR_TABLE_COUNT = 2
};

struct DAYNIGHT_WORK {
	int mode;
	LandTable* pNewLandtable;
	LandTable* pOldLandtable;
	SA2B_VertexData** pVertexColorTable; // pair of src and dst vertexcolor entries (ChunkModelCount * 2 amount of pointers)
	
	NJS_TEXLIST* pSkyboxTexlist;

	int timer;
	int phase;
	NJS_ARGB appliedColor;

	// this is temporary, we only use it rn to dump them for conversion
	// remove later
	// 4 for the phases, 4 for the lights in the file
	Light lights[4][4];
	
	// for phases that don't have a specified light, it will use the one loaded by the game
	// (converted to GC if it isn't GC)
	LightGC fallbackLight;

	DAYNIGHT_SKYBOX_TABLE* pSkyboxTable;
	int skyboxCount;
};


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

static DAYNIGHT_WORK& GetWork(task* tp) {
	return *reinterpret_cast<DAYNIGHT_WORK*>(tp->Data2.Undefined);
}

// Frees the copied object, the sa2bmodel, and the vertex colors it copies
static void AL_DayNightCycle_FreeObject(NJS_OBJECT* pObj, SA2B_VertexData** pColorTable) {
	if (!pObj) return;

	// if we have vertex colors for the node that means we copied everything up to that point 
	// so free them
	auto pDstColors = pColorTable[VERTEX_COLOR_TABLE_DST];
	if (pDstColors) {
		FREE(pDstColors->Data);
		FREE(pObj->sa2bmodel->Vertices);
		FREE(pObj->sa2bmodel);
		FREE(pObj);
	}
}

// Frees the LandTable, its COLs and all their related data
static void AL_DayNightCycle_FreeLandTableCOLAndObjects(task* tp) {
	auto& work = GetWork(tp);
	LandTable* pLand = work.pNewLandtable;

	for (size_t i = 0; i < pLand->ChunkModelCount; i++) {
		AL_DayNightCycle_FreeObject(pLand->COLList[i].Model, &work.pVertexColorTable[i * 2]);
	}

	FREE(pLand->COLList);
	FREE(pLand);
}

// Toggles the "color source" from whatever the original was to material color, and does it for alpha too
static void AL_DayNightCycle_ToggleColorSourceToMaterial(DAYNIGHT_SKYBOX_TABLE& skybox, bool useMaterialColorLighting) {
	for (size_t i = 0; i < skybox.texMapCount; i++) {
		auto& texMap = skybox.pTexMap[i];

		if (!texMap.pLightingParameter) continue;

		uint32_t& flag = *texMap.pLightingParameter;

		if (useMaterialColorLighting) {
			flag |= 2; // this makes it set the alpha source color too
			flag &= ~(1 << 11); // this is the vertex color flag			
		}
		else {
			flag = texMap.originalLightingParameter;
		}
	}
}

// Retrieves the skybox texture specified in the configuration for the phase argument
// Returns the original tex ID if the phase is invalid, for restoring them later on in the destructor
static uint32_t AL_DayNightCycle_GetSkyboxTextureForPhase(const DAYNIGHT_SKYBOX& skyboxEntry, int phase, bool& originalTexlist) {
	originalTexlist = true;

	uint32_t texID = skyboxEntry.origTexID;
	switch (phase) {
	case 0:
		if (skyboxEntry.dayTexID != -1) {
			originalTexlist = false;
			texID = skyboxEntry.dayTexID;
		}
		break;
	case 1:
		if (skyboxEntry.eveningTexID != -1) {
			originalTexlist = false;
			texID = skyboxEntry.eveningTexID;
		}
		break;
	case 2:
		if (skyboxEntry.nightTexID != -1) {
			originalTexlist = false;
			texID = skyboxEntry.nightTexID;
		}
		break;
	}

	return texID;
};

// Changes the textures for the skybox entry according to the phase, check above function for info about "phase" argument's behavior
static bool AL_DayNightCycle_ChangeSkyboxTextures(DAYNIGHT_SKYBOX_TABLE& skybox, int phase) {
	// this bool is needed to tell the displayer if the skybox texture falls back to the original texlist and original texID
	bool isOriginalTexlist = false;

	for (size_t t = 0; t < skybox.texMapCount; t++) {
		const auto changeTex = [](uint32_t& data, uint32_t texID) {
			data &= 0xFFFF0000;
			data |= texID;
		};

		auto& texMap = skybox.pTexMap[t];
		bool outIsOriginalTexlist;
		const auto texID = AL_DayNightCycle_GetSkyboxTextureForPhase(texMap.skybox, phase, outIsOriginalTexlist);

		if (outIsOriginalTexlist) {			
			isOriginalTexlist = true;
		}

		changeTex(*texMap.pTexID, texID);
	}

	return isOriginalTexlist;
}

// Finds the original texture ID in the skybox texture map table, and retrieves the texture IDs to map it to
static bool AL_DayNightCycle_FindSkyboxTexID(uint32_t texID, DAYNIGHT_SKYBOX const ** pSkybox) {
	const auto& skyboxEntries = gDayNightManager.GetSkyboxEntries();

	for (size_t i = 0; i < skyboxEntries.size(); i++) {
		if (texID == skyboxEntries[i].origTexID) {
			*pSkybox = &skyboxEntries[i];
			return true;
		}
	}

	return false;
}

// Checks if the COL's model has any skybox textures, if yes then it fills the list of texture IDs to change
// (and what to change it to)
static bool AL_DayNightCycle_CheckSkybox(const NJS_OBJECT* pSrcObj, DAYNIGHT_SKYBOX_TABLE& entry) {
	static APIErrorUtil error("Error in CheckSkybox: ");

	const SA2B_Model* pModel = pSrcObj->sa2bmodel;
	const size_t sumGeoCount = pModel->OpaqueGeometryCount + pModel->TranslucentGeometryCount;

	std::unique_ptr<DAYNIGHT_SKYBOX_TEXMAP_TABLE[]> texMapArray(new DAYNIGHT_SKYBOX_TEXMAP_TABLE[sumGeoCount]);
	size_t texMapCount = 0;

	bool isMixed = false;
	bool needsOriginalTexlist[NB_PHASE] = { false };
	bool showedError = false;

	const auto checkAndAddGeo = [&](const SA2B_GeometryData* pGeo) {
		// since these separate "geometries" can be sorta interpreted as separate materials
		// basically we're checking if the material's texture matches anything we're looking for
		// if yes, then let's store the pointer to the texture ID, and also the lighting info so we can switch it later on

		uint32_t* pLightingParameter = NULL;
		uint32_t origLightingParameter = 0;

		const DAYNIGHT_SKYBOX* pSkyboxEntry = NULL;
		uint32_t* pTexID = NULL;

		for (size_t i = 0; i < pGeo->ParameterCount; i++) {
			uint32_t* para = reinterpret_cast<uint32_t*>(&pGeo->ParameterOffset[i * 2]);
			const auto type = reinterpret_cast<const uint8_t*>(para)[0];

#ifndef _DEBUG
			if (pSkyboxEntry && pLightingParameter) {
				break;
			}
#endif

			if (type == 2) { //"lighting" parameter type
				pLightingParameter = &para[1];
				origLightingParameter = para[1];
			}

			if (type == 8) { // "texture" parameter type
				const auto texID = para[1] & 0xFFFF;

				const DAYNIGHT_SKYBOX* pResultSkyboxEntry;
				if (AL_DayNightCycle_FindSkyboxTexID(texID, &pResultSkyboxEntry)) {
					// for testing in debug mode, let's check if there's any chance at all there are multiple texture entries per geo
					// as far as i know there's no way there should be
					assert(pSkyboxEntry == NULL);

					pSkyboxEntry = pResultSkyboxEntry;
					pTexID = &para[1];
#ifndef _DEBUG
					break;
#endif
				}
			}
		}

		// if we found an entry earlier
		if (pSkyboxEntry) {
			auto& texMap = texMapArray[texMapCount++];

			texMap.pLightingParameter = pLightingParameter;
			texMap.originalLightingParameter = origLightingParameter;
			texMap.pTexID = pTexID;
			texMap.skybox = *pSkyboxEntry;

			// this check is needed so that a model doesn't use a combination/"hybrid" texlist setup with a material using original tex id and sky texlist tex id
			bool originalTexlist[4] = { false };
			if (texMap.skybox.dayTexID == -1) originalTexlist[PHASE_DAY] = true;
			if (texMap.skybox.eveningTexID == -1) originalTexlist[PHASE_EVE] = true;
			if (texMap.skybox.nightTexID == -1) originalTexlist[PHASE_NGT] = true;
			if (texMap.skybox.cloudyTexID == -1) originalTexlist[PHASE_CLD] = true;
			
			for (size_t i = 0; i < NB_PHASE; i++) {
				if (texMapCount > 1 && originalTexlist[i] != needsOriginalTexlist[i] && !showedError) {
					error.print("one of the skybox models use both the original texture and the replacement texlist. This case cannot be handled, and the rendering is not guaranteed to run stable, please fix it.");
					showedError = true;
				}

				needsOriginalTexlist[i] = originalTexlist[i];
			}
			
		}
	};

	for (size_t i = 0; i < pModel->OpaqueGeometryCount; i++) {
		checkAndAddGeo(&pModel->OpaqueGeoData[i]);
	}

	for (size_t i = 0; i < pModel->TranslucentGeometryCount; i++) {
		checkAndAddGeo(&pModel->TranslucentGeoData[i]);
	}

	assert(texMapCount <= pModel->OpaqueGeometryCount + pModel->TranslucentGeometryCount);
	
	if (texMapCount) {
		entry.pTexMap = ALLOC_ARRAY(texMapCount, DAYNIGHT_SKYBOX_TEXMAP_TABLE);
		entry.texMapCount = texMapCount;

		memcpy(entry.pTexMap, texMapArray.get(), texMapCount * sizeof(DAYNIGHT_SKYBOX_TEXMAP_TABLE));

		AL_DayNightCycle_ToggleColorSourceToMaterial(entry, true);
	}

	return texMapCount > 0;
}

// Copies the vertex data of the source model (for the colors) and gets the pointer to the vertex colors to not have to look for them again
// and also returns if it had vertex colors at all, if it doesn't it won't bother to copy
static bool AL_DayNightCycle_CopyGCModel(SA2B_VertexData** pVertexColorTable, SA2B_VertexData* pSrcVertices, SA2B_VertexData*& pDstVertices) {
	// we only need to clone the vertex colors, so we loop through the vertex types until we find it
	const SA2B_VertexData* pSrcVert = pSrcVertices;
	size_t vertexColorIndex = -1;

	// we first need the size of the whole thing
	while (pSrcVert->DataType != -1) {
		if ((pSrcVert->DataType & 0xF) == 3) { // vertex color datatype == 3
			vertexColorIndex = pSrcVert - pSrcVertices;
		}
		pSrcVert++;
	}

	if (vertexColorIndex == -1) {
		// todo: how do we handle this
		assert(false);
		//error.print("One of the models don't have vertex colors!!");
		return false;
	}

	const size_t vertexDataCount = (pSrcVert + 1 - pSrcVertices);
	pDstVertices = ALLOC_ARRAY(vertexDataCount, SA2B_VertexData);
	memcpy(pDstVertices, pSrcVertices, sizeof(SA2B_VertexData) * vertexDataCount);

	SA2B_VertexData& pVertexColorData = pDstVertices[vertexColorIndex];
	pVertexColorData.Data = ALLOC_ARRAY(pVertexColorData.DataSize, uint8_t);
	//memcpy(pVertexColorData.Data, (pSrc->Vertices + vertexColorIndex)->Data, pVertexColorData.DataSize);

	pVertexColorTable[VERTEX_COLOR_TABLE_SRC] = pSrcVertices + vertexColorIndex;
	pVertexColorTable[VERTEX_COLOR_TABLE_DST] = pDstVertices + vertexColorIndex;

	return true;
}

// Copies the COL and its vertex colors
static void AL_DayNightCycle_CopyCOLObject(const COL* pSrc, COL* pDst, SA2B_VertexData** pVertexColorTable) {
	// !!! we assume there are no children or siblings, DrawLandtable doesn't support hierarchies (thanks shad) !!!

	const SA2B_Model* pSrcModel = pSrc->Model->sa2bmodel;
	if (!pSrcModel) return;
	
	SA2B_VertexData* pDstVertices;
	if (!AL_DayNightCycle_CopyGCModel(pVertexColorTable, pSrcModel->Vertices, pDstVertices)) return;

	// little disorienting naming scheme, the COL's Model refers to it's NJS_OBJECT, not the sa2b_model
	pDst->Model = ALLOC(NJS_OBJECT);
	*pDst->Model = *pSrc->Model;
	
	SA2B_Model* pDstModel = ALLOC(SA2B_Model);
	pDst->Model->sa2bmodel = pDstModel;

	*pDstModel = *pSrcModel;

	pDstModel->Vertices = pDstVertices;
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
	const size_t pointerCount = pSrc->ChunkModelCount * VERTEX_COLOR_TABLE_COUNT;
	work.pVertexColorTable = ALLOC_ARRAY(pointerCount, SA2B_VertexData*);
	memset(work.pVertexColorTable, NULL, sizeof(SA2B_VertexData*) * pointerCount);

	// copy the visual COLs' models
	for (size_t i = 0; i < pSrc->ChunkModelCount; i++) {
		const auto pCOLSrc = &pSrc->COLList[i];
		auto pCOLDst = &pDst->COLList[i];

		// if it's a skybox, add it to the list of skyboxes to interpolate the textures on
		if (AL_DayNightCycle_CheckSkybox(pCOLSrc->Model, pSkyboxColList[skyboxColCount])) {
			pSkyboxColList[skyboxColCount++].pObj = pCOLDst->Model;
			pCOLDst->Flags &= ~0x80000000; // remove visibility col flag, we'll handle rendering
			continue;
		}

		AL_DayNightCycle_CopyCOLObject(pCOLSrc, pCOLDst, &work.pVertexColorTable[i * 2]);		
	}

	// if we found skyboxes, let's allocate the amount we found and copy it all
	if (skyboxColCount > 0) {
		work.pSkyboxTable = ALLOC_ARRAY(skyboxColCount, DAYNIGHT_SKYBOX_TABLE);
		work.skyboxCount = skyboxColCount;

		memcpy(work.pSkyboxTable, pSkyboxColList.get(), sizeof(DAYNIGHT_SKYBOX_TABLE) * skyboxColCount);
	}
}

// Applies the color we want to the destination vertexdata, using the source vertexdata as the base color
static void AL_DayNightCycle_ApplyVertexColor(const NJS_ARGB& mulColor, const SA2B_VertexData* pSrcVertexColorData, SA2B_VertexData* pDstVertexColorData) {
	// technically NJS_COLOR isn't good, because in the GC models it's ARGB not BGRA
	// but we're dirty little cheaters and use NJS_COLOR anyways (we write to GRA instead of RGB)
	NJS_COLOR* pSrcColors = reinterpret_cast<NJS_COLOR*>(pSrcVertexColorData->Data);
	NJS_COLOR* pDstColors = reinterpret_cast<NJS_COLOR*>(pDstVertexColorData->Data);

	// this is the data type part of the data type, not the struct type (aka the one telling us it's vcolor)
	// apparently this means it could theoretically be RGB565 or other formats but i'm really hoping it can't happen
	// just to be safe there's an assert here so i can catch it if it does in any vanilla gardens
	const size_t dataType = (pSrcVertexColorData->DataType >> 4) & 0xF;
	assert(dataType == 0);
	assert(pSrcVertexColorData->ElementCount == pSrcVertexColorData->DataSize / 4);
	assert(pSrcVertexColorData->ElementCount != 0);
	assert(pSrcVertexColorData->ElementCount == pDstVertexColorData->ElementCount);

	for (size_t i = 0; i < pSrcVertexColorData->ElementCount; i++) {
		NJS_COLOR& color = pDstColors[i];

		color = pSrcColors[i];	
		color.argb.g = (Uint8)(color.argb.g * mulColor.r);
		color.argb.r = (Uint8)(color.argb.r * mulColor.g);
		color.argb.a = (Uint8)(color.argb.a * mulColor.b);
	}
}

// Applies the color we want to all the landtable models
static void AL_DayNightCycle_ApplyColorToLandTable(task* tp) {
	auto& work = GetWork(tp);

	// placeholder
	int nextPhase = (work.phase + 1) % 3;

	NJS_ARGB appliedColor;

	const auto& colorA = gDayNightManager.GetColorForPhase(work.phase);
	const auto& colorB = gDayNightManager.GetColorForPhase(nextPhase);

	LerpColor(appliedColor, colorA, colorB, work.timer / (60 * 5.0f));

	SA2B_VertexData** pVertexColorTable = work.pVertexColorTable;
	
	for (size_t colIndex = 0; colIndex < CurrentLandTable->ChunkModelCount; colIndex++) {
		const SA2B_VertexData* pSrcVertices = pVertexColorTable[colIndex * 2 + VERTEX_COLOR_TABLE_SRC];
		SA2B_VertexData* pDstVertices = pVertexColorTable[colIndex * 2 + VERTEX_COLOR_TABLE_DST];

		if (!pDstVertices) continue;

		AL_DayNightCycle_ApplyVertexColor(appliedColor, pSrcVertices, pDstVertices);
	}
}

// Copy the game's lights into ours
static void CopyLights(task* tp, size_t index) {
	auto& work = GetWork(tp);
	memcpy(&work.lights[index], Lights, sizeof(work.lights[index]));
}

static LightGC DCLightToGCLight(size_t index) {
	// if already GC don't bother
	if (LightsGC[index].SomeFlag & 1) return LightsGC[0];

	// enable the "gc light override" flag
	LightsGC[index].SomeFlag |= 1;

	LightsGC[index].direction = Lights[index].direction;

	LightsGC[index].ambientReg = Lights[index].color;
	LightsGC[index].lightColor = Lights[index].color;

	LightsGC[index].ambientReg.x *= Lights[index].ambient;
	LightsGC[index].ambientReg.y *= Lights[index].ambient;
	LightsGC[index].ambientReg.z *= Lights[index].ambient;

	LightsGC[index].lightColor.x *= Lights[index].intensity;
	LightsGC[index].lightColor.y *= Lights[index].intensity;
	LightsGC[index].lightColor.z *= Lights[index].intensity;
}

static void AL_DayNightCycle_ApplyLightLerp(task* tp) {
	auto& work = GetWork(tp);

	const int nextPhase = (work.phase + 1) % 3;

	const bool hasSrcLight = gDayNightManager.HasLightForPhase(work.phase);
	const bool hasDstLight = gDayNightManager.HasLightForPhase(nextPhase);

	const LightGC& pSrcLight = (!hasSrcLight) ? work.fallbackLight : gDayNightManager.GetLightForPhase(work.phase);
	const LightGC& pDstLight = (!hasDstLight) ? work.fallbackLight : gDayNightManager.GetLightForPhase(nextPhase);

	LightsGC[0].SomeFlag |= 1;
	LerpLight(LightsGC[0], pSrcLight, pDstLight, (work.timer / (60.f * 5)));
}

// this is temporary but it will be useful for porting the existing light files to json
static void DumpLightJson(task* tp) {
	auto& work = GetWork(tp);
	const auto dump = [&](Light& light) {
		PrintDebug("\"light\": {");
		PrintDebug("\t\"direction\": [%f, %f, %f]", light.direction.x, light.direction.y, light.direction.z);
		PrintDebug("\t\"diffuse\": [%f, %f, %f]", light.color.x * light.intensity, light.color.y * light.intensity, light.color.z * light.intensity);
		PrintDebug("\t\"ambient\": [%f, %f, %f]", light.color.x * light.ambient, light.color.y * light.ambient, light.color.z * light.ambient);
		PrintDebug("}");
	};

	PrintDebug("JSON LIGHT DUMP STARTS HERE");
	PrintDebug("day");
	dump(work.lights[PHASE_DAY][0]);
	PrintDebug("evening");
	dump(work.lights[PHASE_EVE][0]);
	PrintDebug("night");
	dump(work.lights[PHASE_NGT][0]);
	PrintDebug("cloudy");
	dump(work.lights[PHASE_CLD][0]);
}

// Main update function for the DNC
static void AL_DayNightCycleExecutor(task* tp) {
	if (!AL_IsGarden()) return;

	auto& work = GetWork(tp);

	switch (work.mode) {
		case MODE_INIT_LT_COPY:
			gDayNightManager.LoadConfig(AL_DayNightCycle_GetGardenID());

			{
				const auto& skyboxFilename = gDayNightManager.GetSkyboxTextureFileName();
				if (skyboxFilename) {
					work.pSkyboxTexlist = LoadCharTextures((char*)skyboxFilename);
				}
			}

			// copy the landtable and make the game render our new one, and initialize the skyboxes
			AL_DayNightCycle_InitNewLandTable(tp);
			work.pOldLandtable = CurrentLandTable;
			CurrentLandTable = work.pNewLandtable;

			work.mode++;

			// the config lights are in GC format so we convert any already loaded DC lights into GC
			// so we can smoothly interpolate between the existing one and the ones in config
			// ofc, this is only necessary if there hasn't been a light specified for every phase
			// because then we need to default to the existing one
			for (size_t i = 0; i < 4; i++) {
				//DCLightToGCLight(i);
			}

			/*
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

			DumpLightJson(tp);
			*/
			[[fallthrough]];
		case MODE_LERP:
			// most of this is just temp to test the rendering and all that
			work.timer++;
			if (work.timer > 60 * 5) {
				work.timer = 0;

				work.phase++;
				work.phase = work.phase % 3;
			}

			AL_DayNightCycle_ApplyLightLerp(tp);
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

// Restores all model changes
static void AL_DayNightCycle_RestoreAll(task* tp) {
	auto& work = GetWork(tp);
	
	// restore the original landtable
	CurrentLandTable = work.pOldLandtable;
	CurrentLandTable->field_A = work.pNewLandtable->field_A; // this is a field to keep track of if the texlist is already loaded or not

	for (size_t i = 0; i < work.skyboxCount; i++) {
		auto& skybox = work.pSkyboxTable[i];

		AL_DayNightCycle_ChangeSkyboxTextures(skybox, -1);
		AL_DayNightCycle_ToggleColorSourceToMaterial(skybox, false);
	}
}

// Frees all the allocated copied stuff and skybox stuff
static void AL_DayNightCycleDestructor(task* tp) {
	AL_DayNightCycle_RestoreAll(tp);
	AL_DayNightCycle_FreeLandTableCOLAndObjects(tp);

	auto& work = GetWork(tp);

	FREE(work.pVertexColorTable);

	// the texlist was created with LoadCharTextures which auto-allocates the texlist
	// so alongside releasing the textures in the texlist, we also need to release the texlist itself
	// note that we don't need to release the texnames because it's allocated "with" the texlist
	if (work.pSkyboxTexlist) {
		njReleaseTexture(work.pSkyboxTexlist);
		FREE(work.pSkyboxTexlist);
	}

	if (work.skyboxCount) {
		for (size_t i = 0; i < work.skyboxCount; i++) {
			FREE(work.pSkyboxTable[i].pTexMap);
		}

		FREE(work.pSkyboxTable);
	}

	FREE(tp->Data2.Undefined);

	gDayNightManager.Clear();

	pDayNightTask = nullptr;
}

static void AL_DayNightCycle_SetSkyboxTexturesAndTexlist(task* tp, DAYNIGHT_SKYBOX_TABLE& skybox, int phase) {
	auto& work = GetWork(tp);

	bool isOriginalTexlist = AL_DayNightCycle_ChangeSkyboxTextures(skybox, phase);

	if (isOriginalTexlist) {
		njSetTexture(work.pNewLandtable->TextureList);
	}
	else {
		njSetTexture(work.pSkyboxTexlist);
	}
}

static void AL_DayNightCycleDisplayer(task* tp) {
	auto& work = GetWork(tp);

	// placeholder
	const float t = work.timer / float(5 * 60);
	const int nextPhase = (work.phase + 1) % 3;

	// landtables use this light index for some reason
	DoLighting(2); 

	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(1, NJD_COLOR_BLENDING_INVSRCALPHA);

	SaveControl3D();
	SaveConstantAttr();

	OnControl3D(NJD_CONTROL_3D_CONSTANT_MATERIAL);

	// HACK: we need to temporarily kill the GJ blend mode stuff so it doesn't overwrite out alpha blend
	uint32_t& blendModeGinjaFunc = *(uint32_t*)0x0174B344;
	uint32_t blendModeGinjaFuncBackup = blendModeGinjaFunc;
	blendModeGinjaFunc = (int)nullsub_1;

	// RF API to handle setting the Z buffer modes appropriately for alpha rendering
	rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_TRANSPARENT);

	for (size_t i = 0; i < work.skyboxCount; i++) {
		auto& skybox = work.pSkyboxTable[i];

		NJS_OBJECT* pSkyboxObj = skybox.pObj;
		SA2B_Model* model = pSkyboxObj->sa2bmodel;

		njPushMatrixEx();

		njTranslate(NULL, pSkyboxObj->pos[0], pSkyboxObj->pos[1], pSkyboxObj->pos[2]);

		RotateZ(pSkyboxObj->ang[2]);
		RotateY(pSkyboxObj->ang[1]);
		RotateX(pSkyboxObj->ang[0]);

		SetMaterial(1, 1, 1, 1);
		gjSetDiffuse(0xFFFFFFFF);
		
		AL_DayNightCycle_SetSkyboxTexturesAndTexlist(tp, skybox, work.phase);
		sub_42B5A0(model);
		
		EnableAlpha(1);
		SetMaterial(t, 1, 1, 1);
		gjSetDiffuse(0xFFFFFFFF);

		AL_DayNightCycle_SetSkyboxTexturesAndTexlist(tp, skybox, nextPhase);
		sub_42B5A0(model);
	
		njPopMatrixEx();
	}

	LoadControl3D();
	LoadConstantAttr();

	blendModeGinjaFunc = blendModeGinjaFuncBackup;

	SetMaterial(1, 1, 1, 1);

	rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_END);

	njColorBlendingMode(0, 8);
	njColorBlendingMode(1, 6);
}

void AL_CreateDayNightCycle() {
	if (!gConfigVal.DayNightCycle) return;

	task* tp = LoadObject(4, "AL_DayNightCycle", AL_DayNightCycleExecutor, LoadObj_Data1);
	pDayNightTask = tp;

	tp->Data2.Undefined = ALLOC(DAYNIGHT_WORK);

	memset(tp->Data2.Undefined, 0, sizeof(DAYNIGHT_WORK));

	tp->DisplaySub = AL_DayNightCycleDisplayer;
	tp->DeleteSub = AL_DayNightCycleDestructor;
}

// this hook is done to termporarily disable the vertex buffer caching mechanic for daynight cycle, 
// since we change the vertex colors every frame, it wouldn't update
FunctionHook<void> DrawLandtable_t(0x0047C2E0);
static void DrawLandtable_r() {
	// only run the "don't cache" code if daynight task is running
	if (!pDayNightTask) {
		DrawLandtable_t.Original();
		return;
	}

	*(char*)0x93BEB3 = 0;
	DrawLandtable_t.Original();
	*(char*)0x93BEB3 = 1;
}

void AL_DayNight_Init(const HelperFunctions& helper){
	DrawLandtable_t.Hook(DrawLandtable_r);

	auto* rf = helper.Mods->find("sa2-render-fix");
	if (!rf) return;
		
	rfapi_core = rf->GetDllExport<RFAPI_CORE*>("rfapi_core");
}