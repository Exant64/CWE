#include "stdafx.h"

#include "ChaoMain.h"
#include "alg_kinder_bl.h"
#include "AL_ModAPI.h"

static const char* BMDebug[] = 
{
	"-----",
	"EGG",
	"-----",
	"FRUIT",
	"-----",
	"-----",
	"-----",
	"SEED",
	"ACCESSORY",
	"HAT",
	"SPECIAL",
};

void BlackMarketDebug(BlackMarketData *a1)
{
	static int selection = 0;
	static int category = 0;
	static int mode = 0;

	int iVar6;

	enum {
		MD_IDLE,
		MD_CATEGORY,
		MD_ITEM,
		MD_SETINV
	};

	g_HelperFunctions->SetDebugFontSize(12);
	g_HelperFunctions->SetDebugFontColor(0xFFFFFFFF);
	
	switch (mode)
	{
	case MD_IDLE:
		if (MenuButtons_Held[0] & Buttons_X && MenuButtons_Held[0] & Buttons_Y) 
		{
			selection = 0;
			mode = MD_CATEGORY;
		}
		break;
	case MD_CATEGORY:
		if (MenuButtons_Pressed[0] & Buttons_Up) {
			selection--;
		}
		if (selection < 0) {
			selection = 0;
		}
		if (MenuButtons_Pressed[0] & Buttons_Down) {
			selection++;
		}
		if (selection >= LengthOfArray(BMDebug)) {
			selection = LengthOfArray(BMDebug) - 1;
		}
		
		for (size_t i = 0; i < LengthOfArray(BMDebug); i++)
		{
			if(selection == i)
				g_HelperFunctions->DisplayDebugString(i + 5 | 0x00140000, ">");

			if (BMDebug[i]) 
				g_HelperFunctions->DisplayDebugString(i + 5 | 0x00150000, BMDebug[i]);
		}

		if (MenuButtons_Pressed[0] & Buttons_A && selection != 2 && CategoryAttribs[selection].Count > 0 && CategoryAttribs[selection].attrib) {
			category = selection;
			selection = 0;
			mode = MD_ITEM;
		}
		if (MenuButtons_Pressed[0] & Buttons_B) {
			mode = MD_IDLE;
		}

		MenuButtons_Pressed[0] = 0;
		ControllerPointers[0]->on = 0;
		ControllerPointers[0]->press = 0;
		break;
	case MD_ITEM:
		if (MenuButtons_Pressed[0] & Buttons_Up) 
			selection--;
		if (selection < 0)
			selection = CategoryAttribs[category].Count - 1;
		if (MenuButtons_Pressed[0] & Buttons_Down) 
			selection++;
		if (selection >= CategoryAttribs[category].Count) 
			selection = 0;

		if (MenuButtons_Pressed[0] & Buttons_A) {
			mode = MD_SETINV;
		}

		iVar6 = selection - 5;
		if (iVar6 < 0) {
			iVar6 = selection + (CategoryAttribs[category].Count-5);
		}

		for (size_t i = 0; i < LengthOfArray(BMDebug); i++) {
			int iVar4 = iVar6 + i;
			if (iVar4 >= CategoryAttribs[category].Count) {
				iVar4 = iVar4 - CategoryAttribs[category].Count;
			}

			if (i == 5)
				g_HelperFunctions->DisplayDebugString(i + 5 | 0x00140000, ">");

			if(CategoryAttribs[category].attrib[iVar4].Name != 0)
				g_HelperFunctions->DisplayDebugString(i + 5 | 0x00150000, MsgAlItem[CategoryAttribs[category].attrib[iVar4].Name]);
			else 
				g_HelperFunctions->DisplayDebugString(i + 5 | 0x00150000, "-----");
		}

		if (MenuButtons_Pressed[0] & Buttons_B)
			mode = MD_CATEGORY;

		MenuButtons_Pressed[0] = 0;
		ControllerPointers[0]->on = 0;
		ControllerPointers[0]->press = 0;
		break;
	case MD_SETINV:
		if (category == 9 && selection == 0) {
			mode = MD_IDLE;
			break;
		}

		cweSaveFile.marketInventoryCount[category] = 32;
		for (int i = 0; i < 32; i++) {
			cweSaveFile.marketInventory[category][i].mCategory = category;
			cweSaveFile.marketInventory[category][i].mType = selection;
		}

		mode = MD_IDLE;
		break;
	}
}

FunctionPointer(void, BlackMarketDisplay, (BlackMarketData* a1), 0x058B5E0);
void __cdecl BlackMarketDebugHook(BlackMarketData *a1) {
	BlackMarketDebug(a1);
	BlackMarketDisplay(a1);
}