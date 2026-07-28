#include "stdafx.h"
#include <UsercallFunctionHandler.h>
#include <ninja_functions.h>
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <FunctionHook.h>

#include <api/api_texture.h>

#include <renderfix.h>
#include <asmutil.h>
#include <util.h>
#include <playsound.h>

struct __declspec(align(8)) MinimalData2
{
	int mode;
	int ID;
	int table;
	int angY;
	float velY;
	int someTimer;
	float frame;
	int lastGravityMode;
};

UsercallFuncVoid(RenderAnimal, (task* tp), (tp), 0x0048A2D0, rEAX);

#pragma pack(push, 8)
struct AnimalInvEntry
{
    int type;
    int someTimer;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) AnimalInv
{
    AnimalInvEntry OtherEntries[10];
    AnimalInvEntry ActualEntries[10];
    int Count;
    float field_A4;
    int field_A8;
    float field_AC;
    int field_B0;
};
#pragma pack(pop)

FastcallFunctionPointer(AnimalInv*, GetAnimalInv, (int a1), 0x487260);
DataArray(int, dword_8AC788, 0x8AC788, 1);

ASM_FUNC void njDrawTexture(const NJS_TEXTURE_VTX* polygon, Int count, Int tex, Int flag) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0 +1) ); // flag            : 0+1
    ASM_MOVE( edx, ASM_ESP(3+1 +1) ); // tex             : 1
    ASM_MOVE( eax, ASM_ESP(2+1 +1) ); // count           : 1
    ASM_MOVE( ecx, ASM_ESP(1+1 +1) ); // polygon         : 1

    // call
    ASM_CALL_R( ebx, 0x0077F510 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // pull regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

static void njDrawTextureHook(NJS_TEXTURE_VTX* vtx, size_t type) {
    if (type <= SA2BAnimal_PurpleChaosDrive) {
        njDrawTexture(vtx, 4, dword_8AC788[type], 1);
        return;
    }

    NJS_COLOR insidePanelColor;
    NJS_COLOR borderColor;

    switch (ModAPI_MinimalInfluence[type]) {
        case COLOR_SWIM:
            borderColor.argb.r = 238;
            borderColor.argb.g = 214;
            borderColor.argb.b = 82;
            borderColor.argb.a = 255;

            insidePanelColor = {
               .argb = {
                   .b = 115,
                   .g = 255,
                   .r = 246,
                   .a = 255,
               }
            };
            break;    
        case COLOR_FLY:
            borderColor.argb.r = 189;
            borderColor.argb.g = 64;
            borderColor.argb.b = 246;
            borderColor.argb.a = 255;
            
            insidePanelColor = {
               .argb = {
                   .b = 255,
                   .g = 155,
                   .r = 172,
                   .a = 255,
               }
            };
            break;
        case COLOR_RUN:
            borderColor.argb.r = 8;
            borderColor.argb.g = 210;
            borderColor.argb.b = 98;
            borderColor.argb.a = 255;
            
            insidePanelColor = {
               .argb = {
                   .b = 164,
                   .g = 255,
                   .r = 32,
                   .a = 255,
               }
            };
            break;
        case COLOR_POWER:
            borderColor.argb.r = 246;
            borderColor.argb.g = 68;
            borderColor.argb.b = 131;
            borderColor.argb.a = 255;

            insidePanelColor = {
               .argb = {
                   .b = 180,
                   .g = 180,
                   .r = 255,
                   .a = 255,
               }
            };
            break;
        default:
            borderColor.argb.r = 24;
            borderColor.argb.g = 149;
            borderColor.argb.b = 255;
            borderColor.argb.a = 255;

            insidePanelColor = {
                .argb = {
                    .b = 255,
                    .g = 255,
                    .r = 0,
                    .a = 255,
                }
            };
            break;
    }

    // inside panel
    for (size_t i = 0; i < 4; ++i) {
        vtx[i].col = insidePanelColor.color;
        vtx[i].z -= 0.75f;
    }
    njDrawTexture(vtx, 4, 44657589, 1);

    // border panel
    for (size_t i = 0; i < 4; ++i) {
        vtx[i].col = borderColor.color;
    }
    njDrawTexture(vtx, 4, 44657590, 1);

    const Angle3 rot = { 0, -4000, 0 };
    const SAlItemCwe item = { 2, type };
    
    const float scale = 1.65f;
    NJS_POINT3 center;
    float rad = CalcMaxRadius(ModAPI_MinimalModels[type], center);
    if (rad == 0) {
        center.y = -1;
        rad = 1.0f / 0.5f;
    }

    DrawItem(0.5f * (vtx[0].x + vtx[3].x), 0.5f * (vtx[0].y + vtx[3].y) + (1 / scale) * center.y * ((vtx[3].y - vtx[0].y) / 2) / rad - 5, scale / rad, rot, item);
}

static void njDrawTextureGetType1(int a1, int a2, int a3, AnimalInvEntry* entry, int a5) {
    njDrawTextureHook((NJS_TEXTURE_VTX*)a3, entry->type);
}

static void __declspec(naked) njDrawTextureHookPaused()
{
    __asm
    {
        push[esp + 04h] // a5
        push ebx
        push ecx // a3
        push edx // a2
        push eax // a1

        // Call your __cdecl function here:
        call njDrawTextureGetType1

        pop eax // a1
        pop edx // a2
        pop ecx // a3
        pop ebx
        add esp, 4 // a5
        retn
    }
}

static void __declspec(naked) njDrawTextureHook1()
{
    __asm
    {
        push [esp + 04h] // a5
        push [esp + 08h + 0xE0 - 0xC0]
        push ecx // a3
        push edx // a2
        push eax // a1

        // Call your __cdecl function here:
        call njDrawTextureGetType1

        pop eax // a1
        pop edx // a2
        pop ecx // a3
        add esp, 8 // a5
        retn
    }
}

static void njDrawTextureGetType2(int a1, int a2, int a3, int otherEntryIndex, int a5) {
    njDrawTextureHook((NJS_TEXTURE_VTX*)a3, GetAnimalInv(0)->OtherEntries[otherEntryIndex].type);
}

static void __declspec(naked) njDrawTextureHook2()
{
    __asm
    {
        push[esp + 04h] // a5
        push edi
        push ecx // a3
        push edx // a2
        push eax // a1

        // Call your __cdecl function here:
        call njDrawTextureGetType2

        pop eax // a1
        pop edx // a2
        pop ecx // a3
        pop edi
        add esp, 4 // a5
        retn
    }
}

static void RenderAnimal_r(task* tp) {
	const float someFloat = *(float*)0xB18F54;
	auto* v3 = (MinimalData2*)tp->awp;
	
	if (v3->table > SA2BAnimal_PurpleChaosDrive) {
		const size_t id = v3->table;

		njScale(NULL, someFloat, someFloat, someFloat);
		if (*(int*)0x1945E08 != 3) {
			njRotateZ(NULL, 0x8000);
		}

        g_HelperFunctions->PushInterpolationFix();
		njSetTexture(ModAPI_MinimalTexlists[id]);
		RenderFixBackwardsCompatibilityDrawMotion(ModAPI_MinimalModels[id], ModAPI_MinimalMotion1[id], v3->frame);
        g_HelperFunctions->PopInterpolationFix();

		return;
	}

	RenderAnimal.Original(tp);
}

static void AL_MinimalExecutor_Load_r(char a1, NJS_VECTOR* a2, int a3, void* a4, ITEM_SAVE_INFO* a5) {
    if (a1 >= 21 && a1 < 25) {
        ALO_ChaosDriveCreate(a1 - 21, a2, (NJS_VECTOR*)a4, a5);
        return;
    }

    AL_MinimalCreate(a1, a2, a3, a4, a5);
}

static ASM_FUNC void sub_48ACD0(int a1, float a2, float a3, float a4) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0+1) ); // a4
    ASM_PUSH(      ASM_ESP(3+1+1) ); // a3
    ASM_PUSH(      ASM_ESP(2+2+1) ); // a2
    ASM_MOVE( esi, ASM_ESP(1+3+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x48ACD0 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

FunctionPointer(task*, sub_48AAD0, (float posX, float posY, float posZ, int a4, int mode), 0x48AAD0);
static void sub_48ACD0_r(int i, int a1, float a2, float a3, float a4) {
    // the first parameter (i) is "stolen" from the caller function through register edi
    // we use it to only run the animal spawn checks for the first animal
    
    if (i == 0 && njRandom() < gConfigVal.StageAnimalChance) {
        const size_t animalCount = gConfigVal.StageAnimalMinCount + size_t(njRandom() * (gConfigVal.StageAnimalMaxCount - gConfigVal.StageAnimalMinCount + 0.99999f));
        for (size_t i = 0; i < animalCount; ++i) {
            task* pMinimal = sub_48AAD0(a2, a3, a4, 0, 4);
            auto* work = (MinimalData2*)pMinimal->awp;
            
            const int startIndex = gConfigVal.StageAnimalIncludeSADX ? (SA2BAnimal_PurpleChaosDrive + 1) : 40;
            work->table = startIndex + int(njRandom() * (ModAPI_MinimalInfluence.size() - startIndex));
        }

        return;
    }

    sub_48ACD0(a1,a2,a3,a4);
}

static void __declspec(naked) sub_48ACD0_hook()
{
    __asm
    {
        push[esp + 0Ch] // a4
        push[esp + 0Ch] // a3
        push[esp + 0Ch] // a2
        push esi // a1
        push edi

        // Call your __cdecl function here:
        call sub_48ACD0_r

        pop edi
        pop esi // a1
        add esp, 4 // a2
        add esp, 4 // a3
        add esp, 4 // a4
        retn
    }
}

// hooks stolen from chao partner
static void InitLevelThings_r();
static void LoadLevelDestroy_r();
static FunctionHook<void> InitLevelThings_hook(0x439610);
static FunctionHook<void> LoadLevelDestroy_hook(0x454CC0);

static void InitLevelThings_r() {
    if(ssStageNumber != STAGE_CHAOWORLD) {
        for (const auto& load : TexlistLoads) {
            if (std::find(ModAPI_MinimalTexlists.begin(), ModAPI_MinimalTexlists.end(), load.second) == ModAPI_MinimalTexlists.end()) continue;

            texLoadTexturePvmFile((char*)load.first, load.second);
        }

        texLoadTexturePvmFile((char*)"CWE_UI", &CWE_UI_TEXLIST);
    }

    InitLevelThings_hook.Original();
}

static void LoadLevelDestroy_r() {
    if(ssStageNumber != STAGE_CHAOWORLD) {
        for (const auto& load : TexlistLoads) {
            if (std::find(ModAPI_MinimalTexlists.begin(), ModAPI_MinimalTexlists.end(), load.second) == ModAPI_MinimalTexlists.end()) continue;

            njReleaseTexture(load.second);
        }

        njReleaseTexture(&CWE_UI_TEXLIST);
    }

    LoadLevelDestroy_hook.Original();
}

static void AnimalPickupSoundHook_r(int a1, NJS_VECTOR *a2, int a3, int a4, int a5, int animalID) {
    if (animalID >= 21 && animalID < 25) {
        SE_CallV2(TONE(8, 0x11), a3, a4, a5, a2);
        return;
    }
    
    SE_CallV2(TONE(8, 0xF), a3, a4, a5, a2);
}

static void __declspec(naked) AnimalPickupSoundHook()
{
        __asm
        {
                push ebx
                push [esp + 0Ch] // a5
                push [esp + 0Ch] // a4
                push [esp + 0Ch] // a3
                push esi // a2
                push edi // a1

                // Call your __cdecl function here:
                call AnimalPickupSoundHook_r

                pop edi // a1
                pop esi // a2
                add esp, 4 // a3
                add esp, 4 // a4
                add esp, 4 // a5
                add esp, 4 
                retn
        }
}

void Minimal_Init() {
    if (!gConfigVal.StageAnimals) return;
        
    InitLevelThings_hook.Hook(InitLevelThings_r);
    LoadLevelDestroy_hook.Hook(LoadLevelDestroy_r);

    // sound hook to make custom animals not emit chaos drive sound
    WriteCall((void*)0x00487375, AnimalPickupSoundHook);

    // minimalcreatemanager hooks to remove safety check
    WriteData<5>((char*)0x0054942F, (char)0x90);
    WriteCall((void*)0x0054944E, AL_MinimalExecutor_Load_r);

    // second chaobox animal spawn call
    WriteCall((void*)0x6DE711, sub_48ACD0_hook);

    // kills the completely unseeable njDrawPolygon calls left in the animal inventory display
    // cuz it hides our stuff
    WriteCall((void*)0x0048789B, nullsub_1);
    WriteCall((void*)0x00487BBA, nullsub_1);

    // the animal inventory display, separate hooks because different ways of retrieving the type of the animal
    WriteCall((void*)0x00487698, njDrawTextureHookPaused);
    WriteCall((void*)0x004879B8, njDrawTextureHook1);
    WriteCall((void*)0x00487CE3, njDrawTextureHook2);
    
    // stage animal rendering
	RenderAnimal.Hook(RenderAnimal_r);
}