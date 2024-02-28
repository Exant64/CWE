#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "../AL_ModAPI.h"
#include "../ChaoMain.h"
#include "../al_console.h"

#include <al_behavior/al_behavior.h>
#include <al_behavior/al_intention.h>
signed int __cdecl ALBHV_TouchConsole(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	signed int v3; // eax
	NJS_VECTOR a3; // [esp+8h] [ebp-Ch]

	v1 = (ChaoData1*)a1->Data1.Chao;
	if (v1->Behavior.Mode)
	{
		if (v1->Behavior.Mode != 1)
		{
		LABEL_3:
			AL_Brake(a1, 0.9f);
			return BHV_RET_CONTINUE;
		}
	}
	else
	{
		AL_SetMotionLink(a1, 193);
		++v1->Behavior.Mode;
		v1->Behavior.Timer = 0;
	}
	v3 = v1->Behavior.Timer;
	v1->Behavior.Timer = v3 + 1;
	if (v3 <= 60)
	{
		goto LABEL_3;
	}

	//spawn in front of chao
	a3.x = njSin(v1->entity.Rotation.y) * 3.0f + v1->entity.Position.x;
	a3.y = v1->entity.Position.y + 1.0f;
	a3.z = njCos(v1->entity.Rotation.y) * 3.0f + v1->entity.Position.z;
	//touch
	AL_FieldExecutor_Load(CI_KIND_AL_TOUCH, &a3, 3.5f, 10);
	//wii touch
	//AL_FieldExecutor_Load(303, &a3, 3.5f, 10);

	return BHV_RET_FINISH;
}
int ALBHV_PlayConsole(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	int v5; // eax

	v1 = (ChaoData1*)a1->Data1.Chao;
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	if (v2)
	{
		if (v2 != 1)
		{
			return BHV_RET_CONTINUE;
		}
	}
	else
	{
		consolePtr->Data1.Entity->Action = 1;

		//TV->Action = 13;
		//v2->InvulnerableTime = 0;
		//v2->field_A = (njRandom()) * 4;

		AL_SetMotionLink(a1, 330);
		AL_FaceChangeEye(a1, 4);
		AL_SetItem(a1, 11, &wii_remote_bhav, &AL_WII_TEXLIST);
		++v3->Mode;
		v3->Timer = (unsigned __int16)(600 * 2 + (njRandom() * 201.0));
	}
	v5 = v3->Timer;
	v3->Timer = v5 - 1;
	if (v5 < 0 && !ALO_Field_Find_(a1, 1, 201))
	{
		consolePtr->Data1.Entity->Action = 0;
		//set cooldown timer
		AL_SetIntervalTimer(
			a1,
			INT_TIMER_LTOY,
			(unsigned __int16)(1800 + (njRandom() * 1801.f)));
		return BHV_RET_FINISH;
	}
	return BHV_RET_CONTINUE;
}
void ALBHV_GoToConsole(ObjectMaster* a1)
{
	AL_SetBehaviorWithTimer(a1, (int)0x55C3C0, -1);
	Chao_BehaviourQueue(a1, (int)0x56B480);
	Chao_BehaviourQueue(a1, (int)0x56BA80);
	Chao_BehaviourQueue(a1, (int)ALBHV_TouchConsole);
	Chao_BehaviourQueue(a1, (int)0x56B560); //so this is the first TV thing
	Chao_BehaviourQueue(a1, (int)0x56B6C0); //then theres this which handles the rotating towards TV and the sitting and all that
	Chao_BehaviourQueue(a1, (int)ALBHV_PlayConsole); //i thought this is only clapping but it seems like this is the main TV behav, so changed it to the Wii
}
