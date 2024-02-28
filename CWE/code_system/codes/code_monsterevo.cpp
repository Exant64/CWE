#include "stdafx.h"
#include <Chao.h>
#include "include/code_monsterevo.h"
#include <al_behavior/al_intention.h>

void MonsterEvo::Init() {
	ChaoGlobal.DeformLimit = 2.0f;

	if (m_param == 2)
		ChaoGlobal.GrowthLimit = 2.4f;
}

void MonsterEvo::ChaoObject(ObjectMaster* tp) {
	ChaoDataBase* data = tp->Data1.Chao->pParamGC;
	if (data->StatPoints[0] < 2000 && data->FlySwim < -1) data->FlySwim = -1;
	if (data->StatPoints[0] < 3000 && data->FlySwim < -1.5f) data->FlySwim = -1.5f;

	if (data->StatPoints[1] < 2000 && data->FlySwim > 1) data->FlySwim = 1;
	if (data->StatPoints[1] < 3000 && data->FlySwim > 1.5f) data->FlySwim = 1.5f;

	if (data->StatPoints[2] < 2000 && data->PowerRun < -1) data->PowerRun = -1;
	if (data->StatPoints[2] < 3000 && data->PowerRun < -1.5f) data->PowerRun = -1.5f;

	if (data->StatPoints[3] < 2000 && data->PowerRun > 1) data->PowerRun = 1;
	if (data->StatPoints[3] < 3000 && data->PowerRun > 1.5f) data->PowerRun = 1.5f;

	if (data->StatPoints[4] < 2000 && data->EvolutionProgress > 1.2f) data->EvolutionProgress = 1.2f;
	if (data->StatPoints[4] < 3000 && data->EvolutionProgress > 1.8f) data->EvolutionProgress = 1.8f;
}