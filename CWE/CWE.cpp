#include "stdafx.h"

#include "IniFile.h"
#include "ninja_functions.h"
#include "ChaoMain.h"
#include "Chao.h"
#include "al_parts.h"
#include "al_medal_disp.h"
#include "alr_camera_edit.h"
#include "al_animation.h"
#include "AL_ModAPI.h"

#include "alg_kinder_he.h"
#include "alg_kinder_bl.h"

#include "al_minimal.h"
#include "alo_obakehead.h"
#include "alo_fruit.h"

#include "al_behavior/alsbhv.h"
#include "alo_boat.h"

#include "al_behavior/albhv.h"

#include "alg_kinder_ortho.h"
#include "patchedcolors.h"

#include <windows.h>  
#include <iostream>  
#include <dbghelp.h>  
#include <ctime>  
#include "al_piano.h"

#include "brightfixapi.h"
#include "al_tv.h"
#include <cassert>
#include "al_butterfly.h"
#include "al_sandhole.h"
#include "ALifeSDK_Functions.h"

#include "al_odekake.h"
#include "al_save.h"
#include "al_palette.h"
#include "al_daynight.h"

#include "data/al_seentry.h"

#include "code_system/cwe_codes.h"
#include "code_system/cwe_patch.h"
#include "al_edit.h"
#include "al_omochao_build.h"

#include "al_behavior/al_knowledge.h"

#include "data/ChaoVertexShader.h"
#include "data/ChaoShader.h"
#include "data/ChaoShaderDayNight.h"

#include "al_growtree.h"
#include "Trampoline.h"
#include "al_toy_move.h"
#include "al_parameter.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "data/ShinyJewelColors.h"
#include "ui/al_ortho.h"
#include "alg_kinder_co.h"
#include "al_race.h"
#include "cwe_fixes.h"

#include <chrono>
#include "al_chao_info.h"
#include "code_system/code_manager.h"
#include "util.h"
#include "al_stage.h"

#include "register/api_register.h"
#include "al_chao_counter.h"
#include "alo_special.h"

#include "PaletteFix.h"
#include "alg_kinder_cl.h"
#include <kce_helper.h>
#include <renderfix.h>

#include <data/heroskyboxfix/object_ghero_nk_kumoback_kumoback.h>
#include <data/heroskyboxfix/object_ghero_nk_kumofront_kumofront.h>

#ifdef IMGUIDEBUG
	#include <imgui_debug.h>
#endif
#include <al_garden_info.h>
#include <api/api_json.h>
#include <global_save.h>

const char* PathToModFolder = "";

extern "C"
{
	int __cdecl CreateToyHook()
	{
		int retval = AL_NormalCameraExecutor_Load();
		
#ifdef RENDERTARGET
		LoadObject(4, "testbeginscenehook", [](ObjectMaster* a1) {
			WriteCall((void*)0x00434011, BeginScene_Ex);
			a1->MainSub = DeleteObject_;
			}, (LoadObj)0);
#endif

		switch (AL_GetStageNumber()) {
		case CHAO_STG_NEUT:
		case CHAO_STG_HERO:
		case CHAO_STG_DARK:
			if(gConfigVal.ChaoCounter)
				AL_ChaoCounterCreate();
			break;
		}

		AL_NameDisplayCreate();
		//ALO_Butterfly_Load();

		if (gConfigVal.SpawnOmochao) {
			NJS_POINT3 pos = OmoPositions[AL_GetStageNumber() - 1];
			Angle ang = OmoRot[AL_GetStageNumber() - 1];
			ALO_OmoBuildCreate(&pos, ang);
		}

		if (gConfigVal.UnusedToys)
		{
			if (CurrentChaoArea == 1)
			{
				NJS_POINT3 pos = { -51, 0.448f, -16 };
				ALO_BoatCreate(&pos, 0);
			}
			if (CurrentChaoArea == 2)
			{
				NJS_POINT3 pos = { 88, 0, 50 };
				ALO_FloatCreate(&pos, 0);
			}
			if (
				(CurrentChaoArea == 2 && (ChaoToysUnlocked[0] & 0x400)) || 
				(CurrentChaoArea == 3 && (ChaoToysUnlocked[0] & 0x800))
				)
			{
				NJS_VECTOR GCPos[] = { {-45, 0, 6}, {-102, 0.05f, 4.5f} };
				NJS_VECTOR DCPos[] = { {-48, 0, 16}, {-71, 0, -27} };
				Uint32 Rot[] = { 0x5B0, 0x4000 };
				int index = (AL_GetStageNumber() == 2) ? 0 : 1;
				if (GetModuleHandle(L"DCGarden"))
					ALO_PianoCreate(index, &DCPos[index], Rot[index]);
				else
					ALO_PianoCreate(index, &GCPos[index], Rot[index]);
			}
		}
		return retval;
	}

	static void GuestChao(ChaoDataBase& param) {

		//to hopefully prevent chao getting capped when inside guest menu
		if (AL_GetStageNumber() == CHAO_STG_ODEKAKE)
			return;

		if (param.field_19 != 1) return;

		AL_GUEST& Guest = param.Guest;

		if (Guest.Type == 0) {
			Guest.Type = param.Type;
			Guest.Alignment = param.Alignment;
			Guest.Magnitude = param.EvolutionProgress;
			Guest.FlySwim = param.FlySwim;
			Guest.RunPower = param.PowerRun;

		}
		else {
			param.Type = Guest.Type;
			param.Alignment = Guest.Alignment;
			param.EvolutionProgress = Guest.Magnitude;
			param.FlySwim = Guest.FlySwim;
			param.PowerRun = Guest.RunPower;
		}

		param.Lifespan = 100;
		param.Lifespan2 = 100;

		param.gap_0[0xC] = 0; //?

		for (int i = 0; i < 5; i++) {
			param.StatFractions[i] = 0;

			if (param.StatGrades[i] > ChaoGrade_B) {
				param.StatGrades[i] = ChaoGrade_B;
			}

			if (param.StatPoints[i] > 2000) {
				param.StatPoints[i] = 2000;
				param.StatLevels[i] = 109; //lock icon later
			}

			param.Gene.Abl[i][1] = ChaoGrade_E;
		}

		param.StatGrades[6] = param.StatGrades[7] = 0;

		param.XGradeValue = 0;
		param.UpgradeCounter = 5;
	}

	void __cdecl ALW_Control_Main_Hook(ObjectMaster* a1);
	Trampoline ALW_Control_t(0x00530850, 0x00530859, ALW_Control_Main_Hook);
	void __cdecl ALW_Control_Main_Hook(ObjectMaster* a1)
	{
		if (a1->Data1.Entity->Action == 0) {
			AL_CreateDayNightCycle();
		}

		const auto original = reinterpret_cast<decltype(ALW_Control_Main_Hook)*>(ALW_Control_t.Target());
		original(a1);

		for (auto& c : CodeManager::Instance()) {
			for (size_t i = 0; i < ChaoInfo::Instance().Count(); i++) {
				c->OnChaoData(ChaoInfo::Instance()[i]);
			}

			c->OnALControl(a1);
		}

		if (GetAsyncKeyState(VK_F11)) {
			LoadChaoTexlist("CWE_UI", &CWE_UI_TEXLIST, 0);
		}

		//compatibility layer for old purchased item inventory
		if (PurchasedItemCount > 0)
		{
			cweSaveFile.purchasedItemCount = PurchasedItemCount;
			for (int i = 0; i < PurchasedItemCount; i++)
			{
				save::CWE_PurchasedItems[i].mCategory = PurchasedItems[i].Category;
				save::CWE_PurchasedItems[i].mType = PurchasedItems[i].Type;
			}
			PurchasedItemCount = 0;
		}

		for (int i = 0; i < cweSaveFile.purchasedItemCount; i++) {
			if (cweSaveFile.PurchasedItems[i].mCategory > 0) {
				save::CWE_PurchasedItems[i] = cweSaveFile.PurchasedItems[i];
				cweSaveFile.PurchasedItems[i] = { -1,0 };
			}
		}

		// convert the "accessory hats" to the new accessory format
		ITEM_SAVE_INFO* items = (ITEM_SAVE_INFO*)ChaoHatSlots;
		for (size_t j = 0; j < 24; ++j) {
			auto& originalItem = items[j];
			if (originalItem.Type >= 256) {
				const auto accessoryIndex = originalItem.Type - 256;

				ItemSaveInfoBase* pNewInfo = CWE_GetNewItemSaveInfo(ChaoItemCategory_Accessory);
				// if we don't have space to convert, stop the conversion checks
				if (!pNewInfo) break;

				// if it's an invalid id, don't write it
				char id[METADATA_ID_SIZE];
				if (ItemMetadata::Get()->GetID(ChaoItemCategory_Accessory, accessoryIndex, id)) {
					memcpy(pNewInfo->ID, id, sizeof(pNewInfo->ID));
					pNewInfo->IndexID = accessoryIndex;
					pNewInfo->Garden = originalItem.Garden;
					pNewInfo->Position = originalItem.position;
				}

				// even if it does become an invalid id, we omit the original item
				// to not create "mystery no more garden space" issues
				// TOOD: message box could be useful?
				AL_ClearItemSaveInfo(&originalItem);
			}
		}

		for (int i = 0; i < 24; i++)
		{
			//HYPER FRUIT CHECK, dont age hyper fruit
			ChaoObjectData* objData = (ChaoObjectData*)ChaoFruitSlots;
			if (objData[i].Type >= 29 && objData[i].Type <= 32)
				objData[i].Age = 0;

			//reset upgradecounter on egg chao, maybe move to reincarnation later
			if (ChaoSlots[i].data.Type == 1)
				ChaoSlots[i].data.UpgradeCounter = 0;

			GuestChao(ChaoSlots[i].data);
		}

		if (gConfigVal.ToyReset && !AL_IsGarden() && ToyResetTimer <= 0) {
			if (MenuButtons_Held[0] & Buttons_X && 
				MenuButtons_Held[0] & Buttons_A && 
				MenuButtons_Held[0] & Buttons_L) 
			{
				PrintDebug("toys have been reset");
				ToyResetTimer = 120;
				for (int i = 0; i < NB_ALW_KIND * 3; i++) {
					cweSaveFile.cweToyInfo[i].Garden = 0;
				}
			}
		}

		if (gConfigVal.MoreAnimation || gConfigVal.MoreSound) AL_MoreAnimSound();
	}

	struct  Renderer
	{
		void* vtable;
		char field_4[52];
		IDirect3DDevice9* pointerToDevice;
		char field_3C[70];

	};
	DataPointer(Renderer*, dword_1A557C0, 0x1A557C0);

	void SafetyCheckExternalMods() {
		for (const auto& mod : *g_HelperFunctions->Mods) {
			if (!strcmp(mod.Name, "BrightFix")) {
				gConfigVal.BrightFixEnabled = true;
			}
			else if (!strcmp(mod.Name, "HD Toy Models")) {
				MessageBoxA(0, "Please delete/disable \"HD Toy Models\" and use the one provided in the configuration menu for CWE.", "Chao World Extended", 0);
			}
			else if (!strcmp(mod.Name, "Dreamcast Cocoon Color")) {
				MessageBoxA(0, "Please delete/disable \"Dreamcast Cocoon Color\" and use the one provided in the configuration menu for CWE.", "Chao World Extended", 0);
			}
			else if (!strcmp(mod.Name, "SADX Gardens")) {
				std::string sadxinipath = std::string(mod.Folder);
				sadxinipath += "\\mod.ini";
				IniFile* sadxini = new IniFile(sadxinipath.c_str());
				sadxini->setString("", "GitHubRepo", "cweteam/cwe_sadxgardens");
				sadxini->setString("", "GitHubAsset", "SADXGardens.7z");
				sadxini->save(sadxinipath.c_str());
				delete sadxini;
			}
		}
	}
	
	__declspec(dllexport) void Init(const char* path, const HelperFunctions& helperFunctions, uint32_t modIndex)
	{
		if (helperFunctions.Version < ModLoaderVer) {
			char textbuf[128];
			sprintf_s(textbuf, "The current Mod Loader version (%d) is too old, CWE requires at least version %d. Please update the Mod Loader!", helperFunctions.Version, ModLoaderVer);

			MessageBoxA(
				NULL, 
				textbuf,
				"CWE - Modloader version error", 
				MB_ICONWARNING
			);

			return;
		}

		njRandomSeed(time(0));

		DrawChaoWorldShadow = []() {};
		HMODULE DCShadows = GetModuleHandleA("sa2-dc-lighting");
		if (DCShadows) {
			auto ptr = (decltype(DrawChaoWorldShadow))GetProcAddress(DCShadows, "DrawChaoWorldShadow");
			if (ptr) {
				DrawChaoWorldShadow = ptr;
			}
		}

		g_HelperFunctions = &helperFunctions;

		RenderFix_Init(helperFunctions);
		ScanAllMods();

		cwe_device = dword_1A557C0->pointerToDevice;

#ifdef IMGUIDEBUG
		ImGui_Init();
#endif

#ifdef RENDERTARGET
		device->CreateRenderTarget(HorizontalResolution, VerticalResolution, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE, 0, false, &RenderTarget, 0);
		device->CreateDepthStencilSurface(HorizontalResolution, VerticalResolution, D3DFMT_D16,
			D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE, 0, false, &FStaticStencil, nullptr);
		device->CreateTexture(HorizontalResolution, VerticalResolution, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &OutTexture, 0);
		OutTexture->GetSurfaceLevel(0, &OutSurface);
#endif
		const std::string iniPath = std::string(path) + "\\config.ini";
		IniFile* config = new IniFile(iniPath);

		static_assert(sizeof(ChaoData) == 0x800, "ChaoData incorrect size");
		static_assert(sizeof(AL_GENE) == 0xA4, "AL_GENE incorrect size");
		static_assert(offsetof(ChaoDataBase, Gene) == 0x438, "ChaoDataBase DNA incorrect offset");
		static_assert(offsetof(ChaoDataBase, IsInitializedDX) == 0x4DC, "SA2 chaodatabase size incorrect");

		SafetyCheckExternalMods();
		CWE_Patch_Init(config);

		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);

		ClearAllItemSave();
		GlobalSave_Init();

		KCE_Init();

		//DEBUG LENS THING
		//WriteCall((void*)0x0056D696, SetChunkTexIndexPrimaryHook);
		//WriteCall((void*)0x0056D6B8, SetChunkTexIndexPrimaryHook);
		//AL_Lua_Init();
		CWE_Codes_Init(path, config);

		CWE_Fixes();

		//sfx
		WriteData((int*)0x008A6DB8, (int)165);
		WriteData((int*)0x008A6DBC, (int)stru_8A68B0);

		OrthoInit();
		AL_SaveInit();
		AL_Palette_Init();

		DataArray(int, LessonArray, 0x011D2DC8, 21);
		//11 - trumpet
		//12 - tambourine 
		//13 - organ
		//14 - piano

		//classroom thingy
		//LessonArray[5] = 13;
		//LessonArray[14] = 14;
		
		//Main
		gConfigVal.UnusedToys = config->getBool("Chao World Extended", "UnusedToys", true);
		gConfigVal.SpawnOmochao = config->getBool("Chao World Extended", "SpawnOmochao", true);
		gConfigVal.OmochaoParts = config->getBool("Chao World Extended", "OmochaoParts", false);
		gConfigVal.ToyMove = config->getBool("Chao World Extended", "ToyMove", true);
		gConfigVal.NewInventory = config->getBool("Chao World Extended", "NewInventory", true);

		//Easy
		gConfigVal.EmotionDisplay = config->getBool("Easy", "EmotionDisplay", false);

		//Advanced
		gConfigVal.MedalChaoCanMakeColorChao = config->getBool("Advanced", "AdvancedMedalChaocanmakeJewelChao", false);
		gConfigVal.FixMonsterEvo = config->getBool("Advanced", "FixMonsterEvo", false);

		//Detail
		gConfigVal.MoreSound = config->getBool("Detail", "MoreSound", false);
		gConfigVal.MoreAnimation = config->getBool("Detail", "MoreAnimation", false);
		gConfigVal.ClassroomTimerDisplay = config->getBool("Detail", "ClassroomTimerDisplay", true);
		gConfigVal.HDHoodie = config->getBool("Detail", "HDHoodie", false);
		gConfigVal.DoctorChaoInfo = config->getBool("Detail", "DetailDoctorChaogivemoreInformationEN", false);

		//Misc
		gConfigVal.FixHeroSky = config->getBool("Misc", "FixHeroSky", true);
		gConfigVal.NeutSet = config->getBool("Misc", "NeutSet", true);
		gConfigVal.HeroSet = config->getBool("Misc", "HeroSet", true);
		gConfigVal.DarkSet = config->getBool("Misc", "DarkSet", true);
		gConfigVal.TransporterColor = config->getBool("Misc", "TransporterColor", true);
		gConfigVal.ChaoCounter = config->getBool("Misc", "BonusChaoCounter", false);
		gConfigVal.KeepAnimalParts = config->getBool("Misc", "KeepAnimalParts", false);
		gConfigVal.EnergyCap = config->getBool("Misc", "EnergyCap", false);
		gConfigVal.ForceShinyTT = config->getBool("Misc", "ForceShinyTT", false);
		gConfigVal.DisableArmsLegs = config->getBool("Misc", "OtherDisableAnimalPartsArmslegs", false);
		gConfigVal.DisableAllParts = config->getBool("Misc", "OtherDisableAnimalPartsALL", false);
		gConfigVal.BlackMarketShort = config->getBool("Misc", "BlackMarketShort", false);

		//Cheat
		gConfigVal.ToyReset = config->getBool("Cheat", "ToyReset", false);
		gConfigVal.GlobalPriceMultiplier = config->getInt("Cheat", "GlobalPricePercentage", 100) / 100.f;

		//Race
		gConfigVal.WinJewelChaoInJewelRace = config->getBool("Race", "RaceWinJewelChaoinJewelRace", false);

		//day night cycle
		gConfigVal.DayNightCycle = config->getBool("DayNight", "DayNightCycle", true);
		gConfigVal.DayNightCycleNeutralGardenSkybox = config->getBool("DayNight", "DayNightCycleNeutralGardenSkybox", true);
		gConfigVal.DayNightCycleHeroGardenSkybox = config->getBool("DayNight", "DayNightCycleHeroGardenSkybox", true);
		gConfigVal.DayNightCycleDarkGardenSkybox = config->getBool("DayNight", "DayNightCycleDarkGardenSkybox", true);
		gConfigVal.DayNightCycleNeutralGarden = config->getBool("DayNight", "DayNightCycleNeutralGarden", true);
		gConfigVal.DayNightCycleHeroGarden = config->getBool("DayNight", "DayNightCycleHeroGarden", true);
		gConfigVal.DayNightCycleDarkGarden = config->getBool("DayNight", "DayNightCycleDarkGarden", true);
		gConfigVal.DayNightCycleRace = config->getBool("DayNight", "DayNightCycleRace", true);
		gConfigVal.DayNightCycleKarate = config->getBool("DayNight", "DayNightCycleKarate", true);
		gConfigVal.DayNightCycleHourFrame = 60 * config->getInt("DayNight", "DayNightCycleHourFrame", 60);
		gConfigVal.DayNightDebug = config->getBool("DayNight", "DayNightDebug", false);
		gConfigVal.DayNightCheat = config->getBool("DayNight", "DayNightCheat", false);

		if (gConfigVal.DayNightCycleHourFrame <= 0) {
			MessageBoxA(0, "Day Night Cycle's \"In-Game Hour In Real-Life Seconds\" option cannot be set to zero or lower! Temporarily resetting setting to default.", "Chao World Extended", 0);
			gConfigVal.DayNightCycleHourFrame = 60 * 60;
		}

		if (gConfigVal.FixHeroSky) {
			*(NJS_OBJECT*)GetDllData("object_ghero_nk_kumofront_kumofront") = object_ghero_nk_kumofront_kumofront;
			*(NJS_OBJECT*)GetDllData("object_ghero_nk_kumoback_kumoback") = object_ghero_nk_kumoback_kumoback;
		}

		if (gConfigVal.NeutSet) {
			const std::string dstSetUPath = path + std::string("\\gd_PC\\set_chao_neut_u_cwe.bin");
			const std::string dstSetSPath = path + std::string("\\gd_PC\\set_chao_neut_s_cwe.bin");
			helperFunctions.ReplaceFileAtIndex("resource\\gd_PC\\set_chao_neut_u.bin", dstSetUPath.c_str(), modIndex);
			helperFunctions.ReplaceFileAtIndex("resource\\gd_PC\\set_chao_neut_s.bin", dstSetSPath.c_str(), modIndex);
		}

		if (gConfigVal.HeroSet) {
			const std::string dstSetUPath = path + std::string("\\gd_PC\\set_chao_hero_u_cwe.bin");
			const std::string dstSetSPath = path + std::string("\\gd_PC\\set_chao_hero_s_cwe.bin");
			helperFunctions.ReplaceFileAtIndex("resource\\gd_PC\\set_chao_hero_u.bin", dstSetUPath.c_str(), modIndex);
			helperFunctions.ReplaceFileAtIndex("resource\\gd_PC\\set_chao_hero_s.bin", dstSetSPath.c_str(), modIndex);
		}

		if (gConfigVal.DarkSet) {
			const std::string dstSetUPath = path + std::string("\\gd_PC\\set_chao_dark_u_cwe.bin");
			const std::string dstSetSPath = path + std::string("\\gd_PC\\set_chao_dark_s_cwe.bin");
			helperFunctions.ReplaceFileAtIndex("resource\\gd_PC\\set_chao_dark_u.bin", dstSetUPath.c_str(), modIndex);
			helperFunctions.ReplaceFileAtIndex("resource\\gd_PC\\set_chao_dark_s.bin", dstSetSPath.c_str(), modIndex);
		}

		//compatbility fix for animal part texture mods that are compatible with 9.5.2.6
		static NJS_TEXNAME al_mini_parts_tex_texname[280];
		al_mini_parts_tex_texlist.textures = al_mini_parts_tex_texname;

		//axe reincarnation removing animal parts
		if (gConfigVal.KeepAnimalParts) {
			WriteData<7>((char*)0x00551630, (char)0x90);
		}

		AL_DayNight_Init(iniPath, config, helperFunctions);
		AL_MoreAnimSound_Init();

		Animation_Init();

		if (gConfigVal.ToyMove)
			AL_Toy_Moveable_Init();

		BrightFix_Init(path, (BYTE*)g_vs30_main, gConfigVal.DayNightCycle ? (BYTE*)g_ps30_main_daynight : (BYTE*)g_ps30_main);
		
		PaletteFix_Init(cwe_device);

		if (gConfigVal.UnusedToys)
		{
			PrintDebug("Load UnusedToys");

			//unused rattles
			WriteJump((void*)0x55DDE0, ALBHV_Garagara);
		}
		WriteCall((void*)0x0054C9B3, CreateToyHook);
		WriteCall((void*)0x0054D395, CreateToyHook);
		WriteCall((void*)0x0054B8B5, CreateToyHook);

		if (config->getBool("Cheat", "CheatBlackMarket", false))
			WriteJump((void*)0x058C027, BlackMarketDebugHook);

		//shiny jewel colors array
		WriteData((int*)0x0055E8DC, (int)ShinyJewelColors);
		memcpy((char*)0x129844C, patchedColors, 0x3FD); //cwe mixed colors

		RegisterDataFunc(ALAPI_Register);
		PathToModFolder = path;

		Chao_Init();

		alg_kinder_bl_Init();
		alg_kinder_he_Init();
		alg_kinder_co_Init();
		alg_kinder_cl_Init();

		al_race_Init();

		AL_GardenInfo_Init();
		ChaoMain_Init();

		ALO_ObakeHead_Init();
		ALO_Fruit_Init();
		ALO_GrowTree_Init();

		delete config;
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

