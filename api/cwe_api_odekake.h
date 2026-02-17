#pragma once

#include "cwe_api_common.h"

#ifndef CWEV9
struct ChaoHudThingB {
	int adjust;
	float wd;
	float ht;
	float s0;
	float t0;
	float s1;
	float t1;
	NJS_TEXLIST* pTexlist;
	int TexNum;
};
#endif

#define ODE_FLAGS_NONE (0)
#define ODE_FLAGS_REQUIRE_CHAO BIT_0 // holding chao required to enter the menu (button greyed out if otherwise)
#define ODE_FLAGS_REQUIRE_NO_CHAO BIT_1 // not allowed to enter the menu if you're holding a chao (button greyed out if otherwise)

struct ODE_MENU_MASTER_WORK;

struct CWE_API_ODEKAKE_ENTRY {
	using OdekakeFuncPtr = void(*)(ODE_MENU_MASTER_WORK*);
	using OdekakeConditionPtr = bool(*)();

	// the function ran every frame for the menu logic
	OdekakeFuncPtr pMenuFunc;

	// placeholder for future (intended to be used for "unlocking" menus, ignore for now)
	OdekakeConditionPtr pConditionFunc;

	// flags are above the struct ("ODE_FLAGS")
	Uint32 Flags;

	// the icon sprite on the left of the button
	ChaoHudThingB* ButtonIcon;

	// the text sprite in the middle of the button if the menu is accessible
	ChaoHudThingB* ButtonText;

	// the text sprite in the middle of the button if the menu is inaccessible
	ChaoHudThingB* GreyButtonText;

	// the icon sprite for the top "LargeTitleBar" in the menu
	ChaoHudThingB* BarIcon;

	// the text sprite for the top "LargeTitleBar" in the menu
	ChaoHudThingB* BarText;

	// in 0-1 float format
	float BarColorA;
	float BarColorR;
	float BarColorG;
	float BarColorB;
};