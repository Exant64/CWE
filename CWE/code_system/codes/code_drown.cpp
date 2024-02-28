#include "stdafx.h"
#include "include/code_drown.h"
#include <Chao.h>
#include <random>

int __cdecl ALBHV_Drown(ObjectMaster* a1)
{
	if (!a1 || !a1->Data1.Chao || !a1->EntityData2)
		return 0;
	if (a1->Data1.Entity->Position.y <= a1->EntityData2->field_DC - 2.1f)
	{
		a1->Data1.Entity->Position.y += 0.1f;
		if (a1->Data1.Entity->Position.y > a1->EntityData2->field_DC - 2.1f)
			a1->Data1.Entity->Position.y = a1->EntityData2->field_DC - 2.1f;
		a1->EntityData2->speed.y = -a1->EntityData2->gravity - 0.1f * a1->EntityData2->velocity.y;
	}
	int modePre = a1->Data1.Chao->Behavior.Mode;
	int returnVal = ((BHV_FUNC)0x00568F60)(a1);
	if (modePre == 0 && a1->Data1.Chao->Behavior.Mode == 1)
	{
		AL_SetMotionLink(a1, 151);
	}
	return returnVal;
}

int __cdecl ALBHV_StruggleSwim_DrownCheck(ObjectMaster* a1)
{
	if (a1->Data1.Chao->Behavior.SubTimer % (3 * 60) == 0) //every 10 seconds
		a1->Data1.Chao->pParamGC->Happiness--;

	if (a1->Data1.Chao->pParamGC->Happiness <= -100)
	{
		AL_SetBehavior(a1, ALBHV_Drown);
		return 0;
	}
	return ((BHV_FUNC)0x561E00)(a1);
}

void ChaoDrown::Init() {
	WriteData((int*)(0x0056239A - 4), (int)ALBHV_StruggleSwim_DrownCheck);
}