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

// unfinished api stuff temporarily moved here

enum {
	PHASE_DAY = 0,
	PHASE_EVE = 1,
	PHASE_NGT = 2,
	PHASE_CLD = 3,
	NB_PHASE
};

struct DAYNIGHT_TIME_INFO {
	uint32_t timer;
	uint32_t phase;
	uint32_t day;
	bool nextDayCloudy;

	uint32_t(*GetHourFrameCount)();
	uint32_t(*GetDayFrameCount)(); // = 24 * GetHourFrameCount
	uint32_t(*GetStartHourForPhase)(uint32_t phase);
	uint32_t(*GetTimerRelativeToPhase)(uint32_t timer, uint32_t phase);
	uint32_t(*GetFramesBetweenPhases)(uint32_t phaseA, uint32_t phaseB);
};

struct DAYNIGHT_TIME_WORK {
	uint32_t phase;
	bool nextDayCloudy;
};

struct DAYNIGHT_RENDER_WORK {
	uint32_t phaseA;
	uint32_t phaseB;
	float lerpValue;
};

typedef void(*DAYNIGHT_TIME_MANAGER_FUNC)(const char* pGardenID, void(*pFunc)(const DAYNIGHT_TIME_INFO* pInfo, DAYNIGHT_TIME_WORK* pWork));
typedef void(*DAYNIGHT_RENDER_MANAGER_FUNC)(const char* pGardenID, void(*pFunc)(const DAYNIGHT_TIME_INFO* pInfo, DAYNIGHT_RENDER_WORK* pWork));

struct CWE_API_DAYNIGHT_REGISTER {
	Uint32 Version;

	// Version >= 1
	void(*RegisterECWSkybox)(const char* pGardenID, NJS_OBJECT* pObj);
	void(*RegisterTimeManager)(const char* pGardenID, DAYNIGHT_TIME_MANAGER_FUNC pFunc);
	void(*RegisterRenderManager)(const char* pGardenID, DAYNIGHT_RENDER_MANAGER_FUNC pFunc);
};

struct CWE_API_DAYNIGHT_INFO {
	Uint32 Version;

	// Version >= 1
	uint32_t(*GetCurrentDay)(); // the current day
	uint32_t(*GetCurrentPhase)(); // the current phase
	uint32_t(*GetPhaseCount)(); // the number of phases that have passed between a day
	uint32_t(*GetNextDayCloudy)(); // if the next day will be cloudy
	float(*GetTimeBetweenPhase)(); // value between 0-1 that represents the progress towards the next phase
	uint32_t(*GetTimeForEachPhase)(); // time in frames it takes for each phase to finish
};


struct DAYNIGHT_SAVE {
	uint32_t day;				// starting from index 0, the days that have passed (potential mechanic in the future)
	float time;					// we store the 0-24 hour cycle as a float so that it can be rescaled if its somehow reconfigured
	uint32_t currentPhase;		// current phase
	bool nextDayCloudy;
};

// bit hacky, but crunch moment
extern uint32_t gDayNightCheatPhase;

void AL_DayNightCycle_PushFallbackLight();
void AL_DayNightCycle_PopFallbackLight();

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