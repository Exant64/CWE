#pragma once

#include "stdafx.h"

#include <future>
#include <set>
#include <unordered_map>

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

    bool m_filterLowHanging;
    bool m_filterLedgeSpans;
    bool m_filterWalkableLowHeightSpans;

    NavGenConfig() {
        m_filterLowHanging = true;
        m_filterLedgeSpans = true;
        m_filterWalkableLowHeightSpans = true;

        m_cellSize = 1.2f; //0.5f;
        m_cellHeight = 0.2f;
        m_agentHeight = 5.f;
        m_agentRadius = 3.5f;
        m_agentMaxClimb = 1.9f; // high enough climb to still be able to navigate climbing out of pools and stuff
        m_agentMaxSlope = NJM_RAD_DEG(acosf(0.78f)); // the max slope from the MOV code, ~38.7
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
    std::string m_cachePath;

    struct ClimbSpot {
        NJS_POINT3 m_pos;
        NJS_POINT3 m_extent;
        Angle m_inverseAngY;
        float m_radiusSquared;
    };

    bool m_useCache = false;
    NavGenConfig m_config;

    std::mutex m_toEraseMutex;
    std::vector<uint32_t> m_toErase;
    std::unordered_map<uint32_t, std::shared_future<std::shared_ptr<dtNavMesh>>> m_inProgress;

    const std::string GetCacheFilePath(const uint32_t hash) const;
    void SaveNavMesh(const char* path, const dtNavMesh* mesh);
    dtNavMesh* LoadNavMesh(const char* path);

    void AssignWaterToGroundBelowWater(rcHeightfield* solid);

    void PopulateClimbSpots(std::vector<ClimbSpot>& spots) const;
    bool CheckIfPointInsideAnyClimbSpot(const std::vector<ClimbSpot>& spots, const NJS_POINT3& p) const;
    std::vector<NJS_POINT3> GenerateOffMeshClimbSpots(rcHeightfield* solid, const int walkableRadius, const int walkableClimb, const std::vector<ClimbSpot>& spots) const;
public:
    void CheckCleanInProgress();

    #ifdef IMGUIDEBUG
        void ImGuiDebug();
        void DebugDrawMaxClimbLine();
    #endif

    void SetNavMeshCachePath(const std::string& cachePath);

    std::shared_ptr<dtNavMesh> TryLoad(const uint32_t hash);
    std::shared_future<std::shared_ptr<dtNavMesh>> TryGenerate(const uint32_t hash);
};

extern NavSysGenerator gNavSysGenerator;