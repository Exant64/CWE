#include "stdafx.h"
#include "include/code_drown.h"
#include <Chao.h>
#include <random>

int __cdecl ALBHV_Drown(task* a1)
{
	if (!a1 || !GET_CHAOWK(a1) || !a1->EntityData2)
		return 0;
	if (a1->twp->pos.y <= a1->EntityData2->field_DC - 2.1f)
	{
		a1->twp->pos.y += 0.1f;
		if (a1->twp->pos.y > a1->EntityData2->field_DC - 2.1f)
			a1->twp->pos.y = a1->EntityData2->field_DC - 2.1f;
		a1->EntityData2->speed.y = -a1->EntityData2->gravity - 0.1f * a1->EntityData2->velocity.y;
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
		GET_CHAOPARAM(a1)->Happiness--;

	if (GET_CHAOPARAM(a1)->Happiness <= -100)
	{
		AL_SetBehavior(a1, ALBHV_Drown);
		return 0;
	}
	return ((BHV_FUNC)0x561E00)(a1);
}

void ChaoDrown::Init() {
	WriteData((int*)(0x0056239A - 4), (int)ALBHV_StruggleSwim_DrownCheck);
}