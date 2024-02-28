#include "stdafx.h"
#include "al_sandhole.h"
#include "ChaoMain.h"
#include "data/toy/NeutralSandCastleModel.h"
#include "data/toy/SandHoleModel.h"

void DoLighting(int a1)
{
	__asm
	{
		mov eax, a1
		call DoLightingPtr
	}
}

NJS_OBJECT* object_sandpit[] = {
	&object_8D65212E6E079DB20DD,
	& object_8D6521312A3FB5E8953
};

float scale_sandpit[] = { 0.25f, 0.75f };

void SandHole_Display(ObjectMaster *a1)
{
	DoLighting(LightIndex);

	njPushMatrixEx();
	njTranslateEx(&a1->Data1.Entity->Position);
	
	float scl = scale_sandpit[a1->Data1.Entity->Index];
	njScale(NULL, scl, scl, scl);

	njSetTexture(&AL_SANDHOLE_TEXLIST);
	chCnkDrawObject(object_sandpit[a1->Data1.Entity->Index]);

	njPopMatrixEx();
	DoLighting(LightIndexBackupMaybe);
}

ObjectMaster* __cdecl SandHole_Load(NJS_VECTOR * pos)
{
	ObjectMaster* loaded = LoadObject(4, "SandHole", (ObjectFuncPtr)nullsub_1, LoadObj_Data1);
	loaded->DisplaySub = SandHole_Display;
	loaded->Data1.Entity->Position = *pos;
	return loaded;
}