#include "stdafx.h"

#include <chrono>

#include "navsys_internal.h"
#include "navsys_generator.h"
#include "navsys.h"

#include <al_behavior/albhv_navigation.h>

#ifdef IMGUIDEBUG
    #include "imgui/imgui.h"
    #include <data/debugsphere.h>
#endif

#include "ninja_functions.h"
#include <renderfix.h>

void NavSysGenerator::ImGuiDebug() {
    ImGui::Checkbox("Use Cache", &m_useCache);
    ImGui::SliderFloat("Cell Size", &m_config.m_cellSize, 0.1f, 10.f);
    ImGui::SliderFloat("Cell Height", &m_config.m_cellHeight, 0.1f, 10.f);
    ImGui::SliderFloat("Agent Height", &m_config.m_agentHeight, 0.1f, 10.f);
    ImGui::SliderFloat("Agent Radius", &m_config.m_agentRadius, 0.1f, 10.f);
    ImGui::SliderFloat("Agent Max Climb", &m_config.m_agentMaxClimb, 0.1f, 10.f);
    ImGui::SliderFloat("Agent Max Slope", &m_config.m_agentMaxSlope, 0.f, 90.f);
    ImGui::SliderFloat("Region Min Size", &m_config.m_regionMinSize, 0.f, 10.f);
    ImGui::SliderFloat("Region Merge Size", &m_config.m_regionMergeSize, 0.f, 100.f);
    ImGui::SliderFloat("Edge Max Len", &m_config.m_edgeMaxLen, 0.f, 50.f);
    ImGui::SliderFloat("Edge Max Error", &m_config.m_edgeMaxError, 0.f, 50.f);
    ImGui::SliderFloat("Detail Sample Dist", &m_config.m_detailSampleDist, 0.f, 50.f);
    ImGui::SliderFloat("Detail Sample Max Error", &m_config.m_detailSampleMaxError, 0.f, 50.f);
}

void NavSysGenerator::DebugDrawMaxClimbLine() {
    NJS_POINT3 pos[2];

    pos[0] = pos[1] = {
        MainCharObj1[0]->Position.x + 3,
        MainCharObj1[0]->Position.y,
        MainCharObj1[0]->Position.z + 3
    };

    pos[1].y += m_config.m_agentMaxClimb;

    rfapi_core->pDraw->DrawLineStrip3D(
        pos, 
        _countof(pos), 
        5.f, 
        0xFFFFFFFF
    );
}

void NavSys::DebugDrawNavMesh() {
    if(!m_debugDisplayNavMesh) return;
    if(!m_navMesh) return;

    const auto& mesh = *m_navMesh;
    for (int i = 0; i < mesh.getMaxTiles(); ++i) {
		const dtMeshTile* tile = mesh.getTile(i);
		if (tile->header) {
            dtPolyRef base = mesh.getPolyRefBase(tile);

            int tileNum = mesh.decodePolyIdTile(base);
            NJS_COLOR col;
            col.argb.a = 0xA0;

            // warning: int i here aswell, luckily in separate scope
            for (int i = 0; i < tile->header->polyCount; ++i) {
                const dtPoly* p = &tile->polys[i];
                if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION){
                    const dtOffMeshConnection* con = &tile->offMeshCons[i - tile->header->offMeshBase];
                    NJS_POINT3* pPos = (NJS_POINT3*)con->pos;

                    rfapi_core->pDraw->DrawLineStrip3D(
                        pPos, 
                        2, 
                        3.f, 
                        0xFF00FFFF
                    );

                    njPushMatrixEx();
                    njTranslateEx(pPos);
                    njScale(NULL, 0.15f, 0.15f, 0.15f);
                    njCnkDrawObject(&DebugSphere);
                    njPopMatrixEx();

                    njPushMatrixEx();
                    njTranslateEx(pPos + 1);
                    njScale(NULL, 0.15f, 0.15f, 0.15f);
                    njCnkDrawObject(&DebugSphere);
                    njPopMatrixEx();

                    continue;
                }
                    
                const dtPolyDetail* pd = &tile->detailMeshes[i];

                switch(p->getArea()) {
                    case NAV_AREA_GROUND:
                        col.argb.r = 0;
                        col.argb.g = col.argb.b = 0x70;
                        break;
                    case NAV_AREA_WATER:
                        col.argb.r = col.argb.g = 0x30;
                        col.argb.b = 255;
                        break;
                }
                
                for (int j = 0; j < pd->triCount; ++j) {
                    NJS_POLYGON_VTX vtx[3];
                    const unsigned char* t = &tile->detailTris[(pd->triBase+j)*4];

                    for (int k = 0; k < 3; ++k) {
                        if (t[k] < p->vertCount) {
                            vtx[k] = {
                                tile->verts[p->verts[t[k]]*3], 
                                tile->verts[p->verts[t[k]]*3 + 1], 
                                tile->verts[p->verts[t[k]]*3 + 2], 
                                col.color
                            };
                        }
                        else {
                            vtx[k] = {
                                tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3], 
                                tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3 + 1], 
                                tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3 + 2], 
                                col.color
                            };
                        }
                    }

                    rfapi_core->pNjDraw->DrawPolygon3DEx(vtx, 3, 1);
                }
            }
        }
	}
}

void NavSys::DebugDrawPathResult() {
    const auto result = GetResult(m_debugDisplayPath);
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
}

void NavSys::ImGuiDebug() {
    using namespace std::chrono_literals;

    if(m_loadingNavMeshResult.valid()) {
        ImGui::Text("m_loadingNavMeshResult: %d", m_loadingNavMeshResult.wait_for(0ms));
    }

    ImGui::Text("m_navMesh: %s", m_navMesh.get() ? "Loaded" : "NULL");
    ImGui::Text("WorkerStop: %d", m_workerStop.load());
    ImGui::Text("m_queryIndex: %d", m_queryIndex);

    ImGui::Checkbox("Display NavMesh", &m_debugDisplayNavMesh);

    if(ImGui::TreeNode("Queue")) {
        std::lock_guard<std::mutex> lock(m_queueMutex);

        ImGui::Text("Count: %d", m_queue.size());
        if(!m_queue.empty()) {
            const auto frontElem = m_queue.front();
            ImGui::Text(
                "Front Element: s(%f %f %f) e(%f %f %f) id(%d)",
                frontElem.start.x, frontElem.start.y, frontElem.start.z,
                frontElem.end.x, frontElem.end.y, frontElem.end.z,
                frontElem.queryID
            );
        }

        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Results")) {
        std::lock_guard<std::mutex> lock(m_resultMutex);

        ImGui::Text("Count: %d", m_results.size());
        for(const auto& result : m_results) {
            char buf[220];
            sprintf_s(buf, "Result %d", result.first);

            if(ImGui::TreeNode(buf)) {
                if(ImGui::Button("Display Result")) {
                    m_debugDisplayPath = result.first;
                }
                for(const auto& p : result.second) {
                    ImGui::Text("%f %f %f", p.x, p.y, p.z);
                }
                ImGui::TreePop();
            }
        }
        
        ImGui::TreePop();
    }

    static NJS_POINT3 spos, epos;
    if(ImGui::Button("Set Start")) {
        spos = MainCharObj1[0]->Position;
    }

    if(ImGui::Button("Set End")) {
        epos = MainCharObj1[0]->Position;
    }

    static bool excludeSwim = false;
    ImGui::Checkbox("Exclude Swim", &excludeSwim);

    if(ImGui::Button("Query and Display")) {
        m_debugDisplayPath = QueryPath(spos, epos, !excludeSwim ? 0 : NAV_FLAGS_SWIM);
    }

    if(ImGui::Button("Query and Run BHV")) {
        CreatePathAtPos(0, epos);
    }
}