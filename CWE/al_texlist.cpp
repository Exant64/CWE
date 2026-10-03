#include "stdafx.h"
#include "AL_ModAPI.h"
#include "api/api_main.h"
#include "ChaoMain.h"

NJS_TEXNAME AL_DX_PARTS_TEX_TEXNAME[129];
NJS_TEXLIST AL_DX_PARTS_TEX_TEXLIST = { arrayptrandlength(AL_DX_PARTS_TEX_TEXNAME) };

NJS_TEXNAME AL_ITEM_TEXNAME[NB_CWE_CATEGORY];
NJS_TEXLIST AL_ITEM_TEXLIST = { AL_ITEM_TEXNAME, NB_CWE_CATEGORY };

NJS_TEXLIST* texlist_cwe_object = NULL;

NJS_TEXNAME AL_DRAWING_TEXNAME[21];
NJS_TEXLIST AL_DRAWING_TEXLIST = { AL_DRAWING_TEXNAME, 21 };

NJS_TEXNAME OMO_EYE_TEXNAME[11];
NJS_TEXLIST OMO_EYE_TEXLIST = { OMO_EYE_TEXNAME, 11 };

NJS_TEXNAME NAME_ODE_TEXNAME[2];
NJS_TEXLIST NAME_ODE_TEXLIST = { NAME_ODE_TEXNAME, 2 };

NJS_TEXNAME AL_ODE_GUEST_TEXNAME[3];
NJS_TEXLIST AL_ODE_GUEST_TEXLIST = { AL_ODE_GUEST_TEXNAME, 3 };

NJS_TEXNAME XL_BODY_TEXNAME[143];
NJS_TEXLIST XL_BODY_TEXLIST = { XL_BODY_TEXNAME, 143 };

NJS_TEXNAME texname_animal_inv[2];
NJS_TEXLIST texlist_animal_inv = { texname_animal_inv, _countof(texname_animal_inv) };

NJS_TEXLIST* texlist_birthday_hat;
NJS_TEXLIST* texlist_birthday_cake;
NJS_TEXLIST* texlist_cwe_name;
NJS_TEXLIST* texlist_cwe_sandcastle;
NJS_TEXLIST* texlist_cwe_ui_common;

#define LENSTEX(name) NJS_TEXNAME name## _TEXNAME[7]; \
					  NJS_TEXLIST name## _TEXLIST = { name## _TEXNAME, 7 };

#define NEWLENSTEX(name) NJS_TEXNAME name## _TEXNAME[8]; \
					  NJS_TEXLIST name## _TEXLIST = { name## _TEXNAME, 8 };

LENSTEX(CWE_LENS_GREEN)
LENSTEX(CWE_LENS_MAGENTA)
LENSTEX(CWE_LENS_PURPLE)
LENSTEX(CWE_LENS_RED)
LENSTEX(CWE_LENS_YELLOW)
LENSTEX(CWE_LENS_SPARTOI)
LENSTEX(CWE_LENS_SNAKE)
LENSTEX(CWE_LENS_ROBOT)
NEWLENSTEX(CWE_LENS_AQUA)
NEWLENSTEX(CWE_LENS_JEWEL_BLUE)
NEWLENSTEX(CWE_LENS_JEWEL_YELLOW)
NEWLENSTEX(CWE_LENS_JEWEL_GREEN)
NEWLENSTEX(CWE_LENS_JEWEL_PURPLE)
NEWLENSTEX(CWE_LENS_JEWEL_RED)
NEWLENSTEX(CWE_LENS_JEWEL_SILVER)

void CWE_RegisterTexlists(const CWE_REGAPI* cwe_api) {
	const auto pApiTexture = CWE_API_Main.pRegister->pTexture;

	if (gConfigVal.Birthday) {
		texlist_birthday_hat = pApiTexture->AddAutoTextureLoad("birthdayhat");
		texlist_birthday_cake = pApiTexture->AddAutoTextureLoad("CWE_BIRTHDAY_CAKE");
	}

	texlist_cwe_object = pApiTexture->AddAutoTextureLoad("CWE_OBJECT");
	texlist_cwe_ui_common = pApiTexture->AddAutoTextureLoad("CWE_UI_COMMON");

	if (gConfigVal.StageAnimals) {
		pApiTexture->AddChaoTexlistLoad("CWE_ANIMAL_INV", &texlist_animal_inv);
	}

	if (!gConfigVal.OldName) {
		texlist_cwe_name = pApiTexture->AddAutoTextureLoad("CWE_NAME");
	}

	if (gConfigVal.BhvSandCastle) { 
		texlist_cwe_sandcastle = pApiTexture->AddAutoTextureLoad("CWE_SANDCASTLE");
	}

	pApiTexture->AddChaoTexlistLoad("AL_ITEM", &AL_ITEM_TEXLIST);
	pApiTexture->AddChaoTexlistLoad("NAME_ODE", &NAME_ODE_TEXLIST);
	pApiTexture->AddChaoTexlistLoad("al_ode_guest", &AL_ODE_GUEST_TEXLIST);	
	pApiTexture->AddChaoTexlistLoad("AL_DRAWING", &AL_DRAWING_TEXLIST);
	pApiTexture->AddChaoTexlistLoad("OMO_EYE", &OMO_EYE_TEXLIST);
	pApiTexture->AddChaoTexlistLoad("XL_BODY", &XL_BODY_TEXLIST);
	pApiTexture->AddChaoTexlistLoad("AL_DX_PARTS_TEX", &AL_DX_PARTS_TEX_TEXLIST);
}