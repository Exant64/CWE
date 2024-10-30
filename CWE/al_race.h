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

DataPointer(Uint8, RaceMainType, 0x01A0FBD3);	//beginner,jewel,etc
DataPointer(Uint8, RaceSubType, 0x01A0FBD4);	//like aquamarine, etc
//starts at 4 for jewel race
DataPointer(Uint8, RaceLevel, 0x01A0FBD5);		//0-4, representing levels 1-5

void al_race_Init();