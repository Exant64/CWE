#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "CodeParser.hpp"
#include "../IniFile.h"
#include "../ChaoMain.h"

#include "cwe_transpile.h"
#include "cwe_code.h"
#include "code_manager.h"

#include "codes/include/code_eggcolor.h"
#include "codes/include/code_playerspeed.h"
#include "codes/include/code_birthday.h"
#include "codes/include/code_gardenlimit.h"
#include "codes/include/code_personality.h"
#include "codes/include/code_monsterevo.h"
#include "codes/include/code_sick.h"
#include "codes/include/code_hunger.h"
#include "codes/include/code_drown.h"
#include "codes/include/code_dccocoon.h"
#include "codes/include/code_disjingle.h"
#include "codes/include/code_animal_reuse.h"
#include "codes/include/code_hdtoy.h"
#include "codes/include/code_racefair.h"
#include "codes/include/code_disablebreed.h"
#include "codes/include/code_canthatecharacters.h"

#include "codes/include/bonus/code_mergeherodark.h"

#include "codes/include/code_racecam_switch.h"
#include "codes/include/code_multisave.h"
#include "codes/include/code_fruit_nodespawn.h"
#include "codes/include/code_dynamiclifetime.h"
#include <code_system/codes/include/code_shinyjewelmonotone.h>
#include <code_system/codes/include/cheat/code_levellimit.h>
#include <code_system/codes/include/bonus/code_babychaolevel.h>
#include <code_system/codes/include/advanced/code_gc_shinyjewel.h>

#include <code_system/codes/include/easy/code_disable_reincarnation.h>

CodeParser codeParser;
struct Config { const char* a, * b; };
static Config codeConfig[] = {
	{ "Advanced", "AdvancedMixingColors"},
	{ "Advanced", "AdvancedNewbornChaoHaveVaryingShadesofColor"},
	{ "Advanced", "AdvancedNormalChaocanmakeColorChao"},
	{ "Advanced", "AdvancedEyeColorsforNewbornChao"},
	{ "Advanced", "AdvancedFruitscanSpoilt"},
	{ "Hard", "HardBullyChaoRequiresMoreChaoAnimations"},
	{ "Hard", "HardChaoAttention"},
	{ "Hard", "HardAnnoyingMeanChao"},
	{ "Easy", "EasyInstantGrowTrees"},
	{ "Easy", "EasyAutoGenerateStats"},
	{ "Race", "BonusChaoKarategiveMedal"},
	{ "Misc", "BonusLuckyChao"},
	{ "Input", "ControllerOnlyBetterCameraControl"},
	{ "Input", "KeyboardwithNumpadBetterCameraControl"},
	{ "Input", "KeyboardwithoutNumpadBetterCameraControl"}, 
	{ "Misc", "OtherMoreandRandomChaoFace"},
	{ "Race", "RaceJewelandBeginnerRacebecome8PPartyRace"},
	{ "Race", "RaceChaoGainExpatChaoRace"},
	{ "Input", "SaveHoldXtoLoadNegativeChaoSave"},
	{ "Detail", "DetailFittingPersonality"}
};


//one day this will be moved out to cwe.cpp once we stop needing codeparser
int ToyResetTimer = 0;
void CWE_Codes_OnFrame() {
	ChaoWorldExtendedRequired();
	codeParser.processCodeList();

	for (auto& c : CodeManager::Instance()) {
		c->OnFrame();
	}

	if (CurrentLevel == 90) {
		//if the texlist is loaded at all
		if (CWE_OBJECT_TEXLIST.textures[0].texaddr) {
			if (CWE_OBJECT_TEXLIST.nbTexture < 67 || !CWE_OBJECT_TEXLIST.textures[62].texaddr) {
				g_HelperFunctions->SetDebugFontColor(0xFF00FF00); //ARGB
				g_HelperFunctions->SetDebugFontSize(12);
				g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0,2), "CWE_OBJECT was replaced by another mod with outdated textures");
				g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0,4), "This might cause crashes, beware");
				g_HelperFunctions->SetDebugFontSize(12);
			}
		}

		if (CWE_UI_TEXLIST.textures[0].texaddr) {
			if (CWE_UI_TEXLIST.nbTexture < 33 || !CWE_UI_TEXLIST.textures[26].texaddr) {
				g_HelperFunctions->SetDebugFontColor(0xFF00FF00); //ARGB
				g_HelperFunctions->SetDebugFontSize(12);
				g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0, 3), "CWE_UI was replaced by another mod with outdated textures");
				g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0, 4), "This might cause crashes, beware");
				g_HelperFunctions->SetDebugFontSize(12);
			}
		}
		
		/*
		if (al_mini_parts_tex_texlist.textures[0].texaddr) {
			if (!al_mini_parts_tex_texlist.textures[200].texaddr) {
				g_HelperFunctions->SetDebugFontColor(0xFF00FF00); //ARGB
				g_HelperFunctions->SetDebugFontSize(12);
				g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0, 3), "al_mini_parts_tex was replaced by another mod without the CWE textures");
				g_HelperFunctions->DisplayDebugString(NJM_LOCATION(0, 4), "This might cause crashes, beware");
				g_HelperFunctions->SetDebugFontSize(12);
			}
		}
		*/
	}
	if (ToyResetTimer) {
		ToyResetTimer--;
		g_HelperFunctions->SetDebugFontColor(0xFF00FF00); //ARGB
		g_HelperFunctions->SetDebugFontSize(20);
		g_HelperFunctions->DisplayDebugString(0, "TOYS HAVE BEEN RESET");
		g_HelperFunctions->SetDebugFontSize(12);
	}
	if (gConfigVal.BrightFixEnabled)
	{
		g_HelperFunctions->SetDebugFontColor(0xFF00FF00); //ARGB
		g_HelperFunctions->SetDebugFontSize(20);
		g_HelperFunctions->DisplayDebugString(0, "BrightFix is enabled, please turn it off to prevent interference");
		g_HelperFunctions->SetDebugFontSize(12);
	}
		
}

void CWE_Codes_CheckAndLoadCode(const char* path, const IniFile* config, const Config& param)
{
	char binPath[MAX_PATH];
	if (config->getBool(param.a, param.b, false))
	{
		PrintDebug("CWE_Codes: Loading %s", param.b);
		sprintf(binPath, "%s\\CodeBinary\\%s.bin", path, param.b);
		int ret = codeParser.readCodes(binPath);
		if (ret == -ENOENT)
			PrintDebug("failed to load %s", binPath);
	}
}

void CWE_Codes_Init(const char* path, const IniFile* config) {

	//config gets deleted after init, but it stays inside the codemanager
	//i havent quite figured out yet how to handle this
	CodeManager::Init(config);
	
	CodeManager::Instance().Add<EggColor>("EggColor", true);
	CodeManager::Instance().Add<PlayerSpeed>("PlayerSpeed", true);
	CodeManager::Instance().Add<BirthdayParty>("BirthdayEvent", true);
	CodeManager::Instance().Add<GardenLimit>("GardenLimit", 8, -1);
	CodeManager::Instance().Add<PersonalityMultiply>("PersonalityMultiply", 1, 1);
	CodeManager::Instance().Add<MonsterEvo>("EnableMonsterEvolution", 0, 0);
	CodeManager::Instance().Add<ChaoSick>("ChaoSick", 0, 0);

	CodeManager::Instance().Add<DisableBreed>("DisableBreed", false);
	CodeManager::Instance().Add<ChaoHunger>("ChaoHunger", false);
	CodeManager::Instance().Add<ChaoDrown>("ChaoDrown", false);

	//i had to add the section here because we moved it
	CodeManager::Instance().Add<AnimalReuse>("Easy","AnimalReuse", false);
	CodeManager::Instance().Add<DisableReincarnation>("Easy", "DisableReincarnation", false);
	CodeManager::Instance().Add<CantHateCharacters>("EasyChaocan'thateCharacters", false);
	CodeManager::Instance().Add<MergeHeroDark>("BonusMergeHeroandDarkGardenChaoSlots", false);

	CodeManager::Instance().Add<DCCocoon>("DCCocoon", false);
	CodeManager::Instance().Add<DisableLevelupJingle>("DisableLevelupJingle", false);
	CodeManager::Instance().Add<HDToyModels>("HDToyModels", false);
	CodeManager::Instance().Add<JackInBoxPunishment>("JackInBoxPunishment", false);
	CodeManager::Instance().Add<IgnoreMinigameIntel>("IgnoreMinigameIntel", false);

	//lst ports
	CodeManager::Instance().Add<RaceCamSwitch>("RaceCameraswitchManuallyusingkeyboard", false);
	CodeManager::Instance().Add<MultiSave>("SaveMultiSavefile", false);
	CodeManager::Instance().Add<FruitsDontDespawn>("EasyFruitsNeverDespawn", false);
	CodeManager::Instance().Add<DynamicLifetime>("AdvancedDynamicLifetime", false);
	CodeManager::Instance().Add<ShinyJewelMonotone>("AdvancedCreateNEWShinyJewelMonotone", false);
	CodeManager::Instance().Add<DisableLevelLimit>("CheatDisableLevelLimit", false);
	CodeManager::Instance().Add<StayBabyChaoLvl>("BonusStayBabyChaoUntilalllvl.10", false);
	CodeManager::Instance().Add<GCShinyJewel>("AdvancedTransformPCShinyJewelsintoGCShinyJewels", false);
	
	codeParser.clear();
	for (size_t i = 0; i < LengthOfArray(codeConfig); i++)
	{
		CWE_Codes_CheckAndLoadCode(path, config, codeConfig[i]);
	}
}