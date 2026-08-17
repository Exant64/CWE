#pragma once
#include <SA2ModLoader.h>

//for ALO_PianoCreate
enum {
	CWE_PIANOTYPE_PIANO = 0,
	CWE_PIANOTYPE_ORGAN = 1
};

struct CWE_FUNCS {
	int Version;

	void (*SaveCWESaveFiles)();
	void (*ReadCWESaveFiles)();
	
	task* (*ALO_BoatCreate)(NJS_POINT3* pPos, Angle ang);
	task* (*ALO_FloatCreate)(NJS_POINT3* pPos, Angle ang);
	task* (*ALO_PianoCreate)(int index, NJS_POINT3* pPos, Angle ang);

	task* (*ALO_OmoBuildCreate)(NJS_POINT3* pPos, Angle ang);

	task* (*AL_ChaoCounterCreate)();
	task* (*AL_NameDisplayCreate)();
};