#pragma once

#include "stdafx.h"

#include <future>
#include <set>

#include "external/Recast/Include/Recast.h"
#include "external/Detour/Include/DetourCommon.h"
#include "external/Detour/Include/DetourNavMesh.h"

// todo: this does not work
struct cweRcContext : rcContext {
    void log(const rcLogCategory category, const char* format, ...) {
        char prefix[255];

        va_list argptr;
		va_start(argptr, format);
		vsnprintf(prefix, _countof(prefix), format, argptr);
		va_end(argptr);

        PrintDebug("[NAVI] %s", prefix);
    }

    cweRcContext() : rcContext(true) {
    }
};

struct NavGenConfig {
    float m_cellSize;
    float m_cellHeight;
    float m_agentHeight;
    float m_agentRadius;
    float m_agentMaxClimb;
    float m_agentMaxSlope;
    float m_regionMinSize; // todo: what does this do
    float m_regionMergeSize; // todo : what does this do
    float m_edgeMaxLen; // todo: are the defaults okay for these?
    float m_edgeMaxError; // todo: are the defaults okay for these?
    size_t m_vertsPerPoly; // todo: why 6?
    float m_detailSampleDist;
    float m_detailSampleMaxError;

    NavGenConfig() {
        m_cellSize = 1.2f; //0.5f;
        m_cellHeight = 0.2f;
        m_agentHeight = 5.f;
        m_agentRadius = 3.5f;
        m_agentMaxClimb = 1.3f; // high enough climb to still be able to navigate climbing out of pools and stuff
        m_agentMaxSlope = 45.f;
        m_regionMinSize = 8.f;
        m_regionMergeSize = 20.f;
        m_edgeMaxLen = 12.f;
        m_edgeMaxError = 1.3f;
        m_vertsPerPoly = 6;
        m_detailSampleDist = 6.f;
        m_detailSampleMaxError = 1.f;
    }
};

class NavSysGenerator {
private:
    bool m_useCache = false;

    cweRcContext* m_recastContext;

    rcHeightfield* m_solid;
    rcCompactHeightfield* m_chf;
    rcContourSet* m_cset;
    rcPolyMesh* m_pmesh;
    rcConfig m_cfg;	
    rcPolyMeshDetail* m_dmesh;

    NavGenConfig m_config;

    std::mutex m_inProgressMutex;
    std::set<uint32_t> m_inProgress;

    void SaveNavMesh(const char* path, const dtNavMesh* mesh);
    dtNavMesh* LoadNavMesh(const char* path);
public:
    #ifdef IMGUIDEBUG
        void ImGuiDebug();
    #endif

    std::future<std::unique_ptr<dtNavMesh>> TryLoadGenerate(const uint32_t hash);
};

extern NavSysGenerator gNavSysGenerator;