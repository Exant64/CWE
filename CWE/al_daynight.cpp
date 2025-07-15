#include "stdafx.h"
#include <array>
#include "ChaoMain.h"
#include "al_daynight.h"
#include "al_save.h"
#include <stdexcept>
#include "IniFile.h"
#include "al_stage.h"
#include <util.h>
#include <ninja_functions.h>
#include <al_sandhole.h>
#include <api/api_util.h>
#include "FunctionHook.h"
#include "UsercallFunctionHandler.h"
#include <optional>
#include <set>

#include "rapidjson.h"
#include <error/en.h>
#include <filereadstream.h>
#include <document.h>
#include "al_race.h"
#include "memory.h"
#include "cwe_api.h"
#include <span>
#include <brightfixapi.h>

#include "renderfix.h"

#pragma region Save

uint32_t gDayNightCheatPhase;

static DAYNIGHT_SAVE gDayNightSave;

// if we ever change the struct for some reason we can easily reconfigure it here

uint32_t& AL_DayNightCycle_GetSaveCurrentDay() {
	return gDayNightSave.day;
}

float& AL_DayNightCycle_GetSaveTime() {
	return gDayNightSave.time;
}

uint32_t& AL_DayNightCycle_GetSaveCurrentPhase() {
	return gDayNightSave.currentPhase;
}

bool& AL_DayNightCycle_GetSaveNextDayCloudy() {
	return gDayNightSave.nextDayCloudy;
}

#pragma endregion

#pragma region API

static std::unordered_map<std::string, std::vector<NJS_OBJECT*>> gDayNightSkyboxListMap;
static std::unordered_map<std::string, DAYNIGHT_TIME_MANAGER_FUNC> gDayNightTimeManagerMap;
static std::unordered_map<std::string, DAYNIGHT_RENDER_MANAGER_FUNC> gDayNightRenderManagerMap;

void RegisterECWSkybox(const char* pGardenID, NJS_OBJECT* pObj) {
	if(!gDayNightSkyboxListMap.contains(pGardenID)) {
		gDayNightSkyboxListMap[pGardenID] = {};
	}

	gDayNightSkyboxListMap[pGardenID].push_back(pObj);
}

void RegisterTimeManager(const char* pGardenID, DAYNIGHT_TIME_MANAGER_FUNC pFunc) {
	if (gDayNightTimeManagerMap.contains(pGardenID)) {
		gDayNightTimeManagerMap[pGardenID] = pFunc;
		return;
	}

	gDayNightTimeManagerMap.insert(std::make_pair(std::string(pGardenID), pFunc));
}

void RegisterRenderManager(const char* pGardenID, DAYNIGHT_RENDER_MANAGER_FUNC pFunc) {
	if (gDayNightRenderManagerMap.contains(pGardenID)) {
		gDayNightRenderManagerMap[pGardenID] = pFunc;
		return;
	}

	gDayNightRenderManagerMap.insert(std::make_pair(std::string(pGardenID), pFunc ));
}

#pragma endregion

// the number of frames for each hour
static size_t AL_DayNightCycle_GetHourFrameCount() {
	return gConfigVal.DayNightCycleHourFrame;
}

static size_t AL_DayNightCycle_GetDayFrameCount() {
	return 24 * AL_DayNightCycle_GetHourFrameCount();
}

FunctionPointer(void, gjDrawObject, (NJS_OBJECT* a1), 0x0042B530);

DataPointer(int, nj_cnk_blend_mode, 0x025F0264);

FunctionPointer(void, EnableAlpha, (int a1), 0x4264D0);

// NOT an official name
FunctionPointer(void, gjSetDiffuse, (unsigned int a1), 0x42BA60);

task* pDayNightTask;

static bool AL_DayNightCycle_CanCheckSkybox() {
	switch (AL_GetStageNumber()) {
	case CHAO_STG_NEUT:
		return gConfigVal.DayNightCycleNeutralGardenSkybox;
	case CHAO_STG_HERO:
		return gConfigVal.DayNightCycleHeroGardenSkybox;
	case CHAO_STG_DARK:
		return gConfigVal.DayNightCycleDarkGardenSkybox;
	}

	return false;
}

// Checks if we're in an area that is intended to have daynight cycle
static bool AL_DayNightCycle_IsValidArea() {
	switch (AL_GetStageNumber()) {
	case CHAO_STG_NEUT:
		return gConfigVal.DayNightCycleNeutralGarden;
	case CHAO_STG_HERO:
		return gConfigVal.DayNightCycleHeroGarden;
	case CHAO_STG_DARK:
		return gConfigVal.DayNightCycleDarkGarden;
	case CHAO_STG_ENTRANCE:
		return true;
	case CHAO_STG_RACE:
	case CHAO_STG_RACE_2P:
		return gConfigVal.DayNightCycleRace;
	case CHAO_STG_KARATE:
	case CHAO_STG_KARATE_2P:
		return gConfigVal.DayNightCycleKarate;
	}

	return false;
}

static const char* AL_DayNightCycle_GetGardenID() {
	//todo: ecw function here when it's done

	switch (AL_GetStageNumber()) {
	default:
	case CHAO_STG_NEUT:
		return "neutral";

	case CHAO_STG_HERO:
		return "hero";

	case CHAO_STG_DARK:
		return "dark";

	case CHAO_STG_ENTRANCE:
		return "entrance";

	case CHAO_STG_KARATE:
		return "karate";

	case CHAO_STG_RACE:
		if (RaceMainType == 3) {
			return "race_dark";
		}

		if (RaceMainType == 4) {
			return "race_hero";
		}

		return "race";
	}

	return "none";
}

// Manages the loaded JSON data, contains vectors of said data so that I don't lose my sanity with writing memory management
struct DAYNIGHT_DATA_MANAGER {
private:
	std::optional<std::string> m_textureFileName = std::nullopt;
	std::vector<DAYNIGHT_SKYBOX> m_skyboxEntries;
	NJS_ARGB m_phaseColors[NB_PHASE];
	std::optional<LightGC> m_lights[NB_PHASE];
	std::optional<size_t> m_shinyTextureIndices[NB_PHASE];

public:
	const std::optional<std::string>& GetTextureFileName() const {
		return m_textureFileName;
	}

	const std::optional<size_t>& GetShinyTextureIndexForPhase(size_t phaseIndex) const {
		assert(phaseIndex < NB_PHASE);
		return m_shinyTextureIndices[phaseIndex];
	}

	const NJS_ARGB& GetColorForPhase(size_t phaseIndex) const {
		assert(phaseIndex < NB_PHASE);
		return m_phaseColors[phaseIndex];
	}

	const std::optional<LightGC>& GetLightForPhase(size_t phaseIndex) const {
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

		// rapidjson expects a buffer with size of atleast 4
		sz = max(sz, 4);
		std::unique_ptr<char[]> readBuffer(new char[sz]);

		fseek(fp, 0, SEEK_SET);
		FileReadStream is(fp, readBuffer.get(), sz);

		Document document;
		const auto& d = document.ParseStream(is);

		if (d.HasParseError()) {
			// we allow document empty error so that it's an easy way for a (non-secw) custom garden mod to "disable" DNC
			if (d.GetParseError() != ParseErrorCode::kParseErrorDocumentEmpty) {
				error.print("error parsing at %u: %s\n",
					(unsigned)document.GetErrorOffset(),
					GetParseError_En(d.GetParseError())
				);
			}

			return false;
		}

		// again as mentioned earlier, if the document is empty (which we handle above) or just a "{ }" then that's not an error
		// it just counts the same as not having a config
		if (d.ObjectEmpty()) {
			return false;
		}

		const auto parseFloat3 = [&](const auto& colorElement, NJS_VECTOR& outFloat3) {
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

		if (d.HasMember("textureFileName")) {
			const auto& member = d["textureFileName"];
			if (!member.IsString()) {
				error.print("\"textureFileName\" is not a string!");
				return false;
			}
			else {
				m_textureFileName = std::string(member.GetString());
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

				if (phase.HasMember("shinyTextureID")) {
					if (!m_textureFileName.has_value()) {
						error.print("using \"shinyTextureID\" requires a texture filename specified!");
						return false;
					}

					const auto& shinyTextureIDMember = phase["shinyTextureID"];

					if (!shinyTextureIDMember.IsNumber()) {
						error.print("\"shinyTextureID\" in phase %s is not a number!", phaseKey);
						return false;
					}

					m_shinyTextureIndices[i] = shinyTextureIDMember.GetUint();
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
		m_textureFileName = std::nullopt;
		m_skyboxEntries.clear();
		
		for (size_t i = 0; i < NB_PHASE; i++) {
			m_lights[i] = std::nullopt;
			m_shinyTextureIndices[i] = std::nullopt;
		}
	}
} static gDayNightManager;

static DAYNIGHT_WORK& GetWork(task* tp) {
	return *reinterpret_cast<DAYNIGHT_WORK*>(tp->Data2.Undefined);
}

#pragma region Shiny Texture Lerping (used externally)

UsercallFuncVoid(sub_42C5B0, (uint16_t textureID, uint16_t wrapMode, int index), (textureID, wrapMode, index), 0x42C5B0, rAX, rCX, rEBX);

Uint32 AL_DayNightCycle_PreDrawSetupShinyTexture() {
	if (!pDayNightTask) return -1;
	
	auto& work = GetWork(pDayNightTask);

	if (!work.pTexlist) return -1;

	const auto& shinyTextureIndexFrom = gDayNightManager.GetShinyTextureIndexForPhase(work.phaseA);
	const auto& shinyTextureIndexTo = gDayNightManager.GetShinyTextureIndexForPhase(work.phaseB);

	const Uint32 backupShinyTextureTexAddr = AL_BODY.textures[34].texaddr;

	if (shinyTextureIndexFrom) {
		AL_BODY.textures[34].texaddr = work.pTexlist->textures[*shinyTextureIndexFrom].texaddr;
	}

	// sets texture to interpolate to into slot 3, handled in chao shader
	NJS_TEXLIST* pTexlistBackup = _nj_curr_ctx_->texlist;

	if (shinyTextureIndexTo) {		
		njSetTexture(work.pTexlist);
		sub_42C5B0(*shinyTextureIndexTo, 1, 3);
	}
	else {
		njSetTexture(&AL_BODY);
		sub_42C5B0(34, 1, 3);
	}

	njSetTexture(pTexlistBackup);

	return backupShinyTextureTexAddr;
}

void AL_DayNightCycle_SetLerpShinyTexture() {
	if (!pDayNightTask) return;
	const auto& work = GetWork(pDayNightTask);
	if (!work.pTexlist) return;

	SetPixelShaderFloat(82, work.lerpValue);
}

void AL_DayNightCycle_PostDrawSetupShinyTexture(const Uint32 texID) {
	if (texID == -1) return;

	AL_BODY.textures[34].texaddr = texID;

	// reset the lerp value
	SetPixelShaderFloat(82, 0);
}

#pragma endregion

#pragma region Skybox Utility Functions

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

// Finds the original texture ID in the skybox texture map table, and retrieves the texture IDs to map it to
static bool AL_DayNightCycle_FindSkyboxTexID(uint32_t texID, DAYNIGHT_SKYBOX const** pSkybox) {
	const auto& skyboxEntries = gDayNightManager.GetSkyboxEntries();

	for (size_t i = 0; i < skyboxEntries.size(); i++) {
		if (texID == skyboxEntries[i].origTexID) {
			*pSkybox = &skyboxEntries[i];
			return true;
		}
	}

	return false;
}

// Retrieves the skybox texture specified in the configuration for the phase argument
// Returns the original tex ID if the phase is invalid, for restoring them later on in the destructor
static uint32_t AL_DayNightCycle_GetTextureForPhase(const DAYNIGHT_SKYBOX& skyboxEntry, int phase, bool& originalTexlist) {
	uint32_t texID = skyboxEntry.origTexID;
	originalTexlist = false;

	switch (phase) {
		case PHASE_DAY:
			texID = skyboxEntry.dayTexID;
			break;
		case PHASE_EVE:
			texID = skyboxEntry.eveningTexID;
			break;
		case PHASE_NGT:	
			texID = skyboxEntry.nightTexID;
			break;
		case PHASE_CLD:
			texID = skyboxEntry.cloudyTexID;
			break;
	}

	if (texID == -1) {
		texID = skyboxEntry.origTexID;
		originalTexlist = true;
	}

	return texID;
};

// Changes the textures for the skybox entry according to the phase, check above function for info about "phase" argument's behavior
static bool AL_DayNightCycle_ChangeTextures(DAYNIGHT_SKYBOX_TABLE& skybox, int phase) {
	// this bool is needed to tell the displayer if the skybox texture falls back to the original texlist and original texID
	bool isOriginalTexlist = false;
	
	bool hasTexlist = gDayNightManager.GetTextureFileName().has_value();

	for (size_t t = 0; t < skybox.texMapCount; t++) {
		const auto changeTexGC = [](uint32_t& data, uint32_t texID) {
			data &= ~0x1FFF;
			data |= (texID & 0x1FFF);
		};

		const auto changeTexChunk = [](uint16_t& data, uint32_t texID) {
			data &= ~0x1FFF;
			data |= texID;
		};

		auto& texMap = skybox.pTexMap[t];

		bool outIsOriginalTexlist;
		const auto texID = AL_DayNightCycle_GetTextureForPhase(texMap.skybox, phase, outIsOriginalTexlist);

		if (outIsOriginalTexlist) {
			isOriginalTexlist = true;
		}

		if (skybox.isChunk) {
			changeTexChunk(*texMap.pChunkTexID, texID);
		}
		else {
			changeTexGC(*texMap.pTexID, texID);
		}
	}

	return !hasTexlist || isOriginalTexlist;
}

#pragma endregion

// TexMap table populating for chunk models
static void AL_DayNightCycle_CheckAndPopulateSkybox_Chunk(const NJS_OBJECT* pObject, NJS_OBJECT*& pDstObject, std::vector <DAYNIGHT_SKYBOX_TEXMAP_TABLE>& texMapTableList, bool& showMixedTexlistError) {
	bool needsOriginalTexlist[NB_PHASE] = { false };
	size_t tinyTIDCount = 0;

	// plist parsing code yoinked from shaddatic, credit to them
	// i aped the code a little, sorry shad
	int type;
	Sint16* plist = pObject->chunkmodel->plist;

	while (1)
	{
		type = ((uint8_t*)plist)[0];

		if (type == NJD_ENDOFF)
		{
			/** NJD_ENDOFF **/
			break;
		}

		if (type == NJD_NULLOFF)
		{
			/** NJD_NULLOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type < NJD_TINYOFF)
		{
			/** NJD_BITSOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type < NJD_MATOFF)
		{
			/** NJD_TINYOFF **/
			uint16_t texID = ((Uint16*)plist)[1] & 0x1FFF;

			// don't let the originalTexlist stuff scare you, it's all for errorchecking models with multiple textures,
			// specifically the case where one of the textures dont get overwritten and use the original skybox, while another one is
			// meaning it requires two texlists which is not possible to render
			// that's what we're checking for
			bool originalTexlist[4] = { false, false, false, false };
			const DAYNIGHT_SKYBOX* pResultSkyboxEntry;
			
			if (AL_DayNightCycle_FindSkyboxTexID(texID, &pResultSkyboxEntry)) {
				DAYNIGHT_SKYBOX_TEXMAP_TABLE texMap = {};

				texMap.pChunkTexID = (uint16_t*)&plist[1];
				texMap.skybox = *pResultSkyboxEntry;

				texMapTableList.push_back(texMap);

				// if any of the phases don't have a replacement, that one stays unreplaced aka uses original texlist
				if (texMap.skybox.dayTexID == -1)		originalTexlist[PHASE_DAY] = true;
				if (texMap.skybox.eveningTexID == -1)	originalTexlist[PHASE_EVE] = true;
				if (texMap.skybox.nightTexID == -1)		originalTexlist[PHASE_NGT] = true;
				if (texMap.skybox.cloudyTexID == -1)	originalTexlist[PHASE_CLD] = true;
			}
			else {
				// if the tex ID doesn't have a skybox replacement thing going on
				// then it uses original texlist 
				for (size_t i = 0; i < NB_PHASE; i++) {
					originalTexlist[i] = true;
				}
			}

			tinyTIDCount++;

			// only bother with the error check if it has a skybox texlist (otherwise everything is original texlist anyways)
			if (!showMixedTexlistError && gDayNightManager.GetTextureFileName()) {
				for (size_t i = 0; i < NB_PHASE; i++) {
					// if the original texlist requirement changes after the first entry it means we found a mixed case
					if (tinyTIDCount > 1 && originalTexlist[i] != needsOriginalTexlist[i]) {
						showMixedTexlistError = true;
					}

					needsOriginalTexlist[i] |= originalTexlist[i];
				}
			}

			/** Next offset **/
			plist += 2;
			continue;
		}

		plist += ((uint16_t*)plist)[1] + 2;
	}

	// if the skybox is texmapped, let's remove vertex colors from it if it has any
	// it's the only way we can enable material colors on it
	if (texMapTableList.size() > 0) {
		// first we need to find the end of the vertexchunk
		Uint32* pSrcSearchEnd = (Uint32*)pObject->chunkmodel->vlist;
		Uint8 chunkType = pSrcSearchEnd[0] & 0xFF;

		size_t vertexChunkCount = 0;
		size_t totalVertexCount = 0;

		while (chunkType != 0xFF) {
			if (chunkType != NJD_CV_D8) return;

			const Uint32 vertexCount = pSrcSearchEnd[1] >> 16;
			totalVertexCount += vertexCount;

			pSrcSearchEnd += 1 + (pSrcSearchEnd[0] >> 16);

			vertexChunkCount++;

			chunkType = pSrcSearchEnd[0] & 0xFF;
		}

		// if it has vertex colors we will get here and it doesn't return there above
		pDstObject = ALLOC(NJS_OBJECT);
		*pDstObject = *pObject;

		pDstObject->chunkmodel = ALLOC(NJS_CNK_MODEL);
		*pDstObject->chunkmodel = *pObject->chunkmodel;
	
		const size_t vlistSize = (vertexChunkCount + 1) * 2 + totalVertexCount * 6 + 1;
		pDstObject->chunkmodel->vlist = ALLOC_ARRAY(vlistSize, Sint32);
		memset(pDstObject->chunkmodel->vlist, 0, vlistSize * sizeof(Uint32));

		Uint32* pSrcSearch = (Uint32*)pObject->chunkmodel->vlist;
		Uint32* pDstSearch = (Uint32*)pDstObject->chunkmodel->vlist;
		
		while ((Uint8)(pSrcSearch[0] & 0xFF) != 0xFF) {
			pDstSearch[0] = pSrcSearch[0];
			pDstSearch[1] = pSrcSearch[1];

			pDstSearch[0] &= 0xFFFFFF00;
			pDstSearch[0] |= NJD_CV_VN;
	
			const Uint32 vertexCount = pDstSearch[1] >> 16;
			const Uint32 sizeVal = vertexCount * 6 + 1;

			pDstSearch[0] &= 0xFFFF;
			pDstSearch[0] |= (sizeVal << 16);

			for (size_t i = 0; i < vertexCount; i++) {
				NJS_POINT3* pSrcPosition = reinterpret_cast<NJS_POINT3*>(pSrcSearch + 2 + i * 4);
				NJS_POINT3* pDstPosition = reinterpret_cast<NJS_POINT3*>(pDstSearch + 2 + i * 6);
				
				*pDstPosition = *pSrcPosition;
			}

			// vertexCount * ( 3 for the positions and 1 for the color)
			pSrcSearch += 2 + vertexCount * 4;
			pDstSearch += 2 + vertexCount * 6;
		}

		pDstSearch[0] = pSrcSearch[0];
	}
}

static void dumpGjParameter(const SA2B_Model* pModel) {
	PrintDebug("--start dump--");
	const auto test = [](SA2B_GeometryData* geom, int count) {
		for (size_t i = 0; i < count; i++) {
			for (size_t j = 0; j < geom[i].ParameterCount; j++) {
				if (geom[i].ParameterOffset[j].ParameterType == 8) {
					PrintDebug("PARAM TEX %x", geom[i].ParameterOffset[j].Data);
				}
			}
		}
		};

	test(pModel->OpaqueGeoData, pModel->OpaqueGeometryCount);
	test(pModel->TranslucentGeoData, pModel->TranslucentGeometryCount);
}

// TexMap table populating for GC models
static void AL_DayNightCycle_CheckAndPopulateSkybox_GC(const SA2B_Model* pModel, std::vector <DAYNIGHT_SKYBOX_TEXMAP_TABLE>& texMapTableList, bool& showMixedTexlistError) {
	bool needsOriginalTexlist[NB_PHASE] = { false };
	size_t textureParamCount = 0;
	
	const auto checkAndAddGeo = [&](const SA2B_GeometryData* pGeo) {
		// since these separate "geometries" can be sorta interpreted as separate materials
		// basically we're checking if the material's texture matches anything we're looking for
		// if yes, then let's store the pointer to the texture ID, and also the lighting info so we can switch it later on

		bool originalTexlist[4] = { false };

		uint32_t* pLightingParameter = NULL;
		uint32_t origLightingParameter = 0;

		const DAYNIGHT_SKYBOX* pSkyboxEntry = NULL;
		uint32_t* pTexID = NULL;

		for (size_t i = 0; i < pGeo->ParameterCount; i++) {
			auto& para = pGeo->ParameterOffset[i];
			const auto type = para.ParameterType;

#ifndef _DEBUG
			if (pSkyboxEntry && pLightingParameter) {
				break;
			}
#endif

			if (type == 2) { //"lighting" parameter type
				pLightingParameter = &para.Data;
				origLightingParameter = para.Data;
			}

			if (type == 8) { // "texture" parameter type
				const auto texID = para.Data & 0xFFFF;
				textureParamCount++;

				pTexID = &para.Data;

				const DAYNIGHT_SKYBOX* pResultSkyboxEntry;
				if (AL_DayNightCycle_FindSkyboxTexID(texID, &pResultSkyboxEntry)) {
					// for testing in debug mode, let's check if there's any chance at all there are multiple texture entries per geo
					// as far as i know there's no way there should be
					assert(pSkyboxEntry == NULL);

					pSkyboxEntry = pResultSkyboxEntry;
					
					if (pResultSkyboxEntry->dayTexID == -1)		originalTexlist[PHASE_DAY] = true;
					if (pResultSkyboxEntry->eveningTexID == -1) originalTexlist[PHASE_EVE] = true;
					if (pResultSkyboxEntry->nightTexID == -1)	originalTexlist[PHASE_NGT] = true;
					if (pResultSkyboxEntry->cloudyTexID == -1)	originalTexlist[PHASE_CLD] = true;
#ifndef _DEBUG
					break;
#endif
				}
				else {
					for (size_t i = 0; i < NB_PHASE; i++) {
						originalTexlist[i] = true;
					}
				}
			}
		}
		
		// if we found an entry earlier
		if (pSkyboxEntry) {
			DAYNIGHT_SKYBOX_TEXMAP_TABLE texMap = {};

			texMap.pLightingParameter = pLightingParameter;
			texMap.originalLightingParameter = origLightingParameter;
			texMap.pTexID = pTexID;
			texMap.skybox = *pSkyboxEntry;

			texMapTableList.push_back(texMap);
		}
		
		if (pTexID) {
			// this check is needed so that a model doesn't use a combination/"hybrid" texlist setup with a material using original tex id and sky texlist tex id
			if (!showMixedTexlistError && gDayNightManager.GetTextureFileName()) {
				for (size_t i = 0; i < NB_PHASE; i++) {
					// if the original texlist requirement changes after the first entry it means we found a mixed case
					if (textureParamCount > 1 && originalTexlist[i] != needsOriginalTexlist[i]) {
						showMixedTexlistError = true;
						return;
					}

					needsOriginalTexlist[i] |= originalTexlist[i];
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
}

// Checks if the COL's model has any skybox textures, if yes then it fills the list of texture IDs to change
// (and what to change it to)
static bool AL_DayNightCycle_CheckSkybox(bool isGC, NJS_OBJECT* pSrcObj, DAYNIGHT_SKYBOX_TABLE& entry) {
	static APIErrorUtil error("Error in CheckSkybox: ");

	// for GC models we could "predict" the max size in advance easily, but for chunk it requires parsing it twice
	// i decided the overhead of using std::vector is worth it for not making it an even bigger headache
	std::vector <DAYNIGHT_SKYBOX_TEXMAP_TABLE> texMapTableList;

	bool mixedTexlistError = false;
	NJS_OBJECT* pDstChunkObj = NULL;

	if (isGC) {
		const SA2B_Model* pModel = pSrcObj->sa2bmodel;

		// reserve the max possible amount beforehand, since we can easily calculate it in advance
		const size_t sumGeoCount = pModel->OpaqueGeometryCount + pModel->TranslucentGeometryCount;
		texMapTableList.reserve(sumGeoCount);

		AL_DayNightCycle_CheckAndPopulateSkybox_GC(pModel, texMapTableList, mixedTexlistError);
	}
	else {
		AL_DayNightCycle_CheckAndPopulateSkybox_Chunk(pSrcObj, pDstChunkObj, texMapTableList, mixedTexlistError);
	}

	if (mixedTexlistError) {
		error.print("one of the skybox models use both the original texture and the replacement texlist, this skybox model will be omitted from handling.");
		return false;
	}

	if (texMapTableList.empty()) {
		return false;
	}

	entry.texMapCount = texMapTableList.size();
	entry.pTexMap = ALLOC_ARRAY(entry.texMapCount, DAYNIGHT_SKYBOX_TEXMAP_TABLE);
	entry.isChunk = !isGC;
	entry.isCopied = false;
	entry.pObj = pSrcObj;

	if (entry.isChunk) {
		if (pDstChunkObj) {
			entry.isCopied = true;
			entry.pObj = pDstChunkObj;
		}
	}

	memcpy(entry.pTexMap, texMapTableList.data(), entry.texMapCount * sizeof(DAYNIGHT_SKYBOX_TEXMAP_TABLE));

	if (isGC) {
		AL_DayNightCycle_ToggleColorSourceToMaterial(entry, true);
	}

	return true;
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

// Copies the COL and its vertex colors for GC models
static void AL_DayNightCycle_CopyCOLObject_GC(const COL* pSrc, COL* pDst, SA2B_VertexData** pVertexColorTable) {
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

// Copies the vertex data of the source model (for the colors) 
static bool AL_DayNightCycle_CopyVertexChunk(Uint32** pVertexColorTable, Uint32* pSrcVertices, Uint32*& pDstVertices) {
	Uint32* pSrcSearch = pSrcVertices;
	Uint8 chunkType = pSrcSearch[0] & 0xFF;

	while (chunkType != 0xFF) {
		if (chunkType != NJD_CV_D8) return false;

		const Uint32 vertexCount = pSrcSearch[1] >> 16;
		// vertexCount * ( 3 for the positions and 1 for the color)
		const size_t vertexChunkSize = 2 + vertexCount * 4;

		pSrcSearch += vertexChunkSize;

		chunkType = pSrcSearch[0] & 0xFF;
	}
	
	const size_t vlistSize = pSrcSearch - pSrcVertices + 1;
	pDstVertices = ALLOC_ARRAY(vlistSize, Uint32);

	memcpy(pDstVertices, pSrcVertices, vlistSize * sizeof(Uint32));

	pVertexColorTable[VERTEX_COLOR_TABLE_SRC] = pSrcVertices;
	pVertexColorTable[VERTEX_COLOR_TABLE_DST] = pDstVertices;

	return true;
}

// Copies the COL and its vertex colors for GC models
static void AL_DayNightCycle_CopyCOLObject_Chunk(const COL* pSrc, COL* pDst, Uint32** pVertexColorTable) {
	// !!! we assume there are no children or siblings, DrawLandtable doesn't support hierarchies (thanks shad) !!!

	const NJS_CNK_MODEL* pSrcModel = pSrc->Model->chunkmodel;
	if (!pSrcModel) return;

	Uint32* pDstVertices;
	if (!AL_DayNightCycle_CopyVertexChunk(pVertexColorTable, (Uint32*)pSrcModel->vlist, pDstVertices)) return;

	// little disorienting naming scheme, the COL's Model refers to it's NJS_OBJECT, not the sa2b_model
	pDst->Model = ALLOC(NJS_OBJECT);
	*pDst->Model = *pSrc->Model;

	NJS_CNK_MODEL* pDstModel = ALLOC(NJS_CNK_MODEL);
	pDst->Model->chunkmodel = pDstModel;

	*pDstModel = *pSrcModel;

	pDstModel->vlist = (Sint32*)pDstVertices;
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
	
	work.isChunkLandTable = LandTableSA2BModels != true;

	const size_t pointerCount = pSrc->ChunkModelCount * VERTEX_COLOR_TABLE_COUNT;

	// table of src and dst pointers for each col, to not have to find them every frame
	// we also depend on this to know which COLs we can lerp or not
	if (!work.isChunkLandTable) {	
		work.pVertexColorTableGC = ALLOC_ARRAY(pointerCount, SA2B_VertexData*);
		memset(work.pVertexColorTableGC, NULL, sizeof(SA2B_VertexData*) * pointerCount);
	}
	else {
		work.pVertexColorTableChunk = ALLOC_ARRAY(pointerCount, Uint32*);
		memset(work.pVertexColorTableChunk, NULL, sizeof(Uint32*) * pointerCount);
	}
	
	// copy the visual COLs' models
	for (size_t i = 0; i < pSrc->ChunkModelCount; i++) {
		const auto pCOLSrc = &pSrc->COLList[i];
		auto pCOLDst = &pDst->COLList[i];
		
		// if it's a skybox, add it to the list of skyboxes to interpolate the textures on
		if (AL_DayNightCycle_CanCheckSkybox() && AL_DayNightCycle_CheckSkybox(!work.isChunkLandTable, pCOLSrc->Model, pSkyboxColList[skyboxColCount])) {
			skyboxColCount++;

			pCOLDst->Flags &= ~0x80000000; // remove visibility col flag, we'll handle rendering
			continue;
		}

		if (work.isChunkLandTable) {
			AL_DayNightCycle_CopyCOLObject_Chunk(pCOLSrc, pCOLDst, &work.pVertexColorTableChunk[i * 2]);
		}
		else {
			AL_DayNightCycle_CopyCOLObject_GC(pCOLSrc, pCOLDst, &work.pVertexColorTableGC[i * 2]);
		}
	}

	// if we found skyboxes, let's allocate the amount we found and copy it all
	if (skyboxColCount > 0) {
		work.pSkyboxTable = ALLOC_ARRAY(skyboxColCount, DAYNIGHT_SKYBOX_TABLE);
		work.skyboxCount = skyboxColCount;

		memcpy(work.pSkyboxTable, pSkyboxColList.get(), sizeof(DAYNIGHT_SKYBOX_TABLE) * skyboxColCount);
	}
}

static LightGC pushPopFallbackLight;

// hack to fix lighting on vertex colored objects in garden
// we use it for alo_fruit currently
void AL_DayNightCycle_PushFallbackLight() {
	if (!pDayNightTask) return;

	pushPopFallbackLight = LightsGC[LightIndex];

	LightsGC[LightIndex] = GetWork(pDayNightTask).fallbackLight;
	LightsGC[LightIndex].SomeFlag |= 1;
}

void AL_DayNightCycle_PopFallbackLight() {
	if (!pDayNightTask) return;
	
	LightsGC[LightIndex] = pushPopFallbackLight;
}

static void AL_DayNightCycle_InitFallbackLight(task* tp) {
	auto& work = GetWork(tp);

	// we only care about lerping light 0
	size_t index = 0;

	work.originalLight = LightsGC[index];

	// if already GC don't bother converting
	if (LightsGC[index].SomeFlag & 1) {
		work.fallbackLight = LightsGC[index];
		return;
	}
	
	work.fallbackLight.direction = Lights[index].direction;

	work.fallbackLight.ambientReg = Lights[index].color;
	work.fallbackLight.lightColor = Lights[index].color;

	work.fallbackLight.ambientReg.x *= Lights[index].ambient;
	work.fallbackLight.ambientReg.y *= Lights[index].ambient;
	work.fallbackLight.ambientReg.z *= Lights[index].ambient;

	work.fallbackLight.lightColor.x *= Lights[index].intensity;
	work.fallbackLight.lightColor.y *= Lights[index].intensity;
	work.fallbackLight.lightColor.z *= Lights[index].intensity;
}

static void AL_DayNightCycle_Init(task* tp) {
	auto& work = GetWork(tp);

	const auto& skyboxFilename = gDayNightManager.GetTextureFileName();
	if (skyboxFilename) {
		work.pTexlist = LoadCharTextures((char*)skyboxFilename->c_str());
	}

	// for phases that don't have light specified we fallback to the original index 0 light
	// if it's already GC format we copy it, if it's DC format we convert it losslessly
	AL_DayNightCycle_InitFallbackLight(tp);

	// light 2 is the light used for landtables (only matters for landtable models that use normals)
	if (LightsGC[2].SomeFlag & 1) {
		work.originalLandLightColor = LightsGC[2].lightColor;
	}
	else {
		work.originalLandLightColor = Lights[2].color;
	}
	
	// copy the landtable and make the game render our new one, and initialize the skyboxes
	AL_DayNightCycle_InitNewLandTable(tp);
	work.pOldLandtable = CurrentLandTable;
	CurrentLandTable = work.pNewLandtable;
}

#pragma region Executor

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

// Applies the color we want to the destination vertexdata, using the source vertexdata as the base color (for chunk model)
static void AL_DayNightCycle_ApplyVertexColor_Chunk(const NJS_ARGB& mulColor, const Uint32* pSrcVertexColorData, Uint32* pDstVertexColorData) {
	struct VertexPositionColor {
		NJS_POINT3 Position;
		Uint32 color;
	};

	while ((pSrcVertexColorData[0] & 0xFF) != 0xFF) {
		const VertexPositionColor* pSrcVertices = reinterpret_cast<const VertexPositionColor*>(pSrcVertexColorData + 2);
		VertexPositionColor* pDstVertices = reinterpret_cast<VertexPositionColor*>(pDstVertexColorData + 2);

		Uint32 vertexCount = pSrcVertexColorData[1] >> 16;

		for (size_t i = 0; i < vertexCount; i++) {
			NJS_COLOR color;
			color.color = pSrcVertices[i].color;

			color.argb.r = (Uint8)(color.argb.r * mulColor.r);
			color.argb.g = (Uint8)(color.argb.g * mulColor.g);
			color.argb.b = (Uint8)(color.argb.b * mulColor.b);

			pDstVertices[i].color = color.color;
		}

		pSrcVertexColorData += 2 + (vertexCount) * 4;
		pDstVertexColorData += 2 + (vertexCount) * 4;
	}
}

// Applies the color we want to the destination vertexdata, using the source vertexdata as the base color (for GC model)
static void AL_DayNightCycle_ApplyVertexColor_GC(const NJS_ARGB& mulColor, const SA2B_VertexData* pSrcVertexColorData, SA2B_VertexData* pDstVertexColorData) {
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
		color.argb.g = (Uint8)(color.argb.g * mulColor.b);
		color.argb.r = (Uint8)(color.argb.r * mulColor.g);
		color.argb.a = (Uint8)(color.argb.a * mulColor.r);
	}
}

// Applies the color we want to all the landtable models
static void AL_DayNightCycle_ApplyColorToLandTable(task* tp) {
	auto& work = GetWork(tp);

	for (size_t colIndex = 0; colIndex < work.pNewLandtable->ChunkModelCount; colIndex++) {
		// todo: i don't like this
		if (work.isChunkLandTable) {
			const Uint32* pSrcVertices = work.pVertexColorTableChunk[colIndex * 2 + VERTEX_COLOR_TABLE_SRC];
			Uint32* pDstVertices = work.pVertexColorTableChunk[colIndex * 2 + VERTEX_COLOR_TABLE_DST];

			if (!pDstVertices) continue;

			AL_DayNightCycle_ApplyVertexColor_Chunk(work.appliedColor, pSrcVertices, pDstVertices);
		}
		else {
			const SA2B_VertexData* pSrcVertices = work.pVertexColorTableGC[colIndex * 2 + VERTEX_COLOR_TABLE_SRC];
			SA2B_VertexData* pDstVertices = work.pVertexColorTableGC[colIndex * 2 + VERTEX_COLOR_TABLE_DST];

			if (!pDstVertices) continue;

			AL_DayNightCycle_ApplyVertexColor_GC(work.appliedColor, pSrcVertices, pDstVertices);
		}
	}
}

static void AL_DayNightCycle_ApplyLightLerp(task* tp) {
	auto& work = GetWork(tp);

	// landtable light handling 
	NJS_VECTOR landLightColor = work.originalLandLightColor;
	landLightColor.x *= work.appliedColor.r;
	landLightColor.y *= work.appliedColor.g;
	landLightColor.z *= work.appliedColor.b;

	if (LightsGC[2].SomeFlag & 1) {
		LightsGC[2].lightColor = landLightColor;
	}
	else {
		Lights[2].color = landLightColor;
	}

	const auto& srcLightPhase = gDayNightManager.GetLightForPhase(work.phaseA);
	const auto& dstLightPhase = gDayNightManager.GetLightForPhase(work.phaseB);

	const LightGC& pSrcLight = (!srcLightPhase) ? work.fallbackLight : srcLightPhase.value();
	const LightGC& pDstLight = (!dstLightPhase) ? work.fallbackLight : dstLightPhase.value();

	// we always use GC lights because it's what the json format uses
	// the fallback light is either the first GC light, or the first DC light converted to GC (lossless)
	LightsGC[0].SomeFlag |= 1;

	LightsGC[0].direction = slerp(pSrcLight.direction, pDstLight.direction, work.lerpValue);
	njUnitVector(&LightsGC[0].direction);

	LerpColor(LightsGC[0].lightColor, pSrcLight.lightColor, pDstLight.lightColor, work.lerpValue);
	LerpColor(LightsGC[0].ambientReg, pSrcLight.ambientReg, pDstLight.ambientReg, work.lerpValue);
}

// Stores the work data into the save data (it doesn't write the savefile itself, it just writes to the memory that will be saved later)
static void AL_DayNightCycle_StoreIntoSave(task* tp) {
	if (gConfigVal.DayNightCheat) return;

	const auto& work = GetWork(tp);

	AL_DayNightCycle_GetSaveCurrentPhase() = work.phase;
	AL_DayNightCycle_GetSaveTime() = work.timer / float(AL_DayNightCycle_GetDayFrameCount());
	AL_DayNightCycle_GetSaveCurrentDay() = work.day;
	AL_DayNightCycle_GetSaveNextDayCloudy() = work.nextDayCloudy;
}

struct DAYNIGHT_TIME_CONFIG {
	uint32_t phase;
	uint8_t hour;
} static dayNightTimeConfig[] = {
	{ PHASE_DAY, 12 },
	{ PHASE_EVE, 21 },
	{ PHASE_NGT, 1 }
};

static uint32_t AL_DayNightCycle_GetTimerRelativeToPhase(uint32_t timer, uint32_t phase) {
	const auto framesForHour = dayNightTimeConfig[phase].hour * AL_DayNightCycle_GetHourFrameCount();

	// phase = ngt, timer = 0 (midnight), 0 - 21 * frames case
	if (timer < framesForHour) {
		timer += AL_DayNightCycle_GetDayFrameCount();
		return timer - framesForHour;
	}

	return timer - framesForHour;
}

static uint32_t AL_DayNightCycle_GetFramesBetweenPhases(uint32_t phaseA, uint32_t phaseB) {
	assert(phaseA != PHASE_CLD);
	assert(phaseB != PHASE_CLD);

	auto phaseAHour = dayNightTimeConfig[phaseA].hour;
	auto phaseBHour = dayNightTimeConfig[phaseB].hour;

	// ngt -> day case, phaseA = NGT = 21, phaseB = DAY = 8
	if (phaseBHour < phaseAHour) phaseBHour += 24;

	return (phaseBHour - phaseAHour) * AL_DayNightCycle_GetHourFrameCount();
}

static uint32_t AL_DayNightCycle_GetStartHourForPhase(uint32_t phase) {
	return dayNightTimeConfig[phase].hour;
}

void AL_DayNightCycle_GenericGardenTimeHandler(const DAYNIGHT_TIME_INFO* pInfo, DAYNIGHT_TIME_WORK* pWork) {
	const size_t hourFrames = pInfo->GetHourFrameCount();
	
	size_t phase = -1;
	for (size_t i = PHASE_DAY; i <= PHASE_NGT; i++) {
		if (pInfo->GetStartHourForPhase(i) * hourFrames == pInfo->timer) {
			phase = i;
			break;
		}
	}

	if (phase == -1) return;

	pWork->phase = phase;

	// if the current phase became night let's decide whether cloudy will come after night
	if (pWork->phase == PHASE_NGT) {
		pWork->nextDayCloudy = njRandom() < 0.5f;
	}

	// if the current phase became day then the next phase is either day or cloudy
	if (pWork->phase == PHASE_DAY) {
		pWork->phase = pWork->nextDayCloudy ? PHASE_CLD : PHASE_DAY;
	}
}

static void AL_DayNightCycle_GenericGardenRenderHandler(const DAYNIGHT_TIME_INFO* pInfo, DAYNIGHT_RENDER_WORK* pWork) {
	pWork->phaseA = pInfo->phase;

	if (pWork->phaseA == PHASE_NGT && pInfo->nextDayCloudy) {
		// if we're at night time rn and the next day will be cloudy then make sure to transition to that
		pWork->phaseB = PHASE_CLD;
	}
	else if (pWork->phaseA == PHASE_CLD) {
		// if we're at cloudy time then that corresponds to day
		pWork->phaseB = PHASE_EVE;
	}
	else {
		// day -> eve -> ngt -> day -> ...
		pWork->phaseB = (pInfo->phase + 1) % 3;
	}

	uint32_t timePhaseA = (pWork->phaseA == PHASE_CLD) ? PHASE_DAY : pWork->phaseA;
	uint32_t timePhaseB = (pWork->phaseB == PHASE_CLD) ? PHASE_DAY : pWork->phaseB;
	pWork->lerpValue = pInfo->GetTimerRelativeToPhase(pInfo->timer, timePhaseA)
		/ float(pInfo->GetFramesBetweenPhases(timePhaseA, timePhaseB));
}

static void AL_DayNightCycle_DarkGardenRenderHandler(const DAYNIGHT_TIME_INFO* pTimeInfo, DAYNIGHT_RENDER_WORK* pWork) {
	pWork->phaseA = (pTimeInfo->phase == PHASE_CLD) ? PHASE_CLD : PHASE_DAY;

	if (pTimeInfo->phase == PHASE_NGT && pTimeInfo->nextDayCloudy) {
		pWork->phaseB = PHASE_CLD;
	}
	else {
		pWork->phaseB = PHASE_DAY;
	}

	if (pWork->phaseA == pWork->phaseB) {
		pWork->lerpValue = 0.f;
		return;
	}

	uint32_t timePhaseA = (pTimeInfo->phase == PHASE_CLD) ? PHASE_DAY : pTimeInfo->phase;
	uint32_t timePhaseB = (pTimeInfo->phase == PHASE_CLD) ? PHASE_EVE : ((pTimeInfo->phase + 1) % 3);
	pWork->lerpValue = pTimeInfo->GetTimerRelativeToPhase(pTimeInfo->timer, timePhaseA)
		/ float(pTimeInfo->GetFramesBetweenPhases(timePhaseA, timePhaseB));
}

// Main update function for the DNC
static void AL_DayNightCycleExecutor(task* tp) {
	enum {
		MODE_INIT = 0,
		MODE_IDLE
	};

	auto& work = GetWork(tp);

	switch (work.mode) {
	case MODE_INIT:
		AL_DayNightCycle_Init(tp);
		work.mode = MODE_IDLE;

#if 0
		// this is temporary, enable when porting the lights, remove in final release
		// load the lights we want and copy them
		{
			// this daynightconfig thing is temp for now too
			const AL_DayNightConfig& config = gDayNightConfig.at(CurrentChaoArea);

			LoadStageLight(config.DefaultLight);
			memcpy(&work.lights[LIGHT_DEF], Lights, sizeof(work.lights[LIGHT_DEF]));
			//LoadStageLight(config.EveLight);
			//memcpy(&work.lights[LIGHT_EVE], Lights, sizeof(work.lights[LIGHT_EVE]));
			//LoadStageLight(config.NightLight);
			//memcpy(&work.lights[LIGHT_NGT], Lights, sizeof(work.lights[LIGHT_NGT]));
			LoadStageLight(config.CloudLight);
			memcpy(&work.lights[LIGHT_CLOUD], Lights, sizeof(work.lights[LIGHT_CLOUD]));
		}
		{
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
			//dump(work.lights[PHASE_EVE][0]);
			PrintDebug("night");
			//dump(work.lights[PHASE_NGT][0]);
			PrintDebug("cloudy");
			dump(work.lights[PHASE_CLD][0]);
		}
#endif

		break;
	}

	DAYNIGHT_TIME_INFO timeInfo = {
		.timer = work.timer,
		.phase = work.phase,
		.day = work.day,
		.nextDayCloudy = work.nextDayCloudy,

		.GetHourFrameCount = AL_DayNightCycle_GetHourFrameCount,
		.GetDayFrameCount = AL_DayNightCycle_GetDayFrameCount,
		.GetStartHourForPhase = AL_DayNightCycle_GetStartHourForPhase,
		.GetTimerRelativeToPhase = AL_DayNightCycle_GetTimerRelativeToPhase,
		.GetFramesBetweenPhases = AL_DayNightCycle_GetFramesBetweenPhases,
	};

	bool canTimePass = AL_IsGarden() && AL_DayNightCycle_IsValidArea();
	if (!gConfigVal.DayNightCheat && canTimePass) {
		AL_DayNightCycle_StoreIntoSave(tp);
		
		DAYNIGHT_TIME_WORK timeWork = {
			.phase = work.phase,
			.nextDayCloudy = work.nextDayCloudy
		};

		AL_DayNightCycle_GenericGardenTimeHandler(&timeInfo, &timeWork);

		// copy "output" of API function
		work.phase = timeWork.phase;
		work.nextDayCloudy = timeWork.nextDayCloudy;

		timeInfo.phase = timeWork.phase;
		timeInfo.nextDayCloudy = timeWork.nextDayCloudy;

		work.timer++;
		if (work.timer >= AL_DayNightCycle_GetDayFrameCount()) {
			work.timer = 0;

			work.day++;
		}
	}
	
	DAYNIGHT_RENDER_WORK renderWork = {
		work.phaseA,
		work.phaseB,
		work.lerpValue
	};

	if (gConfigVal.DayNightCheat) {
		renderWork.phaseA = gDayNightCheatPhase;
		renderWork.phaseB = gDayNightCheatPhase;
		renderWork.lerpValue = 0.f;
	}
	else {
		if (AL_GetStageNumber() == CHAO_STG_DARK) {
			AL_DayNightCycle_DarkGardenRenderHandler(&timeInfo, &renderWork);
		}
		else {
			AL_DayNightCycle_GenericGardenRenderHandler(&timeInfo, &renderWork);
		}
	}

	work.phaseA = renderWork.phaseA;
	work.phaseB = renderWork.phaseB;
	work.lerpValue = renderWork.lerpValue;

	const auto& colorA = gDayNightManager.GetColorForPhase(work.phaseA);
	const auto& colorB = gDayNightManager.GetColorForPhase(work.phaseB);

	LerpColor(work.appliedColor, colorA, colorB, work.lerpValue);

	AL_DayNightCycle_ApplyLightLerp(tp);
	AL_DayNightCycle_ApplyColorToLandTable(tp);
}

#pragma endregion

#pragma region Destructor
// Restores all model changes
static void AL_DayNightCycle_RestoreAll(task* tp) {
	auto& work = GetWork(tp);
	
	LightsGC[0] = work.originalLight;

	// restore the original landtable's field to keep track of if the texlist is already loaded or not
	work.pOldLandtable->field_A = work.pNewLandtable->field_A; 

	for (size_t i = 0; i < work.skyboxCount; i++) {
		auto& skybox = work.pSkyboxTable[i];

		AL_DayNightCycle_ChangeTextures(skybox, -1);
		AL_DayNightCycle_ToggleColorSourceToMaterial(skybox, false);
	}
}

// Frees the copied object, the sa2bmodel, and the vertex colors it copies
static void AL_DayNightCycle_FreeObject_Chunk(NJS_OBJECT* pObj, Uint32** pColorTable) {
	if (!pObj) return;

	// if we have vertex colors for the node that means we copied everything up to that point 
	// so free them
	if (pColorTable[VERTEX_COLOR_TABLE_DST]) {
		FREE(pObj->chunkmodel->vlist);
		FREE(pObj->chunkmodel);
		FREE(pObj);
	}
}

// Frees the copied object, the sa2bmodel, and the vertex colors it copies
static void AL_DayNightCycle_FreeObject_GC(NJS_OBJECT* pObj, SA2B_VertexData** pColorTable) {
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
		if (work.isChunkLandTable) {
			AL_DayNightCycle_FreeObject_Chunk(pLand->COLList[i].Model, &work.pVertexColorTableChunk[i * 2]);
		}
		else {
			AL_DayNightCycle_FreeObject_GC(pLand->COLList[i].Model, &work.pVertexColorTableGC[i * 2]);
		}
	}

	FREE(pLand->COLList);
	FREE(pLand);
}

// Frees all the allocated copied stuff and skybox stuff
static void AL_DayNightCycleDestructor(task* tp) {
	AL_DayNightCycle_RestoreAll(tp);
	AL_DayNightCycle_FreeLandTableCOLAndObjects(tp);

	auto& work = GetWork(tp);

	if (work.pVertexColorTableGC) {
		FREE(work.pVertexColorTableGC);
	}

	if (work.pVertexColorTableChunk) {
		FREE(work.pVertexColorTableChunk);
	}

	// the texlist was created with LoadCharTextures which auto-allocates the texlist
	// so alongside releasing the textures in the texlist, we also need to release the texlist itself
	// note that we don't need to release the texnames because it's allocated "with" the texlist
	if (work.pTexlist) {
		njReleaseTexture(work.pTexlist);
		FREE(work.pTexlist);
	}

	if (work.skyboxCount) {
		for (size_t i = 0; i < work.skyboxCount; i++) {
			auto& skybox = work.pSkyboxTable[i];
			FREE(skybox.pTexMap);

			if (skybox.isChunk && skybox.isCopied) {
				FREE(skybox.pObj->chunkmodel->vlist);
				FREE(skybox.pObj->chunkmodel);
				FREE(skybox.pObj);
			}
		}

		FREE(work.pSkyboxTable);
	}

	gDayNightManager.Clear();

	pDayNightTask = nullptr;
}

#pragma endregion

#pragma region Displayer

static void AL_DayNightCycle_SetTexturesAndTexlist(task* tp, DAYNIGHT_SKYBOX_TABLE& skybox, int phase) {
	auto& work = GetWork(tp);

	bool isOriginalTexlist = AL_DayNightCycle_ChangeTextures(skybox, phase);

	if (isOriginalTexlist) {
		njSetTexture(work.pNewLandtable->TextureList);
	}
	else {
		njSetTexture(work.pTexlist);
	}
}

static void AL_DayNightCycleDrawSkyboxes(task* tp, float alpha, int phaseA, int phaseB) {
	auto& work = GetWork(tp);

	for (size_t i = 0; i < work.skyboxCount; i++) {
		auto& skybox = work.pSkyboxTable[i];
		
		const auto draw_skybox = [&](float alpha, int phase) {
			SetMaterial(alpha, 1, 1, 1);
			gjSetDiffuse(0xFFFFFFFF);

			AL_DayNightCycle_SetTexturesAndTexlist(tp, skybox, phase);

			if (skybox.isChunk) njCnkDrawObject(skybox.pObj);
			else gjDrawObject(skybox.pObj);
		};

		// hack to restore hero garden skybox rotation
		const auto backupEval = skybox.pObj->evalflags;
		skybox.pObj->evalflags &= ~NJD_EVAL_UNIT_POS;
		skybox.pObj->evalflags &= ~NJD_EVAL_UNIT_ANG;
		skybox.pObj->evalflags &= ~NJD_EVAL_UNIT_SCL;

		// opaque pass
		// we don't want it to be alpha tested though 
		rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_AUTO_TRANS);

		draw_skybox(1.f, phaseA);

		// transparent pass
		rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_TRANSPARENT);

		OnConstantAttr(0, NJD_FST_UA);

		draw_skybox(alpha, phaseB);

		OffConstantAttr(0, NJD_FST_UA);
		rfapi_core->pApiRenderState->SetTransMode(RFRS_TRANSMD_END);
		skybox.pObj->evalflags = backupEval;
	}
}

static void AL_DayNightCycleDisplayer(task* tp) {
	DataPointer(int, GinjaDrawFlag, 0x25EFEE0);

	auto& work = GetWork(tp);

	if (gConfigVal.DayNightDebug) {
		g_HelperFunctions->SetDebugFontSize(30);
		g_HelperFunctions->SetDebugFontColor(0xFFFFFFFF);
		g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0, 0), "==DAYNIGHT TIME==");
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 1), "PHASE %d", work.phase);
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 2), "DAY %d", work.day);
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 3), "NEXT_CLOUDY %d", work.nextDayCloudy);
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 4), "TIMER %d", work.timer);
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 5), "HOUR %d", work.timer / AL_DayNightCycle_GetHourFrameCount());
		g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0, 6), "==DAYNIGHT RENDER==");
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 7), "PHASE_A %d", work.phaseA);
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 8), "PHASE_B %d", work.phaseB);
		g_HelperFunctions->DisplayDebugStringFormatted(NJM_LOCATION(0, 9), "LERP %f", work.lerpValue);
	}

	// landtables use this light index for some reason
	DoLighting(2); 

	SaveControl3D();
	SaveConstantAttr();

	int backupblend = nj_cnk_blend_mode;
	nj_cnk_blend_mode = NJD_FBS_SA | NJD_FBD_ISA; // source alpha to inverse source alpha blending
	OnControl3D(NJD_CONTROL_3D_CNK_BLEND_MODE);

	// we don't reset these, as far as i know this is the default anyways
	// but i set it incase anything does something fucky before this
	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	// this sets blending based on the values set in njColorBlendingMode
	// this is only needed for ginja skyboxes which doesn't have the "nj_cnk_blend_mode" overrides
	// we disable the ginja blendmode set outside of this function, and force it once here
	EnableAlpha(1);

	// in object list 1, transparent and opaque GJ geo has to be drawn separately 
	// so we have to reenable the normal functionality to draw both at once
	auto backupGinjaDrawFlag = GinjaDrawFlag;
	GinjaDrawFlag = 1 | 2;

	// ignore lighting
	OnControl3D(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
	OnConstantAttr(0, NJD_FST_IL);

	// needed to set the alpha
	OnControl3D(NJD_CONTROL_3D_CONSTANT_MATERIAL);

	// HACK: we need to temporarily kill the GJ blend mode stuff so it doesn't overwrite out alpha blend
	uint32_t& blendModeGinjaFunc = *(uint32_t*)0x0174B344;
	const uint32_t blendModeGinjaFuncBackup = blendModeGinjaFunc;
	blendModeGinjaFunc = (int)nullsub_1;

	// hack to sorta make the transition less obvious
	// the only real fix for this is to use shaders, the "alpha dependent" lerping effect
	// breaks with textures that have transparent pixels (cloud borders on hero garden)
	if (work.lerpValue < 0.5f) {
		AL_DayNightCycleDrawSkyboxes(tp, work.lerpValue, work.phaseA, work.phaseB);
	}
	else {
		AL_DayNightCycleDrawSkyboxes(tp, 1.f - work.lerpValue, work.phaseB, work.phaseA);
	}

	LoadControl3D();
	LoadConstantAttr();

	blendModeGinjaFunc = blendModeGinjaFuncBackup;

	SetMaterial(0, 0, 0, 0);

	njColorBlendingMode(0, 8);
	njColorBlendingMode(1, 6);

	nj_cnk_blend_mode = backupblend;
	GinjaDrawFlag = backupGinjaDrawFlag;

	DoLighting(LightIndexBackupMaybe);
}

#pragma endregion

// IF I don't happen to update this in time when the ECW API comes out
// this will prevent the daynight cycle code from running if an ECW custom garden is active
// !! remove when the support is added !!
static bool AL_DayNightCycle_CheckECWSafety() {
	switch (AL_GetStageNumber()) {
		case CHAO_STG_NEUT:
			if (ChaoSegments[0].Prolog != ChaoStgNeut_Prolog) return false;
			break;
		case CHAO_STG_HERO:
			if (ChaoSegments[1].Prolog != ChaoStgHero_Prolog) return false;
			break;
		case CHAO_STG_DARK:
			if (ChaoSegments[2].Prolog != ChaoStgDark_Prolog) return false;
			break;
	}

	return true;
}

static void AL_DayNightCycleManagerDestructor(task* tp) {
	if (pDayNightTask) {
		DeleteObject_(pDayNightTask);
	}
}

// this serves the purpose of deleting the regular daynight cycle task, since its in object list 1
// and those don't autodestruct on level change
static void AL_CreateDayNightCycleManager() {
	task* tp = LoadObject(4, "AL_DayNightCycleManager", [](task* tp){}, (LoadObj)0);
	tp->DeleteSub = AL_DayNightCycleManagerDestructor;
}

void AL_CreateDayNightCycle() {
	if (!gConfigVal.DayNightCycle) return;
	if (!AL_DayNightCycle_IsValidArea()) return;
	if (!AL_DayNightCycle_CheckECWSafety()) return;
	if (!gDayNightManager.LoadConfig(AL_DayNightCycle_GetGardenID())) return;
	
	task* tp = LoadObject(1, "AL_DayNightCycle", AL_DayNightCycleExecutor, LoadObj_Data1);
	pDayNightTask = tp;

	AL_CreateDayNightCycleManager();

	tp->Data2.Undefined = ALLOC(DAYNIGHT_WORK);

	memset(tp->Data2.Undefined, 0, sizeof(DAYNIGHT_WORK));

	auto& work = GetWork(tp);

	work.timer = AL_DayNightCycle_GetSaveTime() * AL_DayNightCycle_GetDayFrameCount();
	work.phase = AL_DayNightCycle_GetSaveCurrentPhase();
	work.day = AL_DayNightCycle_GetSaveCurrentDay();
	work.nextDayCloudy = AL_DayNightCycle_GetSaveNextDayCloudy();

	if (work.day == 0 && work.timer == 0) {
		// start at day for new save, 1 + to prevent the code trying to transition unnecessarily on the first frame
		work.timer = 1 + AL_DayNightCycle_GetHourFrameCount() * AL_DayNightCycle_GetStartHourForPhase(PHASE_DAY);
		work.phase = PHASE_DAY;
	}

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

void AL_DayNight_Init(const std::string& iniPath, IniFile* pConfig, const HelperFunctions& helper) {
	if (!gConfigVal.DayNightCycle) return;
	
	if (!RenderFix_IsEnabled()) {
		gConfigVal.DayNightCycle = false;

		const auto result = MessageBoxA(
			0, 
			"The 'SA2 Render Fix' mod is not enabled! The 'Day/Night Cycle' feature cannot function properly without it, and has been disabled for now.\n\n"
			"Would you like to download 'SA2 Render Fix' now? Pressing 'Yes' will open the download page in your web browser.\n\n"
			"If not, 'Day/Night Cycle' will be automatically disabled in your config file.",
			"Chao World Extended: \"SA2 Render Fix\" missing! ", 
			MB_YESNO
		);

		if (result == IDNO) {
			pConfig->setBool("DayNight", "DayNightCycle", false);
			pConfig->save(iniPath);
			MessageBoxA(0, "The Day Night Cycle option is now disabled.", "Chao World Extended", 0);
		}
		else {
			system("start https://gamebanana.com/mods/452445");
			MessageBoxA(0, "The Day Night Cycle option is now temporarily disabled.", "Chao World Extended", 0);
		}

		return;
	}
		
	DrawLandtable_t.Hook(DrawLandtable_r);
}