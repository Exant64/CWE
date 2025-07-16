#include "stdafx.h"

#include "ChaoMain.h"
#include "al_minimal.h"
#include "ninja_functions.h"
#include "AL_ModAPI.h"
#include "al_save.h"
#include "alo_special.h"
#include "al_stage.h"
#include <save/save_item.h>
#include <api/api_texture.h>

const HelperFunctions* g_HelperFunctions = nullptr;
void(__cdecl* DrawChaoWorldShadow)() = nullptr;

IDirect3DDevice9* cwe_device;

ConfigValues gConfigVal;

int HyperSwimFruitID;
int HyperFlyFruitID;
int HyperRunFruitID;
int HyperPowerFruitID;
int ShinyFruitID;
int CakeSliceID;
int OrangeID;
int BeeID;
int MirrorID;

const int LoadChaoTexlistPtr = 0x00530280;
void LoadChaoTexlist(const char* a2, NJS_TEXLIST* texlist, int a1)
{
	__asm
	{
		mov ebx, a2
		mov eax, a1
		push texlist
		call LoadChaoTexlistPtr
		add esp, 4
	}
}

VoidFunc(Load_al_palette, 0x00534350);

int CWE_Loaded = 0;
void HookRegisterCWEData()
{
	PrintDebug("ChaoMain Prolog\n");
	if (!CWE_Loaded)
	{

		RegisterCWEData(&cweAPI);
		CWE_Loaded = 1;
	}
}

//i hooked the print call before, now i hook the first loadtexlist call
extern "C" __declspec(dllexport) void ChaoMain_Constructor_Hook()
{
	for(const auto& load: TexlistLoads) {
		LoadChaoTexlist(load.first, load.second, 0);
	}

	LoadChaoTexlist("AL_MINI_PARTS_TEX", (NJS_TEXLIST*)0x01366AE4, 0);
}

VoidFunc(sub_52E400, 0x52E400);
void ChaoMain_subprgmanager_Hook()  //use this to do something on ChaoWorld loading -- NOT WORKING, SINCE SAVE FILE ISNT LOADED YET
{
	sub_52E400();
	//ReadCWESaveFile();
}

void ChaoMain_Init() {	
	WriteCall((void*)0x0052B2DA, ChaoMain_subprgmanager_Hook);

	WriteCall((void*)0x052AB15, HookRegisterCWEData);
	WriteCall((void*)0x0052ABEE, ChaoMain_Constructor_Hook);
}