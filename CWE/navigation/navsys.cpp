#include "stdafx.h"

#include "navsys_internal.h"
#include "navsys_generator.h"

#include "ninja_functions.h"
#include <al_world.h>
#include <al_stage.h>
#include <Chao.h>
#include <renderfix.h>
#include <api/api_idhash.h>
#include <al_msg_font.h>
#include <ALifeSDK_Functions.h>
#include "memory.h"

#include <chrono>
#include <assert.h>

void NavSys::DiscardResult(const uint32_t queryIndex) {
    std::lock_guard<std::mutex> lock(m_resultMutex);
    
    assert(m_results.contains(queryIndex));
    m_results.erase(queryIndex);
}

std::optional<NavSysPathResult> NavSys::GetResult(const uint32_t queryIndex) {
    std::lock_guard<std::mutex> lock(m_resultMutex);
    if(!m_results.contains(queryIndex)) {
        return std::nullopt;
    }

    return m_results[queryIndex];
}

void NavSys::LaunchQueryWorkerThread() {
    m_queryWorkerThread = std::thread([&]{
        PathEntry entry;

        while (true) {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);

                m_workerConditionVariable.wait(lock, [&]{
                    return m_workerStop || !m_queue.empty();
                });

                if (m_workerStop)
                    return;

                entry = m_queue.front();
                m_queue.pop();
            }

            const auto result = CalcStraightPath(entry);

            {
                std::lock_guard<std::mutex> lock(m_resultMutex);
                m_results[entry.queryID] = std::move(result);
            }
        }
    });
}

void NavSys::TryStopQueryWorkerThread() {
    if(m_queryWorkerThread.joinable()) {
        m_workerStop = true;
        m_workerConditionVariable.notify_all();
        m_queryWorkerThread.join();
    }
}

uint32_t NavSys::QueryPath(const NJS_POINT3& startPos, const NJS_POINT3& endPos, const uint32_t excludeFlags) {
    const uint32_t entryIndex = m_queryIndex++;
    const PathEntry entry = {
        .start = startPos,
        .end = endPos,
        .excludeFlags = excludeFlags,
        .queryID = entryIndex
    };

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.emplace(entry);
    }

    m_workerConditionVariable.notify_all();
    
    return entryIndex;
}

NavSys::NavSys() {
    m_navQuery = new dtNavMeshQuery();
}

NavSys::~NavSys() {
    delete m_navQuery;
}

void NavSys::InitQuery() {
    const auto status = m_navQuery->init(m_navMesh.get(), 2048);
    assert(!dtStatusFailed(status));

    m_isQueryReady = true;
}

NavSys::WAIT_FOR_GENERATE_RESULT NavSys::WaitForGenerate() {
    using namespace std::chrono_literals;

    if(!m_loadingNavMeshResult.valid()) {
        return WAIT_FOR_GENERATE_RESULT::WAIT;
    }

    if (m_loadingNavMeshResult.wait_for(0ms) == std::future_status::ready) {
        m_navMesh = m_loadingNavMeshResult.get();

        if(!m_navMesh) {
            return WAIT_FOR_GENERATE_RESULT::FAIL;
        }

        return WAIT_FOR_GENERATE_RESULT::SUCCESS;
    }       

    return WAIT_FOR_GENERATE_RESULT::WAIT;
}

bool NavSys::IsQueryReady() {
    return m_isQueryReady;
}

void NavSys::Generate() {
    uint32_t land_hash = GenerateHashForLandTable(CurrentLandTable);

    m_loadingNavMeshResult = gNavSysGenerator.TryGenerate(land_hash);
}

bool NavSys::CheckAndLoadCache() {
    uint32_t land_hash = GenerateHashForLandTable(CurrentLandTable);

    auto result = gNavSysGenerator.TryLoad(land_hash);
    if(result) {
        m_navMesh = result;

        return true;
    }

    return false;
}

NavSysPathResult NavSys::CalcStraightPath(const PathEntry& entry) {
    NavSysPathResult result;

    float m_polyPickExt[3];
    m_polyPickExt[0] = 5;
	m_polyPickExt[1] = 5;
	m_polyPickExt[2] = 5;

    dtPolyRef startRef;
    dtPolyRef endRef;
    dtPolyRef m_polys[256];
    dtPolyRef m_straightPathPolys[256];
    unsigned char m_straightPathFlags[256];
    dtQueryFilter m_filter;

    m_filter.setExcludeFlags(entry.excludeFlags);

    m_navQuery->findNearestPoly(&entry.start.x, m_polyPickExt, &m_filter, &startRef, 0);
    m_navQuery->findNearestPoly(&entry.end.x, m_polyPickExt, &m_filter, &endRef, 0);

    int m_npolys = 0;
    const auto findStatus = m_navQuery->findPath(startRef, endRef, &entry.start.x, &entry.end.x, &m_filter, m_polys, &m_npolys, 256);

    // I'm not sure how many of these are redundant tbh, but better safe than sorry
    if (dtStatusSucceed(findStatus) && !dtStatusDetail(findStatus, DT_PARTIAL_RESULT) && m_npolys) {
        float epos[3];
        dtVcopy(epos, &entry.end.x);

        // from sample code (like most things in this)
        // but now that we don't allow partial path I don't think we need this...?
        // still gonna leave it here just incase
        #if 0
            // In case of partial path, make sure the end point is clamped to the last polygon.
            if (m_polys[m_npolys-1] != endRef)
                m_navQuery->closestPointOnPoly(m_polys[m_npolys-1], &entry.end.x, epos, 0);
        #endif

        static NJS_POINT3 straightPath[256];
        static int pathCount = 0;
        const auto findStatus = m_navQuery->findStraightPath(&entry.start.x, epos, m_polys, m_npolys,
                                    &straightPath->x, m_straightPathFlags,
                                    m_straightPathPolys, &pathCount, _countof(straightPath), 0);

        if(dtStatusSucceed(findStatus) && !dtStatusDetail(findStatus, DT_PARTIAL_RESULT) && pathCount) {
            std::copy(straightPath, straightPath + pathCount, std::back_inserter(result));
        }
    }

    return result;
}

static task* pNavSysTask;

void NavSysDiscardResult(const uint32_t queryIndex) {
    return GET_NAV_SYS(pNavSysTask)->DiscardResult(queryIndex);
}

uint32_t NavSysQueryPath(const NJS_POINT3* pStartPos, const NJS_POINT3* pEndPos, const uint32_t excludeFlags) {
    return GET_NAV_SYS(pNavSysTask)->QueryPath(*pStartPos, *pEndPos, excludeFlags);
}

std::optional<NavSysPathResult> NavSysGetResult(const uint32_t queryIndex) {
    auto sys = GET_NAV_SYS(pNavSysTask);
    return sys->GetResult(queryIndex);
}

static void NavSysExecutor(task* tp) {
    auto sys = GET_NAV_SYS(tp);
    auto work = tp->Data1.Entity;

    switch(work->Action) {
        case NAV_MD_INIT:
            work->Action = NAV_MD_CHECK_CACHE_GENERATE;
            [[fallthrough]];
        case NAV_MD_CHECK_CACHE_GENERATE:
            if(!CurrentLandTable) {
                PrintDebug("no landtable found, aborting NAV_MD_CHECK_CACHE_GENERATE and deleting navsys task");

                DeleteObject_(tp);
                break;
            }

            if (sys->CheckAndLoadCache()) {
                work->Action = NAV_MD_ACTIVE;
            }
            else {
                work->Action = NAV_MD_LAUNCH_GENERATE;
            }
            break;

        case NAV_MD_LAUNCH_GENERATE:
            sys->Generate();
            work->Action = NAV_MD_WAIT_FOR_GENERATE;
            break;

        case NAV_MD_WAIT_FOR_GENERATE:
            switch(sys->WaitForGenerate()) {
                case NavSys::WAIT_FOR_GENERATE_RESULT::WAIT:
                    break;
                case NavSys::WAIT_FOR_GENERATE_RESULT::FAIL:
                    PrintDebug("navmesh generation failed, aborting NAV_MD_WAIT_FOR_GENERATE and deleting navsys task");
                    DeleteObject_(tp);
                    break;
                case NavSys::WAIT_FOR_GENERATE_RESULT::SUCCESS:
                    work->Action = NAV_MD_ACTIVE;
                    break;
            }
            break;

        case NAV_MD_ACTIVE:
            if(!sys->IsQueryReady()) {
                sys->InitQuery();
                sys->LaunchQueryWorkerThread();
            }
            break;
    }
}

static void NavSysDestructor(task* tp) {
    auto sys = GET_NAV_SYS(tp);
    assert(sys);

    sys->TryStopQueryWorkerThread();

    pNavSysTask = NULL;
    
    delete sys;
    tp->Data2.Undefined = NULL;
}

static void NavSysDisplayer(task* tp) {
    auto sys = GET_NAV_SYS(tp);

    if(!sys->IsQueryReady()) {
        return;

        SetShaders(1);
        *(Uint32*)0x01A267D0 = 0xFFFF6AFF;

        MessageFontThing messageBuffer;

        AlMsgFontCreateCStr(TextLanguage == 0, (int)"Generating navigation...", (int)&messageBuffer, 999);
		AlMsgFontDrawRegionScale2(-1, &messageBuffer, 0, 0, -1, 32, 0, 1, 1);
		AlMsgFontDelete(&messageBuffer);
    }

    #ifdef IMGUIDEBUG
        sys->DebugDrawNavMesh();
        sys->DebugDrawPathResult();

        gNavSysGenerator.DebugDrawMaxClimbLine();
    #endif
}

task* GetNavSysTask() {
    return pNavSysTask;
}

void NavSysCreate() {
    assert(!pNavSysTask);

    if(!AL_IsGarden()) return;

    pNavSysTask = LoadObject(4, "NavSysTask", NavSysExecutor, LoadObj_Data1);
    pNavSysTask->DisplaySub = NavSysDisplayer;
    pNavSysTask->DeleteSub = NavSysDestructor;

    pNavSysTask->Data2.Undefined = (void*)(new NavSys());
}