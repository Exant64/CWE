#include "stdafx.h"
#include "include/code_drown.h"
#include <Chao.h>
#include <random>

int __cdecl ALBHV_Drown(task* a1)
{
	if (!a1 || !GET_CHAOWK(a1) || !a1->mwp)
		return 0;

	MOVE_WORK* move = (MOVE_WORK*)a1->mwp;
	if (a1->twp->pos.y <= move->WaterY - 2.1f)
	{
		a1->twp->pos.y += 0.1f;
		if (a1->twp->pos.y > move->WaterY - 2.1f)
			a1->twp->pos.y = move->WaterY - 2.1f;
		move->Acc.y = -move->Gravity - 0.1f * move->Velo.y;
	}
	int modePre = GET_CHAOWK(a1)->Behavior.Mode;
	int returnVal = ((BHV_FUNC)0x00568F60)(a1);
	if (modePre == 0 && GET_CHAOWK(a1)->Behavior.Mode == 1)
	{
		AL_SetMotionLink(a1, 151);
	}
	return returnVal;
}

int __cdecl ALBHV_StruggleSwim_DrownCheck(task* a1)
{
	if (GET_CHAOWK(a1)->Behavior.SubTimer % (3 * 60) == 0) //every 10 seconds
		GET_CHAOPARAM(a1)->like--;

	if (GET_CHAOPARAM(a1)->like <= -100)
	{
		AL_SetBehavior(a1, ALBHV_Drown);
		return 0;
	}
	return ((BHV_FUNC)0x561E00)(a1);
}

void ChaoDrown::Init() {
	WriteData((int*)(0x0056239A - 4), (int)ALBHV_StruggleSwim_DrownCheck);
}