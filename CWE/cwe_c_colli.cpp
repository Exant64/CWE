#include "stdafx.h"
#include <FunctionHook.h>

#include "c_colli.h"
#include <vector>
#include "ChaoMain.h"
#include "IniFile.h"

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

// naming convention based on symbols
static std::vector<task*> cwe_entry_list;

static bool FoundDrawDistanceMod = false;

void CWE_CCL_Entry(task* tp) {
    if(FoundDrawDistanceMod) {
        CCL_Entry(tp);
        return;
    }


    if(!tp->twp->cwp || tp->exec == DestroyTask)  {
        return;
    }

    CCL_ClearInfo(tp);
    cwe_entry_list.push_back(tp);
}

static void CCL_CCheckColli_r();
static FunctionHook<void> CCL_CCheckColli_hook(0x485C70);
static void CCL_CCheckColli_r() {
    // run chao collision first
    CCL_CCheckColli_hook.Original();

    // pointer to entry list in the ccl_entry instructions
    // to dynamically use the one that any other mod overwrites (us or ecw)
    task** chao_entry_list = *(task***)0x0047E923;

    if(ssStageNumber == STAGE_CHAOWORLD && !cwe_entry_list.empty()) {
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

static void ASM_FUNC CCL_Entry_to_CWE_CCL_Entry_r() {
    ASM_PUSH(esi); // a1

    ASM_PUSH(esi); // a1
    // Call your __cdecl function here:
    ASM_CALL (CWE_CCL_Entry);
    ASM_ESP_ADD(1); // a1
    
    ASM_POP(esi); // a1
    ASM_RET(0);
}

void CWE_CCL_Init() {
    const auto& modList = *g_HelperFunctions->Mods;
    for(const auto& mod : modList) {
        if(!strcmp(mod.Name, "Higher Draw Distance")) {
            std::string iniPath = std::string(mod.Folder);
            iniPath += "\\config.ini";
            
            IniFile* pDrawDistConfig = new IniFile(iniPath.c_str());
            if(pDrawDistConfig && pDrawDistConfig->getBool("Limits", "Collisions", true)) {
                FoundDrawDistanceMod = true;
            }
            delete pDrawDistConfig;

            break;
        }
    }

    // don't apply patches if the limits mod was enabled
    if(FoundDrawDistanceMod) {
        return;
    }

    CCL_CCheckColli_hook.Hook(CCL_CCheckColli_r);

    // optimization to not have to worry about reallocations under normal circumstances
    cwe_entry_list.reserve(256);

    // moves ObakeHeadExecutor's CCL_Entry to use our new system
    // this means both hats and accessories 
    // (since accessories "reuse" ObakeHeadExecutor at the time of writing)
    WriteCall((void*)0x0054A0AE, (void*)CCL_Entry_to_CWE_CCL_Entry_r);
}