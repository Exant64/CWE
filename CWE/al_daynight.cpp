#include "stdafx.h"
#include <array>
#include "ChaoMain.h"
#include "al_daynight.h"
#include "al_save.h"
#include <stdexcept>
#include "al_stage.h"

//struct per level
struct AL_DayNightConfig {
	const char* const DefaultObjTex;

	const char* const DayTex;
	const char* const RainTex;
	const char* const EveTex;
	const char* const NightTex;

	const char* const DefaultLight;
	const char* const CloudLight;
	const char* const EveLight;
	const char* const NightLight;
};

const std::unordered_map<int, AL_DayNightConfig> gDayNightConfig = { {
	{
		CHAO_STG_NEUT,
		{
			"AL_NEUT_OBJ_TEX",

			"al_stg_neut_tex",
			"al_neut_cld_tex",
			"al_neut_eve_tex",
			"al_neut_ngt_tex",

			"stg00_light.bin",
			"stg00_cld_light.bin",
			"stg00_eve_light.bin",
			"stg00_ngt_light.bin"
		}
	},
	{
		CHAO_STG_HERO,
		{
			"AL_HERO_OBJ_TEX",

			"al_stg_hero_tex",
			"al_hero_cld_tex",
			"al_hero_eve_tex",
			"al_hero_ngt_tex",

			"stg01_light.bin",
			"stg01_cld_light.bin",
			"stg01_eve_light.bin",
			"stg01_ngt_light.bin"
		}
	},
	{
		CHAO_STG_DARK,
		{
			"AL_DARK_OBJ_TEX",

			"al_stg_dark_tex",
			"al_dark_bld_tex",
			NULL,
			NULL,

			"stg02_light.bin",
			"stg02_cld_light.bin",
			NULL,
			NULL
		}
	},
	{
		CHAO_STG_RACE,
		{
			NULL,

			NULL,
			NULL,
			"AL_RACE_eve_N_TEX",
			"AL_RACE_ngt_N_TEX",

			NULL,
			NULL,
			NULL,
			NULL
		}
	},
	{
		CHAO_STG_ENTRANCE,
		{
			NULL,

			NULL,
			NULL,
			"AL_ENTRANCE_EVE",
			"AL_ENTRANCE_NGT",

			NULL,
			NULL,
			NULL,
			NULL
		}
	},
	{
		9999,
		{
			NULL,

			NULL,
			NULL,
			"al_karate_eve_tex",
			"al_karate_ngt_tex",

			NULL,
			NULL,
			NULL,
			NULL
		}
	}
} };

void AL_DayNightTexLoad(int stage)
{
	if (!gDayNightConfig.contains(stage)) return;
	
	const AL_DayNightConfig& config = gDayNightConfig.at(stage);

	switch (GetTimeOfDay()) {
	case AL_TIME_DAY:
		//if has all three important components, if not then its not worth loading anyways
		if (config.DayTex && config.DefaultObjTex && config.RainTex) {
			if (GetWeather() == AL_WEATHER_RAIN) {
				LoadTextureList((char*)config.RainTex, &timeofdayTexlistReplacer);
			}
			else {
				LoadTextureList((char*)config.DayTex, &timeofdayTexlistReplacer);
				LoadTextureList((char*)config.DefaultObjTex, (NJS_TEXLIST*)0x1366B04);
			}
		}
		break;
	case AL_TIME_EVENING:
		if (config.EveTex) {
			LoadTextureList((char*)config.EveTex, &timeofdayTexlistReplacer);
		}
		break;
	case AL_TIME_NIGHT:
		if (config.NightTex) {
			LoadTextureList((char*)config.NightTex, &timeofdayTexlistReplacer);
		}
		break;
	}
}

void AL_DayNightLightLoad(int stage)
{
	if (!gDayNightConfig.contains(stage)) return;

	const AL_DayNightConfig& config = gDayNightConfig.at(stage);

	//if theres no default light, theres no special light assigned to this garden
	if (!config.DefaultLight) return;

	LoadStageLight(config.DefaultLight);

	switch (GetTimeOfDay()) {
	case AL_TIME_DAY:
		if (config.CloudLight && GetWeather() == AL_WEATHER_RAIN) {
			LoadStageLight(config.CloudLight);
		}
		break;
	case AL_TIME_EVENING:
		if (config.EveLight) {
			LoadStageLight(config.EveLight);
		}
		break;
	case AL_TIME_NIGHT:
		if (config.NightLight) {
			LoadStageLight(config.NightLight);
		}
		break;
	}
}

const int sub_478790Ptr = 0x478790;
signed int sub_478790(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3, unsigned __int8 a4)
{
	int re;
	__asm
	{
		mov bl, a1
		push dword ptr[a4]
		push dword ptr[a3]
		push dword ptr[a2]
		call sub_478790Ptr
		mov re, eax
		add esp, 12
	}
	return re;
}

void DayNightTransition(ObjectMaster* a1)
{
	switch (a1->Data1.Entity->Action)
	{
	case 0: //init
		//something something store the previous time here
		a1->Data1.Entity->Action++;
		break;
	case 1: //fade to black, second argument is alpha (weird argument order because usercall) (guess is order is BARG)
		if (sub_478790(0, 255, 0, 0))
		{
			a1->Data1.Entity->Action++;
		}
		break;
	case 2: //counter, you would render the display on this mode
		a1->Data1.Entity->Rotation.x++;
		if (a1->Data1.Entity->Rotation.x > 60 * 2)
		{
			SetTimeOfDay( a1->Data1.Entity->Rotation.y);
			SetWeather( a1->Data1.Entity->Rotation.z); 

			AL_DayNightTexLoad(AL_GetStageNumber() - 1);
			AL_DayNightLightLoad(AL_GetStageNumber() - 1);

			a1->Data1.Entity->Action++;
		}
		break;
	case 3: //fade back
		if (sub_478790(0, 0, 0, 0))
		{
			a1->Data1.Entity->Action++;
		}
		break;
	case 4: //kill
		a1->MainSub = DeleteObject_;
		break;
	}
}

void DayNightTransition_Create(int time, int weather)
{
	if (gConfigVal.NoDayNightTransition)
	{
		SetTimeOfDay(time);
		SetWeather(weather);	
		AL_DayNightTexLoad(AL_GetStageNumber());
		AL_DayNightLightLoad(AL_GetStageNumber());
	}
	else 
	{
		ObjectMaster* ob = LoadObject(4, "Daynight", DayNightTransition, LoadObj_Data1);
		ob->Data1.Entity->Rotation.y = time;
		ob->Data1.Entity->Rotation.z = weather;
	}
}

const int sub_486E50 = 0x486E50;
void sub_486E50Thing()
{
	__asm
	{
		mov ecx, 0
		mov eax, 0
		call sub_486E50
	}
}
void LightingNeut()
{
	AL_DayNightLightLoad(0);
}
void LightingHero()
{
	sub_486E50Thing();
	AL_DayNightLightLoad(1);
}
void LightingDark()
{
	sub_486E50Thing();
	AL_DayNightLightLoad(2);
}

void AL_NEUT_OBJ_TEX_HOOK()
{
	LoadTextureList((char*)"AL_NEUT_OBJ_TEX", (NJS_TEXLIST*)0x1366B04);
	AL_DayNightTexLoad(AL_GetStageNumber());
}
void AL_HERO_OBJ_TEX_HOOK()
{
	LoadTextureList((char*)"AL_HERO_OBJ_TEX", (NJS_TEXLIST*)0x1366B0C);
	AL_DayNightTexLoad(AL_GetStageNumber());
}
void AL_DARK_OBJ_TEX_HOOK()
{
	LoadTextureList((char*)"AL_DARK_OBJ_TEX", (NJS_TEXLIST*)0x1366B14);
	AL_DayNightTexLoad(AL_GetStageNumber());
}

void AL_DayNight_Init() {
	

	if (gConfigVal.DayNightCycle)
	{
		WriteCall((void*)0x54C840, AL_NEUT_OBJ_TEX_HOOK);
		WriteCall((void*)0x54D275, AL_HERO_OBJ_TEX_HOOK);
		WriteCall((void*)0x54B795, AL_DARK_OBJ_TEX_HOOK);

		WriteCall((void*)0x0054C92D, LightingNeut);
		WriteCall((void*)0x0054D307, LightingHero);
		WriteCall((void*)0x0054B827, LightingDark);
		//WriteCall((void*)0x52D453, AL_RACE_OBJ_TEX_N_HOOK);
		//WriteCall((void*)0x54DB36, AL_ENTRANCE_OBJ_HOOK);
		//WriteCall((void*)0x52D9FD, AL_KARATE_OBJ_TEX_HOOK);
	}
	else
	{
		SetTimeOfDay(AL_TIME_DAY);
	}

}