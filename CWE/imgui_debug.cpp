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
#include <al_draw.h>
#include <api/api_accessory.h>
#include <al_behavior/al_behavior.h>
#include <al_behavior/albhv_bully.h>
#include <al_daynight_rain.h>
#include <data/more_faces.h>

static int SelectedChaoIndex;
static int SelectedOtherChaoIndex;
static bool ShowChaoInfo = false;
static bool ShowDNC = false;
static bool ShowLight = false;
static bool ShowTaskList = false;
static bool ShowChaoSoundMenu = false;
static bool ShowItemsMenu = false;
static bool ShowAccessoryMenu = false;
static bool ShowMarketMenu = false;
static bool ShowSoundsMenu = false;
static bool ShowMoreFacesMenu = false;

static task* GetSelectedChao() {
    return GetChaoObject(0, SelectedChaoIndex);
}

static task* GetSelectedOtherChao() {
    return GetChaoObject(0, SelectedOtherChaoIndex);
}

static int SetMusicBhv(task* tp) {
    ((task_exec)0x0059D410)(tp);

    return BHV_RET_CONTINUE;
}

static int SetSToyBhv(task* tp) {
    ((task_exec)0x55E7A0)(tp);

    return BHV_RET_CONTINUE;
}

static void MoreFacesMenu() {
    if(!ShowMoreFacesMenu) return;
    if(!playerpwp[0]);

    task* pHeld = playerpwp[0]->htp;
    if(!pHeld) return;
    if(pHeld->exec != ChaoExecutor) return;
    
    static int index = 0;

    static const char* PersonalityStrings[] = {
        "CURIOSITY",
        "KINDNESS",
        "AGRESSIVE",
        "Naive/SLEEPY_HEAD",
        "SOLITUDE",
        "VITALITY",
        "GLUTTON",
        "REGAIN",
        "SKILLFUL",
        "CHARM",
        "CHATTY",
        "Carefree/CALM",
        "FICKLE"
    };

    static const char* RangeStrings[] = {
        "Low",
        "Mid",
        "High"
    };

    if(ShowMoreFacesMenu && ImGui::Begin("More Faces Debug", &ShowMoreFacesMenu)) {
        ImGui::SliderInt("Select", &index, 0, _countof(NewFaceEntries) - 1);

        const auto& entry = NewFaceEntries[index];

        AL_FaceChangeEye(pHeld, entry.Eye);
        AL_FaceChangeMouth(pHeld, entry.Mouth);
        
        for(size_t i = 0; i < entry.PersonalityCount; ++i) {
            ImGui::Text("%s %s", RangeStrings[entry.PersonalityCheckRanges[i]], PersonalityStrings[entry.PersonalityKinds[i] - EM_PER_CURIOSITY]);
        }

        ImGui::End();
    }
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
        auto pParamCwe = GET_CWEPARAM(pChao);
        auto* move_work = pChao->mwp;

        if (ImGui::BeginTabBar("chao_tab_bar")) {
            if (ImGui::BeginTabItem("General")) {
                ImGui::InputScalarN("Position", ImGuiDataType_Float, &work->pos, 3);

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

            if (ImGui::BeginTabItem("Accessories")) {
                const auto& items = ItemMetadata::Get()->GetIDs(ChaoItemCategory_Accessory);

                static const char* AccessoryStrings[3000];
                static size_t AccessoryIndices[3000];
                int setID[4] = { -1, -1, -1, -1 };

                const char* slotNames[] = {
                    "Head",
                    "Face",
                    "Generic1",
                    "Generic2"
                };

                for (size_t i = 0; i < 4; ++i) {
                    ImGui::Text(
                        "%s (internal data ParamID:%s Data1ID:%s Index: %d)", 
                        slotNames[i],
                        pParamCwe->Accessories[i].ID, 
                        work->AccessoryCalculatedID[i], 
                        work->AccessoryIndices[i]
                    );

                    size_t count = 0;
                    for (size_t id = 0; id < items.size(); ++id) {
                        if (GetAccessoryData(id).SlotType != i) continue;
                        AccessoryIndices[count] = id;
                        AccessoryStrings[count++] = items[id].data();
                    }
                    AccessoryStrings[count] = NULL;

                    ImGui::PushID(i);
                    if (ImGui::Combo("Set ID", &setID[i], AccessoryStrings, count)) {
                        strcpy_s(pParamCwe->Accessories[i].ID, AccessoryStrings[setID[i]]);
                        AL_SetAccessory(pChao, AccessoryIndices[setID[i]]);
                    }
                    ImGui::PopID();

                    if (work->AccessoryIndices[i] == -1) continue;
                    
                    for (size_t j = 0; j < GetAccessoryColorCount(work->AccessoryIndices[i]); ++j) {
                        NJS_COLOR* pCol = (NJS_COLOR*) & pParamCwe->Accessories[i].ColorSlots[j];
                        ImGui::PushID(i * 10 + j);
                        float col[3];
                        col[0] = pCol->argb.r / 255.f;
                        col[1] = pCol->argb.g / 255.f;
                        col[2] = pCol->argb.b / 255.f;
                        if (ImGui::ColorEdit3("color", col)) {
                            pCol->argb.r = Uint8(col[0] * 255.f);
                            pCol->argb.g = Uint8(col[1] * 255.f);
                            pCol->argb.b = Uint8(col[2] * 255.f);
                            pParamCwe->Accessories[i].ColorFlags |= (1 << j);
                        }
                        ImGui::PopID();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("ParamFlags")) {
                const char* paramFlagNames[] = {
                    "NAME_NEW",
                    "OLD_GUEST_CHECK",
                    "PARTS_CONVERSION",
                    "ACCESSORIES_NEW",
                };
                for (size_t i = 0; i < 4; ++i) {
                    ImGui::CheckboxFlags(paramFlagNames[i], &pParamCwe->Flags, (1 << i));
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
                    if (ImGui::Button("Music")) {
                        AL_SetBehavior(pChao, SetMusicBhv);
                    }
                    if (ImGui::Button("SToy")) {
                        AL_SetBehavior(pChao, SetSToyBhv);
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
                    &work->Perception.Player,
                    &work->Perception.Chao,
                    &work->Perception.Fruit,
                    &work->Perception.Tree,
                    &work->Perception.Toy
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
                    ImGui::Text("Toy Name: %s", pToy->name);
                    ImGui::Separator();
                }
                
                ImGui::Text("SightRange: %f", work->Perception.SightRange);
                ImGui::Text("SightAngle: %d (%f deg)", work->Perception.SightAngle, NJM_ANG_DEG(work->Perception.SightAngle));
                ImGui::Text("SightAngleHalf: %d (%f deg)", work->Perception.SightAngleHalf, NJM_ANG_DEG(work->Perception.SightAngleHalf));
                ImGui::Text("HearRange: %f", work->Perception.HearRange);
                ImGui::Text("SmellRange: %f", work->Perception.SmellRange);

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
                                    const char* taskName = link->pEntry->tp->name;
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
                {
                    bool flagEnabled = (work->Flag & 2);
                    if(ImGui::Checkbox("Shape Deform", &flagEnabled)) {
                        if(flagEnabled) {
                            work->Flag |= 2;
                        }
                    }
                }
                
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
                    bool flagEnabled = (work->ChaoFlag & bits[i]);
                    ImGui::Checkbox(flagsStrings[i], &flagEnabled);
                    if (!flagEnabled) {
                        work->ChaoFlag &= ~bits[i];
                    }
                    else {
                        work->ChaoFlag |= bits[i];
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

                ImGui::Text("Pose: %s (%d)", poseNames[work->MotionCtrl.posture + 1], work->MotionCtrl.posture);

                static int animID = 0;
                int currentAnimID = work->MotionCtrl.curr_num;
                ImGui::Text("Current Animation: %s (%d)", MotionNames[currentAnimID], currentAnimID);
                ImGui::Text("Speed: %f %f", work->MotionCtrl.PlaySpeed2, work->MotionCtrl.PlaySpeed);

                ImGui::Combo("Filter for pose: %s (%d)", &poseFilter, poseNames, IM_ARRAYSIZE(poseNames));
                if (poseFilter > 0) {
                    int pose = poseFilter - 1;
                    if (ImGui::TreeNode("Filter results")) {
                        MotionTableAction* entries = (MotionTableAction*)GetDataDllProcAddr("al_motion_table");
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
                ImGui::Text("Aim: %f %f %f", move_work->AimPos.x, move_work->AimPos.y, move_work->AimPos.z);
                ImGui::Text("DistFromAim: %f", MOV_DistFromAim(pChao));
                ImGui::Text("DistFromAimXZ: %f", MOV_DistFromAimXZ(pChao));

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Vertices")) {
                AL_OBJECT* pHead = work->Shape.CurrObjectList[1];
                if(pHead) {
                    AL_MODEL* pModel = (AL_MODEL*)pHead->pModel;

                    for(size_t i = 0; i < pModel->nbVertex; ++i) {
                        CNK_VN_VERTEX* pVert = (CNK_VN_VERTEX*)(pModel->VList + 2) + i;
                        ImGui::Text(
                            "%d: Pos %f %f %f Nrm %f %f %f Mag %f",
                            int(i),
                            pVert->Pos.x, pVert->Pos.y, pVert->Pos.z,
                            pVert->Normal.x, pVert->Normal.y, pVert->Normal.z,
                            njScalor(&pVert->Normal)
                        );
                    }
                }

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
        auto& work = *reinterpret_cast<DAYNIGHT_WORK*>(pDayNightTask->awp);
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
            PlaySound_XYZ(SoundID, &playertwp[0]->pos, 0, 0, 110);
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
                        ImGui::Text(!obj->name ? "" : obj->name);
                        obj = obj->last;
                    } while (obj != ObjectLists[i]);
                }

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}

static void ItemsMenu() {
    if (ShowItemsMenu && ImGui::Begin("Items", &ShowItemsMenu)) {
        for (size_t i = 0; i < AccessoryItemList.size(); ++i) {
            ImGui::PushID(i);
            if (ImGui::TreeNode(&AccessoryItemList[i], "%d", int(i))) {
                ImGui::InputInt("IndexID", &AccessoryItemList[i].IndexID);
                ImGui::InputInt("Garden", &AccessoryItemList[i].Garden);
                ImGui::InputText("ID", AccessoryItemList[i].ID, 20);
                ImGui::InputFloat3("Position", &AccessoryItemList[i].Position.x);
                ImGui::InputInt("Angle", &AccessoryItemList[i].Angle);

                ImGui::TreePop();
            }
            ImGui::PopID();
          
        }
        ImGui::End();
    }
}

static void AccessoryInternals() {
    if(ShowAccessoryMenu && ImGui::Begin("Accessory Internals", &ShowAccessoryMenu)) {
        for(size_t i = 0; i < GetAccessoryCount(); ++i) {
            auto& data = GetAccessoryData(i);
            
            if(ImGui::TreeNode(data.ID)) {
                bool isNoRf = data.Flags & CWE_API_ACCESSORY_FLAGS_NO_RF_NORMALDRAW_SUPPORT;
                if(ImGui::Checkbox("NoRFNormalDraw", &isNoRf)) {
                    if(!isNoRf) {
                        data.Flags &= ~CWE_API_ACCESSORY_FLAGS_NO_RF_NORMALDRAW_SUPPORT;
                    }
                    else {
                        data.Flags |= CWE_API_ACCESSORY_FLAGS_NO_RF_NORMALDRAW_SUPPORT;
                    }
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

static void MarketMenu() {
    if(ShowMarketMenu && ImGui::Begin("Market", &ShowMarketMenu)) {
        for(size_t i = 0; i < BlackMarketCategories_Length; ++i) {
            if(!BlackMarketCategories[i].Count) {
                continue;
            }

            if(ImGui::TreeNode(&BlackMarketCategories[i].attrib, "Category %d", int(i))) {
                for(size_t j = 0; j < BlackMarketCategories[i].Count; ++j) {
                    const auto& entry = BlackMarketCategories[i].attrib[j];

                    if(ImGui::TreeNode(&entry, "Entry %d", int(j))) {
                        ImGui::Text("Name: %s", MsgAlItem[entry.Name]);
                        ImGui::Text("Description: %s", MsgAlItem[entry.Descriptions]);
                        ImGui::Text("Purchase/Sale: %d %d", entry.PurchasePrice, entry.SalePrice);
                        ImGui::Text("Emblems: %d", entry.RequiredEmblems);

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }
        }

        ImGui::End();
    }
}

static void SoundsMenu() {
    typedef struct {
        int8_t      mbank;
        int8_t      index;

        int8_t      pri;            /* priority */
        int8_t      angle;          /* sound panning */

        int8_t      volcur;
        int8_t      vol;
        int8_t      volmax;

        uint8_t      bflag;
        uint16_t    mode;

        int16_t     pitch;
        int16_t     timer;
        uint16_t    sctimer;

        void*       idp;            /* play id              */
        uint16_t    tone;           /* sound number         */

        NJS_VECTOR* pPos;
        NJS_VECTOR  pos;

        float       distmax;
        float       distcur;
    } SEENTRY;

    #define SEWK_FXFLAG_ON       (1<<0) /* enable forced reverb value                   */
    #define SEWK_FXFLAG_GAME     (1<<1) /* set reverb value on sound pause/resume       */

    typedef struct {
        uint8_t          fxflag;
        uint8_t          fxlev_stop;         /* reverb level when paused          [-127~127] */
        uint8_t          fxlev_game;         /* reverb level when playing         [-127~127] */

        uint8_t          next_snd;           /* next sound buffer entry               [0~38] */
        uint8_t          next_seq;           /* next seq buffer entry                [39~42] */

        uint8_t          unk1[3];

        float            unkBankVal;         /* unknown, always -1                           */

        uint8_t           unk2[4];

        SEENTRY             sebuf[43];           /* sound call buffer                            */

        uint8_t          unk3[32];

        void*   bank[8];            /* bank lists                                   */
    } SEWORK;

    DataPointer(SEWORK*, sework, 0x01A55874);
    if (ShowSoundsMenu && ImGui::Begin("SoundSystem", &ShowSoundsMenu)) {
        if(ImGui::BeginTable("sndtbl", 16, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter)) {
            ImGui::TableSetupColumn("mbank");
            ImGui::TableSetupColumn("index");
            ImGui::TableSetupColumn("pri");
            ImGui::TableSetupColumn("angle");
            ImGui::TableSetupColumn("volcur");
            ImGui::TableSetupColumn("vol");
            ImGui::TableSetupColumn("volmax");
            ImGui::TableSetupColumn("bflag");
            ImGui::TableSetupColumn("mode");
            ImGui::TableSetupColumn("pitch");
            ImGui::TableSetupColumn("timer");
            ImGui::TableSetupColumn("sctimer");
            ImGui::TableSetupColumn("idp");
            ImGui::TableSetupColumn("tone");
            ImGui::TableSetupColumn("distmax");
            ImGui::TableSetupColumn("distcur");
            ImGui::TableHeadersRow();

            for(size_t i = 0; i < _countof(sework->sebuf); ++i) {
                const auto& entry = sework->sebuf[i];

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.mbank);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.index);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.pri);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.angle);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.volcur);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.vol);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.volmax);
                ImGui::TableNextColumn();

                ImGui::Text("%x", uint32_t(entry.bflag));
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.mode);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.pitch);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.timer);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.sctimer);
                ImGui::TableNextColumn();

                ImGui::Text("%x", entry.idp);
                ImGui::TableNextColumn();

                ImGui::Text("%d", entry.tone);
                ImGui::TableNextColumn();

                ImGui::Text("%f", entry.distmax);
                ImGui::TableNextColumn();

                ImGui::Text("%f", entry.distcur);
            }
            ImGui::EndTable();
        }
    
        ImGui::End();
    }
}

static void ImGuiMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menus")) {
            ImGui::MenuItem("Chao", NULL, &ShowChaoInfo);
            ImGui::MenuItem("Light", NULL, &ShowLight);
            // ImGui::MenuItem("DayNight Cycle", NULL, &ShowDNC);
            ImGui::MenuItem("Sound", NULL, &ShowChaoSoundMenu);
            ImGui::MenuItem("Tasks", NULL, &ShowTaskList);
            ImGui::MenuItem("Items", NULL, &ShowItemsMenu);
            ImGui::MenuItem("Accessory Internals", NULL, &ShowAccessoryMenu);
            ImGui::MenuItem("Market", NULL, &ShowMarketMenu);
            ImGui::MenuItem("Sound System", NULL, &ShowSoundsMenu);
            ImGui::MenuItem("More Chao Faces", NULL, &ShowMoreFacesMenu);
            
            ImGui::EndMenu();
        }

        ChaoSoundMenu();
        ChaoInfoMenu();
        // DayNightMenu();
        LightMenu();
        ItemsMenu();
        TaskListMenu();
        AccessoryInternals();
        MarketMenu();
        SoundsMenu();
        MoreFacesMenu();

        ImGui::EndMainMenuBar();
    }

    if (false && ImGui::Begin("rain test")) {
        static int timer = 4000;
        ImGui::SliderInt("timer", &timer, 0, 10000);

        if (ImGui::Button("spawn")) {
            raintp = AL_CreateDayNightRain(timer, 0xA0A0A0FF);
            raintp->dest = RainDelete;
        }

        if (raintp) {
            auto* work = (RAIN_WORK*)raintp->awp;
            
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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_EnableDpiAwareness();

    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
    
    // m_renderDevice->m_initInfo.BackBufferWidth/Height
	DataPointer(void*, dword_1A557C0, 0x1A557C0);
    io.DisplaySize = { (float)*(int*)((int)dword_1A557C0 + 0x1C), (float)*(int*)((int)dword_1A557C0 + 0x1C + 4) };
    io.DisplayFramebufferScale = {1, 1};

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

	ImGui_ImplWin32_Init(MainWindowHandle);
	ImGui_ImplDX9_Init(cwe_device);

}