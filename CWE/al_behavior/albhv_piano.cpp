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
#include <util.h>

static int GetPianoType (task* pToy) {
	return pToy->Data1.Entity->Index;
}

static int ALBHV_PlayPiano(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;
	UnknownData2* move = tp->Data2.Unknown_Chao;

	task* pToy;
	NJS_POINT3 toyPos;
	int timer;

	switch (bhv->Mode) {
	case 0:
		pToy = ALW_GetLockOnTask(tp);
		SetPianoWaypoint(pToy, &toyPos);

		toyPos.y = work->entity.Position.y;
		work->entity.Position = toyPos;
		work->entity.Rotation.y = pToy->Data1.Entity->Rotation.y + 0x8000;

		switch (GetPianoType(pToy))
		{
		case PIANOTYPE_PIANO:
			AL_SetMotionLink(tp, ALM_PIANO);
			break;
		case PIANOTYPE_ORGAN:
			AL_SetMotionLink(tp, ALM_ORGAN);
			break;
		}

		AL_FaceChangeEye(tp, ChaoEyes_ClosedUp);
		AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);

		++bhv->Mode;

		timer = (int)(1800 + (njRandom() * 800.f)) * 2;

		work->ObjectMaster_ptr1 = AL_FieldExecutor_Load(
			CI_KIND_AL_RANDOM_MUSIC,
			&work->entity.Position,
			20,
			timer);
		work->ObjectMaster_ptr2 = AL_FieldExecutor_Load( //165 is shared with bands, so that shouldn't stop the pianist to join, which is why i created 225
			CI_KIND_AL_PIANO,
			&work->entity.Position,
			20,
			timer);
		break;
	case 1:
		if (!ALO_Field_Find_(tp, 1, CI_KIND_AL_RANDOM_MUSIC)) {
			bhv->IntervalTimer[INT_TIMER_LTOY] = 1800 + (njRandom() * 1801.f);
			return BHV_RET_FINISH;
		}
	}

	return BHV_RET_CONTINUE;
}

static int ALBHV_InterpolateToPiano(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	task* pToy = ALW_GetLockOnTask(tp);
	NJS_POINT3 pianoPos;

	const int TIME_TO_LERP = 90;
	const int ANGLE_SPD = 384;

	SetPianoWaypoint(pToy, &pianoPos);
	
	const Angle targetAng = pToy->Data1.Entity->Rotation.y + 0x8000;
	MOV_SetAimPos(tp, &pianoPos);

	switch (bhv->Mode) {
	case 0:
		bhv->Timer = 0;
		bhv->Mode++;
		[[fallthrough]];
	case 1:
		MOV_TurnToAim2(tp, ANGLE_SPD);

		{
			const float speed = GET_GLOBAL()->WalkAcc * 0.8f;
			tp->EntityData2->speed.x = njSin(work->entity.Rotation.y) * speed;
			tp->EntityData2->speed.z = njCos(work->entity.Rotation.y) * speed;
		}
		
		if (MOV_DistFromAimXZ(tp) < 0.5f) {
			bhv->Mode++;
		}
		break;
	case 2:
		work->entity.Rotation.y = AdjustAngle(work->entity.Rotation.y, targetAng, ANGLE_SPD);

		if (abs(work->entity.Rotation.y - targetAng) <= ANGLE_SPD) {
			return BHV_RET_FINISH;
		}
		break;
	}

	return BHV_RET_CONTINUE;
}

int ALBHV_GoToPiano(task* tp) {
	task *pToy = AL_GetFoundToyTask(tp);
	NJS_POINT3 toyPos;

	//if theres already a piano field or if any chao is playing the piano
	if (!pToy || ALW_IsSheAttentionOtherOne(tp, pToy) || ALO_Field_Find_(tp, 1, CI_KIND_AL_PIANO))
	{
		return BHV_RET_FINISH;
	}

	SetPianoWaypoint(pToy, &toyPos);
	ALW_LockOn(tp, pToy);
	ALW_CommunicationOn(tp, pToy);

	MOV_SetAimPos(tp, &toyPos);

	AL_SetBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>); // PostureChangeStand
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B480>); // Notice
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<(BHV_FUNC)0x56B560>); // GoToAim
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_InterpolateToPiano>);
	switch (GetPianoType(pToy)) {
		case PIANOTYPE_PIANO:
			AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeSit>); // PostureChangeSit
			break;	
		case PIANOTYPE_ORGAN:
			AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>); // PostureChangeStand
			break;
	}
	AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PlayPiano>);

	return BHV_RET_CONTINUE;

}