#include "stdafx.h"
#include "../cwe_code.h"
#include <al_behavior/al_intention.h>
#include "include/code_personality.h"

void PersonalityMultiply::Init() {
	if(m_param <= 1) return;

	const auto multiplier = ((m_param - 2) % 2) + 2;
	const bool affectHunger = m_param < 4;

	ChaoGlobal.ParamSubAnger *= multiplier; //Anger (Crybaby-Energetic, Kindness and Carefree)
	ChaoGlobal.ParamSubSorrow *= multiplier;  //Urge to cry (Crybaby-Energetic and Carefree)
	ChaoGlobal.ParamSubFear *= multiplier; //Fear (Crybaby-Energetic and Carefree)
	ChaoGlobal.ParamSubSurprise *= multiplier; //Surprise (Carefree)
	ChaoGlobal.ParamSubPain *= multiplier; //Dizziness (Regain)
	ChaoGlobal.ParamSubRelax *= multiplier; //Relax (Curious and Fickle) 
	ChaoGlobal.ParamAddSleepy *= multiplier; //Sleepiness (Naive)

	if(affectHunger) {
		ChaoGlobal.ParamAddHunger *= multiplier; //Hunger (Big Eater)
	}

	ChaoGlobal.ParamAddBreed *= multiplier; //Desire to mate (Vitality)
	ChaoGlobal.ParamAddTediousCuriosity *= multiplier; //Boredom (Curious)
	ChaoGlobal.ParamAddTediousAggressive *= multiplier; //Boredom (Crybaby-Energetic)
	ChaoGlobal.ParamAddTediousCalm *= multiplier; //Boredom (Carefree)
	ChaoGlobal.ParamAddLonely *= multiplier; //Lonely (Solitude)
	ChaoGlobal.ParamAddTireNormal *= multiplier; //Tire (Vitality)
}