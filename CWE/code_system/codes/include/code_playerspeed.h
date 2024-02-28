#pragma once
#include "../../cwe_code.h"

class PlayerSpeed : public Code {
public:

	using Code::Code;

	void OnALControl(ObjectMaster* tp) override;
};