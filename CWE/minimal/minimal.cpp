#include "stdafx.h"
#include <UsercallFunctionHandler.h>
#include <ninja_functions.h>
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <ALifeSDK_Functions.h>

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

const int sub_7808B0Ptr = 0x7808B0;
void sub_7808B0(int a1, float a2, NJS_MOTION* a3) {
	__asm {
		mov ecx, a3
		push a2
		push a1
		call sub_7808B0Ptr
		add esp, 8
	}
}
FunctionPointer(void, njCnkMotion, (NJS_OBJECT* a1, NJS_MOTION* a2, float a3), 0x782780);

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

static void njDrawTextureHook(NJS_TEXTURE_VTX* vtx, const size_t type) {
    if (type <= SA2BAnimal_PurpleChaosDrive) {
        njDrawTexture(4, dword_8AC788[type], (int)vtx, 1);
        return;
    }

    NJS_COLOR insidePanelColor;

    switch (ModAPI_MinimalInfluence[type]) {
        case COLOR_SWIM:
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

    for (size_t i = 0; i < 4; ++i) {
        vtx[i].col = insidePanelColor.color;
    }

    njDrawTexture(4, 44657587, (int)vtx, 1);
    Rotation none = { 0, -4000, 0 };
    SAlItem item = { 2, type };
    
    DrawItem(0.5f * (vtx[0].x + vtx[3].x), vtx[0].y + 5, 0.7f, none, item);
}

void sub_4874F0(int a1, float a1a, float a2, float a3)
{
    AnimalInv* v4; // eax
    AnimalInv* v5; // ebx
    int v6; // edi
    AnimalInvEntry* v7; // ebx
    double i; // st7
    int v9; // edx
    double v10; // st3
    double v11; // rt2
    double v12; // st0
    double v13; // st1
    double v14; // st7
    int v15; // edi
    double v16; // st6
    double v17; // st5
    AnimalInvEntry* v18; // eax
    int v19; // eax
    double v20; // rt0
    double v21; // st5
    double v22; // st5
    int v23; // edx
    double v24; // st7
    int v25; // ecx
    int v26; // eax
    signed int v27; // edi
    double v28; // st6
    double v29; // st7
    int v30; // ecx
    int v31; // edx
    int v32; // [esp+10h] [ebp-C8h]
    float v33; // [esp+10h] [ebp-C8h]
    float v34; // [esp+10h] [ebp-C8h]
    float v35; // [esp+10h] [ebp-C8h]
    int v36; // [esp+10h] [ebp-C8h]
    int v37; // [esp+14h] [ebp-C4h]
    float v38; // [esp+14h] [ebp-C4h]
    float v39; // [esp+14h] [ebp-C4h]
    float v40; // [esp+14h] [ebp-C4h]
    float v41; // [esp+18h] [ebp-C0h]
    AnimalInvEntry* v42; // [esp+18h] [ebp-C0h]
    float v43; // [esp+18h] [ebp-C0h]
    float v44; // [esp+1Ch] [ebp-BCh]
    float v45; // [esp+1Ch] [ebp-BCh]
    float v46; // [esp+1Ch] [ebp-BCh]
    float v47; // [esp+1Ch] [ebp-BCh]
    float v48; // [esp+20h] [ebp-B8h]
    float v49; // [esp+20h] [ebp-B8h]
    double v50; // [esp+20h] [ebp-B8h]
    float v51; // [esp+20h] [ebp-B8h]
    Float v52; // [esp+28h] [ebp-B0h]
    float v53; // [esp+28h] [ebp-B0h]
    float v54; // [esp+28h] [ebp-B0h]
    double v55; // [esp+28h] [ebp-B0h]
    NJS_TEXTURE_VTX a3a[4]; // [esp+30h] [ebp-A8h] BYREF
    NJS_POLYGON_VTX a4[4]; // [esp+90h] [ebp-48h] BYREF
    double v58; // [esp+D0h] [ebp-8h]

    v4 = GetAnimalInv(a1);
    v5 = v4;
    if (GameState == GameStates_Pause)
    {
        v6 = v4->Count - 1;
        if (v6 >= 0)
        {
            v32 = v6 + 0xA;
            v7 = &v4->ActualEntries[v6];
            v41 = a3 * 0.5 * 0.05000000074505806;
            for (i = v41; ; i = v41)
            {
                v9 = dword_8AC788[v7->type];
                a3a[3].col = -1;
                a3a[2].col = -1;
                a3a[1].col = -1;
                a3a[0].col = -1;
                v10 = (1.09375 - (double)(v32 - 8) * 0.0625) * a3 + a1a;
                v11 = HorizontalResolution / 640.0 / (VerticalResolution / 480.0) - 1.0;
                v48 = v10 - (v10 - 320.0) * v11;
                v12 = i + v48;
                v13 = v12 - 320.0;
                a3a[1].x = v48 - i + v11 * v13;
                a3a[0].x = a3a[1].x;
                a3a[3].x = v12 + v11 * v13;
                a3a[2].x = a3a[3].x;
                a3a[2].y = a2 - v41;
                a3a[0].y = a3a[2].y;
                a3a[3].y = a2 + v41;
                a3a[1].y = a3a[3].y;
                a3a[3].z = (double)v32 * 0.001000000047497451 + 0.9800000190734863;
                a3a[2].z = a3a[3].z;
                a3a[1].z = a3a[3].z;
                a3a[0].z = a3a[3].z;
                a3a[0].u = 0.0;
                a3a[0].v = 0.0;
                a3a[1].u = 0.0;
                a3a[2].v = 0.0;
                a3a[1].v = 1.0;
                a3a[2].u = 1.0;
                a3a[3].u = 1.0;
                a3a[3].v = 1.0;
                njDrawTextureHook(a3a, v7->type);
                --v6;
                --v7;
                --v32;
                if (v6 < 0)
                {
                    break;
                }
            }
        }
    }
    else if (CurrentLevel != 90)
    {
        v14 = a3;
        v15 = v4->Count - 1;
        v16 = a2;
        v17 = 320.0;
        if (v15 >= 0)
        {
            v37 = v15 + 10;
            v18 = &v4->ActualEntries[v15];
            v42 = &v5->ActualEntries[v15];
            do
            {
                v19 = v18->someTimer;
                if (v19 < 30)
                {
                    v44 = (1.09375 - ((double)(v37 - 8) + v5->field_AC) * (v5->field_A4 * 0.0625)) * v14 + a1a;
                    v33 = *(float*)0xB5D148;
                    if (v19 > 0)
                    {
                        v33 = (double)v19 * 0.0008333333535119891 + 0.05000000074505806;
                    }
                    v20 = v17;
                    a4[3].col = -1;
                    a4[2].col = -1;
                    a4[1].col = -1;
                    v49 = v14 * 0.5 * v33;
                    a4[0].col = -1;
                    v34 = v49 - 2.0;
                    v21 = HorizontalResolution / 640.0 / (VerticalResolution / 480.0) - 1.0;
                    v45 = v44 - (v44 - v20) * v21;
                    v50 = v45 - v20;
                    v22 = v21 * v50;
                    a4[1].x = v45 - v34 + v22 + *(float*)0x1A5A334;
                    a4[0].x = a4[1].x;
                    a4[3].x = *(float*)0x1A5A334 + v22 + v45 + v34;
                    a4[2].x = a4[3].x;
                    a4[2].y = v16 - v34;
                    a4[0].y = a4[2].y;
                    a4[3].y = v34 + v16;
                    a4[1].y = a4[3].y;
                    v52 = (double)v37 * 0.001000000047497451 + 0.9899999761581421;
                    a4[3].z = v52;
                    a4[2].z = v52;
                    a4[1].z = v52;
                    a4[0].z = v52;
                    njDrawPolygon(0, 4, a4);
                    v35 = v34 + 2.0;
                    v23 = dword_8AC788[v42->type];
                    a3a[3].col = -1;
                    a3a[2].col = -1;
                    a3a[1].col = -1;
                    a3a[0].col = -1;
                    v24 = (HorizontalResolution / 640.0 / (VerticalResolution / 480.0) - 1.0) * v50;
                    a3a[1].x = v45 - v35 + v24 + *(float*)0x1A5A334;
                    a3a[0].x = a3a[1].x;
                    a3a[3].x = *(float*)0x1A5A334 + v24 + v45 + v35;
                    a3a[2].x = a3a[3].x;
                    a3a[2].y = a2 - v35;
                    a3a[0].y = a3a[2].y;
                    a3a[3].y = v35 + a2;
                    a3a[1].y = a3a[3].y;
                    a3a[3].z = v52;
                    a3a[2].z = v52;
                    a3a[1].z = v52;
                    a3a[0].z = v52;
                    a3a[0].u = 0.0;
                    a3a[0].v = 0.0;
                    a3a[1].u = 0.0;
                    a3a[1].v = 1.0;
                    a3a[2].u = 1.0;
                    a3a[3].u = 1.0;
                    a3a[3].v = 1.0;
                    a3a[2].v = 0.0;
                    njDrawTextureHook(a3a, v42->type);
                    v14 = a3;
                    v16 = a2;
                    v17 = 320.0;
                }
                --v37;
                --v15;
                v18 = --v42;
            } while (v15 >= 0);
        }
        v25 = 9;
        v36 = 9;
        while (1)
        {
            v26 = v5->OtherEntries[v25].someTimer;
            if (v26 <= 0)
            {
                break;
            }
            v27 = 0xD400 - 0x300 * v26;
            v43 = (1.09375 - ((double)(v25 - 8) + v5->field_AC) * (v5->field_A4 * 0.0625)) * v14 + a1a;
            v51 = v16 + (double)(60 - v26) * (v14 * 0.006250000093132257);
            v53 = v14 * 0.5 * 1.414213538169861 * 0.05000000074505806;
            v38 = njSin(v27) * v53;
            v39 = v38 - 2.0;
            v54 = njCos(v27) * v53;
            v46 = v54 - 2.0;
            v55 = v43 - 320.0;
            v28 = v55 * (HorizontalResolution / 640.0 / (VerticalResolution / 480.0) - 1.0);
            a4[0].x = v39 + v43 + v28;
            a4[1].x = v43 - v46 + v28;
            a4[2].x = v46 + v43 + v28;
            a4[3].x = v28 + v43 - v39;
            a4[0].y = v51 - v46;
            a4[1].y = v51 - v39;
            a4[3].col = -1;
            a4[2].col = -1;
            a4[2].y = v39 + v51;
            a4[1].col = -1;
            a4[0].col = -1;
            a4[3].y = v46 + v51;
            v58 = (double)v36 * 0.001000000047497451;
            a4[3].z = v58 + 0.8999999761581421;
            a4[2].z = a4[3].z;
            a4[1].z = a4[3].z;
            a4[0].z = a4[3].z;
            njDrawPolygon(0, 4, a4);
            v40 = v39 + 2.0;
            v47 = v46 + 2.0;
            v29 = (HorizontalResolution / 640.0 / (VerticalResolution / 480.0) - 1.0) * v55;
            a3a[0].x = v40 + v43 + v29;
            a3a[0].y = v51 - v47;
            a3a[1].x = v43 - v47 + v29;
            a3a[1].y = v51 - v40;
            a3a[2].x = v47 + v43 + v29;
            a3a[2].y = v40 + v51;
            a3a[3].x = v29 + v43 - v40;
            a3a[3].y = v47 + v51;
            a3a[3].z = v58 + 0.9800000190734863;
            a3a[2].z = a3a[3].z;
            a3a[1].z = a3a[3].z;
            v30 = v5->OtherEntries[v36].type;
            a3a[0].z = a3a[3].z;
            v31 = dword_8AC788[v30];
            a3a[0].u = 0.0;
            a3a[0].v = 0.0;
            a3a[1].u = 0.0;
            a3a[3].col = -1;
            a3a[1].v = 1.0;
            a3a[2].col = -1;
            a3a[2].u = 1.0;
            a3a[1].col = -1;
            a3a[3].u = 1.0;
            a3a[0].col = -1;
            a3a[3].v = 1.0;
            a3a[2].v = 0.0;
            njDrawTextureHook(a3a, v30);
            if (--v36 < 0)
            {
                break;
            }
            v14 = a3;
            v25 = v36;
            v16 = a2;
        }
    }
}

static void __declspec(naked) sub_4874F0Hook()
{
    __asm
    {
        push[esp + 0Ch] // a3
        push[esp + 0Ch] // a2
        push[esp + 0Ch] // a1a
        push ecx // a1

        // Call your __cdecl function here:
        call sub_4874F0

        pop ecx // a1
        add esp, 4 // a1a
        add esp, 4 // a2
        add esp, 4 // a3
        retn
    }
}

static void RenderAnimal_r(task* tp) {
	const float someFloat = *(float*)0xB18F54;
	auto* v3 = (MinimalData2*)tp->Data2.Undefined;
	
	if (true || v3->table > SA2BAnimal_PurpleChaosDrive) {
		v3->table = SA2BAnimal_PurpleChaosDrive + SADXAnimal_Skunk + 2;
		const size_t id = v3->table;

		njScale(NULL, someFloat, someFloat, someFloat);
		if (*(int*)0x1945E08 != 3) {
			njRotateZ(NULL, 0x8000);
		}
		static bool test = false;
		if (!test) {
			RegisterCWEData(&cweAPI);
			for (const auto& load : TexlistLoads) {
				LoadChaoTexlist(load.first, load.second, 0);
			}
			LoadChaoTexlist("al_minimal_tex", &al_minimal_texlist, 0);

			test = true;
		}

		njSetTexture(ModAPI_MinimalTexlists[id]);
		njCnkMotion(ModAPI_MinimalModels[id], ModAPI_MinimalMotion1[id], v3->frame);

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

void Minimal_Init() {
    // minimalcreatemanager hooks
    WriteData<5>((char*)0x0054942F, (char)0x90);
    WriteCall((void*)0x0054944E, AL_MinimalExecutor_Load_r);

    WriteJump((void*)0x4874F0, sub_4874F0Hook);
	//WriteCall((void*)0x00487CE3, nullsub_1);
	//WriteCall((void*)0x00487CE3, nullsub_1);
	RenderAnimal.Hook(RenderAnimal_r);
}