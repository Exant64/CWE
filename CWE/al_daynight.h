#pragma once
enum
{
	AL_WEATHER_NONE = 0,
	AL_WEATHER_RAIN = 1
};
enum
{
	AL_TIME_DAY = 0,
	AL_TIME_EVENING = 1,
	AL_TIME_NIGHT = 2
};
enum
{
	AL_SEASON_SPRING = 0,
	AL_SEASON_SUMMER = 1,
	AL_SEASON_FALL = 2,
	AL_SEASON_WINTER = 3
};

struct DAYNIGHT_SAVE {
	uint32_t day;				// starting from index 0, the days that have passed (potential mechanic in the future)
	float time;					// we store the 0-24 hour cycle as a float so that it can be rescaled if its somehow reconfigured
	uint32_t currentPhase;		// current phase
	bool nextDayCloudy;
};

// bit hacky, but crunch moment
extern uint32_t gDayNightCheatPhase;

// save related functions
uint32_t& AL_DayNightCycle_GetSaveCurrentDay();
float& AL_DayNightCycle_GetSaveTime();
uint32_t& AL_DayNightCycle_GetSaveCurrentPhase();
bool& AL_DayNightCycle_GetSaveNextDayCloudy();

// shiny texture functions
void AL_DayNightCycle_PreDrawSetupShinyTexture();
void AL_DayNightCycle_SetLerpShinyTexture();
void AL_DayNightCycle_PostDrawSetupShinyTexture();

// creating the task
void AL_CreateDayNightCycle();

// initializing the feature (todo move to renderfix file?)
void AL_DayNight_Init(const HelperFunctions& helper);