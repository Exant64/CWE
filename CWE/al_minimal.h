#pragma once
 
#include <vector>
#include <map>

struct CWE_MINIMAL_FRUIT
{
	int minimalID;
	short chanceMin, chanceMax;
};

extern std::vector< int> ModAPI_MinimalInfluence;
extern std::vector< ChaoItemStats> ModAPI_MinimalStats;
extern std::vector< NJS_TEXLIST*>  ModAPI_MinimalTexlists;
extern std::vector< NJS_OBJECT*>   ModAPI_MinimalModels;
extern std::vector< NJS_MOTION*>   ModAPI_MinimalMotion0;
extern std::vector< NJS_MOTION*>   ModAPI_MinimalMotion1;
extern std::vector< NJS_MOTION*>   ModAPI_MinimalMotion2;
extern std::vector< NJS_MOTION*>   ModAPI_MinimalMotion3;
extern std::map<int, std::vector<CWE_MINIMAL_FRUIT>> ModAPI_MinimalFruit;
void AL_Minimal_Timer(ObjectMaster* a1);
void ParamFukidasiExecutor_Load(ObjectMaster* a1);
void al_minimal_Init();