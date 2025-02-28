#include "stdafx.h"

#include "ninja_functions.h"
#include "alo_fruit.h"
#include "AL_ModAPI.h"
#include "al_sandhole.h"
#include "ALifeSDK_Functions.h"
#include <vector>
#include "ChaoMain.h"
#include "al_modelcontainer.h"

#include "al_behavior/al_intention.h"
#include <api/api_tree.h>
#include <al_daynight.h>

struct FRUIT_INFO
{
    float growth;
    float AimGrowth;
    int SwingCounter;
    NJS_POINT3 pos;
    int angx;
    int angy;
};

#pragma pack(push, 8)
struct __declspec(align(4)) ALO_GrowTreeExecutor_Data
{
    EntityData1 entityData;
    char kind;
    char state;
    __int16 field_32;
    float growth;
    int life;
    int water;
    int pos_num;
    FRUIT_INFO fruit[3];
    float scale;
    float ScaleSpd;
    int ThirstyFlag;
    int WaterFlag;
    int LeafRotAng;
    float LeafWidth;
    int LeafDispNum;
    int FruitRotAng;
    float FruitWidth;
    int FruitDispNum;
    int SwingFlag;
    int SwingCountFlag;
    float SwingDist;
    int RollAngle;
    int TouchAngle;
    int ChaoSwingPhase;
    int RollPhase;
    int garden;
    void* pMySaveInfo;
    NJS_TEXLIST* texlist;
    NJS_OBJECT* pLocalObject;
    NJS_OBJECT* pShadowObject;
    NJS_OBJECT* pCopyObject;
};
#pragma pack(pop)

enum eTREE_STATE
{
    TREE_ST_PLANT = 0x0,
    TREE_ST_SEED = 0x1,
    TREE_ST_SAPLING = 0x2,
    TREE_ST_ADULT = 0x3,
    TREE_ST_DEAD = 0x4,
    TREE_ST_LOCAL = 0x5,
};


DataArray(int, dword_12E8AEC, 0x12E8AEC,10);
DataArray(int, dword_13291B4, 0x13291B4, 10);
void __cdecl sub_547440(ALO_GrowTreeExecutor_Data* a1, NJS_OBJECT* a2)
{
    double v7; // st7
    int v9; // edx
    FRUIT_INFO* v11; // edx
    double v14; // st7
    float v21; // [esp+58h] [ebp+8h]

    while (1)
    {
        njSetTexture(a1->texlist);
        njPushMatrixEx();
        njTranslateEx((NJS_VECTOR*)a2->pos);
        RotateZ(a2->ang[2]);
        RotateY(a2->ang[1]);
        RotateX(a2->ang[0]);
        if (!a2->model)
        {
            v9 = a1->FruitDispNum;
            v21 = a1->fruit[v9].growth;
            v11 = &a1->fruit[v9];
            if (v21 <= 1)
            {
                if (v21 <= 0)
                {
                LABEL_33:
                    ++a1->FruitDispNum;
                    goto LABEL_34;
                }
            }
            else
            {
                v21 = 1;
            }
            if (v11->growth <= 0.9f)
            {
                v14 = njSin(a1->FruitRotAng + dword_13291B4[a1->FruitDispNum]) * (a1->FruitWidth * 0.2f);
            }
            else
            {
                //njPushMatrixEx();
                //v13[3] = _nj_current_matrix_ptr_[3];
                //v13[4] = _nj_current_matrix_ptr_[7];
                //v13[5] = _nj_current_matrix_ptr_[11];
                //njPopMatrixEx();
                v14 = njSin(a1->FruitRotAng + dword_13291B4[a1->FruitDispNum]) * a1->FruitWidth;
            }
            RotateX(NJM_DEG_ANG(v14));
            njScale(NULL, v21, v21, v21);
            njTranslate(NULL, 0, -1.8f, 0);
           
            switch (a1->garden)
            {
            case 1:
            case 2:
            case 3:
                ObjectRegistry::DrawModel(ChaoItemCategory_Fruit, a1->garden - 1);
                break;
            default:
                int lookup = ModAPI_TreeEntries[a1->kind].FruitIDs[v9];
                ObjectRegistry::DrawModel(ChaoItemCategory_Fruit, lookup);
                break;
            }
            goto LABEL_33;
        }
        v7 = njSin(a1->LeafRotAng + dword_12E8AEC[a1->LeafDispNum]) * a1->LeafWidth;
        RotateX(NJM_DEG_ANG(v7));
        chCnkDrawModel(a2->chunkmodel);
        ++a1->LeafDispNum;
    LABEL_34:
        if (a2->child)
        {
            sub_547440(a1, a2->child);
        }
        njPopMatrixEx();

        if (!a2->sibling)
            break;
        
        a2 = a2->sibling;
    }
}

int CurrentFruitSlot = 0;
FunctionPointer(void, sub_546670, (NJS_VECTOR* a1, ALO_GrowTreeExecutor_Data* a2, int a3), 0x546670);
void __cdecl sub_546670Hook(NJS_VECTOR* a1, ALO_GrowTreeExecutor_Data* a2, int a3)
{
    CurrentFruitSlot = a3;
    sub_546670(a1, a2, a3);
}

ObjectMaster* __cdecl AL_GrowTree_CreateFruit(int a1, NJS_VECTOR* position, Angle angle, NJS_VECTOR* a4, ChaoData* a5)
{    
    if(a1 >= SA2BFruit_ChaoGardenFruit && a1 <= SA2BFruit_DarkGardenFruit)
        return ALO_FruitExecutor_Load(a1, position, angle, a4, a5);

    int lookup = ModAPI_TreeEntries[a1 - 2].FruitIDs[CurrentFruitSlot];
    return ALO_FruitExecutor_Load(lookup, position, angle, a4, a5);
}

__declspec(naked) void AL_GrowTree_CreateFruitHook()
{
    __asm
    {
        push ebx
        call AL_GrowTree_CreateFruit
        add esp,4
        retn
    }
}

void __cdecl ALO_SeedExecutor_Display_(ObjectMaster* eax0)
{
    EntityData1* v1; // esi

    v1 = eax0->Data1.Entity;
    if (ScaleObjectMaster_XYZ(eax0, 1.5f, 1.5f, 1.0f))
    {
        DoLighting(LightIndex);
       
        njPushMatrixEx();
        if (v1->Status < 0)
        {
            njTranslate(NULL, 0, *(float*)0xA3329C, 0);
        }
        njTranslateEx(&v1->Position);
        RotateY( v1->Rotation.y);
        njScale(NULL, 1.5f, 1.5f, 1.5f);
        njControl3D |= 0x2400u;
        njSetTexture(ModAPI_SeedTexlists[v1->Rotation.x]);
        chCnkDrawModel(ModAPI_SeedModels[v1->Rotation.x]->chunkmodel);
        njControl3D &= ~0x2400;

        if (eax0->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev1 > *(float*)&eax0->UnknownA_ptr->field_30)
        {
            njScale(NULL, 0.38f, 0.7f, 0.38f);
            njTranslate(NULL, 0, 0.1f, 0);
            DrawChaoWorldShadow();
        }

        njPopMatrixEx();
        DoLighting(LightIndexBackupMaybe);
    }
}

void ALO_GrowTreeExecutor_Display_(ObjectMaster* a1)
{
    ALO_GrowTreeExecutor_Data* v1 = (ALO_GrowTreeExecutor_Data *)a1->Data1.Undefined;
    if(v1->state != TREE_ST_LOCAL)
        v1->texlist = ModAPI_TreeEntries[v1->kind].pTexlist;
    
    AL_DayNightCycle_PushFallbackLight();
    ALO_GrowTreeExecutor_Display(a1);
    AL_DayNightCycle_PopFallbackLight();
    // this call is already done in the function but we do it again after the pop to be safe
    // since we're not "in" the function we cant do it properly before the dolighting call
    DoLighting(LightIndexBackupMaybe); 
}

void __cdecl FixTransition(ALO_GrowTreeExecutor_Data* v2)
{
    v2->entityData.Action = 8;
    v2->entityData.NextAction = 0;
    v2->entityData.field_6 = 0;
    v2->entityData.Index = 0;
    v2->life = 0;
    v2->growth = 0;
}

__declspec(naked) void FixTransitionHook()
{
    __asm
    {
        push ebx
        call FixTransition
        add esp, 4
        retn
    }
}
void ALO_GrowTree_Init()
{
    //WriteCall((void*)0x0547F47, nullsub_1);
    WriteData((int*)0x005482AB, (int)ALO_GrowTreeExecutor_Display_);
    
    WriteCall((void*)0x00546CA1, FixTransitionHook);
    WriteData<2>((char*)0x00546CA6, (char)0x90);

    WriteJump(ALO_SeedExecutor_Display, ALO_SeedExecutor_Display_);
    WriteCall((Void*)0x05470CB, sub_546670Hook);
    WriteCall((void*)0x005471D7, AL_GrowTree_CreateFruit);
    WriteJump((void*)0x547440, sub_547440);
}