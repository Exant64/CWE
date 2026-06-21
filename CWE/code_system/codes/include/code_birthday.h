#pragma once
#include "../../cwe_code.h"
class BirthdayParty : public Code {
public:

	using Code::Code;

	void OnALControl(task* tp) override;
};