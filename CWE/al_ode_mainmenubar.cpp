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
static task** pOdeButtons;

static const float GetButtonPosition(const int index) {
	return 209 + (56 * ((float)index - 1));
}

static const bool IsCursorOnTopOfScreen() {
	return AL_OdekakeMenuMaster_Data_ptr->cursorY < START_SCROLL;
}

static const bool IsCursorOnBottomOfScreen() {
	return AL_OdekakeMenuMaster_Data_ptr->cursorY > odekakeMenuEntries.size() - (NB_BUTTONS_VISIBLE - 1);
}

static void ScrollingLogic(task* a1) {
	const EASE_TYPE ease = EASE_OUT;
	const INTERP_TYPE interp = INTERP_EXPO;
	const int timer = 15;

	// if there aren't enough buttons anyways, no need to run this code
	if (odekakeMenuEntries.size() <= NB_BUTTONS_VISIBLE) {
		return;
	}

	for (size_t i = 0; i < odekakeMenuEntries.size(); i++) {
		const taskwk* pOdeButtonData = pOdeButtons[i]->twp;

		// buttons store their position in this field
		float* pPosY = (float*)&pOdeButtonData->ang.y;
		
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
			if (pOdeButtonData->scl.z != 0) {
				// tween to complete transparency (alpha 0)
				CreateTween(
					a1,
					ease,
					interp,
					&pOdeButtons[i]->twp->scl.z,
					0.0f,
					alphaTimer,
					nullptr
				);
			}
		}
		else if (pOdeButtonData->scl.z != 1) {
			// tween to opaque (alpha 1)
			CreateTween(
				a1,
				ease,
				interp,
				&pOdeButtons[i]->twp->scl.z,
				1.0f,
				alphaTimer,
				nullptr
			);
		}
	}
}

static bool AL_OdekakeIsGuest() {
	CHAO_PARAM_GC* pParam = GBAManager_GetChaoDataPointer();

	return Odekake_EnabledButtons && pParam && pParam->field_19 == 1;
}

static void AL_OdeScrollArrowExecutor(task* tp) {
	if (!AL_OdekakeMenuMaster_Data_ptr->EndFlag) {
		return;
	}

	if (tp->twp->mode != 0) {
		return;
	}

	CreateTween(
		tp,
		EASE_OUT,
		INTERP_ELASTIC,
		&tp->twp->scl.x,
		0.0f,
		30,
		[](task* pParent) {
			DestroyTask(pParent);
		}
	);

	tp->twp->mode = 1;
}

static void AL_OdeScrollArrowDisplayer(task* tp) {
	SetShaders(1);
	chSetBillboardColor(tp->twp->scl.z, 1, 1, 1);

	static CHS_BILL_INFO UpArrow = { 1, 50, 25, 0, 0, 1, 0.5f, &CWE_UI_TEXLIST, 34 };
	static CHS_BILL_INFO GreyUpArrow = { 1, 50, 25, 0, 0.5f, 1, 1, &CWE_UI_TEXLIST, 34 };
	static CHS_BILL_INFO DownArrow = { 1, 50, 25, 0, 0.5f, 1, 0, &CWE_UI_TEXLIST, 34 };
	static CHS_BILL_INFO GreyDownArrow = { 1, 50, 25, 0, 1, 1, 0.5f, &CWE_UI_TEXLIST, 34 };

	float scl = tp->twp->scl.x;
	chDrawBillboardSR(IsCursorOnTopOfScreen() ? &GreyUpArrow : &UpArrow, 320 + 140, GetButtonPosition(2) - 25 / 1.5f, -100, scl, scl, 0, 0);
	chDrawBillboardSR(IsCursorOnBottomOfScreen() ? &GreyDownArrow : &DownArrow, 320 + 140, GetButtonPosition(2) + 25 / 1.5f, -100, scl, scl, 0, 0);
}

static void AL_CreateOdeScrollArrow(task* pParent) {
	task* tp = CreateChildTask(LoadObj_Data1, AL_OdeScrollArrowExecutor, pParent);
	tp->disp_dely = AL_OdeScrollArrowDisplayer;

	tp->twp->scl.x = 0.0f;
	tp->twp->scl.z = 1.0f;
	
	CreateTween(
		tp,
		EASE_OUT,
		INTERP_ELASTIC,
		&tp->twp->scl.x,
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
		pOdeButtons[i] = ObjectLists[3]->last;
		// default alpha with 1 if supposed to be on screen when spawned, 0 if not
		pOdeButtons[i]->twp->scl.z = i > 5 ? 0.0f : 1.0f;
	}

	// spawn scrolling arrows, only if there are enough buttons for it to be needed
	if (odekakeMenuEntries.size() > NB_BUTTONS_VISIBLE) {
		AL_CreateOdeScrollArrow(AL_OdekakeMenuMaster_Data_ptr->tp);
	}

	CreateButtonGuide(SELECT | CONFIRM | BACK);
}

static void NewButtonDraw(task* a1) {
	taskwk* v1 = a1->twp;
	chSetBillboardColor(*(float*)&v1->ang.z, 1, 1, 1);

	float v15, v23, v13, a1a, v22;
	a1a = *(float*)&v1->ang.x;
	v22 = *(float*)&v1->ang.y;
	CHS_BILL_INFO* v12 = odekakeMenuEntries[v1->id].ButtonText;
	if (!*(int*)(*(int*)(&v1->scl.y)))
	{
		v12 = odekakeMenuEntries[v1->id].GreyButtonText;
	}
	v23 = v22 - 5;
	v13 = a1a - 44;
	chDrawBillboardSR(v12, v13, v23, -100, 1, 1, -1, 1);

	if (!*(int*)(*(int*)(&v1->scl.y)))
	{
		v15 = *(float*)0xB50FC4;
	}
	else
	{
		v15 = 0.08f;
	}
	float v16 = v15;
	float v17 = njSin(*(int*)(&v1->pos.y)) * (v1->pos.z * v16) + 1;
	float v18 = v17;
	float v19 = 2 - v17;
	float a4 = v18;
	float a1b = a1a - 72;
	chDrawBillboardSR(odekakeMenuEntries[v1->id].ButtonIcon, a1b, v23, -100, a4, v19, 0, 1);
	chSetBillboardColor(1, 1, 1, 1);
}

static void ButtonDraw(task* tp) {
	// we store our alpha for the scrolling effect in scale z

	// i believe this alpha value is used for the text and icon on the button?
	float* rotZAlphaThing = (float*)(&tp->twp->ang.z);
	float backupRotAlpha = *rotZAlphaThing;

	// when it sets it to this alpha, it would overwrite the one we set below
	// so we multiply it together to make our alpha affect this one properly
	*rotZAlphaThing *= tp->twp->scl.z;

	// set our one alpha
	chSetBillboardColor(tp->twp->scl.z, 1, 1, 1);
	sub_5AC010(tp);
	NewButtonDraw(tp);

	// restore original "text/icon alpha"
	*rotZAlphaThing = backupRotAlpha;
}

void AL_Odekake_MenuMaster_Selection() {
	task* tp = AL_OdekakeMenuMaster_Data_ptr->tp;

	if (SWDATAE[0] & BTN_UP) {
		--AL_OdekakeMenuMaster_Data_ptr->cursorY;
		if (AL_OdekakeMenuMaster_Data_ptr->cursorY < 0) {
			AL_OdekakeMenuMaster_Data_ptr->cursorY = odekakeMenuEntries.size() - 1;
		}
		PlaySoundProbably(0x8000, 0, 0, 0);

		// trigger scrolling
		ScrollingLogic(tp);
	}

	if (SWDATAE[0] & BTN_DOWN) {
		++AL_OdekakeMenuMaster_Data_ptr->cursorY;
		if (AL_OdekakeMenuMaster_Data_ptr->cursorY > odekakeMenuEntries.size() - 1) {
			AL_OdekakeMenuMaster_Data_ptr->cursorY = 0;
		}

		PlaySoundProbably(0x8000, 0, 0, 0);

		// trigger scrolling
		ScrollingLogic(tp);
	}

	if (SWDATAE[0] & BTN_B) {
		AL_OdekakeMenuMaster_Data_ptr->cursorY = odekakeMenuEntries.size() - 1;

		// trigger scrolling when jumping to exit button
		ScrollingLogic(tp);
	}
}

void AL_Odekake_MainMenuBar_Finalize() {
	// we memleak these :)
	Odekake_EnabledButtonsCWE = (int*)calloc(1, sizeof(int) * odekakeMenuEntries.size());
	pOdeButtons = (task**)calloc(1, sizeof(task*) * odekakeMenuEntries.size());

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