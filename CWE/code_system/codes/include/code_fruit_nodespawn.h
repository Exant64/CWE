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
	for (size_t i = 0; i < _countof(AL_GetCurrGardenInfo()->fruit); i++) {
		AL_GetCurrGardenInfo()->fruit[i].nbVisit = 0;
	}
}