#include "stdafx.h"

#include <chrono>

#include "navsys_internal.h"
#include "navsys_generator.h"
#include "navsys.h"

#include <al_behavior/albhv_navigation.h>

#ifdef IMGUIDEBUG
    #include "imgui/imgui.h"
#endif

void NavSysGenerator::ImGuiDebug() {
    if(GetNavSysTask()) {
        GetNavSysTask()->Data1.Entity->Position.y = m_config.m_agentMaxClimb;
    }
    
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

void NavSys::ImGuiDebug() {
    using namespace std::chrono_literals;

    if(m_loadingNavMeshResult.valid()) {
        ImGui::Text("m_loadingNavMeshResult: %d", m_loadingNavMeshResult.wait_for(0ms));
    }

    ImGui::Text("m_navMesh: %s", m_navMesh.get() ? "Loaded" : "NULL");
    ImGui::Text("WorkerStop: %d", m_workerStop.load());
    ImGui::Text("m_queryIndex: %d", m_queryIndex);

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
                    GetNavSysTask()->Data1.Entity->Rotation.x = result.first;
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
        GetNavSysTask()->Data1.Entity->Rotation.x = AddPath(spos, epos, !excludeSwim ? 0 : NAV_FLAGS_SWIM);
    }

    if(ImGui::Button("Query and Run BHV")) {
        CreatePathAtPos(0, epos);
    }
}