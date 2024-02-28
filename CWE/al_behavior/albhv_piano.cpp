#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "../ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include "../Chao.h"
#include <random>
#include "../al_piano.h"
#include "../al_world.h"
#include "albhv.h"
#include <al_face.h>

signed int __cdecl ALBHV_PlayPiano(ObjectMaster* a1)
{
	ChaoData1* data1 = (ChaoData1*)a1->Data1.Chao;
	UnknownData2* data2 = a1->Data2.Unknown_Chao;
	if (data1->Behavior.Mode)
	{
		a1->Data1.Chao->Behavior.IntervalTimer[8] = (unsigned __int16)(1800 //INT_TIMER_LTOY
			+ (signed int)(njRandom() * 1801.f));

		return !ALO_Field_Find_((ObjectMaster*)a1, 1, 165);
	}
	else
	{
		NJS_VECTOR position;
		ObjectMaster* toy = (ObjectMaster*)ALW_GetLockOnTask(a1);
		SetPianoWaypoint(toy, &position);

		position.y = a1->Data1.Entity->Position.y;
		a1->Data1.Entity->Position = position;
		a1->Data1.Entity->Rotation.y = toy->Data1.Entity->Rotation.y + 0x8000;

		switch (toy->Data1.Entity->Index)
		{
		case PIANOTYPE_PIANO:
			AL_SetMotionLink(a1, 340);
			break;
		case PIANOTYPE_ORGAN:
			AL_SetMotionLink(a1, 341);
			break;
		}

		AL_FaceChangeEye(a1, ChaoEyes_ClosedUp);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);
		++data1->Behavior.Mode;

		int timer = (int)(1800 + (njRandom() * 800.f)) * 2;

		a1->Data1.Chao->ObjectMaster_ptr1 = AL_FieldExecutor_Load(
			CI_KIND_AL_RANDOM_MUSIC,
			&a1->Data1.Entity->Position,
			20,
			timer);
		a1->Data1.Chao->ObjectMaster_ptr2 = AL_FieldExecutor_Load( //165 is shared with bands, so that shouldn't stop the pianist to join, which is why i created 225
			CI_KIND_AL_PIANO,
			&a1->Data1.Entity->Position,
			20,
			timer);
	}

	return BHV_RET_CONTINUE;
}

signed int __cdecl ALBHV_GoToPiano(ObjectMaster* a1)
{
	ObjectMaster* v1; // edi
	NJS_VECTOR a2; // [esp+8h] [ebp-Ch]

	v1 = AL_GetFoundToyTask(a1);
	//if theres already a piano field or if any chao is playing the piano
	if (!v1 || ALW_IsSheAttentionOtherOne(a1, v1) || ALO_Field_Find_(a1, 1, CI_KIND_AL_PIANO))
	{
		return BHV_RET_FINISH;
	}

	SetPianoWaypoint(v1, &a2);
	//a2.y = a1->Data1.Entity->Position.y;
	ALW_LockOn(a1, v1);
	ALW_CommunicationOn(a1, v1);
	a1->EntityData2->Waypoint.x = a2.x;
	a1->EntityData2->Waypoint.z = a2.z;

	AL_SetBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x55C3C0>);
	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B480>);
	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B560>);
	AL_SetNextBehavior(a1, ALBHV_ToyMoveCheck<ALBHV_PlayPiano>);

	return BHV_RET_CONTINUE;

}