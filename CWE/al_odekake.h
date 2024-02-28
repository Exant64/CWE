#pragma once
#include "alg_kinder_he.h"
#include "al_msg_warn.h"

#include "cwe_api.h"

struct ODE_MENU_MASTER_WORK
{
	int PreStage;
	int CurrStage;
	int NextStage;
	int mode;
	int timer;
	int subtimer;
	int counter;
	int state;
	int EndFlag;
	int cursorX;
	int cursorY;
	ObjectMaster* tp;
	void* mpStageWork;
	void(__cdecl* mfStageExit)(ODE_MENU_MASTER_WORK*);
};

FunctionPointer(void, LargeTitleBarExecutor_Load, (char a1, float a2, float a3), 0x005ABD30);
ThiscallFunctionPointer(void, sub_5A6C20, (ODE_MENU_MASTER_WORK* a1), 0x5A6C20);
FastcallFunctionPointer(void, sub_5A6F50, (ODE_MENU_MASTER_WORK* a1), 0x5A6F50);
DataPointer(ODE_MENU_MASTER_WORK*, AL_OdekakeMenuMaster_Data_ptr, 0x01AED324);
void sub_558BA0(int a1, int a2, int a3, float a4, float a5, float a6);
ChaoDataBase* __cdecl GBAManager_GetChaoDataPointer();

void __cdecl sub_582F60_CheckGuest(char* a1);
void sub_582F60(char* a1);

void AddOdekakeMenu(const CWE_API_ODEKAKE_ENTRY& entry);

void AL_Odekake_Init();
void AL_Odekake_Finalize();
void AL_Odekake_Update();