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

static NJS_CNK_OBJECT* object_sandpit[] = {
	&object_8D65212E6E079DB20DD,
	& object_8D6521312A3FB5E8953
};

static void ALO_SandHoleDisplayer(task *tp) {
	const float scale_sandpit[] = { 0.25f, 0.75f };

	DoLighting(LightIndex);

	njPushMatrixEx();
	njTranslateEx(&tp->twp->pos);
	
	float scl = scale_sandpit[tp->twp->btimer];
	njScale(NULL, scl, scl, scl);

	njSetTexture(&AL_SANDHOLE_TEXLIST);
	chCnkDrawObject(object_sandpit[tp->twp->btimer]);

	njPopMatrixEx();
	DoLighting(LightIndexBackupMaybe);
}

task* ALO_SandHoleCreate(NJS_POINT3* pPos) {
	task* loaded = CreateElementalTask(IM_TWK, LEV_4, (task_exec)nullsub_1, "SandHole");
	loaded->disp = ALO_SandHoleDisplayer;
	loaded->twp->pos = *pPos;
	return loaded;
}