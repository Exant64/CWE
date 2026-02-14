#include "stdafx.h"
#include <UsercallFunctionHandler.h>
#include <ninja_functions.h>
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <ALifeSDK_Functions.h>
#include <FunctionHook.h>

#include <api/api_texture.h>

#include <renderfix.h>

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

const int njDrawTexturePtr = 0x0077F510;
void njDrawTexture(int a1, int a2, int a3, int a5) {
    __asm {
        push a5
        mov ecx, a3
        mov edx, a2
        mov eax, a1
        call njDrawTexturePtr
        add esp, 4
    }
}

const int njDrawPolygonPtr = 0x77F7F0;
void njDrawPolygon(int alpha, int count, NJS_POLYGON_VTX* a4) {
    __asm {
        mov eax, alpha
        mov ecx, count
        push a4
        call njDrawPolygonPtr
        add esp, 4
    }
}

static NJS_POINT3 CalcRadiusCenter;
static float CalcRadius;

static int MaxRadius(NJS_CNK_MODEL* pModel) {
    if (!pModel) {
        return 0;
    }

    NJS_POINT3 modelCenter = pModel->center;
    sub_426CC0(_nj_current_matrix_ptr_, &modelCenter, &modelCenter, 0);

    if (CalcRadius < 0) {
        CalcRadiusCenter = modelCenter;
        CalcRadius = pModel->r;
    }
    else if (pModel->r > 0) {
        NJS_POINT3 d = {
            modelCenter.x - CalcRadiusCenter.x,
            modelCenter.y - CalcRadiusCenter.y,
            modelCenter.z - CalcRadiusCenter.z
        };

        float dist = njUnitVector(&d);

        if (dist + pModel->r > CalcRadius) {
            if (dist + CalcRadius <= pModel->r) {
                CalcRadiusCenter = modelCenter;
                CalcRadius = pModel->r;
            }
            else {
                float newRadius = (dist + pModel->r + CalcRadius) * 0.5f;
                const float difference = (newRadius - CalcRadius);
                NJS_POINT3 newCenter = {
                    .x = CalcRadiusCenter.x + d.x * difference,
                    .y = CalcRadiusCenter.y + d.y * difference,
                    .z = CalcRadiusCenter.z + d.z * difference
                };

                CalcRadiusCenter = newCenter;
                CalcRadius = newRadius;
            }
        }
    }

    return 0;
}

static float CalcMaxRadius(NJS_OBJECT* pObj, NJS_POINT3& center) {
    CalcRadius = -1;

    njPushUnitMatrix();
    ProcessChunkModelsWithCallback(pObj, MaxRadius);
    njPopMatrixEx();

    center = CalcRadiusCenter;
    return CalcRadius;
}

static void njDrawTextureHook(NJS_TEXTURE_VTX* vtx, size_t type) {
    if (type <= SA2BAnimal_PurpleChaosDrive) {
        njDrawTexture(4, dword_8AC788[type], (int)vtx, 1);
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
    njDrawTexture(4, 44657589, (int)vtx, 1);

    // border panel
    for (size_t i = 0; i < 4; ++i) {
        vtx[i].col = borderColor.color;
    }
    njDrawTexture(4, 44657590, (int)vtx, 1);

    const Rotation rot = { 0, -4000, 0 };
    const SAlItem item = { 2, type };
    
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
	auto* v3 = (MinimalData2*)tp->Data2.Undefined;
	
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

static void AL_MinimalExecutor_Load_r(char a1, NJS_VECTOR* a2, int a3, void* a4, int a5) {
    if (a1 >= 21 && a1 < 25) {
        ALO_ChaosDriveExecutor_Load(a1 - 21, a2, (NJS_VECTOR*)a4, (ChaoData*)a5);
        return;
    }

    AL_MinimalExecutor_Load(a1, a2, a3, a4, a5);
}

const int sub_48ACD0Ptr = 0x48ACD0;
static void sub_48ACD0(int a1, float a2, float a3, float a4) {
    __asm {
        mov esi, a1
        push a4
        push a3
        push a2
        call sub_48ACD0Ptr
        add esp, 12
    }
}

FunctionPointer(ObjectMaster*, sub_48AAD0, (float posX, float posY, float posZ, int a4, int mode), 0x48AAD0);
static void sub_48ACD0_r(int i, int a1, float a2, float a3, float a4) {
    // the first parameter (i) is "stolen" from the caller function through register edi
    // we use it to only run the animal spawn checks for the first animal
    
    if (i == 0 && njRandom() < gConfigVal.StageAnimalChance) {
        const size_t animalCount = gConfigVal.StageAnimalMinCount + size_t(njRandom() * (gConfigVal.StageAnimalMaxCount - gConfigVal.StageAnimalMinCount + 0.99999f));
        for (size_t i = 0; i < animalCount; ++i) {
            task* pMinimal = sub_48AAD0(a2, a3, a4, 0, 4);
            auto* work = (MinimalData2*)pMinimal->Data2.Undefined;
            
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
static void LoadLevelInit_r();
static void LoadLevelDestroy_r();
static FunctionHook<void> LoadLevelInit_hook(0x43CB10);
static FunctionHook<void> LoadLevelDestroy_hook(0x454CC0);

static void LoadLevelInit_r() {
    for (const auto& load : TexlistLoads) {
        if (std::find(ModAPI_MinimalTexlists.begin(), ModAPI_MinimalTexlists.end(), load.second) == ModAPI_MinimalTexlists.end()) continue;

        LoadTextureList((char*)load.first, load.second);
    }

    LoadTextureList((char*)"CWE_UI", &CWE_UI_TEXLIST);

    LoadLevelInit_hook.Original();
}

static void LoadLevelDestroy_r() {
    for (const auto& load : TexlistLoads) {
        if (std::find(ModAPI_MinimalTexlists.begin(), ModAPI_MinimalTexlists.end(), load.second) == ModAPI_MinimalTexlists.end()) continue;

        njReleaseTexture(load.second);
    }

    njReleaseTexture(&CWE_UI_TEXLIST);

    LoadLevelDestroy_hook.Original();
}

void Minimal_Init() {
    if (!gConfigVal.StageAnimals) return;
        
    LoadLevelInit_hook.Hook(LoadLevelInit_r);
    LoadLevelDestroy_hook.Hook(LoadLevelDestroy_r);

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