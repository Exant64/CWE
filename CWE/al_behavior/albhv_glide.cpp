#include "stdafx.h"
#include <ALifeSDK_Functions.h>
#include <AL_ModAPI.h>
#include <al_behavior/al_behavior.h>

int __cdecl ALBHV_Glide(task* a1)
{
	task* v2; // ebx
	MOVE_WORK* v3; // esi
	chaowk* v4; // edi

	v2 = a1;
	v3 = a1->EntityData2;
	v4 = GET_CHAOWK(a1);

	if (!v4->Behavior.Mode)
	{
		AL_FaceChangeEye(a1, 4);
		Chao_RegAnimation(a1, "alm_tailsfly");
		++v4->Behavior.Mode;
		v4->Behavior.Timer = (500 + (signed int)(njRandom() * 301.f));
	}
	else
	{
		MOV_TurnToAim2(v2, 256);
		if (v4->Behavior.Mode == 1)
		{
			v3->Acc.y = 0;
			v3->Velo.y = 0.25f;
			//v4->Position.y += 0.05f;
			if (v4->pos.y > 25)
			{
				v4->pos.y = 25;
			}
			v4->Behavior.Timer--;
			if (v4->Behavior.Timer <= 0)
			{
				v4->Behavior.Mode++;
				Chao_RegAnimation(a1, "alm_tailsfly_tired");
				AL_FaceChangeEye(a1, ChaoEyes_Painful);
				AL_FaceChangeMouth(a1, ChaoMouth_ClosedFrown);
			}
		}
		else if (v4->Behavior.Mode == 2)
		{
			if (v3->Flag & 0x400)
				AL_SetBehaviorWithTimer(a1, (int)0x00562D20, -1); //00562D20 = albhv_move

			v3->Acc.y = v3->Gravity * 0.05f;
		}
	}
	return BHV_RET_CONTINUE;
}

int __cdecl ALBHV_GlideCondition(task* a1)
{
	if (GET_CHAOPARAM(a1)->type == ChaoType_Tails)
		AL_SetBehavior(a1, ALBHV_Glide);
	else
		AL_SetBehavior(a1, (BHV_FUNC)0x005630C0);
	return BHV_RET_CONTINUE;
}
//tails fly
//WriteData((int*)0x05A3227, (int)& ALBHV_GlideCondition);