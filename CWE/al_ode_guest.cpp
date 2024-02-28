#include "stdafx.h"

#include "al_msg_warn.h"
#include "al_odekake.h"
#include "ninja_functions.h"
#include <cstdio>
#include "al_ode_guide.h"

//please please please clean this up
const int someUIProjectionCodeCopyPtr = 0x0055A060;
void someUIProjectionCodeCopy(NJS_VECTOR* a1, NJS_VECTOR* a2)
{
	__asm
	{
		mov edi, a1
		mov esi, a2
		call someUIProjectionCodeCopyPtr
	}
}

ObjectMaster* pGuestChao = 0;
void AL_OdekakeGuest(ODE_MENU_MASTER_WORK* a1)
{
	NJS_VECTOR posIn = { 240, 300, -25 };

	switch (a1->mode)
	{
	case 0:
		CreateButtonGuide(BACK);
		LargeTitleBarExecutor_Load(AL_OdekakeMenuMaster_Data_ptr->CurrStage, 650, 66);

		ChaoParamWindowExecutor_Load(160, 248);
		/*
		someUIProjectionCodeCopy(&posIn, &posOut);
		pGuestChao = CreateChao((ChaoData*)GBAManager_GetChaoDataPointer(), 0, 0, &posOut, 0);
		pGuestChao->Data1.Chao->field_B0 &= ~8u;
		pGuestChao->Data1.Chao->field_B0 &= ~2u;
		pGuestChao->Data1.Chao->field_B0 &= ~0x10u;
		*/

		AlMsgWarnCreate(0,300, 125, 320, 250);
		AlMsgWarnOpen(0);
		AlMsgWarnAddLineC(0,"If you export this Chao through the editor while you're in this menu, the exported file will be a Guest Chao. You can send Guest Chao to other players.");
		AlMsgWarnWaitClose(0);
		AlMsgWarnAddLineC(0, "Guest Chao are immortal, and can't be edited.");

		GBAManager_GetChaoDataPointer()->field_19 = 1;

		a1->mode++;
		break;
	case 1:
		if (MenuButtons_Pressed[0] & Buttons_B) {
			AL_OdekakeMenuMaster_Data_ptr->EndFlag = 1;

			//DeleteObject_(pGuestChao);
			//pGuestChao = nullptr;

			GBAManager_GetChaoDataPointer()->field_19 = 0;
			a1->mode++;
		}
		break;
	case 2:
		if (AL_MSGWarnKinderMessageArray[0].mWin->wordsLeftMaybe > 0) break;

		AL_OdekakeMenuMaster_Data_ptr->timer++;
		if (AL_OdekakeMenuMaster_Data_ptr->timer > 60) {
			AlMsgWarnDelete(0);

			AL_OdekakeMenuMaster_Data_ptr->NextStage = 0;

			AL_OdekakeMenuMaster_Data_ptr->PreStage = AL_OdekakeMenuMaster_Data_ptr->CurrStage;
			AL_OdekakeMenuMaster_Data_ptr->CurrStage = AL_OdekakeMenuMaster_Data_ptr->NextStage;
			AL_OdekakeMenuMaster_Data_ptr->mode = 0;
			AL_OdekakeMenuMaster_Data_ptr->timer = 0;
			AL_OdekakeMenuMaster_Data_ptr->subtimer = 0;
			AL_OdekakeMenuMaster_Data_ptr->state = 0;
			AL_OdekakeMenuMaster_Data_ptr->cursorX = 0;
			AL_OdekakeMenuMaster_Data_ptr->cursorY = 2;
			AL_OdekakeMenuMaster_Data_ptr->EndFlag = 0;
			AL_OdekakeMenuMaster_Data_ptr->mfStageExit = 0;
			AL_OdekakeMenuMaster_Data_ptr->mpStageWork = 0;			
		}
		break;
	}
}