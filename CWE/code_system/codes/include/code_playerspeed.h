#pragma once
#include "../../cwe_code.h"

class PlayerSpeed : public Code {
public:

	using Code::Code;

	void OnALControl(task* tp) override;
};