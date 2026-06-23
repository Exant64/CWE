#pragma once
#include "../../cwe_code.h"
class EggColor : public Code {
public:

	using Code::Code;

	void OnChaoData(CHAO_PARAM_GC& chao) override;
};