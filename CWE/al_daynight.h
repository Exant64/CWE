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
	uint32_t phaseCount;		// keeping track of the phases passed to know when to increment day (this way we can keep the rest of the phase logic separated)
	float timeBetweenPhase;		// we store the 0-1 phase not the actual frame timer so if somebody changes the time between phases
								// we can calculate the new frame timer easily
	uint32_t currentPhase;		// current phase
	bool nextDayCloudy;
};

// save related functions
uint32_t& AL_DayNightCycle_GetSaveCurrentDay();
uint32_t& AL_DayNightCycle_GetSavePhaseCount();
float& AL_DayNightCycle_GetSaveTimeBetweenPhase();
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