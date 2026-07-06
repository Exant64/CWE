#pragma once
#include <SA2ModLoader.h>
#include <ChaoMain.h>
#include <code_system/cwe_code.h>

class FruitsDontDespawn : public Code {
public:
	using Code::Code;

	void OnALControl(task* tp) override;
};

void FruitsDontDespawn::OnALControl(task* tp) {
	for (size_t i = 0; i < 40; i++) {
		ITEM_SAVE_INFO* objData = (ITEM_SAVE_INFO*)ChaoFruitSlots;
		objData[i].nbVisit = 0;
	}
}