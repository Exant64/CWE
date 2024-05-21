#include "stdafx.h"
#include "al_odekake.h"
#include "ninja_functions.h"
#include <al_ode_guide.h>
#include <ui/al_tween.h>
#include <al_texlist.h>
#include <ALifeSDK_Functions.h>

DataPointer(int, Odekake_EnabledButtons, 0x01DB1020);
FunctionPointer(void, sub_5AC390, (char a1, float a2, float a3, __int16 a4, int* a5), 0x5AC390);

ObjectFunc(sub_5AC010, 0x5AC010);

// the number of buttons that can be visible on screen, this is used in many calculations
#define NB_BUTTONS_VISIBLE 6
// the button to start scrolling the page at (from the top)
#define START_SCROLL 3

// we init these in AL_Odekake_MainMenuBar_Finalize
static int* Odekake_EnabledButtonsCWE;
static ObjectMaster** pOdeButtons;

static const float GetButtonPosition(const int index) {
	return 209 + (56 * ((float)index - 1));
}

static const bool IsCursorOnTopOfScreen() {
	return AL_OdekakeMenuMaster_Data_ptr->cursorY < START_SCROLL;
}

static const bool IsCursorOnBottomOfScreen() {
	return AL_OdekakeMenuMaster_Data_ptr->cursorY > odekakeMenuEntries.size() - (NB_BUTTONS_VISIBLE - 1);
}

static void ScrollingLogic(ObjectMaster* a1) {
	const EASE_TYPE ease = EASE_OUT;
	const INTERP_TYPE interp = INTERP_EXPO;
	const int timer = 15;

	// if there aren't enough buttons anyways, no need to run this code
	if (odekakeMenuEntries.size() <= NB_BUTTONS_VISIBLE) {
		return;
	}

	for (size_t i = 0; i < odekakeMenuEntries.size(); i++) {
		const EntityData1* pOdeButtonData = pOdeButtons[i]->Data1.Entity;

		// buttons store their position in this field
		float* pPosY = (float*)&pOdeButtonData->Rotation.y;
		
		float targetPosY;
		if (IsCursorOnTopOfScreen()) {
			// if the selected button is on the top of the screen
			// force all buttons to just be positioned from the top of the screen
			// this is needed if "down" is pressed on the quit button to make it loop back around properly
			targetPosY = GetButtonPosition(i);
		}
		else if (IsCursorOnBottomOfScreen()) {
			// same here but from the bottom of the screen
			targetPosY = GetButtonPosition(i - (odekakeMenuEntries.size() - (NB_BUTTONS_VISIBLE - 1)) + 1);
		}
		else {
			// position the buttons relative to where the cursor is when it starts scrolling
			// (in the case of START_SCROLL = 3 for example, when you scroll the button always lands on the third position from the top)
			targetPosY = GetButtonPosition((START_SCROLL - 1) + i - AL_OdekakeMenuMaster_Data_ptr->cursorY);
		}

		// do tween to target position
		CreateTween(
			a1,
			ease,
			interp,
			pPosY,
			targetPosY,
			timer,
			nullptr
		);

		const int alphaTimer = 10;

		// if the target position is "offscreen" (above the first button or below the second one)
		if (targetPosY < GetButtonPosition(0) || targetPosY > GetButtonPosition(NB_BUTTONS_VISIBLE - 1)) {
			// don't bother to tween if already 0
			if (pOdeButtonData->Scale.z != 0) {
				// tween to complete transparency (alpha 0)
				CreateTween(
					a1,
					ease,
					interp,
					&pOdeButtons[i]->Data1.Entity->Scale.z,
					0.0f,
					alphaTimer,
					nullptr
				);
			}
		}
		else if (pOdeButtonData->Scale.z != 1) {
			// tween to opaque (alpha 1)
			CreateTween(
				a1,
				ease,
				interp,
				&pOdeButtons[i]->Data1.Entity->Scale.z,
				1.0f,
				alphaTimer,
				nullptr
			);
		}
	}
}

static bool AL_OdekakeIsGuest() {
	ChaoDataBase* pParam = GBAManager_GetChaoDataPointer();

	return Odekake_EnabledButtons && pParam && pParam->field_19 == 1;
}

static void AL_OdeScrollArrowExecutor(task* tp) {
	if (!AL_OdekakeMenuMaster_Data_ptr->EndFlag) {
		return;
	}

	if (tp->Data1.Entity->Action != 0) {
		return;
	}

	CreateTween(
		tp,
		EASE_OUT,
		INTERP_ELASTIC,
		&tp->Data1.Entity->Scale.x,
		0.0f,
		30,
		[](task* pParent) {
			DeleteObject_(pParent);
		}
	);

	tp->Data1.Entity->Action = 1;
}

static void AL_OdeScrollArrowDisplayer(task* tp) {
	SetShaders(1);
	SetChaoHUDThingBColor(tp->Data1.Entity->Scale.z, 1, 1, 1);

	static ChaoHudThingB UpArrow = { 1, 50, 25, 0, 0, 1, 0.5f, &CWE_UI_TEXLIST, 34 };
	static ChaoHudThingB GreyUpArrow = { 1, 50, 25, 0, 0.5f, 1, 1, &CWE_UI_TEXLIST, 34 };
	static ChaoHudThingB DownArrow = { 1, 50, 25, 0, 0.5f, 1, 0, &CWE_UI_TEXLIST, 34 };
	static ChaoHudThingB GreyDownArrow = { 1, 50, 25, 0, 1, 1, 0.5f, &CWE_UI_TEXLIST, 34 };

	float scl = tp->Data1.Entity->Scale.x;
	DrawChaoHudThingB(IsCursorOnTopOfScreen() ? &GreyUpArrow : &UpArrow, 320 + 140, GetButtonPosition(2) - 25 / 1.5f, -100, scl, scl, 0, 0);
	DrawChaoHudThingB(IsCursorOnBottomOfScreen() ? &GreyDownArrow : &DownArrow, 320 + 140, GetButtonPosition(2) + 25 / 1.5f, -100, scl, scl, 0, 0);
}

static void AL_CreateOdeScrollArrow(task* pParent) {
	task* tp = LoadChildObject(LoadObj_Data1, AL_OdeScrollArrowExecutor, pParent);
	tp->field_1C = AL_OdeScrollArrowDisplayer;

	tp->Data1.Entity->Scale.x = 0.0f;
	tp->Data1.Entity->Scale.z = 1.0f;
	
	CreateTween(
		tp,
		EASE_OUT,
		INTERP_ELASTIC,
		&tp->Data1.Entity->Scale.x,
		1.0f,
		30,
		nullptr
	);
}

static void AL_OdekakeButtons(char a1, float a2, float a3, __int16 a4, int* a5) {
	const int isThereChao = Odekake_EnabledButtons;
	const bool guest = AL_OdekakeIsGuest();

	for (size_t i = 0; i < odekakeMenuEntries.size(); i++) {
		const auto& entry = odekakeMenuEntries[i];

		if (entry.Flags & ODE_FLAGS_REQUIRE_CHAO)
			Odekake_EnabledButtonsCWE[i] = (isThereChao && !guest) ? 1 : 0;
		else if (entry.Flags & ODE_FLAGS_REQUIRE_NO_CHAO)
			Odekake_EnabledButtonsCWE[i] = isThereChao ? 0 : 1; //flipped
		else
			Odekake_EnabledButtonsCWE[i] = 1;

		// spawn buttons
		sub_5AC390(
			(Sint8)i,
			320,
			GetButtonPosition(i),
			38 + (short)(8 * i),
			&Odekake_EnabledButtonsCWE[i]
		);

		// hack to get the last spawned object (button is in objectlist 3)
		pOdeButtons[i] = ObjectLists[3]->NextObject;
		// default alpha with 1 if supposed to be on screen when spawned, 0 if not
		pOdeButtons[i]->Data1.Entity->Scale.z = i > 5 ? 0.0f : 1.0f;
	}

	// spawn scrolling arrows, only if there are enough buttons for it to be needed
	if (odekakeMenuEntries.size() > NB_BUTTONS_VISIBLE) {
		AL_CreateOdeScrollArrow(AL_OdekakeMenuMaster_Data_ptr->tp);
	}

	CreateButtonGuide(SELECT | CONFIRM | BACK);
}

static void NewButtonDraw(ObjectMaster* a1) {
	EntityData1* v1 = a1->Data1.Entity;
	SetChaoHUDThingBColor(*(float*)&v1->Rotation.z, 1, 1, 1);

	float v15, v23, v13, a1a, v22;
	a1a = *(float*)&v1->Rotation.x;
	v22 = *(float*)&v1->Rotation.y;
	ChaoHudThingB* v12 = odekakeMenuEntries[v1->field_2].ButtonText;
	if (!*(int*)(*(int*)(&v1->Scale.y)))
	{
		v12 = odekakeMenuEntries[v1->field_2].GreyButtonText;
	}
	v23 = v22 - 5;
	v13 = a1a - 44;
	DrawChaoHudThingB(v12, v13, v23, -100, 1, 1, -1, 1);

	if (!*(int*)(*(int*)(&v1->Scale.y)))
	{
		v15 = *(float*)0xB50FC4;
	}
	else
	{
		v15 = 0.08f;
	}
	float v16 = v15;
	float v17 = njSin(*(int*)(&v1->Position.y)) * (v1->Position.z * v16) + 1;
	float v18 = v17;
	float v19 = 2 - v17;
	float a4 = v18;
	float a1b = a1a - 72;
	DrawChaoHudThingB(odekakeMenuEntries[v1->field_2].ButtonIcon, a1b, v23, -100, a4, v19, 0, 1);
	SetChaoHUDThingBColor(1, 1, 1, 1);
}

static void ButtonDraw(ObjectMaster* tp) {
	// we store our alpha for the scrolling effect in scale z

	// i believe this alpha value is used for the text and icon on the button?
	float* rotZAlphaThing = (float*)(&tp->Data1.Entity->Rotation.z);
	float backupRotAlpha = *rotZAlphaThing;

	// when it sets it to this alpha, it would overwrite the one we set below
	// so we multiply it together to make our alpha affect this one properly
	*rotZAlphaThing *= tp->Data1.Entity->Scale.z;

	// set our one alpha
	SetChaoHUDThingBColor(tp->Data1.Entity->Scale.z, 1, 1, 1);
	sub_5AC010(tp);
	NewButtonDraw(tp);

	// restore original "text/icon alpha"
	*rotZAlphaThing = backupRotAlpha;
}

void AL_Odekake_MenuMaster_Selection() {
	ObjectMaster* tp = AL_OdekakeMenuMaster_Data_ptr->tp;

	if (MenuButtons_Pressed[0] & Buttons_Up) {
		--AL_OdekakeMenuMaster_Data_ptr->cursorY;
		if (AL_OdekakeMenuMaster_Data_ptr->cursorY < 0) {
			AL_OdekakeMenuMaster_Data_ptr->cursorY = odekakeMenuEntries.size() - 1;
		}
		PlaySoundProbably(0x8000, 0, 0, 0);

		// trigger scrolling
		ScrollingLogic(tp);
	}

	if (MenuButtons_Pressed[0] & Buttons_Down) {
		++AL_OdekakeMenuMaster_Data_ptr->cursorY;
		if (AL_OdekakeMenuMaster_Data_ptr->cursorY > odekakeMenuEntries.size() - 1) {
			AL_OdekakeMenuMaster_Data_ptr->cursorY = 0;
		}

		PlaySoundProbably(0x8000, 0, 0, 0);

		// trigger scrolling
		ScrollingLogic(tp);
	}

	if (MenuButtons_Pressed[0] & Buttons_B) {
		AL_OdekakeMenuMaster_Data_ptr->cursorY = odekakeMenuEntries.size() - 1;

		// trigger scrolling when jumping to exit button
		ScrollingLogic(tp);
	}
}

void AL_Odekake_MainMenuBar_Finalize() {
	// we memleak these :)
	Odekake_EnabledButtonsCWE = (int*)calloc(1, sizeof(int) * odekakeMenuEntries.size());
	pOdeButtons = (ObjectMaster**)calloc(1, sizeof(ObjectMaster*) * odekakeMenuEntries.size());

	// mainmenu checks if the button is grey or not through this
	WriteData((int*)0x5A6DC6, (int)Odekake_EnabledButtonsCWE);
}

void AL_Odekake_MainMenuBar_Init() {
	// kill the scrolling code so we can have our own in the MenuMaster hook
	WriteData<6>((char*)0x005A6EBE, (char)0x90);

	// new button renderer
	WriteData((int*)0x5ac3da, (int)ButtonDraw);
	WriteData<5>((char*)0x005AC2AD, (char)0x90);
	WriteData<5>((char*)0x005AC345, (char)0x90);

	// hook button creation
	WriteCall((void*)0x005A6CFA, AL_OdekakeButtons);

	// kills first button so i can fully create my own
	WriteData<5>((char*)0x005A6CD3, (char)0x90);
}