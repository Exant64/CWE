#pragma once
#include "../../../cwe_code.h"
#include <unordered_map>

class GCShinyJewel : public Code {
public:

	using Code::Code;

	void OnChaoData(ChaoDataBase& chao) override;
};

const std::unordered_map<Uint8, Uint8> pcToGc = {
	{1, 82},
	{2, 217},
	{4, 212},
	{6, 235},
	{7, 82},
	{8, 200},
	{9, 237},
	{10, 82},
	{11, 229},
	{12, 190},
	{13, 127},
};

void GCShinyJewel::OnChaoData(ChaoDataBase& chao) {
	if (chao.Texture < 1) return;
	if (chao.Shiny != 1) return;
	if (chao.Color >= 16) return; //already transformed probably

	if (pcToGc.count(chao.Color)) 
		chao.Color = pcToGc.at(chao.Color);
}