#pragma once
#include "../../../cwe_code.h"

class DisableLevelLimit : public Code {
public:

	using Code::Code;

	//maybe change to OnChaoObject?
	void OnChaoData(ChaoDataBase& chao) override;
};

void DisableLevelLimit::OnChaoData(ChaoDataBase& chao) {
	for (int i = 0; i < 5; i++)
		if (chao.StatLevels[i] == 99)
			chao.StatLevels[i] = 0;
}