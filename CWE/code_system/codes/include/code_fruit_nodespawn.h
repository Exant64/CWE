#pragma once
#include <SA2ModLoader.h>
#include <ChaoMain.h>
#include <code_system/cwe_code.h>

class FruitsDontDespawn : public Code {
public:
	using Code::Code;

	void OnALControl(ObjectMaster* tp) override;
};

void FruitsDontDespawn::OnALControl(ObjectMaster* tp) {
	for (size_t i = 0; i < 40; i++) {
		ChaoObjectData* objData = (ChaoObjectData*)ChaoFruitSlots;
		objData[i].Age = 0;
	}
}