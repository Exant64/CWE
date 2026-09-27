#pragma once

#include <stdint.h>
#include "MemAccess.h"
#include "SA2Structs.h"
#include "SA2Enums.h"

#include "asmutil.h"

#define ObjectFunc(NAME, ADDRESS) FunctionPointer(void,NAME,(task *obj),ADDRESS)
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
FunctionPointer(int, ___OutputDebugString, (const char*, ...), 0x426740);
FunctionPointer(int, ProcessChunkModel, (NJS_CNK_MODEL *a1), 0x42D650);
FunctionPointer(void, njCnkTransformObject, (NJS_CNK_OBJECT *object, int(__cdecl *callback)(NJS_CNK_MODEL *)), 0x42EB30);
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
FunctionPointer(void, SetCurrentCharacter1P, (int a1), 0x43D850);
FastcallFunctionPointer(void, UpdateMenuInput, (int a1), 0x441AB0);
VoidFunc(Control, 0x441BA0);
VoidFunc(Control_B, 0x441D50);
VoidFunc(WriteAnalogs, 0x441EF0);
FunctionPointer(int, UpdateStaticControllers, (), 0x4421B0);
VoidFunc(UpdateUselessButtonPressBooleans, 0x442390);
ObjectFunc(ADX_TaskLoop_Delete, 0x442850);
ObjectFunc(ADX_TaskLoop, 0x442AD0);
VoidFunc(BGM_Replay, 0x442D90);
VoidFunc(BGM_Stop, 0x442F50);
ObjectFunc(JingleTask_Delete, 0x443320);
ObjectFunc(JingleTask, 0x4433D0);
FunctionPointer(int, ProbablySavesSaveFile, (), 0x4436A0);
FunctionPointer(int, ProbablyLoadsSave, (char), 0x445100);
FunctionPointer(int, SetWorkingSave, (), 0x445310);
FunctionPointer(int, SetWorkingSaveD, (), 0x445330);
ObjectFunc(DrawLine3DExec, 0x44B680);
FunctionPointer(int, Get_dword_1A559C8, (), 0x44BFE0);
FunctionPointer(double, MaybeThisIsDeltaTimeOrSomething, (), 0x44C1A0);
FastcallFunctionPointer(signed int, texLoadTexturePvmFile, (char *filename, NJS_TEXLIST *texlist), 0x44C350);
ThiscallFunctionPointer(NJS_TEXLIST *, texCreateTexlist, (char *filename), 0x44C510);
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
FunctionPointer(Bool, CheckBreakObject, (task *_this, task *other), 0x46EC00);
ObjectFunc(DestroyTask, 0x46F720);
FunctionPointer(void, RunObjectIndex, (int index), 0x46F8C0);
FunctionPointer(int, RunMostObjects, (), 0x46FBA0);
VoidFunc(UpdateObjects, 0x470010);
FunctionPointer(taskwk *, AllocateEntityData1, (), 0x470B40);
FunctionPointer(EntityData2 *, AllocateEntityData2, (), 0x470B70);
FunctionPointer(ObjUnknownB *, AllocateObjUnknownB, (), 0x470BD0);
FunctionPointer(task *, CreateChildTask, (__int8 a3, void(__cdecl *a4)(task *), task *parent), 0x470C00);
ObjectFunc(DeleteChildObjects, 0x470C80);
ObjectFunc(StageNameDisplayExecutor_Main, 0x4724C0);
FunctionPointer(int, DamagePlayer, (taskwk *data1, playerwk *data2), 0x473800);
ObjectFunc(ExecutePlayerDrawMotionWithAlphaReducing, 0x476BD0);
ObjectFunc(PSubCallSEWithWait, 0x476EA0);
ObjectFunc(LoopSECallForTheMode, 0x476F20);
FunctionPointer(signed int, ScreenFadeIn, (), 0x478690);
FunctionPointer(signed int, ScreenFadeOut, (), 0x4786E0);
FunctionPointer(signed int, LoadLandManager, (OBJ_LANDTABLE *a1), 0x47BD30);
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
FunctionPointer(int, MINIMAL, (task *a1), 0x48ADE0);
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
FunctionPointer(CHAO_GARDEN_INFO *, AL_GetCurrGardenInfo, (), 0x52E440);
FunctionPointer(CHAO_GARDEN_INFO*, AL_GetAnotherGardenInfo, (), 0x52E460);
ThiscallFunctionPointer(bool, CheckChaoChecksum, (char *data), 0x52F030);
ObjectFunc(FExec_2, 0x52FC60);
ObjectFunc(ALW_Control_Main, 0x530850);
ObjectFunc(ALW_Control_Display, 0x530B00);
ObjectFunc(ALW_Control_Delete, 0x530B70);
FunctionPointer(int, AL_SetChaoOnTheGarden, (), 0x531B10);
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
FunctionPointer(task *, ALO_ChaosDriveCreate, (unsigned __int8 a1, NJS_VECTOR *position, NJS_VECTOR *idkvector, ITEM_SAVE_INFO *a4), 0x545600);
FunctionPointer(task *, ALO_FruitCreate, (int a1, NJS_VECTOR *position, Angle angle, NJS_VECTOR *a4, ITEM_SAVE_INFO *a5), 0x546180);
ObjectFunc(ALO_GrowTreeDisplayer, 0x547E70);
FunctionPointer(task *, AL_MinimalCreate, (char a1, NJS_VECTOR *a2, int a3, void *a4, ITEM_SAVE_INFO* a5), 0x548D30);
FunctionPointer(task *, ALO_SeedCreate, (int a1, NJS_VECTOR *position, NJS_VECTOR *a3, ITEM_SAVE_INFO* a4), 0x549B00);
ObjectFunc(ALO_ObakeHeadExecutor, 0x54A050);
FunctionPointer(task *, ALO_ObakeHeadCreate, (int a1, NJS_VECTOR *position, int a3, NJS_VECTOR *a4, ITEM_SAVE_INFO* a5), 0x54A540);

ObjectFunc(AL_DarkGardenMaster, 0x54B730);
ObjectFunc(AL_OdekakeStage, 0x54BCE0);
ObjectFunc(AL_LobbyMaster, 0x54C2A0);
ObjectFunc(AL_NeutGardenMaster, 0x54C7B0);
ObjectFunc(AL_HeroGardenMaster, 0x54D210);
ObjectFunc(AL_EntranceMaster, 0x54DAD0);
ObjectFunc(AL_StadiumMaster, 0x54E010);
ObjectFunc(AlgKinderMaster, 0x54E780);
ObjectFunc(ChaoExecutor, 0x54FE20);
ObjectFunc(ChaoDestructor, 0x54FF30);
ObjectFunc(ChaoDisplayer, 0x54FF80);
FunctionPointer(task *, CreateChaoExtra, (CHAO_PARAM_GC *chaoData, int a2, AL_SHAPE_ELEMENT *a3, NJS_VECTOR *position, Angle angle), 0x5501D0);
ObjectFunc(ChaoSelectMenuManager, 0x5542B0);
ObjectFunc(ALO_TVDisplayer, 0x55C9E0);
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
ObjectFunc(ALW_CancelEntry, 0x57B9B0);
FunctionPointer(task *, CreateEgg, (const void *a1, CHAO_PARAM_GC *chaoData, int a3, NJS_VECTOR *position, int a5), 0x57B9C0);
ObjectFunc(ALO_RadicaseExecutor_Main, 0x57C840);
ObjectFunc(ALO_RadicaseDisplayer, 0x57CA80);
ObjectFunc(ALO_RadicaseExecutor_Delete, 0x57CC80);
FunctionPointer(int, ALO_RadicaseExecutor_Load, (int), 0x57CCA0);
ObjectFunc(AL_NormalCameraExecutor_Main, 0x57CD50);
FunctionPointer(int, AL_CreateNormalCameraTask, (), 0x57D2E0);
ObjectFunc(ALCAM_CameraManagerExecutor, 0x57D400);
ObjectFunc(ALO_OdekakeMachine_Main2, 0x57D540);
ObjectFunc(ALO_OdekakeMachine_Delete, 0x57E040);
ObjectFunc(ALO_OdekakeMachine_Display, 0x57E060);
ObjectFunc(ALO_OdekakeMachine_Main, 0x57E460);
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
ObjectFunc(ALO_BoxExecutor, 0x5801C0);
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

ObjectFunc(Omochao_Main, 0x6C0780);
ObjectFunc(MsgerDmyTaskExec, 0x6C0A50);
ObjectFunc(EfObjLightExec, 0x6C37A0);
ThiscallFunctionPointer(Sint32, LoadStageLight, (const char *filename), 0x6C3AE0);
FunctionPointer(void, SomethingParticleRelated, (Angle3 *a1, NJS_VECTOR *a2, float a3), 0x6CD720);
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
FunctionPointer(void, Exec_Load, (NJS_VECTOR *a1, Angle3 *a2, float a3, float a4), 0x6EDEE0);
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

FunctionPointer(int, GetDataDllProcAddr, (LPCSTR lpProcName), 0x77DEF0);
FunctionPointer(void *, MemoryManager__Allocate, (int size, char *file, int line), 0x77DFA0);
FunctionPointer(void *, MemoryManager__AllocateArray, (int count, int size), 0x77DFB0);
FunctionPointer(void, MemoryManager__Deallocate2, (AllocatedMem *a1, size_t count), 0x77DFE0);
VoidFunc(UpdateControllers, 0x77E780);

FunctionPointer(MOVE_WORK *, MOV_Init, (task *tp), 0x7966D0);
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

static ASM_FUNC void SE_Call(int a1, int a2, char a3, char a4) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0+1) ); // a4
    ASM_PUSH(      ASM_ESP(3+1+1) ); // a3
    ASM_PUSH(      ASM_ESP(2+2+1) ); // a2
    ASM_MOVE( esi, ASM_ESP(1+3+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x437260 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void Jingle_Play(const char *a1) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( ebx, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x00443480 );

    // restore regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void BGM_SetFile(const char *song) {
    // save regs
    ASM_PUSH( edi );

    // arguments
    ASM_MOVE( edi, ASM_ESP(1+0+1) ); // song

    // call
    ASM_CALL_R( edx, 0x442CF0 );

    // restore regs
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void SE_CallV2(int a1, int a3, char a4, char a5, NJS_VECTOR *a2) {
    // save regs
    ASM_PUSH( esi );
    ASM_PUSH( edi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(5+0 +2) ); // pPos
    ASM_PUSH(      ASM_ESP(4+0 +2) ); // volofs
    ASM_PUSH(      ASM_ESP(3+1 +2) ); // pri
    ASM_PUSH(      ASM_ESP(2+2 +2) ); // id
    ASM_MOVE( edi, ASM_ESP(1+3 +2) ); // tone

    // call
    ASM_CALL_R( edx, 0x4372E0 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // pull regs
    ASM_POP( edi );
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC task* CreateElementalTask(__int8 flags, tasklevel list, void(__cdecl *mainSub)(task *), const char *name) {
    // save regs
    ASM_PUSH( edi );

    // arguments
    ASM_MOVE( eax, ASM_ESP(4+0 +1) ); // name
    ASM_MOVE( edi, ASM_ESP(3+0 +1) ); // exec
    ASM_MOVE( ecx, ASM_ESP(2+0 +1) ); // level
    ASM_PUSH(      ASM_ESP(1+0 +1) ); // im

    // call
    ASM_CALL_R( edx, 0x46F610 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // pull regs
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC int CCL_Init(task *obj, CCL_INFO *collision, int count, unsigned __int8 a4) {
    // arguments
    ASM_PUSH(      ASM_ESP(4+0) ); // id
    ASM_PUSH(      ASM_ESP(3+1) ); // nbInfo
    ASM_PUSH(      ASM_ESP(2+2) ); // info
    ASM_MOVE( eax, ASM_ESP(1+3) ); // tp

    // call
    ASM_CALL_R( edx, 0x47E520 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void njDrawSprite2D(NJS_SPRITE *_sp, Int n, Float pri, char attr) {
    // arguments
    ASM_PUSH(      ASM_ESP(4+0) ); // attr
    ASM_PUSH(      ASM_ESP(3+1) ); // pri
    ASM_MOVE( eax, ASM_ESP(2+2) ); // n
    ASM_PUSH(      ASM_ESP(1+2) ); // sp

    // call
    ASM_CALL_R( edx, 0x77D0B0 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // return
    ASM_RET( 0 );
}

static Angle AdjustAngle(Angle ang0, Angle ang1, Angle dang) {
    Sint16 diff;

    ang0 = (Uint16)ang0;
    ang1 = (Uint16)ang1;
    
    diff = ang1 - ang0;
    if(diff <= dang && diff >= -dang) {
        return ang1;
    }
    
    if (diff & 0x8000) {
        diff = ang0 - dang;
    }   
    else {
        diff = ang0 + dang;
    }

    return (Uint16)diff;
}