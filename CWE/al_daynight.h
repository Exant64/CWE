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
void AL_DayNightTexLoad(int stage);
void DayNightTransition_Create(int time, int weather);
void AL_DayNightLightLoad(int stage);

void AL_DayNightCycle_PreDrawSetupShinyTexture();
void AL_DayNightCycle_SetLerpShinyTexture();
void AL_DayNightCycle_PostDrawSetupShinyTexture();

void AL_DayNight_Init(const HelperFunctions& helper);
void AL_CreateDayNightCycle();