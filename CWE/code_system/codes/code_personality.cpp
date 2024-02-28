#include "stdafx.h"
#include "../cwe_code.h"
#include <al_behavior/al_intention.h>
#include "include/code_personality.h"

void PersonalityMultiply::Init() {
	ChaoGlobal.ParamSubAnger *= m_param; //Anger (Crybaby-Energetic, Kindness and Carefree)
	ChaoGlobal.ParamSubSorrow *= m_param;  //Urge to cry (Crybaby-Energetic and Carefree)
	ChaoGlobal.ParamSubFear *= m_param; //Fear (Crybaby-Energetic and Carefree)
	ChaoGlobal.ParamSubSurprise *= m_param; //Surprise (Carefree)
	ChaoGlobal.ParamSubPain *= m_param; //Dizziness (Regain)
	ChaoGlobal.ParamSubRelax *= m_param; //Relax (Curious and Fickle) 
	ChaoGlobal.ParamAddSleepy *= m_param; //Sleepiness (Naive)
	ChaoGlobal.ParamAddHunger *= m_param; //Hunger (Big Eater)
	ChaoGlobal.ParamAddBreed *= m_param; //Desire to mate (Vitality)
	ChaoGlobal.ParamAddTediousCuriosity *= m_param; //Boredom (Curious)
	ChaoGlobal.ParamAddTediousAggressive *= m_param; //Boredom (Crybaby-Energetic)
	ChaoGlobal.ParamAddTediousCalm *= m_param; //Boredom (Carefree)
	ChaoGlobal.ParamAddLonely *= m_param; //Lonely (Solitude)
	ChaoGlobal.ParamAddTireNormal *= m_param; //Tire (Vitality)
}