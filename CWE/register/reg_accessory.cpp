#include "stdafx.h"
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <data/accessory/ala_all.h>

void ALAPI_RegisterAccessory(CWE_REGAPI_LEGACY* cwe_api) {
	return;

	BlackMarketItemAttributes accessoryAttrib = { 2500,500,0,-1,-1,0 };
	cwe_api->RegisterChaoAccessory(EAccessoryType::Head, &object_ala_baseball, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Baseball Cap", "");
	cwe_api->RegisterChaoAccessory(EAccessoryType::Head, &object_ala_bowtie_red, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Bowtie", "");
	cwe_api->RegisterChaoAccessory(EAccessoryType::Face, &object_ala_glasses_round, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Round Glasses", "");
	cwe_api->RegisterChaoAccessory(EAccessoryType::Face, &object_ala_glasses_aviators, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Aviators", "Glasses for high flyers.");
	cwe_api->RegisterChaoAccessory(EAccessoryType::Head, &headphone_cups, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Headphones", "Music makes you lose control.");
	cwe_api->RegisterChaoAccessory(EAccessoryType::Face, &object_00245C64, &AL_BODY, &accessoryAttrib, "Pacifier", "");
	cwe_api->RegisterChaoAccessory(EAccessoryType::Head, &object_ala_presshat, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Press Hat", "For writing news and playing pinball.");
	NJS_OBJECT* pHoodie = &object_ala_hoodie_low;
	NJS_OBJECT* pHoodie_pink = &object_ala_hoodie_pink_low;
	if (gConfigVal.HDHoodie)
	{
		pHoodie = &object_ala_hoodie;
		pHoodie_pink = &object_ala_hoodie_pink;
	}

	int hoodie = cwe_api->RegisterChaoAccessory(EAccessoryType::Generic1, pHoodie, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Blue Hoodie", "For smaller sized chao in cold climates.");
	cwe_api->AccessoryMakeBald(hoodie);
	hoodie = cwe_api->RegisterChaoAccessory(EAccessoryType::Generic1, pHoodie_pink, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Pink Hoodie", "For smaller sized chao in cold climates.");
	cwe_api->AccessoryMakeBald(hoodie);

	cwe_api->RegisterChaoAccessory(EAccessoryType::Generic2, &object_ala_neck_bowtie, &CWE_OBJECT_TEXLIST, &accessoryAttrib, "Cheese Bowtie", "");

}