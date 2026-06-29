#include "stdafx.h"
#include "../cwe_code.h"
#include <al_behavior/al_intention.h>
#include "include/code_hdtoy.h"
#include <ChaoMain.h>

#pragma warning(push)
#pragma warning( disable: 4838 )

#include <data/hdtoy/alo_hd_tv.nja>
#include <data/hdtoy/alo_hd_radicase.nja>
#include <data/hdtoy/alo_hd_horse.nja>
#include <data/hdtoy/alo_hd_jokebox.nja>
#include <data/hdtoy/alo_hd_rappa.nja>
#include <data/hdtoy/alo_hd_drum.nja>
#include <data/hdtoy/alo_hd_bubble.nja>

#pragma warning(pop)

void HDToyModels::Init() {
	*((NJS_OBJECT*)0x121D804) = object_cha_toy_jokebox;
	*((NJS_CNK_MODEL*)0x121D264) = model_cha_toy_jokebox_head;

	*((NJS_OBJECT*)0x121AA0C) = cha_toy_horse;

	*((NJS_OBJECT*)0x12FE33C) = cha_toy_bubble_Mesh_0000_1;

	*((NJS_OBJECT*)0x11C62C4) = toy_drum;
	*((NJS_OBJECT*)0x11C6674) = drumstick_l;
	*((NJS_OBJECT*)0x11C6A14) = drumstick_r;
	
	*((NJS_OBJECT*)0x11C830C) = cha_toy_trumpet;

	*((NJS_OBJECT*)0x1301D74) = *tvOld.child;
	tvOld.child = (NJS_OBJECT*)0x1301D74;
	*((NJS_OBJECT*)0x1302C1C) = tvOld;
	*((NJS_OBJECT*)0x125BF74) = radio;

	g_HelperFunctions->ReplaceTexture(
		"AL_TOY",
		"tv01",
		g_HelperFunctions->GetReplaceablePath(".\\resource\\gd_PC\\hd_toys_atlas.png"),
		2348581,
		0,
		0
	);

	g_HelperFunctions->ReplaceTexture(
		"AL_TOY",
		"tv03",
		g_HelperFunctions->GetReplaceablePath(".\\resource\\gd_PC\\hd_toys_atlas2.png"),
		2348582,
		0,
		0
	);

	g_HelperFunctions->ReplaceTexture(
		"AL_TOY",
		"tv04",
		g_HelperFunctions->GetReplaceablePath(".\\resource\\gd_PC\\c_silver_k00.png"),
		2348583,
		0,
		0
	);

	g_HelperFunctions->ReplaceTexture(
		"AL_TOY",
		"joke01",
		g_HelperFunctions->GetReplaceablePath(".\\resource\\gd_PC\\c_env_k01.png"),
		2348584,
		0,
		0
	);
}