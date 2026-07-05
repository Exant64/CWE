#include "stdafx.h"

#include <vector>
#include <al_behavior/al_behavior.h>
#include <al_behavior/albhv_birthday.h>

#include "include/code_birthday.h"
#include <al_world.h>
#include <Chao.h>

void BirthdayParty::OnALControl(task* tp) {
	if (tp->twp->mode == 1)
	{
		std::vector<task*> otherChao;
		*(int*)0x01DBE574 = 0;
		pBirthdayChao = nullptr;

		//should only run for one frame, around second frame of chao world
		for (int i = 0; i < ALW_CountEntry(0); i++)
		{
			task* pChao = GetChaoObject(0, i);
			if (!pChao) continue;

			CHAO_PARAM_GC* param = GET_CHAOPARAM(pChao);
			
			if (!pBirthdayChao &&
				param->age > 0 &&
				GET_CWEPARAM(pChao)->Birthday > 0 &&
				(param->age % GET_CWEPARAM(pChao)->Birthday) == 0)
			{
				param->age++;
				pBirthdayChao = pChao;
			}
			else {
				otherChao.push_back(pChao);
			}
		}

		if (pBirthdayChao && njRandom() < 0.50f) //50% chance
		{
			//is underwater
			if (pBirthdayChao->twp->pos.y + 2.0 < pBirthdayChao->EntityData2->WaterY) return;

			AL_SetBehavior(pBirthdayChao, ALBHV_Birthday);
			for (task* chao : otherChao)
			{
				if (AL_GetBehavior(chao) != (BHV_FUNC)0x54EF10 &&
					AL_GetBehavior(chao) != (BHV_FUNC)0x54F7A0 &&
					chao != pBirthdayChao)
					AL_SetBehavior(chao, ALBHV_Celebrate);
			}
		}
	}
}