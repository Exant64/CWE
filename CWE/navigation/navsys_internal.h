#pragma once

#include "navsys.h"

#include <vector>
#include <queue>
#include <optional>
#include <unordered_map>
#include <mutex>
#include <future>

#include "external/Detour/Include/DetourNavMeshBuilder.h"
#include "external/Detour/Include/DetourNavMesh.h"
#include "external/Detour/Include/DetourNavMeshQuery.h"
#include "external/Detour/Include/DetourCommon.h"

class NavSys {
private:
    struct PathEntry {
        NJS_POINT3 start, end;
        uint32_t excludeFlags;
        uint32_t queryID;
    };

    #ifdef IMGUIDEBUG
        uint32_t m_debugDisplayPath;
        bool m_debugDisplayNavMesh;
    #endif
    
    bool m_isQueryReady;
    std::future<std::shared_ptr<dtNavMesh>> m_loadingNavMeshResult;
    std::shared_ptr<dtNavMesh> m_navMesh;

    std::mutex m_queueMutex;
    std::mutex m_resultMutex;
    std::thread m_queryWorkerThread;
    std::condition_variable m_workerConditionVariable;
    std::atomic<bool> m_workerStop = false;

    uint32_t m_queryIndex = 0;
    std::queue<PathEntry> m_queue;
    std::unordered_map<uint32_t, NavSysPathResult> m_results;

    dtNavMeshQuery* m_navQuery;

    NavSysPathResult CalcStraightPath(const PathEntry& entry);
public:
    enum class WAIT_FOR_GENERATE_RESULT {
        WAIT,
        FAIL,
        SUCCESS
    };

#ifdef IMGUIDEBUG
    void ImGuiDebug();
    void DebugDrawNavMesh();
    void DebugDrawPathResult();
#endif

    NavSys();
    ~NavSys();

    void InitQuery();
    bool IsQueryReady();
    void LaunchQueryWorkerThread();
    void TryStopQueryWorkerThread();

    bool CheckAndLoadCache();
    void Generate();
    WAIT_FOR_GENERATE_RESULT WaitForGenerate();

    void DiscardResult(const uint32_t queryIndex);
    uint32_t QueryPath(const NJS_POINT3& startPos, const NJS_POINT3& endPos, const uint32_t excludeFlags);
    std::optional<NavSysPathResult> GetResult(const uint32_t queryIndex);
};

#define GET_NAV_SYS(tp) ((NavSys*)(tp->Data2.Undefined))

enum {
    NAV_MD_INIT,
    NAV_MD_CHECK_CACHE_GENERATE,
    NAV_MD_LAUNCH_GENERATE,
    NAV_MD_WAIT_FOR_GENERATE,
    NAV_MD_ACTIVE
};
