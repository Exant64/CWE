#include "stdafx.h"

#include "navsys_internal.h"
#include "navsys_generator.h"

#include "ninja_functions.h"
#include "data/debugsphere.h"
#include <al_world.h>
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

void NavSys::LaunchThread() {
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

void NavSys::TryStopThread() {
    if(m_queryWorkerThread.joinable()) {
        m_workerStop = true;
        m_workerConditionVariable.notify_all();
        m_queryWorkerThread.join();
    }
}

uint32_t NavSys::AddPath(const NJS_POINT3& startPos, const NJS_POINT3& endPos, const uint32_t excludeFlags) {
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

bool NavSys::WaitForGenerate() {
    using namespace std::chrono_literals;

    if(!m_loadingNavMeshResult.valid()) {
        return false;
    }

    if (m_loadingNavMeshResult.wait_for(0ms) == std::future_status::ready) {
        m_navMesh = m_loadingNavMeshResult.get();

        assert(m_navMesh);

        const auto status = m_navQuery->init(m_navMesh.get(), 2048);
        assert(!dtStatusFailed(status));

        m_isReady = true;

        return true;
    }       

    return false;
}

bool NavSys::IsReady() {
    return m_isReady;
}

void NavSys::Generate() {
    uint32_t land_hash = GenerateHashForLandTable(CurrentLandTable);

    m_loadingNavMeshResult = gNavSysGenerator.TryLoadGenerate(land_hash);
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

uint32_t NavSysAddPath(const NJS_POINT3* pStartPos, const NJS_POINT3* pEndPos, const uint32_t excludeFlags) {
    return GET_NAV_SYS(pNavSysTask)->AddPath(*pStartPos, *pEndPos, excludeFlags);
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
            work->Action++;
        case NAV_MD_LAUNCH_GENERATE:
            sys->Generate();
            work->Action++;
            break;
        case NAV_MD_WAIT_FOR_GENERATE:
            if(sys->WaitForGenerate()) {
                sys->LaunchThread();
                work->Action++;
            }
            break;
        case NAV_MD_ACTIVE:
            break;
    }
}

static void NavSysDestructor(task* tp) {
    auto sys = GET_NAV_SYS(tp);
    assert(sys);

    sys->TryStopThread();

    pNavSysTask = NULL;
    
    delete sys;
    tp->Data2.Undefined = NULL;
}

static void NavSysDisplayer(task* tp) {
    auto sys = GET_NAV_SYS(tp);

    if(!sys->IsReady()) {
        return;

        SetShaders(1);
        *(Uint32*)0x01A267D0 = 0xFFFF6AFF;

        MessageFontThing messageBuffer;

        AlMsgFontCreateCStr(TextLanguage == 0, (int)"Generating navigation...", (int)&messageBuffer, 999);
		AlMsgFontDrawRegionScale2(-1, &messageBuffer, 0, 0, -1, 32, 0, 1, 1);
		AlMsgFontDelete(&messageBuffer);
    }

    #ifdef IMGUIDEBUG
        // display max climb
        {
            // debug menu stores the current maxclimb in pos.y
            NJS_POINT3 pos[2];
            pos[0] = pos[1] = {
                MainCharObj1[0]->Position.x + 3,
                MainCharObj1[0]->Position.y,
                MainCharObj1[0]->Position.z + 3
            };

            pos[1].y += tp->Data1.Entity->Position.y;

            rfapi_core->pDraw->DrawLineStrip3D(
                pos, 
                _countof(pos), 
                5.f, 
                0xFFFFFFFF
            );
        }
        // debug menu sets Rotation.x for the selected path result to display
        const auto resultID = tp->Data1.Entity->Rotation.x;
        const auto result = sys->GetResult(resultID);

        if(!result) return;

        if(rfapi_core && result->size() >= 2) {
            rfapi_core->pDraw->DrawLineStrip3D(
                result->data(), 
                result->size(), 
                5.f, 
                0xFFFFFFFF
            );
        }

        for(size_t i = 0; i < result->size(); ++i) {
            const auto& p = result->at(i);

            njPushMatrixEx();
            njTranslate(NULL, p.x, p.y, p.z);
            njScale(NULL, 0.05f, 0.05f, 0.05f);
            njCnkDrawObject(&DebugSphere);
            njPopMatrixEx();
        }
    #endif
}

task* GetNavSysTask() {
    return pNavSysTask;
}

void NavSysCreate() {
    assert(!pNavSysTask);

    pNavSysTask = LoadObject(4, "NavSysTask", NavSysExecutor, LoadObj_Data1);
    pNavSysTask->DisplaySub = NavSysDisplayer;
    pNavSysTask->DeleteSub = NavSysDestructor;

    pNavSysTask->Data2.Undefined = (void*)(new NavSys());
}