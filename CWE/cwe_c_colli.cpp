#include "stdafx.h"
#include <FunctionHook.h>

#include <vector>

// credit to sadx decomp for reference code

// i skipped wall here, only player checks it so i didn't need it
DataArray(task*, player_entry_list, 0x1946660, 16);
DataArray(task*, bullet_entry_list, 0x19466A0, 128);
DataArray(task*, enemy_entry_list, 0x1DE4EE0, 128);
DataArray(task*, enemy2_entry_list, 0x1DE6DA0, 128);
DataArray(task*, object_entry_list, 0x1945E18, 511);
DataArray(task*, object2_entry_list, 0x19468B8, 128);
DataArray(task*, item_entry_list, 0x1946AC8, 128);
DataArray(task*, item2_entry_list, 0x1946CC8, 128);
DataPointer(Uint16, player_entry_num, 0x1946AB8);
DataPointer(Uint16, bullet_entry_num, 0x1945E0C);
DataPointer(Uint16, enemy_entry_num, 0x1DE8C2A);
DataPointer(Uint16, enemy2_entry_num, 0x1DE944C);
DataPointer(Uint16, object_entry_num, 0x194665C);
DataPointer(Uint16, object2_entry_num, 0x1946AC0);
DataPointer(Uint16, item_entry_num, 0x19468B0);
DataPointer(Uint16, item2_entry_num, 0x1945E10);
DataPointer(Uint16, chao_entry_num, 0x1946ABC);

FunctionPointer(void, CCL_CalcColli, (task *tp1, task *tp2), 0x485850);

static const int CCL_ClearInfoPtr = 0x47E6C0;
static void CCL_ClearInfo(task* tp) {
    __asm {
        mov eax, tp
        call CCL_ClearInfoPtr
    }
}

// todo: replace struct properly in the future
typedef CollisionInfo CCL_INFO;

// naming convention based on cwe
static std::vector<task*> cwe_entry_list;

void CWE_CCL_Entry(task* tp) {
    CCL_INFO* pInfo = tp->Data1.Entity->Collision;

    if(!pInfo || tp->MainSub == DeleteObject_)  {
        return;
    }

    CCL_ClearInfo(tp);
    cwe_entry_list.push_back(tp);
}

static void CCL_CCheckColli_r();
static FunctionHook<void> CCL_CCheckColli_hook(0x485C70, CCL_CCheckColli_r);
static void CCL_CCheckColli_r() {
    // run chao collision first
    CCL_CCheckColli_hook.Original();

    // pointer to entry list in the ccl_entry instructions
    // to dynamically use the one that any other mod overwrites (us or ecw)
    task** chao_entry_list = *(task***)0x0047E923;

    if(CurrentLevel == LevelIDs_ChaoWorld && !cwe_entry_list.empty()) {
        // self colli check
        for(size_t i = 0; i < cwe_entry_list.size() - 1; ++i) {
            for(size_t j = i + 1; j < cwe_entry_list.size(); ++j) {
                CCL_CalcColli(cwe_entry_list[i], cwe_entry_list[j]);
            }
        }

        for(size_t i = 0; i < cwe_entry_list.size(); ++i) {
            // it looks like player can only interact with it if they're in the first param here
            for (size_t j = 0; j < player_entry_num; ++j) {
                CCL_CalcColli(player_entry_list[j], cwe_entry_list[i]);
            }

            // no bullet check, really doubt its relevant in chao world lol

            for (size_t j = 0; j < enemy_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], enemy_entry_list[j]);
            }

            for (size_t j = 0; j < enemy2_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], enemy2_entry_list[j]);
            }

            for (size_t j = 0; j < object_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], object_entry_list[j]);
            }

            for (size_t j = 0; j < object2_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], object2_entry_list[j]);
            }

            for (size_t j = 0; j < item_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], item_entry_list[j]);
            }

            for (size_t j = 0; j < item2_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], item2_entry_list[j]);
            }

            for (size_t j = 0; j < chao_entry_num; ++j) {
                CCL_CalcColli(cwe_entry_list[i], chao_entry_list[j]);
            }
        }
    }

    cwe_entry_list.clear();
}

static void __declspec(naked) CCL_Entry_to_CWE_CCL_Entry_r() {
    __asm {
        push esi // a1

        // Call your __cdecl function here:
        call CWE_CCL_Entry

        pop esi // a1
        retn
    }
}

void CWE_CCL_Init() {
    // optimization to not have to worry about reallocations under normal circumstances
    cwe_entry_list.reserve(256);

    // moves ObakeHeadExecutor's CCL_Entry to use our new system
    // this means both hats and accessories 
    // (since accessories "reuse" ObakeHeadExecutor at the time of writing)
    WriteCall((void*)0x0054A0AE, CCL_Entry_to_CWE_CCL_Entry_r);
}