#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "../AL_ModAPI.h"
#include "../ChaoMain.h"
#include "../data/toy/al_toy_rattle.nja"
#include "../al_omochao_build.h"
#include "al_knowledge.h"
#include <al_stage.h>
#include <al_garden_info.h>

int __cdecl ALBHV_Wrench(ObjectMaster* a1)
{
	switch (a1->Data1.Chao->Behavior.Mode)
	{
	case 0:
		AL_SetMotionLink(a1, 191);
		++a1->Data1.Chao->Behavior.Mode;
		break;
	case 1:
		MOV_TurnToAim2(a1, 384);
		AL_ForwardAcc(a1, ChaoGlobal.WalkAcc * 0.8f);
		if (MOV_DistFromAim(a1) >= 0.0)
		{
			if (MOV_DistFromAim(a1) < 4.0)
			{
				++a1->Data1.Chao->Behavior.Mode;
				al_entry_work* v7 = ALW_IsCommunicating(a1);

				//failsafe
				if (!v7 || !v7->tp)
					return BHV_RET_FINISH;

				if (AL_GetItemSaveInfo(v7->tp))
				{
					void* v9 = AL_GetItemSaveInfo(v7->tp);
					AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)v9);
					AL_ClearItemSaveInfoPtr(v7->tp);
				}
				v7->tp->MainSub = DeleteObject_;
			}
		}
		break;
	case 2:
		sub_54A690(a1);
		sub_54A730(a1);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);
		Chao_RegAnimation(a1, "alm_wrench_start");//AL_SetMotionLink(a1, 231);
		AL_SetItem(a1, 11, &object_al_toy_wrench, &AL_EYE_TEXLIST);
		a1->Data1.Chao->Behavior.Mode++;
		a1->Data1.Chao->Behavior.Timer = (int)(njRandom() * 300.f) + 600;
		break;
	case 3:
		a1->Data1.Chao->Behavior.Timer--;
		if (a1->Data1.Chao->Behavior.Timer <= 0)
		{
			GetOmoData()->phase |= MANNEQUIN;
			if (GetOmoData()->phase == 0xFF)
				return BHV_RET_FINISH;

			int flag = 0;
			do
			{
				int random = (int)(njRandom() * 7.99f);
				flag = (1 << random);
			} 
			while (GetOmoData()->phase & flag); 

			GetOmoData()->phase |= flag;
			return BHV_RET_FINISH;
		}
		break;
	}
	AL_MoveHoldingObject(a1);
	return BHV_RET_CONTINUE;
}

int __cdecl ALBHV_BuildStart(ObjectMaster* a1)
{
	OMOCHAO_INFO* omo = GetOmoData();

	MOV_SetAimPos(a1, &OmoPositions[AL_GetStageNumber() - 1]);
	
	//a1->EntityData2->Waypoint.y = CalculateFalloffPosition_(OmoPositions[CurrentChaoArea - 1].x, 25, OmoPositions[CurrentChaoArea - 1].z, &a1->Data1.Entity->Rotation);
	if (omo->phase == 0 || !AL_KW_IDExists(omo->chaoID))
	{
		ChaoDataBase* pParamGC = a1->Data1.Chao->pParamGC;

		if (!AL_KW_IDExists(omo->chaoID))
		{
			omo->phase = 0;
		}
		omo->color = pParamGC->Color;
		omo->shiny = pParamGC->Shiny;
		omo->monotone = pParamGC->MonotoneHighlights;
		omo->jewel = pParamGC->Texture;
		omo->chaoID = pParamGC->ChaoID;
	}
	AL_SetBehavior(a1, ALBHV_Wrench);
	return BHV_RET_CONTINUE;
}