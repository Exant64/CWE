#include "stdafx.h"
#include "al_sandhole.h"
#include "ChaoMain.h"
#include "data/toy/NeutralSandCastleModel.h"
#include "data/toy/SandHoleModel.h"

__declspec(naked) void DoLighting(int a1)
{
	__asm
	{
		mov eax, [esp+4]
		call DoLightingPtr
		ret
	}
}

NJS_OBJECT* object_sandpit[] = {
	&object_8D65212E6E079DB20DD,
	& object_8D6521312A3FB5E8953
};

float scale_sandpit[] = { 0.25f, 0.75f };

void SandHole_Display(task *a1)
{
	DoLighting(LightIndex);

	njPushMatrixEx();
	njTranslateEx(&a1->twp->pos);
	
	float scl = scale_sandpit[a1->twp->btimer];
	njScale(NULL, scl, scl, scl);

	njSetTexture(&AL_SANDHOLE_TEXLIST);
	chCnkDrawObject(object_sandpit[a1->twp->btimer]);

	njPopMatrixEx();
	DoLighting(LightIndexBackupMaybe);
}

task* __cdecl SandHole_Load(NJS_VECTOR * pos)
{
	task* loaded = CreateElementalTask(LoadObj_Data1, 4, (task_exec)nullsub_1, "SandHole");
	loaded->disp = SandHole_Display;
	loaded->twp->pos = *pos;
	return loaded;
}