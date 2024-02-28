#pragma once
#include <ninja.h>

//for ALO_PianoCreate
enum {
	CWE_PIANOTYPE_PIANO = 0,
	CWE_PIANOTYPE_ORGAN = 1
};

struct CWE_FUNCS {
	int Version;

	void (*SaveCWESaveFiles)();
	void (*ReadCWESaveFiles)();
	
	void (*ALO_BoatCreate)(NJS_POINT3* pPos, Angle ang);
	void (*ALO_FloatCreate)(NJS_POINT3* pPos, Angle ang);
	void(*ALO_PianoCreate)(int index, NJS_POINT3* pPos, Angle ang);

	void (*ALO_OmoBuildCreate)(NJS_POINT3* pPos, Angle ang);

	void(*AL_ChaoCounterCreate)();
	void(*AL_NameDisplayCreate)();
};