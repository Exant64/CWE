#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "../AL_ModAPI.h"
#include "../ALifeSDK_Functions.h"
#include "../Chao.h"
#include "../ChaoMain.h"
#include "../al_sandhole.h"
#include "../data/toy/alo_pail.nja"

signed int __cdecl ALBHV_SandHole(task* a1)
{
	switch (GET_CHAOWK(a1)->Behavior.Mode)
	{
	case 0:
		Chao_RegAnimation(a1, "alm_sunaasubi");

		*(int*)0x12F5954 = (int)0x012FDE44;
		DrawToyTransform(a1, 0x12F5954, 11);

		AL_SetItem(a1, 0, &alo_bucket, texlist_cwe_object);
		
		GET_CHAOWK(a1)->Behavior.Mode++;
		GET_CHAOWK(a1)->Behavior.Timer = (int)(180 + (njRandom() * 121.f));
		break;
	case 1:
		GET_CHAOWK(a1)->Behavior.Timer--;
		if (GET_CHAOWK(a1)->Behavior.Timer <= 0)
		{
			GET_CHAOWK(a1)->Behavior.Mode++;
			GET_CHAOWK(a1)->Behavior.Timer = (int)(180 + (njRandom() * 121.0));
			NJS_VECTOR a3;
			a3.x = njSin(a1->twp->ang.y) * 3.0f + a1->twp->pos.x;
			a3.y = a1->twp->pos.y;
			a3.z = njCos(a1->twp->ang.y) * 3.0f + a1->twp->pos.z;
			GET_CHAOWK(a1)->task_ptr1 = SandHole_Load(&a3);
		}
		break;
	case 2:
		GET_CHAOWK(a1)->Behavior.Timer--;
		if (GET_CHAOWK(a1)->Behavior.Timer <= 0)
		{
			GET_CHAOWK(a1)->Behavior.Mode++;
			GET_CHAOWK(a1)->Behavior.Timer = (int)(180 + (njRandom() * 121.f));
			GET_CHAOWK(a1)->task_ptr1->twp->btimer++;
		}
		break;
	case 3:
		GET_CHAOWK(a1)->Behavior.Timer--;
		if (GET_CHAOWK(a1)->Behavior.Timer <= 0)
		{
			GET_CHAOWK(a1)->task_ptr1 = 0;
			return BHV_RET_FINISH;
		}
		break;
	}
	GET_CHAOWK(a1)->Behavior.SubTimer++;
	if ((GET_CHAOWK(a1)->Behavior.SubTimer % 180) == 0 && njRandom() < 0.5f && CurrentChaoArea == NextChaoArea)
	{
		PlaySound_XYZ((24576 + 164), &a1->twp->pos, 0, 0, 56);
	}
	return BHV_RET_CONTINUE;
}