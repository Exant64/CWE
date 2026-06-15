#pragma once

#include <al_world.h>

#include "../../../cwe_code.h"

class InstantGrowTrees : public Code {
	public:
		using Code::Code;
		void OnALControl(task* tp) override;
};

void InstantGrowTrees::OnALControl(task* tp) {
    const size_t count = ALW_CountEntry(5);

    for(size_t i = 0; i < count; ++i) {
        task* pTree = GetChaoObject(5, i);

        if(!pTree) continue;

        EntityData1* work = pTree->Data1.Entity;

        if(work->Action == 4) {
            work->Action = 5;
        }
    }
}