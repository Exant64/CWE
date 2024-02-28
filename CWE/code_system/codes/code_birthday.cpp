#include "stdafx.h"

#include <vector>
#include <al_behavior/al_behavior.h>
#include <al_behavior/albhv_birthday.h>

#include "include/code_birthday.h"
#include <al_world.h>
#include <Chao.h>

void BirthdayParty::OnALControl(ObjectMaster* tp) {
	if (tp->Data1.Entity->Action == 1)
	{
		std::vector<ObjectMaster*> otherChao;
		*(int*)0x01DBE574 = 0;
		pBirthdayChao = nullptr;

		//should only run for one frame, around second frame of chao world
		for (int i = 0; i < ALW_CountEntry(0); i++)
		{
			ObjectMaster* pChao = GetChaoObject(0, i);
			if (!pChao) continue;

			ChaoDataBase* param = pChao->Data1.Chao->pParamGC;

			
			if (!pBirthdayChao &&
				param->ClockRollovers > 0 &&
				param->Birthday > 0 &&
				(param->ClockRollovers % param->Birthday) == 0)
			{
				param->ClockRollovers++;
				pBirthdayChao = pChao;
			}
			else {
				otherChao.push_back(pChao);
			}
		}

		if (pBirthdayChao && njRandom() < 0.50f) //50% chance
		{
			//is underwater
			if (pBirthdayChao->Data1.Entity->Position.y + 2.0 < pBirthdayChao->EntityData2->field_DC) return;

			AL_SetBehavior(pBirthdayChao, ALBHV_Birthday);
			for (ObjectMaster* chao : otherChao)
			{
				if (AL_GetBehavior(chao) != (BHV_FUNC)0x54EF10 &&
					AL_GetBehavior(chao) != (BHV_FUNC)0x54F7A0 &&
					chao != pBirthdayChao)
					AL_SetBehavior(chao, ALBHV_Celebrate);
			}
		}
	}
}