#pragma once
#include "../../cwe_code.h"
class MonsterEvo : public Code {
public:

	using Code::Code;

	void Init() override;
	void ChaoObject(ObjectMaster* tp) override;
};