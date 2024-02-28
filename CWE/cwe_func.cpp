#include "stdafx.h"
#include "cwe_func.h"
#include "alo_boat.h"
#include "al_piano.h"
#include "al_omochao_build.h"
#include "al_chao_counter.h"
#include "ChaoMain.h"
#include "al_save.h"

extern "C" __declspec(dllexport) CWE_FUNCS cwe_funcs = {
	.Version = 0,
	.SaveCWESaveFiles = SaveCWESaveFiles,
	.ReadCWESaveFiles = ReadCWESaveFiles,
	.ALO_BoatCreate = ALO_BoatCreate,
	.ALO_FloatCreate = ALO_FloatCreate,
	.ALO_PianoCreate = ALO_PianoCreate,
	.ALO_OmoBuildCreate = ALO_OmoBuildCreate,
	.AL_ChaoCounterCreate = AL_ChaoCounterCreate,
	.AL_NameDisplayCreate = AL_NameDisplayCreate
};