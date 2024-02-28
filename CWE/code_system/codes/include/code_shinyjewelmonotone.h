#pragma once
#include "../../cwe_code.h"
class ShinyJewelMonotone : public Code {
public:

	using Code::Code;

	void OnChaoData(ChaoDataBase& chao) override;
};

void ShinyJewelMonotone::OnChaoData(ChaoDataBase& chao) {
	if (chao.Reincarnations == 0 && chao.Type == 1)
		chao.ShinyJewelMonotone = 1;
}