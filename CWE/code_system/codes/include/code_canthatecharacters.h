#pragma once
#include "../../cwe_code.h"

class CantHateCharacters : public Code {
public:

	using Code::Code;

	void OnChaoData(ChaoDataBase& chao) override;
};

void CantHateCharacters::OnChaoData(ChaoDataBase& chao) {
	for (int i = 0; i < 6; i++) {
		if (chao.Knowledge.player[i].like < 0) {
			chao.Knowledge.player[i].like = 0;
		}
	}
}