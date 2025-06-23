#include "stdafx.h"
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>
#include <exception>
#include <FunctionHook.h>
#include <d3d9.h>
#include <al_world.h>
#include <Chao.h>
#include <al_behavior/albhv.h>
#include <al_behavior/alsbhv.h>
#include <al_daynight.h>
#include <ChaoMain.h>
#include <al_draw.h>
#include <imgui_debug_motionstrings.h>
#include <al_daynight_rain.h>

static int SelectedChaoIndex;
static int SelectedOtherChaoIndex;
static bool ShowChaoInfo = false;
static bool ShowDNC = false;
static bool ShowLight = false;
static bool ShowTaskList = false;
static bool ShowChaoSoundMenu = false;

static task* GetSelectedChao() {
    return GetChaoObject(0, SelectedChaoIndex);
}

static task* GetSelectedOtherChao() {
    return GetChaoObject(0, SelectedOtherChaoIndex);
}

static void ChaoInfoMenu() {
    if (ShowChaoInfo && ImGui::Begin("Chao Info", &ShowChaoInfo)) {
        ImGui::SliderInt("ID", &SelectedChaoIndex, 0, ALW_CountEntry(0));

        task* pChao = GetSelectedChao();
        if (!pChao) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
            ImGui::Text("Chao with selected ID not found!");
            ImGui::PopStyleColor();
            ImGui::End();
            return;
        }

        chaowk* work = GET_CHAOWK(pChao);
        auto* move_work = pChao->EntityData2;

        if (ImGui::BeginTabBar("chao_tab_bar")) {
            if (ImGui::BeginTabItem("General")) {
                ImGui::InputScalarN("Position", ImGuiDataType_Float, &work->entity.Position, 3);

                static bool ChaoDebugDistEnabled = false;
                ImGui::Checkbox("Distance Debug", &ChaoDebugDistEnabled);
                if (ChaoDebugDistEnabled) {
                    ChaoDebugDistSelected = pChao;
                    ImGui::SliderFloat("Distance", &ChaoDebugDist, 1, 1000);
                }
                else {
                    ChaoDebugDistSelected = NULL;
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Behavior")) {
                if (ImGui::TreeNode("Start behaviors")) {
                    if (ImGui::Button("Piano")) {
                        AL_SetBehavior(pChao, ALBHV_GoToPiano);
                    }
                    if (ImGui::Button("Boat")) {
                        AL_SetBehavior(pChao, ALBHV_GoToBoat);
                    }
                    if (ImGui::Button("Float")) {
                        AL_SetBehavior(pChao, ALBHV_GoToFloat);
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Start Social Behaviors")) {
                    ImGui::SliderInt("Other Chao ID", &SelectedOtherChaoIndex, 0, ALW_CountEntry(0));

                    task* pOtherChao = GetSelectedOtherChao();
                    if (pOtherChao) {
                        ALW_LockOn(pChao, pOtherChao);

                        if (ImGui::Button("Talk")) {
                            AL_SetBehavior(pChao, ALBHV_Talk);
                        }
                        if (ImGui::Button("Gossip")) {
                            ALBHV_Gossip(pChao, pOtherChao);
                        }
                        if (ImGui::Button("Hold Hands")) {
                            AL_SetBehavior(pChao, ALBHV_InitHoldHands);
                        }
                        if (ImGui::Button("Hug")) {
                            AL_SetBehavior(pChao, ALBHV_InitHug);
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Perception")) {
                const AL_PERCEPTION_INFO* perceptionList[] = {
                    &work->PlayerObjects,
                    &work->ChaoObjects,
                    &work->FruitObjects,
                    &work->TreeObjects,
                    &work->ToyObjects
                };

                const char* const perceptionDropDownNames[] = {
                    "Players",
                    "Chao",
                    "Fruit",
                    "Tree",
                    "Toy"
                };

                static_assert(_countof(perceptionList) == _countof(perceptionDropDownNames));

                task* pToy = AL_GetFoundToyTask(pChao);
                if (pToy) {
                    ImGui::Text("Toy Name: %s", pToy->Name);
                    ImGui::Separator();
                }
                
                ImGui::Text("SightRange: %f", work->ObjectListInfo.SightRange);
                ImGui::Text("SightAngle: %d (%f deg)", work->ObjectListInfo.SightAngle, NJM_ANG_DEG(work->ObjectListInfo.SightAngle));
                ImGui::Text("SightAngleHalf: %d (%f deg)", work->ObjectListInfo.SightAngleHalf, NJM_ANG_DEG(work->ObjectListInfo.SightAngleHalf));
                ImGui::Text("HearRange: %f", work->ObjectListInfo.HearRange);
                ImGui::Text("SmellRange: %f", work->ObjectListInfo.SmellRange);

                for (size_t i = 0; i < _countof(perceptionList); i++) {
                    const AL_PERCEPTION_INFO* perception = perceptionList[i];
                    if (ImGui::TreeNode(perceptionDropDownNames[i])) {
                        ImGui::Text("InSightFlag: %d", perception->InSightFlag);
                        ImGui::Text("HeardFlag: %d", perception->HeardFlag);
                        ImGui::Text("SmellFlag: %d", perception->SmellFlag);
                        ImGui::Text("NearestDist: %f", perception->NearestDist);
                        ImGui::Text("NearestNum: %d", perception->NearestNum);

                        char buf[40];
                        sprintf_s(buf, "%s_%d", perceptionDropDownNames[i], i);

                        if (ImGui::TreeNode(buf, "Entries (%d)", perception->nbPerception)) {
                            for (size_t j = 0; j < perception->nbPerception; j++) {
                                sprintf_s(buf, "%s_%d_%d", perceptionDropDownNames[i], i, j);
                                if (!ImGui::TreeNode(buf, "Entry %d", int(j))) continue;

                                auto* link = &perception->field_18[j];
                                ImGui::Text("Info: %h %h %h %h", link->info[0], link->info[1], link->info[2], link->info[3]);
                                ImGui::Text("dist: %f", link->dist);
                                ImGui::Text("InSightFlag: %d", link->InSightFlag);
                                ImGui::Text("HearFlag: %d", link->HearFlag);
                                ImGui::Text("SmellFlag: %d", link->SmellFlag);

                                // todo: pEntry view opener
                                if (!link->pEntry) {
                                    ImGui::Text("No al_entry_work?");
                                }
                                else if (!link->pEntry->tp) {
                                    ImGui::Text("No al_entry_work->tp?");
                                }
                                else {
                                    const char* taskName = link->pEntry->tp->Name;
                                    ImGui::Text("Name: %s", !taskName ? "(no name)" : taskName);
                                }

                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Flags")) {
                const Uint32 bits[] = {
                    BIT_1,
                    BIT_2,
                    BIT_3,
                    BIT_4,
                    BIT_5,
                    BIT_9,
                    BIT_10,
                    BIT_11,
                    BIT_12,
                    BIT_13
                };

                const char* const flagsStrings[] = {
                    "Move",
                    "Motion",
                    "CCL",
                    "BHV",
                    "Buyo",
                    "Draw",
                    "Hold Left",
                    "Hold Right", 
                    "Is Visible",
                    "Race BHV"
                };
                
                static_assert(_countof(flagsStrings) == _countof(bits));

                for (size_t i = 0; i < _countof(flagsStrings); i++) {
                    bool flagEnabled = (work->field_B0 & bits[i]);
                    ImGui::Checkbox(flagsStrings[i], &flagEnabled);
                    if (!flagEnabled) {
                        work->field_B0 &= ~bits[i];
                    }
                    else {
                        work->field_B0 |= bits[i];
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("MotionTable")) {
                const char* const poseNames[] = {
                    "None",
                    "AL_PST_STAND",
                    "AL_PST_SIT",
                    "AL_PST_LIE",
                    "AL_PST_FUSE",
                    "AL_PST_OTHERS"
                };

                static int poseFilter = 0;

                ImGui::Text("Pose: %s (%d)", poseNames[work->MotionTable.gap2 + 1], work->MotionTable.gap2);

                static int animID = 0;
                int currentAnimID = work->MotionTable.AnimID;
                ImGui::Text("Current Animation: %s (%d)", MotionNames[currentAnimID], currentAnimID);

                ImGui::Combo("Filter for pose: %s (%d)", &poseFilter, poseNames, IM_ARRAYSIZE(poseNames));
                if (poseFilter > 0) {
                    int pose = poseFilter - 1;
                    if (ImGui::TreeNode("Filter results")) {
                        MotionTableAction* entries = (MotionTableAction*)GetDllData("al_motion_table");
                        for (size_t i = 0; i <= ALM_TAIL; i++) {
                            if (entries[i].field_6 != pose) continue;
                            ImGui::Text("%s (%d)", MotionNames[i], i);
                        }
                        ImGui::TreePop();
                    }
                }

                ImGui::Combo("Animation", &animID, MotionNames, IM_ARRAYSIZE(MotionNames));

                ImGui::InputInt("Anim ID", &animID);
                ImGui::SameLine();
                if (ImGui::Button("Link")) {
                    AL_SetMotionLink(pChao, animID);
                }

                static float motionSpd = 1.0f;
                ImGui::SliderFloat("Motion Spd", &motionSpd, 0, 2);
                ImGui::SameLine();
                if (ImGui::Button("Set")) {
                    AL_SetMotionSpd(pChao, motionSpd);
                }
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Move")) {
                ImGui::Text("Aim: %f %f %f", move_work->Waypoint.x, move_work->Waypoint.y, move_work->Waypoint.z);
                ImGui::Text("DistFromAim: %f", MOV_DistFromAim(pChao));
                ImGui::Text("DistFromAimXZ: %f", MOV_DistFromAimXZ(pChao));

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

static void DayNightMenu() {
    if (!pDayNightTask) return;
   
    if (ShowDNC && ImGui::Begin("DayNight Cycle", &ShowDNC)) {
        auto& work = *reinterpret_cast<DAYNIGHT_WORK*>(pDayNightTask->Data2.Undefined);
        ImGui::SliderInt("Timer", (int*) & work.timer, 0, 24 * gConfigVal.DayNightCycleHourFrame);
        ImGui::Text("Phase: %d", work.phase);
        ImGui::Text("Day: %d", work.day);
        ImGui::Text("NextCloudyFlag: %d", work.nextDayCloudy);
        ImGui::Text("Timer: %d", work.timer);
        ImGui::Text("Hour: %d", work.timer / gConfigVal.DayNightCycleHourFrame);
        ImGui::Text("PhaseA %d", work.phaseA);
        ImGui::Text("PhaseB %d", work.phaseB);
        ImGui::Text("Lerp %f", work.lerpValue);
        ImGui::End();
    }
}

static void LightMenu() {
    if (ShowLight && ImGui::Begin("Light", &ShowLight)) {
        if (ImGui::TreeNode("DC Lights")) {
            for (size_t i = 0; i < 12; i++) {
                char buf[40];
                sprintf_s(buf, "Light %d", int(i));

                if (ImGui::TreeNode(buf)) {
                    auto& light = Lights[i];

                    ImGui::InputFloat("Ambient", &light.ambient);
                    ImGui::InputFloat("Intensity", &light.intensity);
                    ImGui::ColorEdit3("Color", &light.color.x);
                    ImGui::InputScalarN("Direction", ImGuiDataType_Float, &light.direction, 3);

                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("GC Lights")) {
            for (size_t i = 0; i < 12; i++) {
                char buf[40];
                sprintf_s(buf, "Light %d", int(i));

                if (ImGui::TreeNode(buf)) {
                    auto& light = LightsGC[i];

                    ImGui::Checkbox("Use GC", (bool*) & light.SomeFlag);
                    ImGui::ColorEdit3("Ambient", &light.ambientReg.x);
                    ImGui::ColorEdit3("Color", &light.lightColor.x);
                    ImGui::InputScalarN("Direction", ImGuiDataType_Float, &light.direction, 3);

                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }
}

static void ChaoSoundMenu() {
    static int SoundID = 0x6000;
    if (ShowChaoSoundMenu && ImGui::Begin("Sound", &ShowChaoSoundMenu)) {
        ImGui::Text("%x", SoundID);
        if (ImGui::Button("+")) {
            SoundID++;
        }
        ImGui::SameLine();
        if (ImGui::Button("++")) {
            SoundID += 0x10;
        }
        ImGui::SameLine();
        if (ImGui::Button("-")) {
            SoundID--;
        }
        ImGui::SameLine();
        if (ImGui::Button("--")) {
            SoundID-= 0x10;
        }

        if (ImGui::Button("Play")) {
            PlaySound_XYZ(SoundID, &MainCharObj1[0]->Position, 0, 0, 110);
        }

        if (ImGui::Button("Stop Music")) {
            StopMusic();
        }

        ImGui::End();
    }
}

static void TaskListMenu() {
    if (ShowTaskList && ImGui::Begin("Tasks", &ShowTaskList)) {
        for (size_t i = 0; i < ObjectLists_Length; i++) {
            char path[40];
            sprintf_s(path, "List %d", int(i));

            if (ImGui::TreeNode(path)) {
                auto* obj = ObjectLists[i];
                if (obj) {
                    do {
                        ImGui::Text(!obj->Name ? "" : obj->Name);
                        obj = obj->NextObject;
                    } while (obj != ObjectLists[i]);
                }

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}

static task* raintp = NULL;
static void RainDelete(task* tp) {
    raintp = NULL;
}

static void ImGuiMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menus")) {
            ImGui::MenuItem("Chao", NULL, &ShowChaoInfo);
            ImGui::MenuItem("Light", NULL, &ShowLight);
            ImGui::MenuItem("DayNight Cycle", NULL, &ShowDNC);
            ImGui::MenuItem("Sound", NULL, &ShowChaoSoundMenu);
            ImGui::MenuItem("Tasks", NULL, &ShowTaskList);
            ImGui::EndMenu();
        }

        ChaoSoundMenu();
        ChaoInfoMenu();
        DayNightMenu();
        LightMenu();
        TaskListMenu();

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("rain test")) {
        static int timer = 4000;
        ImGui::SliderInt("timer", &timer, 0, 10000);

        if (ImGui::Button("spawn")) {
            raintp = AL_CreateDayNightRain(timer, 0xA0A0A0FF);
            raintp->DeleteSub = RainDelete;
        }

        if (raintp) {
            auto* work = (RAIN_WORK*)raintp->Data2.Undefined;
            
            for (size_t i = 0; i < DROP_COUNT; i++) {
                if (!work->drops[i].lifeCount) {
                    char num[20];
                    sprintf_s(num, "inactive %d", int(i));
                    if (ImGui::TreeNode(num)) {
                        ImGui::TreePop();
                    }
                    continue;
                }

                char num[10];
                sprintf_s(num, "drop %d", int(i));

                if (ImGui::TreeNode(num)) {
                    ImGui::Text("pos %f %f %f", work->drops[i].startPos.x, work->drops[i].startPos.y, work->drops[i].startPos.z);
                    ImGui::Text("scale %d", int(work->drops[i].scale));
                    ImGui::Text("progress %f", work->drops[i].progress);
                    ImGui::TreePop();
                }
                
            }
        }

        ImGui::End();
    }
}

// imgui implementation below, keep cwe logic above here

extern IDirect3DDevice9* cwe_device;

FunctionHook<void> BeginScene(0x00424740);
FunctionHook<void> FuncBeforeEndScene(0x44C260);
StdcallFunctionHook<LRESULT, HWND, UINT, WPARAM, LPARAM> WndProcFunc(0x00401810);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return WndProcFunc.Original(hWnd, uMsg, wParam, lParam);
}

ImGuiKey ImGui_ImplWin32_KeyEventToImGuiKey(WPARAM wParam, LPARAM lParam);
static void ImGuiDraw() {

    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < 256; i++) {
        io.AddKeyEvent(ImGui_ImplWin32_KeyEventToImGuiKey(i, 0), GetAsyncKeyState(i) <  0);
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiMenu();

    ImGui::EndFrame();
}

static void BeginSceneHook() {
    ImGuiDraw();
    BeginScene.Original();
}

static void BeforeEndSceneHook() {
    FuncBeforeEndScene.Original();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Init() {
    BeginScene.Hook(BeginSceneHook);
    FuncBeforeEndScene.Hook(BeforeEndSceneHook);
    WndProcFunc.Hook(WndProcHook);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(MainWindowHandle);
	ImGui_ImplDX9_Init(cwe_device);
}