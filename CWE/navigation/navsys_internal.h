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
    
    bool m_isReady;
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
#ifdef IMGUIDEBUG
    void ImGuiDebug();
#endif

    NavSys();
    ~NavSys();

    void LaunchThread();
    void TryStopThread();

    void Generate();
    bool WaitForGenerate();

    bool IsReady();

    void DiscardResult(const uint32_t queryIndex);
    uint32_t AddPath(const NJS_POINT3& startPos, const NJS_POINT3& endPos, const uint32_t excludeFlags);
    std::optional<NavSysPathResult> GetResult(const uint32_t queryIndex);
};

#define GET_NAV_SYS(tp) ((NavSys*)(tp->Data2.Undefined))

enum {
    NAV_MD_INIT,
    NAV_MD_LAUNCH_GENERATE,
    NAV_MD_WAIT_FOR_GENERATE,
    NAV_MD_ACTIVE
};
