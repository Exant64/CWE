#pragma once
#include "../../cwe_code.h"
class ShinyJewelMonotone : public Code {
public:

	using Code::Code;

	void OnChaoData(CHAO_PARAM_GC& chao) override;
};

void ShinyJewelMonotone::OnChaoData(CHAO_PARAM_GC& chao) {
	if (chao.nbSucceed == 0 && chao.type == 1)
		GET_CWEPARAM(&chao)->ShinyJewelMonotone = 1;
}