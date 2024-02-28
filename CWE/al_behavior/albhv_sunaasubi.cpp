#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "../AL_ModAPI.h"
#include "../ALifeSDK_Functions.h"
#include "../Chao.h"
#include "../ChaoMain.h"
#include "../al_sandhole.h"
#include "../data/toy/alo_pail.nja"

signed int __cdecl ALBHV_SandHole(ObjectMaster* a1)
{
	switch (a1->Data1.Chao->Behavior.Mode)
	{
	case 0:
		Chao_RegAnimation(a1, "alm_sunaasubi");

		*(int*)0x12F5954 = (int)0x012FDE44;
		DrawToyTransform(a1, 0x12F5954, 11);

		AL_SetItem(a1, 0, &alo_bucket, &CWE_OBJECT_TEXLIST);
		
		a1->Data1.Chao->Behavior.Mode++;
		a1->Data1.Chao->Behavior.Timer = (int)(180 + (njRandom() * 121.f));
		break;
	case 1:
		a1->Data1.Chao->Behavior.Timer--;
		if (a1->Data1.Chao->Behavior.Timer <= 0)
		{
			a1->Data1.Chao->Behavior.Mode++;
			a1->Data1.Chao->Behavior.Timer = (int)(180 + (njRandom() * 121.0));
			NJS_VECTOR a3;
			a3.x = njSin(a1->Data1.Entity->Rotation.y) * 3.0f + a1->Data1.Entity->Position.x;
			a3.y = a1->Data1.Entity->Position.y;
			a3.z = njCos(a1->Data1.Entity->Rotation.y) * 3.0f + a1->Data1.Entity->Position.z;
			a1->Data1.Chao->ObjectMaster_ptr1 = SandHole_Load(&a3);
		}
		break;
	case 2:
		a1->Data1.Chao->Behavior.Timer--;
		if (a1->Data1.Chao->Behavior.Timer <= 0)
		{
			a1->Data1.Chao->Behavior.Mode++;
			a1->Data1.Chao->Behavior.Timer = (int)(180 + (njRandom() * 121.f));
			a1->Data1.Chao->ObjectMaster_ptr1->Data1.Entity->Index++;
		}
		break;
	case 3:
		a1->Data1.Chao->Behavior.Timer--;
		if (a1->Data1.Chao->Behavior.Timer <= 0)
		{
			a1->Data1.Chao->ObjectMaster_ptr1 = 0;
			return BHV_RET_FINISH;
		}
		break;
	}
	a1->Data1.Chao->Behavior.SubTimer++;
	if ((a1->Data1.Chao->Behavior.SubTimer % 180) == 0 && njRandom() < 0.5f && CurrentChaoArea == NextChaoArea)
	{
		PlaySound_XYZ((24576 + 164), &a1->Data1.Entity->Position, 0, 0, 56);
	}
	return BHV_RET_CONTINUE;
}