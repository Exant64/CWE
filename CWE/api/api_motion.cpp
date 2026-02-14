#include "stdafx.h"

#include <string>
#include <map>
#include <vector>

#include "api_motion.h"
#include "api_util.h"
#include "al_draw.h"
#include <ChaoMain.h>
#include <cwe_api.h>

static std::vector<MOTION_TABLE> al_motion_table_api;
static std::map<std::string, int> IDMotionMap;

MOTION_TABLE* GetChaoMotionTable(size_t index) {
	if (index >= al_motion_table_api.size()) return nullptr;

	return &al_motion_table_api[index];
}

size_t AddChaoMotion(const std::string& name, MOTION_TABLE* pMotionTable) {
    APIErrorUtil error("_AddChaoMotion error for animation \"%s\": ", name.c_str());

    if (!pMotionTable) {
        error.print("action is null!");
        return -1;
    }

    if (!pMotionTable->pMotion) {
        error.print("the action's animation is null! (missing file?)");
        return -1;
    }

    if (pMotionTable->pMotion->type != (NJD_MTYPE_POS_0 | NJD_MTYPE_ANG_1)) {
        error.print("the animation's type is invalid! (scale keyframes in animation?)");
        return -1;
    }

    const NJS_MDATA2* mdata = reinterpret_cast<NJS_MDATA2*>(pMotionTable->pMotion->mdata);
    for (size_t i = 0; i < AL_PART_END; i++) {
        for (size_t j = 0; j < 2; j++) {
            if (mdata[i].nb[j] && mdata[i].nb[j] > pMotionTable->pMotion->nbFrame) {
                error.print("the animation's model count is invalid!");
                return -1;
            }
        }
    }

    int index = al_motion_table_api.size();
    IDMotionMap.insert(std::make_pair(name, index));
    al_motion_table_api.push_back(*pMotionTable);
    return index;
}

size_t AddChaoMotion(const char* pID, MOTION_TABLE* pMotionTable) {
    return AddChaoMotion(std::string{pID}, pMotionTable);
}

// LEGACY
extern "C" __declspec(dllexport) int RegisterChaoAnimation(std::string name, MOTION_TABLE* pMotionTable) {
    return AddChaoMotion(name, pMotionTable);
}

bool SetChaoMotionTransition(const std::string& from, const std::string& to) {
    APIErrorUtil error("SetChaoMotionTransition error (\"%s\"->\"%s\"): ", from.c_str(), to.c_str());
    
    if (!IDMotionMap.contains(from)) {
        error.print("the \"from\" animation is not registered!");
        return false;
    }

    if (!IDMotionMap.contains(to)) {
        error.print("the \"to\" animation is not registered!");
        return false;
    }

    al_motion_table_api[IDMotionMap[from]].next = IDMotionMap[to];
    return true;
}

bool SetChaoMotionTransition(const char* pFromID, const char* pToID) {
    return SetChaoMotionTransition(std::string{pFromID}, std::string{pToID});
}

void Chao_RegAnimation(ObjectMaster* a1, const std::string& name) {
    AL_SetMotionLink(a1, IDMotionMap[name]);
}

void Chao_RegAnimationTbl(MotionTableData* a1, const std::string& name) {
    Chao_Animation(a1, IDMotionMap[name]);
}

size_t GetChaoMotionIndex(const std::string& name) {
    return IDMotionMap[name];
}

size_t GetChaoMotionIndex(const char* pID) {
    return GetChaoMotionIndex(std::string{pID});
}

CWE_API_REGISTER_MOTION AL_ModAPI_Motion = {
    .Version = CWE_API_REGISTER_MOTION_VER,

    .AddChaoMotion = AddChaoMotion,
    .GetChaoMotionIndex = GetChaoMotionIndex,
    .GetChaoMotionTable = GetChaoMotionTable,
    .SetChaoMotionTransition = SetChaoMotionTransition
};

void AL_ModAPI_Motion_Update() {
    g_HelperFunctions->HookExport("al_motion_table", al_motion_table_api.data());
}

void AL_ModAPI_Motion_Init() {
	MOTION_TABLE* al_motion_table = (MOTION_TABLE*)GetDllData("al_motion_table");
	al_motion_table_api.insert(al_motion_table_api.end(), al_motion_table, &al_motion_table[622]);
}