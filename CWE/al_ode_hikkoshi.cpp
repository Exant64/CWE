#include "stdafx.h"

#include "alg_kinder_he.h"
#include "al_odekake.h"

#include "ninja_functions.h"
#include <cstdio>
#include "ChaoMain.h"
#include "al_ode_guide.h"
#include "al_ode_menu.h"

static void AL_OdekakeMove(ODE_MENU_MASTER_WORK* a1);

CWE_API_ODEKAKE_ENTRY OdakakeMoveEntry = {
	AL_OdekakeMove, 
	nullptr, 
	ODE_FLAGS_REQUIRE_NO_CHAO , 
	&stru_11BA528[31],
	&stru_11BA528[6],
	&stru_11BA528[7],
	&stru_11BA528[20],
	&stru_11BA528[21], 
	1.0, 1.0, 0.5, 0.0 
};

#pragma pack(push, 8)
struct SelectMenuThing
{
	int dword0;
	int dword4;
	int dword8;
};
#pragma pack(pop)

struct MENU_CURSOR
{
	int enable;
	int decide;
	int x;
	int y;
};
struct MENU_SLOT
{
	int curr_garden;
	int GardenChangeFlag;
	unsigned int multiselect;
};
struct MENU_PORT
{
	ChaoData* pChaoInfo;
	int changed;
};

#pragma pack(push, 8)
struct __declspec(align(32)) ChaoSelectMenuManager_Data
{
	int purpose;
	float offsetx;
	float offsety;
	ObjectMaster* tp;
	int IsEnd;
	int IsBusy;
	int status;
	int tmpStatus;
	MENU_CURSOR cursor;
	MENU_SLOT slot[2];
	MENU_PORT port[4];
};
#pragma pack(pop)

enum SortPattern {
	SORT_NORMAL = 0,
	SORT_HERO = 1,
	SORT_DARK = 2,
	SORT_HIKKOSHI = 3,
};
FunctionPointer(void, AL_SortChaoSaveInfo, (SortPattern pattern), 0x530FE0);

DataPointer(ChaoSelectMenuManager_Data*, SelectMenuManager, 0x01AED24C);

// we want the modloader's ReplaceFile functions to affect our savefiles (ecw compatibility mod for example)
// so we use the game's file op functions instead of our own
FunctionPointer(FILE*, fopen__, (const char* a1, const char* a2), 0x07A6B8C);
FunctionPointer(size_t, fread__, (void* DstBuf, size_t ElementSize, size_t Count, FILE* File), 0x07A6E96);
FunctionPointer(size_t, fwrite__,(const void* DstBuf, size_t Size, size_t Count, FILE* File),0x007A8348);
FunctionPointer(int, fclose__, (FILE* File), 0x007A73BE);

// in CurrentChaoSaveFileString, this is the character right before the "ALF" part of the string
// it's where CWE puts the number of the multisave
DataPointer(char, CurrentMultiSave, 0x136606D);

// this is the filename string the game loads, both CWE and ECW's multisave edits this
static char* CurrentChaoSaveFileString = (char*)0x136604C;
// pointer to the second chao save (chao saves are in an array of 2, because of GC's two memory card slots)
// we use it because the SelectMenuManager supports reading from it (move menu is meant to be somewhat of a restoration)
static void* SecondChaoSaveFilePointer = (void*)0x01A02ECC;

// todo: move most of these util variables to a struct
static int selectedMultiSave = 0;
static bool updateCurrentSave = 0;

// matching the full file size of a regular ALF/chao garden save
static char saveBuffer[0xFC00];

const size_t SizeOfChaoGardenInfo = 0xCA6C;

static char selectedSavePath[] = "./resource/gd_PC/SAVEDATA/SONIC2B__ALF";

const char multiSaveIndices[10] = { '_', '1', '2', '3', '4', '5', '6', '7' ,'8' ,'9' };

enum EMenuPurpose {
	MenuPurpose_KARATE1P = 0,
	MenuPurpose_KARATE2P = 1,
	MenuPurpose_RACE1P = 2,
	MenuPurpose_RACE2P = 3,
	MenuPurpose_HIKKOSHI = 4,
};

static const int ChaoSelectMenuManager_LoadPtr = 0x00554490;
static void AL_CreateChaoSelectMenu(int a1)
{
	_asm
	{
		mov eax, a1
		call ChaoSelectMenuManager_LoadPtr
	}
}

// creates the "saving chao file" dialog (which also saves the chao file)
VoidFunc(al_confirmsave_load_zero, 0x00530230);

// to check the current save we extract the "save number" from the filename
// multiSaveIndices[] is also this array
static char GetMultiSaveIndex() {
	// ecw's multisaves are longer than the vanilla/cwe ones, that's how we detect if it's ecw's or not
	if (strlen(selectedSavePath) == strlen(CurrentChaoSaveFileString)) {
		// if length matches, we're in a vanilla/cwe file
		return CurrentMultiSave;
	}

	// ecw savefiles: save 1 (default save, SONIC2B__ALF), save 2 however is SONIC2B__ALF_02 (01 is skipped)
	// so in order it's _, 2, 3, 4, 5, 6, 7, 8, 9, 10 which we'll convert to 
	//                  _, 1, 2, 3, 4, 5, 6, 7, 8, 9
	// getting the number through atoi and doing - 1

	const size_t numberIndex = strlen(CurrentChaoSaveFileString) - 2;
	const int numberConverted = atoi(&CurrentChaoSaveFileString[numberIndex]) - 1;

	// the save is outside of the move menu's "range", so we use a character that doesn't match any of the ones
	// in our lookup table (multiSaveIndices), which allows us to move between any of the 1-10 files
	// basically i mean that none of the saves will be blocked off because they're also the current save (which might be obvious to most people)
	if (numberConverted > 9) {
		// i chose '!' as that magic character
		return '!';
	}

	// finally the character
	return '0' + numberConverted;
}

static void AL_SaveSecondFile() {
	WriteChaoSaveChecksum((char*)SecondChaoSaveFilePointer);

	// i don't remember why the offset we need is exactly 0x3040 but... yeah
	memcpy(&saveBuffer[0x3040], (void*)SecondChaoSaveFilePointer, SizeOfChaoGardenInfo); // copy back the save to the buffer

	FILE* f;
	f = fopen__(selectedSavePath, "wb");
	if (f)
	{
		fwrite__(saveBuffer, sizeof(saveBuffer), 1, f);
		fclose__(f);
	}
}

static ChaoData* AL_GetSecondFileChao() {
	int backup = ChaoSaveIndexThing;
	ChaoSaveIndexThing = 1;
	ChaoData* data = AL_GetNewChaoSaveInfo();
	ChaoSaveIndexThing = backup;
	return data;
}

static void AL_HikkoshiMenuExecutor(ObjectMaster *a1) {
	if (a1->Data1.Entity->Action == 0) {
		if (*(int*)0x1AED254 == 2) { // menu is quitting 
			AL_OdekakeMenuMaster_Data_ptr->EndFlag = 1;
			a1->MainSub = DeleteObject_;
			return;
		}
	}

	if (*(int*)0x19F645C) // if saving currently dont run logic
		return; // sadly i cant stop the selectmanager from running so you can still move the cursor
		
	if(SelectMenuManager->cursor.x == 4 && SelectMenuManager->cursor.y == 2) // the move button is vertical 2 for some reason, and horizontal 4 is shared by the second panel first tile
		a1->Data1.Entity->Rotation.x += 1280;
	else 
		a1->Data1.Entity->Rotation.x = 0;

	// move button pressed
	if (SelectMenuManager->cursor.x == 4 && SelectMenuManager->cursor.y == 2) {
		if(MenuButtons_Pressed[0] & Buttons_A) {
			ChaoData* data1 = ChaoSelectData[((SelectMenuManager->slot[0].multiselect - 1) & 7) + 8 * ((signed int)(SelectMenuManager->slot[0].multiselect - 1) >> 3)];
			ChaoData* data2 = ChaoSelectData[((SelectMenuManager->slot[1].multiselect - 1) & 7) + 8 * ((signed int)(SelectMenuManager->slot[1].multiselect - 1) >> 3) + 24];
			if (data1 || data2)
			{
				ChaoData tempSwap;
				int Data2garden, data1Garden;
				if (data2)
				{
					Data2garden = data2->data.Garden;
				}
				else
				{
					Data2garden = ((signed int)(SelectMenuManager->slot[1].multiselect - 1) >> 3) + 1;
					data2 = AL_GetSecondFileChao();
				}
				if (data2) 
				{
					memcpy(&tempSwap, data2, sizeof(ChaoData));
					if (data1)
					{
						data1Garden = data1->data.Garden;
					}
					else
					{
						data1Garden = ((signed int)(SelectMenuManager->slot[0].multiselect - 1) >> 3) + 1;
						data1 = AL_GetNewChaoSaveInfo();
					}
					if (data1) 
					{
						memcpy(data2, data1, sizeof(ChaoData));
						data2->data.Garden = Data2garden;
						memcpy(data1, &tempSwap, sizeof(ChaoData));
						data1->data.Garden = data1Garden;

						//update display
						AL_SortChaoSaveInfo(SORT_NORMAL);
						SelectMenuManager->slot[0].GardenChangeFlag = 1;
						SelectMenuManager->slot[1].GardenChangeFlag = 1;
						
						//save the file
						AL_SaveSecondFile();

						//save current file too
						al_confirmsave_load_zero();
					}
				}
			}
		}
	}

	a1->Data1.Entity->Rotation.y += 1792;

	// quit button temp(?)
	if (SelectMenuManager->cursor.x == 3 && SelectMenuManager->cursor.y == 2) {
		a1->Data1.Entity->Scale.x += 0.05f;
		if (a1->Data1.Entity->Scale.x > 1)
			a1->Data1.Entity->Scale.x = 1;
	}
	else  {
		a1->Data1.Entity->Scale.x -= 0.05f;
		if (a1->Data1.Entity->Scale.x < 0)
			a1->Data1.Entity->Scale.x = 0;
	}
	
}

static ChaoHudThingB HikkoshiUI[] = {
	{0, 21, 16, 152 ,			 65 * 2,		 (152 + 21 * 1) , (65 + 16) * 2, (NJS_TEXLIST*)0x01314058 , 6},//the numbers ~~mason~~
	{0, 21, 16, (152 + 21 * 1) , 65 * 2,		 (152 + 21 * 2) , (65 + 16) * 2 , (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 2) , 65 * 2,		 (152 + 21 * 3) , (65 + 16) * 2, (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 3) , 65 * 2,		 (152 + 21 * 4) , (65 + 16) * 2, (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 4) , 65 * 2,		 (152 + 21 * 5) , (65 + 16) * 2, (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, 152 ,			 (65 + 16) * 2 , (152 + 21 * 1) , (65 + 32) * 2 , (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 1) , (65 + 16) * 2 , (152 + 21 * 2) , (65 + 32) * 2 , (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 2) , (65 + 16) * 2,  (152 + 21 * 3) , (65 + 32) * 2, (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 3) , (65 + 16) * 2 , (152 + 21 * 4) , (65 + 32) * 2 , (NJS_TEXLIST*)0x01314058 , 6},
	{0, 21, 16, (152 + 21 * 4) , (65 + 16) * 2,  (152 + 21 * 5) , (65 + 32) * 2, (NJS_TEXLIST*)0x01314058 , 6},
	{0, 74, 16, 144 , (48.5f / 128.0f) * 256.0f, 144 + 74 , (63 / 128.0f) * 256.0f, (NJS_TEXLIST*)0x01314058 , 6}, //the SAVEFILE text
};

// the generic menu sprite array used throughout the odekake menus
DataArray(ChaoHudThingB, MenuArray, 0x11BA528, 0x61);

static void AL_HikkoshiMenuDisplayer(ObjectMaster* a1) {
	*(char*)0x25EFFCC = 0;

	//DrawChaoHudThingB(MenuArray[2], 472, 399, -1.2f, 1, 1, 0, 0); //the middle arrow thing
	float scale = njSin(a1->Data1.Entity->Rotation.x) * 0.1f + 0.85f;
	DrawChaoHudThingB((ChaoHudThingB*)0x011BB0D4, 320, 272, -1.2f, scale, scale, 0,0); //the middle arrow thing
	
	float v14 = 340;
	float a3 = 424;
	float v9 = a3 - 5.0f;
	float v7 = (njSin(a1->Data1.Entity->Rotation.y) * a1->Data1.Entity->Scale.x * 0.1f) + 1.0f;
	float sizeX = 40.0f;

	DrawChaoHudThingB(&MenuArray[40], 458 + 18 - 60,  145, -1, 1,     1, 1,  1);  //button1 
	DrawChaoHudThingB(&MenuArray[41], 458 + 18,		  145, -1, 60 + 60, 1, 0,  1);  //button2
	DrawChaoHudThingB(&MenuArray[42], 458 + 18 + 60,  145, -1, 1,     1, -1, 1); //button3

	DrawChaoHudThingB(&HikkoshiUI[10],				  458,      135, -1.2f, 1, 1, 0, 0); //number
	DrawChaoHudThingB(&HikkoshiUI[selectedMultiSave], 458 + 75, 135, -1.2f, 1, 1, 0, 0); //number

	DrawChaoHudThingB(&MenuArray[40], v14 - sizeX, a3, -1, 1, 1, 1,1); //button1
	DrawChaoHudThingB(&MenuArray[41], v14        , a3, -1, sizeX+sizeX, 1.0, 0, 1); //button2
	DrawChaoHudThingB(&MenuArray[42], v14 + sizeX, a3, -1, 1, 1, -1, 1); //button3
	v7 = (njSin(a1->Data1.Entity->Rotation.y) * a1->Data1.Entity->Scale.x * 0.1f) + 1.0f;
	float newVal = 2 - v7;
	DrawChaoHudThingB(&MenuArray[2],  v14 + 8,   v9, -1, v7, newVal, 0, 1); //back text
	v7 = (njSin(a1->Data1.Entity->Rotation.y) * a1->Data1.Entity->Scale.x * 0.1f) + 1; //HACK: patches bug that happens to what im assuming the compiler trying to preserve on stack but failing 
	DrawChaoHudThingB(&MenuArray[34], v14 - 42,  v9, -1, v7, 2 - v7, 0, 1); //swirly quit
	*(char*)0x25EFFCC = 1;
}

static void AL_HikkoshiMenu() {
	ObjectMaster* a1 = LoadObject(3, "HikkoshiMenuExecutor", AL_HikkoshiMenuExecutor, LoadObj_Data1);
	a1->DisplaySub = AL_HikkoshiMenuDisplayer;
	a1->Data1.Entity->Scale.x = 0;
}

static void AL_OdekakeMove(ODE_MENU_MASTER_WORK* a1) {
	int previousSelected;

	switch (a1->mode) {
	case 0:
		selectedMultiSave = 0;
		updateCurrentSave = true;

		CreateButtonGuide(SELECT | CONFIRM | BACK);
		LargeTitleBarExecutor_Load(AL_OdekakeMenuMaster_Data_ptr->CurrStage, 650, 66);

		AlMsgWarnCreate(0, 120, 120, 400, 200);
		AlMsgWarnOpen(0);
		AlMsgWarnAddLineC(0, "Please note that this menu might not work properly if you have more than 8 Chao in a garden.");
		AlMsgWarnAddLineC(0, "Use X and Y on the controller to switch between savefiles.");
		AlMsgWarnWaitClose(0);

		a1->mode = 5;

		break;
	case 1:
		previousSelected = selectedMultiSave;

		if (MenuButtons_Pressed[0] & Buttons_X && selectedMultiSave > 0) {
			selectedMultiSave--;
		}
		if (MenuButtons_Pressed[0] & Buttons_Y && selectedMultiSave < 9)  {
			selectedMultiSave ++;
		}
			
		// todo: is updateCurrentSave necessary if it only gets set to true here,
		// and gets set back to false right afterwards anyways?
		// the only reason i'm not removing it right now is because it's set to true in mode 0
		if (selectedMultiSave != previousSelected) {
			updateCurrentSave = true;
		}

		if (updateCurrentSave) {
			selectedSavePath[33] = multiSaveIndices[selectedMultiSave];
			updateCurrentSave = false;
			FILE* f = fopen__(selectedSavePath, "rb");
			if (f)  {
				PrintDebug("save index: %c", GetMultiSaveIndex());

				// this magic int disables the second selection menu (the big X)
				// if the selected savefile is the same one as the current one
				if (multiSaveIndices[selectedMultiSave] == GetMultiSaveIndex())
					*(int*)0x019F6458 = 0;
				else
					*(int*)0x019F6458 = 1;

				fread__(saveBuffer, sizeof(saveBuffer), 1, f);
				fclose__(f);
				memcpy(SecondChaoSaveFilePointer, &saveBuffer[0x3040], SizeOfChaoGardenInfo);
				AL_SortChaoSaveInfo(SORT_NORMAL); //updates the pointer list
			}
			else
			{
				memset(SecondChaoSaveFilePointer, 0, SizeOfChaoGardenInfo);
				memset(&ChaoSelectData[24], 0, 24 * 4); // clears the pointer list for slot 2
				*(int*)0x019F6458 = 0; // if there's no savefile in the selected slot, disable the second screen
			}

			SelectMenuManager->slot[0].GardenChangeFlag = 1;
			SelectMenuManager->slot[1].GardenChangeFlag = 1;
		}

		// objects spawned inside the odekake menu use these to signal the menu to shutdown (for example a quit button)
		if (AL_OdekakeMenuMaster_Data_ptr->EndFlag)  {
			a1->mode = 2;
		}
		break;
	case 2:
		a1->timer++;
		if (a1->timer > 30)
		{
			a1->timer = 0;
			a1->mode++;
		}
	case 3:
		if (a1->state == 0)
		{
			a1->mode++;
		}
		break;
	case 4:
		AL_OdeMenuSetNextStage(0);
		AL_OdeMenuChangeStage();
		break;
	case 5:
		// todo: use al_msg_warn.h functions for this
		if (AL_MSGWarnKinderMessageArray[0].mWin->wordsLeftMaybe <= 0) {
			AlMsgWarnDelete(0);
			*(int*)0x19F6458 = 1; //disable big X (set to 1 by save loading function)
			AL_CreateChaoSelectMenu(MenuPurpose_HIKKOSHI);
			AL_HikkoshiMenu();
			a1->mode = 1;
		}
		break;
	}
}