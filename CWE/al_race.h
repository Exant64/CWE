#pragma once


struct RaceConfigThing
{
	int raceGroupNum;
	int raceIndex;
	int level;
	int field_C;
	int toyUnlock;
};

DataArray(RaceConfigThing, HeroRaceConfigs, 0x01354C60, 4);
DataArray(RaceConfigThing, DarkRaceConfigs, 0x01354CB0, 4);
DataArray(RaceConfigThing, ChallengeRaceConfigs, 0x01354B70, 4);

void al_race_Init();