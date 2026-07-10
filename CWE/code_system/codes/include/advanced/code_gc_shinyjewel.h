#pragma once
#include "../../../cwe_code.h"
#include <unordered_map>

class GCShinyJewel : public Code {
public:

	using Code::Code;

	void OnChaoData(CHAO_PARAM_GC& chao) override;
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

void GCShinyJewel::OnChaoData(CHAO_PARAM_GC& chao) {
	if (chao.body.JewelNum < 1) return;
	if (!chao.body.MultiNum) return;
	if (chao.body.ColorNum >= 16) return; //already transformed probably

	if (pcToGc.count(chao.body.ColorNum)) 
		chao.body.ColorNum = pcToGc.at(chao.body.ColorNum);
}