#include "stdafx.h"

#include "asmutil.h"
#include "ChaoMain.h"
#include "al_minimal.h"
#include "ninja_functions.h"
#include "AL_ModAPI.h"
#include "al_save.h"
#include "alo_special.h"
#include "al_stage.h"
#include <save/save_item.h>
#include <api/api_texture.h>
#include <save/save_item.h>

const HelperFunctions* g_HelperFunctions = nullptr;

IDirect3DDevice9* cwe_device;

uint32_t CWE_ModIndex;

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

ASM_FUNC void AL_LoadTex(const char* a2, NJS_TEXLIST* texlist, int a1) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( ebx, ASM_ESP(1+0 +1) ); // lev
    ASM_PUSH(      ASM_ESP(2+0 +1) ); // pTexlist
    ASM_MOVE( eax, ASM_ESP(3+1 +1) ); // filename

    // call
    ASM_CALL_R( edx, 0x00530280 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // pull regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

VoidFunc(Load_al_palette, 0x00534350);

//i hooked the print call before, now i hook the first loadtexlist call
extern "C" __declspec(dllexport) void ChaoMain_Constructor_Hook()
{
	for(const auto& load: TexlistLoads) {
		AL_LoadTex(load.first, load.second, 0);
	}

	AL_LoadTex("AL_MINI_PARTS_TEX", (NJS_TEXLIST*)0x01366AE4, 0);
}

VoidFunc(sub_52E400, 0x52E400);
void ChaoMain_subprgmanager_Hook()  //use this to do something on ChaoWorld loading -- NOT WORKING, SINCE SAVE FILE ISNT LOADED YET
{
	sub_52E400();
	//ReadCWESaveFile();
}

void ChaoMain_Init() {	
	WriteCall((void*)0x0052B2DA, ChaoMain_subprgmanager_Hook);

	WriteCall((void*)0x0052ABEE, ChaoMain_Constructor_Hook);
}