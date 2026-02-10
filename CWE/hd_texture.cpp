#include "stdafx.h"
#include <IniFile.h>
#include <ChaoMain.h>

void HDTexture_Init(const char* const path , const IniFile* const pConfig) {
	const std::string pathStr{ path + std::string("\\gd_PC\\HDTexture\\")};

	if (pConfig->getBool("HDTex", "ChaoTex", false)) {
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_body.prs", (pathStr + "al_body.prs").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_jewel.prs", (pathStr + "al_jewel.prs").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\xl_body.prs", (pathStr + "xl_body.prs").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\ecw_al_body.prs", (pathStr + "ecw_al_body.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\ecw_al_jewels.prs", (pathStr + "ecw_al_jewels.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\ecw_al_masks.prs", (pathStr + "ecw_al_masks.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\ecw_al_rares.prs", (pathStr + "ecw_al_rares.pak").c_str(), CWE_ModIndex);
	}

	if (pConfig->getBool("HDTex", "ObjectTex", false)) {
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_object.prs", (pathStr + "al_object.pak").c_str(), CWE_ModIndex);
	}

	if (pConfig->getBool("HDTex", "ToyTex", false)) {
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_toy.prs", (pathStr + "al_toy.pak").c_str(), CWE_ModIndex);
	}

	if (pConfig->getBool("HDTex", "FaceTex", false)) {
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_eye.prs", (pathStr + "al_eye.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_mouth.prs", (pathStr + "al_mouth.pak").c_str(), CWE_ModIndex);
	}

	if (pConfig->getBool("HDTex", "IconTex", false)) {
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\al_icon.prs", (pathStr + "al_icon.pak").c_str(), CWE_ModIndex);
	}

	if (pConfig->getBool("HDTex", "LensTex", false)) {
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\cwe_eye_green.prs", (pathStr + "cwe_eye_green.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\cwe_eye_magenta.prs", (pathStr + "cwe_eye_magenta.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\cwe_eye_purple.prs", (pathStr + "cwe_eye_purple.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\cwe_eye_red.prs", (pathStr + "cwe_eye_red.pak").c_str(), CWE_ModIndex);
		g_HelperFunctions->ReplaceFileAtIndex("resource\\gd_PC\\cwe_eye_yellow.prs", (pathStr + "cwe_eye_yellow.pak").c_str(), CWE_ModIndex);
	}
}