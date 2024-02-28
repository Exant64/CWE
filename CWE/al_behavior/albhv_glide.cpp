#include "stdafx.h"
#include <ALifeSDK_Functions.h>
#include <AL_ModAPI.h>
#include <al_behavior/al_behavior.h>

int __cdecl ALBHV_Glide(ObjectMaster* a1)
{
	ObjectMaster* v2; // ebx
	UnknownData2* v3; // esi
	ChaoData1* v4; // edi

	v2 = a1;
	v3 = a1->EntityData2;
	v4 = a1->Data1.Chao;

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
			v3->speed.y = 0;
			v3->velocity.y = 0.25f;
			//v4->entity.Position.y += 0.05f;
			if (v4->entity.Position.y > 25)
			{
				v4->entity.Position.y = 25;
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
			if (v3->field_40 & 0x400)
				AL_SetBehaviorWithTimer(a1, (int)0x00562D20, -1); //00562D20 = albhv_move

			v3->speed.y = v3->gravity * 0.05f;
		}
	}
	return BHV_RET_CONTINUE;
}

int __cdecl ALBHV_GlideCondition(ObjectMaster* a1)
{
	if (a1->Data1.Chao->pParamGC->Type == ChaoType_Tails)
		AL_SetBehavior(a1, ALBHV_Glide);
	else
		AL_SetBehavior(a1, (BHV_FUNC)0x005630C0);
	return BHV_RET_CONTINUE;
}
//tails fly
//WriteData((int*)0x05A3227, (int)& ALBHV_GlideCondition);