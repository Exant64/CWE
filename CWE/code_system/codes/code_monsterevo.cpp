#include "stdafx.h"
#include <Chao.h>
#include "include/code_monsterevo.h"
#include <al_behavior/al_intention.h>

void MonsterEvo::Init() {
	ChaoGlobal.DeformLimit = 2.0f;

	if (m_param == 2)
		ChaoGlobal.GrowthLimit = 2.4f;
}

void MonsterEvo::ChaoObject(task* tp) {
	CHAO_PARAM_GC* data = GET_CHAOPARAM(tp);
	
	if (data->Skill[0] < 2000 && data->body.VPos < -1) data->body.VPos = -1;
	if (data->Skill[0] < 3000 && data->body.VPos < -1.5f) data->body.VPos = -1.5f;

	if (data->Skill[1] < 2000 && data->body.VPos > 1) data->body.VPos = 1;
	if (data->Skill[1] < 3000 && data->body.VPos > 1.5f) data->body.VPos = 1.5f;

	if (data->Skill[2] < 2000 && data->body.HPos < -1) data->body.HPos = -1;
	if (data->Skill[2] < 3000 && data->body.HPos < -1.5f) data->body.HPos = -1.5f;

	if (data->Skill[3] < 2000 && data->body.HPos > 1) data->body.HPos = 1;
	if (data->Skill[3] < 3000 && data->body.HPos > 1.5f) data->body.HPos = 1.5f;

	if (data->Skill[4] < 2000 && data->body.growth > 1.2f) data->body.growth = 1.2f;
	if (data->Skill[4] < 3000 && data->body.growth > 1.8f) data->body.growth = 1.8f;
}