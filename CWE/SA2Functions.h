#pragma once

#include <stdint.h>
#include "MemAccess.h"
#include "SA2Structs.h"
#include "SA2Enums.h"

#define ObjectFunc(NAME, ADDRESS) FunctionPointer(void,NAME,(ObjectMaster *obj),ADDRESS)
// SA2 Functions
StdcallFunctionPointer(LRESULT, WndProc, (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam), 0x401810);
ThiscallFunctionPointer(int, CopyString_, (void *_this, const char *Src), 0x401E90);
ThiscallFunctionPointer(void *, CopyString, (void *destination, const void *source, rsize_t size), 0x402250);
StdcallFunctionPointer(BOOL, DialogFunc, (HWND, UINT, WPARAM, LPARAM), 0x403BD0);
ThiscallFunctionPointer(LaunchConfig *, LaunchConfig__dtor, (LaunchConfig *_this, char a2), 0x4052C0);
FunctionPointer(int, GetConfig, (), 0x4052E0);
FunctionPointer(int, Menu_BonusVideo, (), 0x40B410);
FunctionPointer(int, Menu_HowToPlay, (), 0x40C680);
FunctionPointer(int, Menu_Controls, (), 0x40D0F0);
FunctionPointer(int, SteamStatisticsStuff, (), 0x40E880);
StdcallFunctionPointer(int, nullsub_5, (int), 0x411600);
FunctionPointer(int, GetCrappyReplacementDepartureMachine, (), 0x41DFB0);
FunctionPointer(int, controller_useless, (), 0x425700);
FunctionPointer(void, MemoryManager__Deallocate, (AllocatedMem *data, char *a2, int a3), 0x425B50);
FunctionPointer(int, PrintDebug, (const char*, ...), 0x426740);
FunctionPointer(int, ProcessChunkModel, (NJS_CNK_MODEL *a1), 0x42D650);
FunctionPointer(void, ProcessChunkModelsWithCallback, (NJS_OBJECT *object, int(__cdecl *callback)(NJS_CNK_MODEL *)), 0x42EB30);
FunctionPointer(void, LoadTextureList_NoName, (NJS_TEXLIST *), 0x42FD10);
VoidFunc(main_gc_free, 0x433E60);
FunctionPointer(int, GameLoop, (), 0x433EE0);
FunctionPointer(signed int, GameModeHandler, (), 0x434160);
VoidFunc(main_gc_alloc, 0x434F00);
ThiscallFunctionPointer(int, LoadMLT, (const char *name), 0x435880);
FunctionPointer(int, Menu_Unknown_13, (), 0x436040);
ObjectFunc(VibTask_Delete, 0x438C90);
ObjectFunc(VibTask_Main, 0x438D30);
FunctionPointer(int, VibTask_Load, (char, char, char, int, int), 0x438DC0);
FunctionPointer(int, ResetSeedAndCharacter, (), 0x4395C0);
FunctionPointer(signed int, GameStateHandler, (), 0x43A780);
FunctionPointer(int, SetNextLevel, (), 0x43C4D0);
VoidFunc(MissionStartVariableSetup, 0x43C8B0);
VoidFunc(InitCurrentLevelAndScreenCount, 0x43CB10);
VoidFunc(CallControlAndDoOtherThings, 0x43D240);
FunctionPointer(int, PauseMenu_Main, (), 0x43D310);
FunctionPointer(int, LoadCharacters, (), 0x43D630);
FunctionPointer(void, SetCurrentCharacter1P, (Characters a1), 0x43D850);
FastcallFunctionPointer(void, UpdateMenuInput, (int a1), 0x441AB0);
VoidFunc(Control, 0x441BA0);
VoidFunc(Control_B, 0x441D50);
VoidFunc(WriteAnalogs, 0x441EF0);
FunctionPointer(int, UpdateStaticControllers, (), 0x4421B0);
VoidFunc(UpdateUselessButtonPressBooleans, 0x442390);
ObjectFunc(ADX_TaskLoop_Delete, 0x442850);
ObjectFunc(ADX_TaskLoop, 0x442AD0);
VoidFunc(ResetMusic, 0x442D90);
VoidFunc(StopMusic, 0x442F50);
ObjectFunc(JingleTask_Delete, 0x443320);
ObjectFunc(JingleTask, 0x4433D0);
FunctionPointer(int, ProbablySavesSaveFile, (), 0x4436A0);
FunctionPointer(int, ProbablyLoadsSave, (char), 0x445100);
FunctionPointer(int, SetWorkingSave, (), 0x445310);
FunctionPointer(int, SetWorkingSaveD, (), 0x445330);
ObjectFunc(DrawLine3DExec, 0x44B680);
FunctionPointer(int, Get_dword_1A559C8, (), 0x44BFE0);
FunctionPointer(double, MaybeThisIsDeltaTimeOrSomething, (), 0x44C1A0);
FastcallFunctionPointer(signed int, LoadTextureList, (char *filename, NJS_TEXLIST *texlist), 0x44C350);
ThiscallFunctionPointer(NJS_TEXLIST *, LoadCharTextures, (char *filename), 0x44C510);
FunctionPointer(int, SetupGameplayVariables, (), 0x44C8F0);
FunctionPointer(int, GetHandicapThingMaybe, (int id), 0x44CBA0);
ObjectFunc(DispTechniqueScore_Main, 0x44D320);
FunctionPointer(int, DispTechniqueScore_Something, (int), 0x44D360);
ObjectFunc(DispTechniqueScore_Delete, 0x44D8D0);
FunctionPointer(int, DispTechniqueScore_Load, (), 0x44D910);
ObjectFunc(CounterOfDestructedEnemy, 0x44DB80);
ObjectFunc(ScoreWindow, 0x44EEA0);
ObjectFunc(CalcTotalScore, 0x44F7C0);
ObjectFunc(CalcTotalScore_Delete, 0x4505A0);
ObjectFunc(CalcTotalTime, 0x450780);
ObjectFunc(CalcTotalTime_Delete, 0x450F60);
ObjectFunc(NotCalcTotalScore, 0x450F90);
ObjectFunc(DispWinnerAndContinue, 0x451050);
ObjectFunc(DispWinnerAndContinue_Something, 0x451250);
ObjectFunc(DispWinnerAndContinue_Delete, 0x451430);
FunctionPointer(void, LoadResultScreenObjects, (char player), 0x451450);
ObjectFunc(CountDownTimerForTimeLimit2P_Something, 0x4516E0);
ObjectFunc(CountDownTimerForTimeLimit2P, 0x451930);
FunctionPointer(void, CountDownTimerForTimeLimit2P_Load, (char a1), 0x451B00);
ObjectFunc(CountDownTimerForGameOver, 0x451DF0);
ObjectFunc(execTotalBossScore, 0x452400);
ObjectFunc(DisplayTotalRings, 0x453150);
ObjectFunc(CardIndicatorExec_Delete, 0x455E40);
ObjectFunc(CardIndicatorExec_Main, 0x456280);
ObjectFunc(CardCloseOperationExec, 0x4566F0);
ObjectFunc(WriteTaskWithWaiting, 0x456820);
ObjectFunc(miniEventExec, 0x4579E0);
FunctionPointer(int, LoadCharacterSoundBanks, (int, int a1), 0x459100);
VoidFunc(Load_PLCOMMTN_Stuff, 0x459370);
FunctionPointer(int, SetPhysicsParamsAndGiveUpgrades, (), 0x4599C0);
ObjectFunc(GamePlayerMissed, 0x46ABD0);
ObjectFunc(DeathZoneObject_Delete, 0x46AD40);
ObjectFunc(DeathZoneObject, 0x46AD50);
ObjectFunc(BreathCounterP, 0x46BC60);
ThiscallFunctionPointer(void, DoNextAction, (int playerNum, char action, int unknown), 0x46BFF0);
ObjectFunc(PlayerSpeedUp_Main, 0x46DF80);
ObjectFunc(PlayerSpeedUp_Delete, 0x46DFC0);
ObjectFunc(PlayerSpeedUp, 0x46E000);
FunctionPointer(Bool, CheckBreakObject, (ObjectMaster *_this, ObjectMaster *other), 0x46EC00);
ObjectFunc(DeleteObject_, 0x46F720);
FunctionPointer(void, RunObjectIndex, (int index), 0x46F8C0);
FunctionPointer(int, RunMostObjects, (), 0x46FBA0);
VoidFunc(UpdateObjects, 0x470010);
FunctionPointer(EntityData1 *, AllocateEntityData1, (), 0x470B40);
FunctionPointer(EntityData2 *, AllocateEntityData2, (), 0x470B70);
FunctionPointer(ObjUnknownA *, AllocateObjUnknownA, (), 0x470BA0);
FunctionPointer(ObjUnknownB *, AllocateObjUnknownB, (), 0x470BD0);
FunctionPointer(ObjectMaster *, LoadChildObject, (LoadObj a3, void(__cdecl *a4)(ObjectMaster *), ObjectMaster *parent), 0x470C00);
ObjectFunc(DeleteChildObjects, 0x470C80);
ObjectFunc(StageNameDisplayExecutor_Main, 0x4724C0);
FunctionPointer(int, DamagePlayer, (EntityData1 *data1, CharObj2Base *data2), 0x473800);
ObjectFunc(ExecutePlayerDrawMotionWithAlphaReducing, 0x476BD0);
ObjectFunc(PSubCallSEWithWait, 0x476EA0);
ObjectFunc(LoopSECallForTheMode, 0x476F20);
FunctionPointer(signed int, ScreenFadeIn, (), 0x478690);
FunctionPointer(signed int, ScreenFadeOut, (), 0x4786E0);
FunctionPointer(signed int, LoadLandManager, (LandTable *a1), 0x47BD30);
ObjectFunc(LandManager_Main, 0x47C180);
FunctionPointer(signed int, LoadSetObject, (ObjectListHead *list, void *setfile), 0x487E40);
FunctionPointer(int, DeleteSetObject, (), 0x487F00);
ObjectFunc(SetObject_Delete, 0x487F20);
ObjectFunc(SetObject_Main, 0x487F60);
FunctionPointer(int, ByteswapSETFile, (), 0x487FC0);
VoidFunc(ReadSET_2P, 0x4883D0);
VoidFunc(ReadSET_1P, 0x488630);
ThiscallFunctionPointer(void *, LoadStageSETFile, (char *filename, int size), 0x488F60);
VoidFunc(CountPerfectRings, 0x4890E0);
ObjectFunc(MinimalCounterExecutor, 0x489240);
ObjectFunc(MinimalCaptureEffect_Exec, 0x489650);
ObjectFunc(Minimal_Exec, 0x4898B0);
FunctionPointer(int, MINIMAL, (ObjectMaster *a1), 0x48ADE0);
FunctionPointer(int, ChaosDrive_Unknown, (int), 0x48F0E0);
ObjectFunc(ChaosDrive_Delete, 0x48F7C0);
ObjectFunc(ChaosDrive_Load2, 0x48F810);
ThiscallFunctionPointer(unsigned int, PRSDec, (unsigned __int8 *src, uint8_t *dst), 0x48F980);
FunctionPointer(signed int, LoadStagePaths, (LoopHead **a1), 0x490110);
FunctionPointer(void, LoadPathObjects, (LoopHead **a1), 0x490180);
ObjectFunc(ParticleCoreTask_Load, 0x491C20);
ObjectFunc(BgExec_01, 0x4ACDB0);
FunctionPointer(int, LoadStg05Module, (), 0x4B3760);
FunctionPointer(int, PumpkinHill_Init, (), 0x4B3770);
ObjectFunc(ObjSetManExec_1, 0x4B3E30);
ObjectFunc(BgExec_02, 0x4B9FB0);
ObjectFunc(BgClip_01, 0x4BA350);
ObjectFunc(CalcLightForPlayer_1, 0x4BA490);
ObjectFunc(EffectExplosionExec, 0x4C6D00);
ObjectFunc(BgClip_02, 0x4CA800);
ObjectFunc(ObjectGlobalLightManager, 0x4CAB20);
ObjectFunc(BgExec_03, 0x4CAE90);
ObjectFunc(BgColi, 0x4CF130);
ObjectFunc(CountDownTimerManagerTaskExec, 0x4D2D50);
ObjectFunc(VibSlightExec, 0x4D4300);
FunctionPointer(int, VibSlightExec_Load, (), 0x4D4350);
ObjectFunc(BgExec_04, 0x4D4A70);
FunctionPointer(int, LoadStg37Module, (), 0x4DB110);
FunctionPointer(int, CannonsCoreRouge_Init, (), 0x4DB120);
FunctionPointer(int, ResetArbitraryGravity, (int), 0x4E94B0);
ObjectFunc(ManGCylExecutor_Main, 0x4E9510);
FunctionPointer(int, ManGCylExecutor_Load, (), 0x4E9570);
FunctionPointer(void, InitSplitscreen, (int num), 0x4EB2B0);

VoidFunc(LoadChaomainModule, 0x52AB10);
FunctionPointer(int, ChaoMain_Constructor, (), 0x52AB60);
FunctionPointer(int, GetCurrentChaoStage, (), 0x52B5A0);
ObjectFunc(AL_PlayerControlManager, 0x52B750);
FunctionPointer(int, LoadChaoRaceModule, (), 0x52BAE0);
ObjectFunc(AL_RaceMaster, 0x52D1C0);
FunctionPointer(int, LoadChaoKarateModule, (), 0x52D6E0);
ObjectFunc(AL_KarateMaster, 0x52D990);
ObjectFunc(FExec_1, 0x52DD90);
FunctionPointer(char *, GetChaoSavePointer, (), 0x52E440);
FunctionPointer(int, GetChaoSavePointer_, (), 0x52E460);
ThiscallFunctionPointer(bool, CheckChaoChecksum, (char *data), 0x52F030);
ObjectFunc(FExec_2, 0x52FC60);
ObjectFunc(ALW_Control_Main, 0x530850);
ObjectFunc(ALW_Control_Display, 0x530B00);
ObjectFunc(ALW_Control_Delete, 0x530B70);
FunctionPointer(int, SpawnAllChaoInGarden, (), 0x531B10);
FunctionPointer(int, AL_GBAManagerExecutor_Load, (), 0x532710);
ObjectFunc(AL_GBAManagerExecutor, 0x532A60);
ObjectFunc(AL_GBAManagerExecutor_Delete, 0x532C70);
ObjectFunc(AL_EntranceMenuManagerExecutor, 0x533100);
ObjectFunc(ALO_StageTitleExec_Delete, 0x535110);
FunctionPointer(int, ALO_StageTitleExec_Unknown, (int), 0x535130);
ObjectFunc(ALO_StageTitleExec_Main, 0x535190);
FunctionPointer(void, ALO_StageTitleExec_Load, (int a2, int a3), 0x5352D0);
ObjectFunc(ALR_ADV_CongraManagerExecutor, 0x5371F0);
ObjectFunc(FExec_3, 0x537480);
ObjectFunc(ALR_CameraManager, 0x538110);
ObjectFunc(ALR_ADV_GoalManagerExecutor, 0x5394E0);
ObjectFunc(ALR_ADV_MapManagerExecutor, 0x5398A0);
ObjectFunc(ToyPresenterExecutor, 0x539C80);
ObjectFunc(ALR_ADV_StartManagerExecutor, 0x53A670);
ObjectFunc(ALO_RacePitfallExecutor_Main, 0x53AB00);
ObjectFunc(ALO_RacePitfallExecutor_Display, 0x53ABB0);
FunctionPointer(int, ALO_RacePitfallExecutor_Load, (), 0x53AC20);
ObjectFunc(ALO_RaceKarimonoQuestionPanelExecutor_Main, 0x53AD10);
ObjectFunc(ALO_RaceKarimonoQuestionPanelExecutor_Display, 0x53ADB0);
ObjectFunc(FallObjectExecutor_Main, 0x53AE10);
FunctionPointer(float *, FallObjectExecutor_Load, (NJS_VECTOR *a1), 0x53B0A0);
ObjectFunc(ALO_RaceKarimonoAnswerPanelExecutor_Main, 0x53B220);
ObjectFunc(ALO_RaceKarimonoAnswerPanelExecutor_Display, 0x53B4D0);
FunctionPointer(int, ALO_RaceKarimonoQuestionPanelExecutor_Load, (), 0x53B530);
ObjectFunc(ALR_ADV_TryAgainManagerExecutor, 0x53ECF0);
ObjectFunc(j_Chao_Display, 0x53FCE0);
ObjectFunc(AL_ExecShadowTex, 0x5412E0);
ObjectFunc(KarateMainExec, 0x542DA0);
ObjectFunc(FExec_4, 0x5436B0);
ObjectFunc(ALO_ChaosDriveExecutor_Main, 0x5450C0);
ObjectFunc(ALO_ChaosDriveExecutor_Display, 0x545150);
FunctionPointer(int, ALO_ChaosDriveExecutor_Unknown, (int), 0x545430);
ObjectFunc(ALO_ChaosDriveExecutor_Delete, 0x5455B0);
FunctionPointer(ObjectMaster *, ALO_ChaosDriveExecutor_Load, (unsigned __int8 a1, NJS_VECTOR *position, NJS_VECTOR *idkvector, ChaoData *a4), 0x545600);
ObjectFunc(ALO_FruitExecutor_Main, 0x545E40);
ObjectFunc(ALO_FruitExecutor_Display, 0x545EE0);
FunctionPointer(ObjectMaster *, ALO_FruitExecutor_Load, (int a1, NJS_VECTOR *position, Angle angle, NJS_VECTOR *a4, ChaoData *a5), 0x546180);
ObjectFunc(ALO_GrowTreeExecutor_Main, 0x546810);
ObjectFunc(ALO_GrowTreeExecutor_Display, 0x547E70);
ObjectFunc(ALO_GrowTreeExecutor_Delete, 0x5481E0);
FunctionPointer(int, ALO_GrowTreeExecutor_Load, (int, int), 0x548210);
ObjectFunc(AL_MinimalExecutor_Display, 0x5489D0);
ObjectFunc(AL_MinimalExecutor_Delete, 0x548C30);
ObjectFunc(AL_MinimalExecutor_Main, 0x548C50);
FunctionPointer(ObjectMaster *, AL_MinimalExecutor_Load, (char a1, NJS_VECTOR *a2, int a3, void *a4, int a5), 0x548D30);
ObjectFunc(AL_MinimalCreateManagerExecutor, 0x5492B0);
ObjectFunc(ALO_SeedExecutor_Main, 0x549860);
ObjectFunc(ALO_SeedExecutor_Display, 0x5498E0);
FunctionPointer(ObjectMaster *, ALO_SeedExecutor_Load, (int a1, NJS_VECTOR *position, NJS_VECTOR *a3, int a4), 0x549B00);
FastcallFunctionPointer(int, CalcChaoSaveChecksum, (char *a1), 0x549C40);
ObjectFunc(ALO_ObakeHeadExecutor_Main, 0x54A050);
ObjectFunc(ALO_ObakeHeadExecutor_Display, 0x54A0F0);
FunctionPointer(ObjectMaster *, ALO_ObakeHeadExecutor_Load, (int a1, NJS_VECTOR *position, int a3, NJS_VECTOR *a4, int a5), 0x54A540);
FunctionPointer(int, ChaoStgDark_Prolog, (), 0x54B420);
ObjectFunc(AL_DarkGardenMaster, 0x54B730);
ObjectFunc(AL_OdekakeStage, 0x54BCE0);
FunctionPointer(int, ChaoStgLobby_Prolog, (), 0x54BFB0);
ObjectFunc(AL_LobbyMaster, 0x54C2A0);
FunctionPointer(int, ChaoStgNeut_Prolog, (), 0x54C4F0);
ObjectFunc(AL_NeutGardenMaster, 0x54C7B0);
FunctionPointer(int, ChaoStgHero_Prolog, (), 0x54CCE0);
ObjectFunc(AL_HeroGardenMaster, 0x54D210);
ObjectFunc(AL_EntranceMaster, 0x54DAD0);
ObjectFunc(AL_StadiumMaster, 0x54E010);
ObjectFunc(AlgKinderMaster, 0x54E780);
ObjectFunc(Chao_Main, 0x54FE20);
ObjectFunc(Chao_Delete, 0x54FF30);
ObjectFunc(Chao_Display, 0x54FF80);
FunctionPointer(ObjectMaster *, CreateChao, (ChaoData *chaoData, int a2, KarateOpponent *a3, NJS_VECTOR *position, Angle angle), 0x5501D0);
ObjectFunc(ChaoSelectMenuManager, 0x5542B0);
ObjectFunc(ALO_EntranceElevatorExecutor_Main, 0x557FC0);
ObjectFunc(ALO_EntranceElevatorExecutor_Display, 0x558350);
ObjectFunc(ALO_EntranceElevatorExecutor_Delete, 0x5584C0);
FunctionPointer(int, ALO_EntranceElevatorExecutor_Load, (), 0x558510);
ObjectFunc(ALO_RaceFruit_Main2, 0x55A290);
ObjectFunc(ALO_RaceFruit_Display, 0x55A2F0);
ObjectFunc(ALO_RaceFruit_Main, 0x55A560);
FunctionPointer(int, ALO_RaceFruit_Load, (int), 0x55A5E0);
ObjectFunc(AL_FieldExecutor_Main, 0x55A6A0);
ObjectFunc(AL_FieldExecutor_Display, 0x55A6D0);
FunctionPointer(ObjectMaster *, AL_FieldExecutor_Load, (char a1, NJS_VECTOR *a2, float a3, int a4), 0x55A850);
ObjectFunc(ALO_TVExecutor_Main, 0x55C540);
ObjectFunc(ALO_TVExecutor_Display, 0x55C9E0);
ObjectFunc(ALO_TVExecutor_Delete, 0x55CB70);
FunctionPointer(void, ALO_TVExecutor_Load, (NJS_VECTOR *position), 0x55CB90);
ObjectFunc(ALO_Ball_Main2, 0x55D310);
ObjectFunc(ALO_Ball_Display, 0x55D3B0);
ObjectFunc(ALO_Ball_Main, 0x55D640);
FunctionPointer(char *, ALO_Ball_Load, (NJS_VECTOR *position, NJS_VECTOR *a2), 0x55D6B0);
ObjectFunc(ALO_ShabonExecutor_Display, 0x55D830);
ObjectFunc(ALO_ShabonExecutor_Main, 0x55D960);
ObjectFunc(BookExecutor, 0x55DF90);
ObjectFunc(ALO_RaceKariToy_Main2, 0x5601C0);
ObjectFunc(ALO_RaceKariToy_Display, 0x560220);
ObjectFunc(ALO_RaceKariToy_Main, 0x560470);
FunctionPointer(int, ALO_RaceKariToy_Load, (char, int, int, int), 0x5604F0);
ObjectFunc(ParamFukidasiExecutor, 0x564C80);
ObjectFunc(WaterManagerExecutor, 0x56AF60);
ObjectFunc(CnkWaterManagerExecutor, 0x56B1B0);
ObjectFunc(KarateCameraExec, 0x574480);
ObjectFunc(KarateChaoExec_Main, 0x578D30);
ObjectFunc(EGG, 0x57B520);
ObjectFunc(ALO_Delete, 0x57B9B0);
FunctionPointer(ObjectMaster *, CreateChaoEgg, (const void *a1, ChaoData *chaoData, int a3, NJS_VECTOR *position, int a5), 0x57B9C0);
ObjectFunc(ALO_RadicaseExecutor_Main, 0x57C840);
ObjectFunc(ALO_RadicaseExecutor_Display, 0x57CA80);
ObjectFunc(ALO_RadicaseExecutor_Delete, 0x57CC80);
FunctionPointer(int, ALO_RadicaseExecutor_Load, (int), 0x57CCA0);
ObjectFunc(AL_NormalCameraExecutor_Main, 0x57CD50);
FunctionPointer(int, AL_NormalCameraExecutor_Load, (), 0x57D2E0);
ObjectFunc(ALCAM_CameraManagerExecutor, 0x57D400);
ObjectFunc(ALO_OdekakeMachine_Main2, 0x57D540);
ObjectFunc(ALO_OdekakeMachine_Delete, 0x57E040);
ObjectFunc(ALO_OdekakeMachine_Display, 0x57E060);
ObjectFunc(ALO_OdekakeMachine_Main, 0x57E460);
ObjectFunc(AL_OdekakeMenuMaster, 0x57E5F0);
ObjectFunc(FExec_5, 0x57EDD0);
ObjectFunc(ALO_LobbyGateKinderExecutor, 0x57EF00);
ObjectFunc(ALO_LobbyGateKinderExecutor_Display, 0x57EF30);
ObjectFunc(ALO_LobbyExitExecutor, 0x57F0E0);
ObjectFunc(ALO_LobbyGateNeutExecutor, 0x57F780);
ObjectFunc(ALO_LobbyGateNeutExecutor_Display, 0x57F7B0);
ObjectFunc(ALO_LobbyGateHeroExecutor_Main, 0x57F940);
ObjectFunc(ALO_LobbyGateHeroExecutor_Display, 0x57F980);
ObjectFunc(ALO_LobbyGateDarkExecutor_Main, 0x57FD60);
ObjectFunc(ALO_LobbyGateDarkExecutor_Display, 0x57FDF0);
ObjectFunc(ALO_BoxExecutor_Main, 0x5801C0);
ObjectFunc(ALO_BoxExecutor_Display, 0x580510);
ObjectFunc(ALO_BoxExecutor_Delete, 0x580870);
FunctionPointer(void, ALO_BoxExecutor_Load, (NJS_VECTOR *position), 0x580890);
ObjectFunc(ALO_Horse_Main, 0x580E80);
FunctionPointer(int, ALO_Horse_Load, (), 0x580F30);
FunctionPointer(int, ALO_EntranceLampExecutor_Unknown, (int), 0x580F90);
ObjectFunc(ALO_EntranceLampManager_Main, 0x581020);
FunctionPointer(int, ALO_EntranceLamp_Load, (float, float, float), 0x581070);
ObjectFunc(ALO_EntranceGateExecutor_Main, 0x581520);
ObjectFunc(ALO_EntranceGateExecutor_Display, 0x581620);
ObjectFunc(ALO_EntranceGateExecutor_Delete, 0x5816D0);
FunctionPointer(int, ALO_EntranceGateExecutor_Load, (), 0x5816E0);
ObjectFunc(AL_CaveMenuMaster, 0x581870);
ObjectFunc(AL_CaveMenuMaster_display, 0x581A30);
ObjectFunc(CaveOmochaoExec, 0x581A70);
ObjectFunc(AL_KinderPMessageExec, 0x585300);
ObjectFunc(j_DeleteChildObjects, 0x588360);
ObjectFunc(EntryPortExec, 0x590FC0);
ObjectFunc(ChaoSelectWindowExecutor, 0x591190);
ObjectFunc(AL_ChaoParamWindowExecutor, 0x592530);
ObjectFunc(ALR_ADV_StaminaManagerExecutor, 0x5972F0);
ObjectFunc(ALR_ADV_TimeManagerExecutor, 0x5980C0);
ObjectFunc(FTaskExec, 0x5A6860);
ObjectFunc(BGExecutor, 0x5A7480);
ObjectFunc(WelcomeExecutor, 0x5A7C00);
ObjectFunc(BackButtonExecutor, 0x5A80B0);
ObjectFunc(CaveArrowExecutor, 0x5A8510);
ObjectFunc(ALO_RakugakiExecutor_Main, 0x5AAFB0);
ObjectFunc(ALO_RakugakiExecutor_Display, 0x5AB020);
ObjectFunc(ALO_RakugakiExecutor_Delete, 0x5AB1F0);
FunctionPointer(int, ALO_RakugakiExecutor_Load, (char, int), 0x5AB200);
ObjectFunc(LargeTitleBarExecutor, 0x5AB830);
ObjectFunc(MainMenuBarExecutor, 0x5ABDA0);
ObjectFunc(SayounaraWindowExecutor, 0x5AC410);
ObjectFunc(ChaoParamWindowExecutor, 0x5ACC50);
ObjectFunc(ChaoParamWindowExecutor_Display, 0x5ACF40);
ObjectFunc(ChaoParamWindowExecutor_Delete, 0x5AD660);
FunctionPointer(int, ChaoParamWindowExecutor_Load, (float, float), 0x5AD690);

ObjectFunc(Omochao_Main, 0x6C0780);
ObjectFunc(MsgerDmyTaskExec, 0x6C0A50);
ObjectFunc(EfObjLightExec, 0x6C37A0);
ThiscallFunctionPointer(Sint32, LoadStageLight, (const char *filename), 0x6C3AE0);
FunctionPointer(void, SomethingParticleRelated, (Rotation *a1, NJS_VECTOR *a2, float a3), 0x6CD720);
ObjectFunc(FogTaskExec_Main, 0x6DFCC0);
ObjectFunc(FogTaskExec_Delete, 0x6DFD20);
ObjectFunc(FogtaskManExec_Main, 0x6DFD40);
FunctionPointer(int, FogtaskManExec_Delete, (int), 0x6DFD90);
ObjectFunc(EfCrash3DExec, 0x6E1B80);
ObjectFunc(ChaoKeyTask, 0x6E99D0);
ObjectFunc(ObjectChaoKeyExec, 0x6E9DE0);
ObjectFunc(SplashExec, 0x6ED6E0);
FunctionPointer(void, ObjectWaterripple_Load, (NJS_VECTOR *position, NJS_VECTOR *a2, float a3), 0x6EDAF0);
ObjectFunc(ObjectWaterripple, 0x6EDB80);
ObjectFunc(ObjectWaterripple_Main, 0x6EDBA0);
ObjectFunc(ObjectWaterripple_Display, 0x6EDC30);
ObjectFunc(ObjectWaterripple_Delete, 0x6EDD80);
FunctionPointer(void, Exec_Load, (NJS_VECTOR *a1, Rotation *a2, float a3, float a4), 0x6EDEE0);
ObjectFunc(Exec, 0x6EE310);
ObjectFunc(bubble_crush_exec, 0x6EFAA0);
ObjectFunc(bbl_ripple_exec, 0x6EFC90);
ObjectFunc(EsShakeRad, 0x6F01B0);
ObjectFunc(WpHoleExec, 0x6F1030);
ObjectFunc(ObjectAsiotoExec, 0x6F1A00);
FunctionPointer(void, LoadSonic, (int playerNum), 0x716E00);
FunctionPointer(void, LoadAmy, (int playerNum), 0x7170E0);
FunctionPointer(void, LoadShadow, (int playerNum), 0x717360);
FunctionPointer(int, LoadMetalSonic, (int a2), 0x717640);
FunctionPointer(void, LoadKnuckles, (int playerNum), 0x728110);
FunctionPointer(void, LoadRouge, (int playerNum), 0x728460);
FunctionPointer(void, LoadTikal, (int playerNum), 0x7288B0);
FunctionPointer(void, LoadChaos, (int playerNum), 0x728B60);
FunctionPointer(void, LoadMechEggman, (int playerNum), 0x740C50);
FunctionPointer(void, LoadMechTails, (int playerNum), 0x740EB0);
FunctionPointer(void, LoadChaoWalker, (int playerNum), 0x741110);
FunctionPointer(void, LoadDarkChaoWalker, (int playerNum), 0x7412F0);

FunctionPointer(int, GetDllData, (LPCSTR lpProcName), 0x77DEF0);
FunctionPointer(void *, MemoryManager__Allocate, (int size, char *file, int line), 0x77DFA0);
FunctionPointer(void *, MemoryManager__AllocateArray, (int count, int size), 0x77DFB0);
FunctionPointer(void, MemoryManager__Deallocate2, (AllocatedMem *a1, size_t count), 0x77DFE0);
VoidFunc(UpdateControllers, 0x77E780);

FunctionPointer(UnknownData2 *, AllocateUnknownData2, (ObjectMaster *obj), 0x7966D0);
VoidFunc(nullsub_1, 0x6FE430);
FunctionPointer(int, nullsub_2, (), 0x7B4AEF);
FunctionPointer(int, nullsub_3, (), 0x7F4700);
StdcallFunctionPointer(int, nullsub_7, (int), 0x8331D0);
StdcallFunctionPointer(int, nullsub_9, (int, int), 0x840AE0);
FunctionPointer(int, nullsub_14, (), 0x863DD0);
StdcallFunctionPointer(int, nullsub_12, (int), 0x868810);
StdcallFunctionPointer(int, nullsub_10, (int), 0x8688F0);
StdcallFunctionPointer(int, nullsub_11, (int, int), 0x868960);
VoidFunc(nullsub_8, 0x8704F0);
VoidFunc(nullsub_6, 0x870580);
VoidFunc(nullsub_4, 0x870590);

// HWND __usercall@<eax>(int nWidth@<eax>, HINSTANCE hInstance, int nHeight, unsigned __int8 isWindowed)
static const void *const CreateMainWindowPtr = (void*)0x4019D0;
static inline HWND CreateMainWindow(int nWidth, HINSTANCE hInstance, int nHeight, unsigned __int8 isWindowed)
{
	HWND result;
	__asm
	{
		push dword ptr [isWindowed]
		push[nHeight]
		push[hInstance]
		mov eax, [nWidth]
		call CreateMainWindowPtr
		add esp, 12
		mov result, eax
	}
	return result;
}

// UserConfigData *__usercall@<eax>(LaunchConfig *a1@<ebx>)
static const void *const ReadConfigPtr = (void*)0x405310;
static inline UserConfigData * ReadConfig(LaunchConfig *a1)
{
	UserConfigData * result;
	__asm
	{
		mov ebx, [a1]
		call ReadConfigPtr
		mov result, eax
	}
	return result;
}

// int __usercall@<eax>(char *path@<ecx>, void *a2@<edx>, size_t count)
static const void *const WriteSaveFileThingPtr = (void*)0x426760;
static inline int WriteSaveFileThing(char *path, void *a2, size_t count)
{
	int result;
	__asm
	{
		push[count]
		mov edx, [a2]
		mov ecx, [path]
		call WriteSaveFileThingPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// int __usercall@<eax>(char *path@<ecx>, void *buffer@<edx>, size_t size)
static const void *const ReadSaveFileThingPtr = (void*)0x426860;
static inline int ReadSaveFileThing(char *path, void *buffer, size_t _size)
{
	int result;
	__asm
	{
		push[_size]
		mov edx, [buffer]
		mov ecx, [path]
		call ReadSaveFileThingPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// char __usercall@<al>(_DWORD *a1@<esi>)
static const void *const IsByteswappedPtr = (void*)0x429840;
static inline char IsByteswapped(void *a1)
{
	char result;
	__asm
	{
		mov esi, [a1]
		call IsByteswappedPtr
		mov result, al
	}
	return result;
}

// void __usercall(int a1@<esi>, int a2, char a3, char a4)
static const void *const PlaySoundProbablyPtr = (void*)0x437260;
static inline void PlaySoundProbably(int a1, int a2, char a3, char a4)
{
	__asm
	{
		push dword ptr [a4]
		push dword ptr [a3]
		push[a2]
		mov esi, [a1]
		call PlaySoundProbablyPtr
		add esp, 12
	}
}

// void __usercall(int pnum@<eax>, signed int a2@<edx>, signed int a3@<ecx>, int a4)
static const void *const KnockBackRumblePtr = (void*)0x438F80;
static inline void KnockBackRumble(int pnum, signed int a2, signed int a3, int a4)
{
	__asm
	{
		push[a4]
		mov ecx, [a3]
		mov edx, [a2]
		mov eax, [pnum]
		call KnockBackRumblePtr
		add esp, 4
	}
}

// void __usercall(__int16 stageNumber@<ax>)
static const void *const SetCurrentLevelPtr = (void*)0x43D8A0;
static inline void SetCurrentLevel(__int16 stageNumber)
{
	__asm
	{
		mov ax, [stageNumber]
		call SetCurrentLevelPtr
	}
}

// Sint32 __usercall@<eax>(int playerNumber@<ecx>, NJS_VECTOR *position@<edi>, Rotation *rotation)
static const void *const LoadStartPositionPtr = (void*)0x43D8E0;
static inline Sint32 LoadStartPosition(int playerNumber, NJS_VECTOR *position, Rotation *rotation)
{
	Sint32 result;
	__asm
	{
		push[rotation]
		mov edi, [position]
		mov ecx, [playerNumber]
		call LoadStartPositionPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// void __usercall(int n@<eax>)
static const void *const InitPlayerPtr = (void*)0x43DA40;
static inline void InitPlayer(int n)
{
	__asm
	{
		mov eax, [n]
		call InitPlayerPtr
	}
}

// void __usercall(int a1@<eax>)
static const void *const Load2PIntroPosPtr = (void*)0x43DBD0;
static inline void Load2PIntroPos(int a1)
{
	__asm
	{
		mov eax, [a1]
		call Load2PIntroPosPtr
	}
}

// signed int __usercall@<eax>(int playerNum@<eax>)
static const void *const LoadEndPositionPtr = (void*)0x43DD50;
static inline signed int LoadEndPosition(int playerNum)
{
	signed int result;
	__asm
	{
		mov eax, [playerNum]
		call LoadEndPositionPtr
		mov result, eax
	}
	return result;
}

// void __usercall(signed int n@<esi>)
static const void *const AwardWinPtr = (void*)0x43E6D0;
static inline void AwardWin(signed int n)
{
	__asm
	{
		mov esi, [n]
		call AwardWinPtr
	}
}

// void __usercall(int pnum@<esi>)
static const void *const PlayWinnerVoiceProbablyPtr = (void*)0x43ECA0;
static inline void PlayWinnerVoiceProbably(int pnum)
{
	__asm
	{
		mov esi, [pnum]
		call PlayWinnerVoiceProbablyPtr
	}
}

// void __usercall(const char *song@<edi>)
static const void *const PlayMusicPtr = (void*)0x442CF0;
static inline void PlayMusic(const char *song)
{
	__asm
	{
		mov edi, [song]
		call PlayMusicPtr
	}
}

// void __usercall(const char *a1@<edi>)
static const void *const _PlayMusicOncePtr = (void*)0x442E60;
static inline void _PlayMusicOnce(const char *a1)
{
	__asm
	{
		mov edi, [a1]
		call _PlayMusicOncePtr
	}
}

// void __usercall(int a1@<ecx>, const char *a2@<edi>)
static const void *const PlayMusicOncePtr = (void*)0x442EF0;
static inline void PlayMusicOnce(int a1, const char *a2)
{
	__asm
	{
		mov edi, [a2]
		mov ecx, [a1]
		call PlayMusicOncePtr
	}
}

// signed int __usercall@<eax>(int idk@<edx>, int num)
static const void *const PlayVoicePtr = (void*)0x443130;
static inline signed int PlayVoice(int idk, int num)
{
	signed int result;
	__asm
	{
		push[num]
		mov edx, [idk]
		call PlayVoicePtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// void __usercall(const char *song@<eax>)
static const void *const PlaySong_QueuePtr = (void*)0x443530;
static inline void PlaySong_Queue(const char *song)
{
	__asm
	{
		mov eax, [song]
		call PlaySong_QueuePtr
	}
}

// void __usercall(TexPackInfo *pack@<eax>)
static const void *const LoadTexturesPtr = (void*)0x44C2D0;
static inline void LoadTextures(TexPackInfo *pack)
{
	__asm
	{
		mov eax, [pack]
		call LoadTexturesPtr
	}
}

// void __usercall(char id@<al>, __int16 count@<bx>)
static const void *const AddLivesPtr = (void*)0x44CB10;
static inline void AddLives(char id, __int16 count)
{
	__asm
	{
		mov bx, [count]
		mov al, [id]
		call AddLivesPtr
	}
}

// void __usercall(char playerNum@<al>, int numRings@<edx>)
static const void *const AddRingsPtr = (void*)0x44CE10;
static inline void AddRings(char playerNum, int numRings)
{
	__asm
	{
		mov edx, [numRings]
		mov al, [playerNum]
		call AddRingsPtr
	}
}

// void __usercall(int a1@<eax>)
static const void *const AddScorePtr = (void*)0x44D2D0;
static inline void AddScore(int a1)
{
	__asm
	{
		mov eax, [a1]
		call AddScorePtr
	}
}

// ModelIndex *__usercall@<eax>(char *filename@<eax>)
static const void *const LoadMDLFilePtr = (void*)0x459590;
static inline ModelIndex * LoadMDLFile(char *filename)
{
	ModelIndex * result;
	__asm
	{
		mov eax, [filename]
		call LoadMDLFilePtr
		mov result, eax
	}
	return result;
}

//void __usercall(int a1@<edi>, NJS_VECTOR *a2@<esi>, int a3, char a4, char a5)
static const void *const PlaySound_XYZPtr = (void*)0x4372E0;
static inline void PlaySound_XYZ(int a1, NJS_VECTOR *a2, int a3, char a4, char a5)
{
	__asm
	{
		push dword ptr[a5]
		push dword ptr[a4]
		push a3
		mov esi, a2
		mov edi, a1
		call PlaySound_XYZPtr
		add esp, 12
	}
}

// void __usercall(ModelIndex *a1<esi>)
static const void *const ReleaseMDLFilePtr = (void*)0x4596D0;
static inline void ReleaseMDLFile(ModelIndex *a1)
{
	__asm
	{
		mov esi, [a1]
		call ReleaseMDLFilePtr
	}
}

// AnimationIndex *__usercall@<eax>(char *filename@<eax>)
static const void *const LoadMTNFilePtr = (void*)0x459740;
static inline AnimationIndex * LoadMTNFile(char *filename)
{
	AnimationIndex * result;
	__asm
	{
		mov eax, [filename]
		call LoadMTNFilePtr
		mov result, eax
	}
	return result;
}

// int __usercall@<eax>(ObjectMaster *a1@<eax>)
static const void *const GetPlayerNumberPtr = (void*)0x46DCC0;
static inline int GetPlayerNumber(ObjectMaster *a1)
{
	int result;
	__asm
	{
		mov eax, [a1]
		call GetPlayerNumberPtr
		mov result, eax
	}
	return result;
}

// ObjectMaster *__usercall@<eax>(int list@<ecx>, char *name@<eax>, void (__cdecl *mainSub)(ObjectMaster *)@<edi>, LoadObj flags)
static const void *const LoadObjectPtr = (void*)0x46F610;
static inline ObjectMaster * LoadObject(int list, const char *name, void(__cdecl *mainSub)(ObjectMaster *), LoadObj flags)
{
	ObjectMaster * result;
	__asm
	{
		push dword ptr [flags]
		mov edi, [mainSub]
		mov eax, [name]
		mov ecx, [list]
		call LoadObjectPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// ObjectMaster *__usercall@<eax>(void (__cdecl *mainSub)(ObjectMaster *)@<edi>, int list@<esi>, char *name)
static const void *const AllocateObjectMasterPtr = (void*)0x46F680;
static inline ObjectMaster * AllocateObjectMaster(void(__cdecl *mainSub)(ObjectMaster *), int list, const char *name)
{
	ObjectMaster * result;
	__asm
	{
		push[name]
		mov esi, [list]
		mov edi, [mainSub]
		call AllocateObjectMasterPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// int __usercall@<eax>(int targetAlpha@<edx>)
static const void *const ScreenFadePtr = (void*)0x478730;
static inline int ScreenFade(int targetAlpha)
{
	int result;
	__asm
	{
		mov edx, [targetAlpha]
		call ScreenFadePtr
		mov result, eax
	}
	return result;
}

// signed int __usercall@<eax>(ObjectMaster *obj@<eax>, CollisionData *collision, int count, unsigned __int8 a4)
static const void *const InitCollisionPtr = (void*)0x47E520;
static inline signed int InitCollision(ObjectMaster *obj, CollisionData *collision, int count, unsigned __int8 a4)
{
	signed int result;
	__asm
	{
		push dword ptr [a4]
		push[count]
		push[collision]
		mov eax, [obj]
		call InitCollisionPtr
		add esp, 12
		mov result, eax
	}
	return result;
}

// bool __usercall@<eax>(NJS_VECTOR *a1@<ecx>, float x, float y, float z, float distance_maybe)
static const void *const SETDistanceCheckThingPtr = (void*)0x488340;
static inline bool SETDistanceCheckThing(NJS_VECTOR *a1, float x, float y, float z, float distance_maybe)
{
	bool result;
	__asm
	{
		push[distance_maybe]
		push[z]
		push[y]
		push[x]
		mov ecx, [a1]
		call SETDistanceCheckThingPtr
		add esp, 16
		mov result, al
	}
	return result;
}

// void *__usercall@<eax>(int size@<eax>, char *name_s@<ecx>, char *name_u)
static const void *const LoadSETFilePtr = (void*)0x488DD0;
static inline void * LoadSETFile(int _size, char *name_s, char *name_u)
{
	void * result;
	__asm
	{
		push[name_u]
		mov ecx, [name_s]
		mov eax, [_size]
		call LoadSETFilePtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// void __usercall(NJS_VECTOR *a1@<ebx>)
static const void *const ChaosDrive_LoadPtr = (void*)0x48F8E0;
static inline void ChaosDrive_Load(NJS_VECTOR *a1)
{
	__asm
	{
		mov ebx, [a1]
		call ChaosDrive_LoadPtr
	}
}

// void __usercall(char *a1@<esi>)
static const void *const WriteChaoSaveChecksumPtr = (void*)0x52EEE0;
static inline void WriteChaoSaveChecksum(char *a1)
{
	__asm
	{
		mov esi, [a1]
		call WriteChaoSaveChecksumPtr
	}
}

// signed int __usercall@<eax>(unsigned __int16 a1@<cx>, ObjectMaster *obj@<ebx>, __int16 a3, ChaoData *data)
static const void *const AddToGlobalChaoThingMaybePtr = (void*)0x530750;
static inline signed int AddToGlobalChaoThingMaybe(unsigned __int16 a1, ObjectMaster *obj, __int16 a3, ChaoData *data)
{
	signed int result;
	__asm
	{
		push[data]
		push[a3]
		mov ebx, [obj]
		mov cx, [a1]
		call AddToGlobalChaoThingMaybePtr
		add esp, 6
		mov result, eax
	}
	return result;
}

// void __usercall(char *a1@<eax>, char *a2)
static const void *const AddChaoSaveSignaturePtr = (void*)0x5326C0;
static inline void AddChaoSaveSignature(char *a1, char *a2)
{
	__asm
	{
		push[a2]
		mov eax, [a1]
		call AddChaoSaveSignaturePtr
		add esp, 4
	}
}

// int __usercall@<eax>(AL_GENE *a1@<esi>)
static const void *const InitChaoDNAPtr = (void*)0x5506B0;
static inline int InitChaoDNA(AL_GENE *a1)
{
	int result;
	__asm
	{
		mov esi, [a1]
		call InitChaoDNAPtr
		mov result, eax
	}
	return result;
}

// void __usercall(ObjectMaster *a1@<eax>)
static const void *const LoadChaoMotionTablePtr = (void*)0x55C370;
static inline void LoadChaoMotionTable(ObjectMaster *a1)
{
	__asm
	{
		mov eax, [a1]
		call LoadChaoMotionTablePtr
	}
}

// int __usercall@<eax>(NJS_VECTOR *position@<ebx>, int a2)
static const void *const ALO_ShabonExecutor_LoadPtr = (void*)0x55DA10;
static inline int ALO_ShabonExecutor_Load(NJS_VECTOR *position, int a2)
{
	int result;
	__asm
	{
		push[a2]
		mov ebx, [position]
		call ALO_ShabonExecutor_LoadPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// EntityData1 *__usercall@<eax>(NJS_VECTOR *position@<ebx>, int a2)
static const void *const ALO_OdekakeMachine_LoadPtr = (void*)0x57E4F0;
static inline EntityData1 * ALO_OdekakeMachine_Load(NJS_VECTOR *position, int a2)
{
	EntityData1 * result;
	__asm
	{
		push[a2]
		mov ebx, [position]
		call ALO_OdekakeMachine_LoadPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// ObjectMaster *__usercall@<eax>(NJS_VECTOR *position@<ebx>)
static const void *const ALO_LobbyGateKinderExecutor_LoadPtr = (void*)0x57F060;
static inline ObjectMaster * ALO_LobbyGateKinderExecutor_Load(NJS_VECTOR *position)
{
	ObjectMaster * result;
	__asm
	{
		mov ebx, [position]
		call ALO_LobbyGateKinderExecutor_LoadPtr
		mov result, eax
	}
	return result;
}

// ObjectMaster *__usercall@<eax>(NJS_VECTOR *position@<ebx>)
static const void *const ALO_LobbyGateNeutExecutor_LoadPtr = (void*)0x57F8C0;
static inline ObjectMaster * ALO_LobbyGateNeutExecutor_Load(NJS_VECTOR *position)
{
	ObjectMaster * result;
	__asm
	{
		mov ebx, [position]
		call ALO_LobbyGateNeutExecutor_LoadPtr
		mov result, eax
	}
	return result;
}

// ObjectMaster *__usercall@<eax>(NJS_VECTOR *position@<ebx>)
static const void *const ALO_LobbyGateHeroExecutor_LoadPtr = (void*)0x57FCE0;
static inline ObjectMaster * ALO_LobbyGateHeroExecutor_Load(NJS_VECTOR *position)
{
	ObjectMaster * result;
	__asm
	{
		mov ebx, [position]
		call ALO_LobbyGateHeroExecutor_LoadPtr
		mov result, eax
	}
	return result;
}

// ObjectMaster *__usercall@<eax>(NJS_VECTOR *position@<ebx>)
static const void *const ALO_LobbyGateDarkExecutor_LoadPtr = (void*)0x580140;
static inline ObjectMaster * ALO_LobbyGateDarkExecutor_Load(NJS_VECTOR *position)
{
	ObjectMaster * result;
	__asm
	{
		mov ebx, [position]
		call ALO_LobbyGateDarkExecutor_LoadPtr
		mov result, eax
	}
	return result;
}

// void __usercall(NJS_SPRITE *_sp@<ebx>, Int n@<eax>, Float pri, Uint32 attr)
static const void *const njDrawSprite2D_0Ptr = (void*)0x66EF40;
static inline void njDrawSprite2D_0(NJS_SPRITE *_sp, Int n, Float pri, Uint32 attr)
{
	__asm
	{
		push[attr]
		push[pri]
		mov eax, [n]
		mov ebx, [_sp]
		call njDrawSprite2D_0Ptr
		add esp, 8
	}
}

// Sint32 __usercall@<eax>(const char *path@<eax>, FogData *data@<ebx>)
static const void *const LoadFogDataPtr = (void*)0x6DF840;
static inline Sint32 LoadFogData(const char *path, FogData *data)
{
	Sint32 result;
	__asm
	{
		mov ebx, [data]
		mov eax, [path]
		call LoadFogDataPtr
		mov result, eax
	}
	return result;
}

// void __usercall(const char *path@<eax>, FogData *data@<ecx>)
static const void *const LoadFogData_FogtaskPtr = (void*)0x6DFF00;
static inline void LoadFogData_Fogtask(const char *path, FogData *data)
{
	__asm
	{
		mov ecx, [data]
		mov eax, [path]
		call LoadFogData_FogtaskPtr
	}
}

// void __usercall(NJS_VECTOR *a1@<ebx>, float a2, float a3)
static const void *const LoadChaoKeyPtr = (void*)0x6E9D10;
static inline void LoadChaoKey(NJS_VECTOR *a1, float a2, float a3)
{
	__asm
	{
		push[a3]
		push[a2]
		mov ebx, [a1]
		call LoadChaoKeyPtr
		add esp, 8
	}
}

// void __usercall(int a1@<eax>)
static const void *const LoadEggmanPtr = (void*)0x73C220;
static inline void LoadEggman(int a1)
{
	__asm
	{
		mov eax, [a1]
		call LoadEggmanPtr
	}
}

// void __usercall(int a1@<eax>)
static const void *const LoadTailsPtr = (void*)0x74CF00;
static inline void LoadTails(int a1)
{
	__asm
	{
		mov eax, [a1]
		call LoadTailsPtr
	}
}

// void __usercall(NJS_SPRITE *sp, Int n@<eax>, Float pri, char attr)
static const void *const njDrawSprite2DPtr = (void*)0x77D0B0;
static inline void njDrawSprite2D(NJS_SPRITE *_sp, Int n, Float pri, char attr)
{
	__asm
	{
		push dword ptr [attr]
		push[pri]
		mov eax, [n]
		push[_sp]
		call njDrawSprite2DPtr
		add esp, 12
	}
}

// void *__usercall@<eax>(int size, int count@<edx>, char *file@<ecx>, int line@<eax>)
static const void *const AllocateArrayPtr = (void*)0x77DF40;
static inline void * AllocateArray(int _size, int count, char *file, int line)
{
	void * result;
	__asm
	{
		mov eax, [line]
		mov ecx, [file]
		mov edx, [count]
		push[_size]
		call AllocateArrayPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

// PDS_PERIPHERAL *__usercall@<eax>(signed int a1@<eax>, int a2@<ecx>)
static const void *const pdGetPeripheralPtr = (void*)0x77E8A0;
static inline PDS_PERIPHERAL * pdGetPeripheral(signed int a1, int a2)
{
	PDS_PERIPHERAL * result;
	__asm
	{
		mov ecx, [a2]
		mov eax, [a1]
		call pdGetPeripheralPtr
		mov result, eax
	}
	return result;
}

// void *__usercall<eax>(char *filename<eax>)
static const void *const LoadPRSFilePtr = (void*)0x4548C0;
static inline void *LoadPRSFile(const char *filename)
{
	void *result;
	__asm
	{
		mov eax, [filename]
		call LoadPRSFilePtr
		mov result, eax
	}
	return result;
}

// int __usercall@<eax>(int buttons@<edx>)
static const void *const XInputToDreamcastButtonsPtr = (void*)0x77E910;
static inline int XInputToDreamcastButtons(int buttons)
{
	int result;
	__asm
	{
		mov edx, [buttons]
		call XInputToDreamcastButtonsPtr
		mov result, eax
	}
	return result;
}

// double __usercall@<st0>(NJS_VECTOR *a1@<eax>, NJS_VECTOR *a2@<ecx>)
static const void *const CheckDistancePtr = (void*)0x77FBD0;
static inline float CheckDistance(NJS_VECTOR *a1, NJS_VECTOR *a2)
{
	float result;
	__asm
	{
		mov ecx, [a2]
		mov eax, [a1]
		call CheckDistancePtr
		fstp result
	}
	return result;
}

// double __usercall@<st0>(NJS_VECTOR *a1@<eax>)
static const void *const njScalorPtr = (void*)0x77FC30;
static inline double njScalor(NJS_VECTOR *a1)
{
	double result;
	__asm
	{
		mov eax, [a1]
		call njScalorPtr
		fstp result
	}
	return result;
}
