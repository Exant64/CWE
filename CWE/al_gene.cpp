#include "stdafx.h"

#include "al_behavior/al_knowledge.h"
#include "Chao.h"

#include "data/color_mix_table.h"

#include <random>
#include <unordered_map>
#include "ChaoMain.h"

const int sub_536450Ptr = 0x536450;
void sub_536450(KarateOpponent* a1, ChaoDataBase* a2)
{
	__asm
	{
		mov edi, a1
		mov esi, a2
		call sub_536450Ptr
	}
}

//unused for now
void InitChaoDNA_Hook(AL_GENE* a1, int a2, size_t a3) //use this to edit how chao are generated
{
	memset(a1, 0, sizeof(AL_GENE));

	//code here
}

const int AL_GeneAnalyzeCommmonPtr = 0x00551DA0;
void AL_GeneAnalyzeCommon(AL_GENE* a1, ChaoDataBase* a2)
{
	__asm
	{
		mov edi, a1
		push a2
		call AL_GeneAnalyzeCommmonPtr
		add esp, 4
	}
}

static void AL_GeneColorMixing(AL_GENE* pGene, ChaoDataBase* pParam) {
	// we know normal colored chao can't mix so don't even bother checking
	if (!pGene->Color[0] || !pGene->Color[1]) {
		return;
	}

	// hack: in the original lst code, these corresponded to a different value
	// than the pair the other way around
	// darky said this was because it was different for some reason on photoshop too
	// and he didn't mind because it was a happy accident, so we'll keep it for now too
	if(pGene->Color[0] == 2 && pGene->Color[1] == 7) {
		pParam->Color = 43;
		return;
	}

	if(pGene->Color[0] == 13 && pGene->Color[1] == 7) {
		pParam->Color = 54;
		return;
	}

	for(size_t i = 0; i < _countof(color_mix_table); ++i) {
		const auto& entry = color_mix_table[i];

		const bool checkPair1 = pGene->Color[0] == entry.color_a && pGene->Color[1] == entry.color_b;
		const bool checkPair2 = pGene->Color[1] == entry.color_a && pGene->Color[0] == entry.color_b;

		if(checkPair1 || checkPair2) {
			pParam->Color = entry.color_result;
			break;
		}
	}
}

static void AL_GeneVaryingShades(AL_GENE* pGene, ChaoDataBase* pParam) {
	// I'm guessing the idea for this was that it would be for black market chao only
	if(pGene->Color[0] != pGene->Color[1]) {
		return;
	}

	const auto color = pGene->Color[0];

	// normal chao or not a vanilla color
	if(!color || color > SA2BColour_Black) {
		return;
	}

	std::vector<uint8_t> shades;
	switch (color) {
		case SA2BColour_Yellow:
			shades = { 0x12, 0x9C, 0xAE, 0xB3 };
			break;
		case SA2BColour_White:
			shades = { 0x13, 0xB4, 0xB6, 0xB9 };
			break;
		case SA2BColour_Brown:
			shades = { 0x14, 0xBA, 0xBC, 0xC0 };
			break;
		case SA2BColour_SkyBlue:
			shades = { 0x16, 0xC2, 0xC3, 0xC5 };
			break;
		case SA2BColour_Pink:
			shades = { 0x17, 0xC6, 0xC9, 0xCC };
			break;
		case SA2BColour_Blue:
			shades = { 0x18, 0xCE, 0xD0, 0xD1 };
			break;
		case SA2BColour_Grey:
			shades = { 0x1E, 0xD2, 0xD8 };
			break;
		case SA2BColour_Green:
			shades = { 0x19, 0xDA, 0xDD, 0xDE };
			break;
		case SA2BColour_Red:
			shades = { 0x1A, 0xE1, 0xE3, 0xE4 };
			break;
		case SA2BColour_LimeGreen:
			shades = { 0x1B, 0xE6, 0xE9, 0xEA };
			break;
		case SA2BColour_Purple:
			shades = { 0x1C, 0xF0, 0xF2, 0xF3 };
			break;
		case SA2BColour_Orange:
			shades = { 0x1F, 0xF5, 0xF6, 0xF9 };
			break;
		case SA2BColour_Black:
			shades = { 0xFC, 0xFE, 0xFF };
			break;
	}

	// add the original shade to the pool aswell
	shades.push_back(color);

	pParam->Color = shades[size_t(njRandom() * (shades.size() - 0.001f))];
}

// this is where you would add anything that gets applied to the chaodata from the dna
void AL_GeneAnalyzeCommonAdd(AL_GENE* pGene, ChaoDataBase* pParam)
{
	AL_GeneAnalyzeCommon(pGene, pParam);	

	if(gConfigVal.ColorMixing) {
		AL_GeneColorMixing(pGene, pParam);
	}

	if(gConfigVal.NewbornVaryingShades) {
		AL_GeneVaryingShades(pGene, pParam);
	}

	if (gConfigVal.LuckyChao) {
		if(njRandom() < 6 / 100.f) {
			pParam->Headgear = 1 + Uint8(njRandom() * (14 - 1 + 0.999f));
		}
	}

	if (gConfigVal.EyeColorsForNewbornChao) {
		pParam->EyeColor = Uint8(njRandom() * 5.999f);
	}
	
	pParam->Flags |= AL_PARAM_FLAG_NEWBORN;

	pParam->Negative = pGene->Negative[0] || pGene->Negative[1];

	if (pGene->MotherID.id[0] != 0)
	{
		ChaoDataBase* dataID = AL_KW_FindChaoBasedOnId(pGene->MotherID);
		memcpy(pParam->MotherName, dataID->Name, sizeof(AL_NAME));
		memcpy(pParam->MGroundFatherName, dataID->FatherName, sizeof(AL_NAME));
		memcpy(pParam->MGroundMotherName, dataID->MotherName, sizeof(AL_NAME));
		sub_536450(&pParam->motherData, dataID);
	}
	if (pGene->FatherID.id[0] != 0)
	{
		ChaoDataBase* dataID = AL_KW_FindChaoBasedOnId(pGene->FatherID);
		memcpy(pParam->FatherName, dataID->Name, sizeof(AL_NAME));
		memcpy(pParam->FGroundFatherName, dataID->FatherName, sizeof(AL_NAME));
		memcpy(pParam->FGroundMotherName, dataID->MotherName, sizeof(AL_NAME));
		sub_536450(&pParam->fatherData, dataID);
	}
}
static void __declspec(naked) AL_GeneAnalyzeCommonHook()
{
	__asm
	{
		push[esp + 04h] // a2
		push edi // a1

		// Call your __cdecl function here:
		call AL_GeneAnalyzeCommonAdd

		pop edi // a1
		add esp, 4 // a2
		retn
	}
}

const int AL_BlendGenePtr = 0x00551840;
void AL_BlendGene(AL_GENE* a1, AL_GENE* a2, AL_GENE* pDestGene)
{
	__asm
	{
		mov ecx, a1
		mov eax, a2
		mov esi, pDestGene
		call AL_BlendGenePtr
	}
}

static void AL_GetMedalGene(const ChaoDataBase* param, AL_GENE& gene) {
	if (param->Texture != 0) return;
	if (param->field_19 == 1) return;

	switch (param->Medal) {
	case ChaoMedal_Aquamarine:
	case ChaoMedal_Sapphire:
		gene.Jewel[0] = SA2BTexture_SkyBlueJewel;
		gene.Jewel[1] = SA2BTexture_BlueJewel;
		break;
	case ChaoMedal_Topaz:
		gene.Jewel[0] = SA2BTexture_OrangeJewel;
		gene.Jewel[1] = SA2BTexture_OrangeJewel;
		break;
	case ChaoMedal_Peridot:
	case ChaoMedal_Emerald:
		gene.Jewel[0] = SA2BTexture_LimeGreenJewel;
		gene.Jewel[1] = SA2BTexture_GreenJewel;
		break;
	case ChaoMedal_Garnet:
	case ChaoMedal_Ruby:
		gene.Jewel[0] = SA2BTexture_RedJewel;
		gene.Jewel[1] = SA2BTexture_PinkJewel;
		break;
	case ChaoMedal_Onyx:
	case ChaoMedal_Amethyst:
		gene.Jewel[0] = SA2BTexture_BlackJewel;
		gene.Jewel[1] = SA2BTexture_PurpleJewel;
		break;
	case ChaoMedal_Diamond:
		gene.Jewel[0] = rand() % 0x7E;
		gene.Jewel[1] = rand() % 0x7E;
		break;
	case ChaoMedal_Hero:
		gene.Jewel[0] = SA2BTexture_YellowJewel;
		gene.Jewel[1] = SA2BTexture_YellowJewel;
		break;
	case ChaoMedal_Dark:
		gene.Jewel[0] = SA2BTexture_WhiteJewel;
		gene.Jewel[1] = SA2BTexture_WhiteJewel;
		break;
	case ChaoMedal_Pearl:
		gene.Jewel[0] = SA2BTexture_Pearl;
		gene.Jewel[1] = SA2BTexture_Pearl;
		break;
	}
}

static void AL_GeneNormalChaoColors(task* tp, AL_GENE& gene) {
	const auto param = GET_CHAOPARAM(tp);

	// if not normal colored or not regular adult chao
	if(param->Color || param->Type < 5 || param->Type >= ChaoType_Neutral_Chaos) {
		return;
	}

	gene.Color[0] = 0;
	gene.Color[1] = 0;

	if(param->EvolutionProgress < 0.8f) {
		return;
	}

	struct GENE_NORMAL_COLORS {
		uint8_t swim, monster_swim;
		uint8_t fly, monster_fly;
		uint8_t run, monster_run;
		uint8_t power, monster_power;
	};

	static const GENE_NORMAL_COLORS HeroColor = { 
		2, 0x7F,
		2, 0x7F,
		2, 0x7F,
		2, 0x7F
	};

	// array starts from neutral/normal (first adult type index)
	static const GENE_NORMAL_COLORS ColorEntries[] = {
		{ 0x04, 0xD4, 0x04, 0xD4, 0x04, 0xD4, 0x04, 0xD4 },
		HeroColor,
		{ 0x86, 0xF4, 0x05, 0xBB, 0x0B, 0xE5, 0x0C, 0xBE },

		{ 0x15, 0x8F, 0x05, 0xD9, 0x04, 0xAF, 0x01, 0x83 },
		HeroColor,
		{ 0x88, 0xC1, 0x87, 0x92, 0x04, 0xD4, 0x08, 0xC8 },

		{ 0x8A, 0xF4, 0x8D, 0xAB, 0x9A, 0xD9, 0x96, 0x97 },
		HeroColor,
		{ 0x8E, 0xB5, 0x8D, 0xAB, 0x89, 0xA5, 0x8A, 0x8C },

		{ 0x04, 0xAF, 0x73, 0xEB, 0x52, 0xFB, 0x0B, 0xF8 },
		HeroColor,
		{ 0x90, 0xB0, 0x88, 0xC1, 0x09, 0xED, 0x52, 0xFB },

		{ 0x05, 0xD3, 0x05, 0xD3, 0x0C, 0xBE, 0x09, 0xB1 },
		HeroColor,
		{ 0x04, 0xB5, 0x87, 0xE2, 0x0A, 0xE0, 0x94, 0xBE },
	};

	const auto& entry = ColorEntries[param->Type - 5];

	if(param->PowerRun > 0.8f) {
		if(param->PowerRun < 1.5f) {
			gene.Color[0] = gene.Color[1] = entry.power;
		}
		else {
			gene.Color[0] = gene.Color[1] = entry.monster_power;
		}
	}

	if(param->PowerRun < -0.8f) {
		if(param->PowerRun > -1.5f) {
			gene.Color[0] = gene.Color[1] = entry.run;
		}
		else {
			gene.Color[0] = gene.Color[1] = entry.monster_run;
		}
	}

	if(param->FlySwim > 0.8f) {
		if(param->FlySwim < 1.5f) {
			gene.Color[0] = gene.Color[1] = entry.fly;
		}
		else {
			gene.Color[0] = gene.Color[1] = entry.monster_fly;
		}
	}

	if(param->FlySwim < -0.8f) {
		if(param->FlySwim > -1.5f) {
			gene.Color[0] = gene.Color[1] = entry.swim;
		}
		else {
			gene.Color[0] = gene.Color[1] = entry.monster_swim;
		}
	}
}

void AL_CreateChildGene(ObjectMaster* pMotherTask, ObjectMaster* pFatherTask, AL_GENE* pChildGene)
{
	AL_GENE FatherGene = pFatherTask->Data1.Chao->pParamGC->Gene;
	AL_GENE MotherGene = pMotherTask->Data1.Chao->pParamGC->Gene;

	if (gConfigVal.MedalChaoCanMakeColorChao) {
		AL_GetMedalGene(pFatherTask->Data1.Chao->pParamGC, FatherGene);
		AL_GetMedalGene(pMotherTask->Data1.Chao->pParamGC, MotherGene);
	}

	if (gConfigVal.NormalChaoMakeColorChao) {
		AL_GeneNormalChaoColors(pFatherTask, FatherGene);
		AL_GeneNormalChaoColors(pMotherTask, MotherGene);
	}

	AL_BlendGene(&MotherGene, &FatherGene, pChildGene);

	//IS NEW CHALLENGE ENABLED??
	if (*(char*)0x0053FD6C == 1)
	{
		float Chance = 0.0f;
		if (pMotherTask->Data1.Chao->pParamGC->field_19 != 1 && pMotherTask->Data1.Chao->pParamGC->Medal == 7)
			Chance += 0.38f;

		if (pFatherTask->Data1.Chao->pParamGC->field_19 != 1 && pFatherTask->Data1.Chao->pParamGC->Medal == 7)
			Chance += 0.38f;

		if (Chance > 0)
		{
			int count = 0;
			for (int i = 0; i < 5; i++)
			{
				if (njRandom() < Chance)
				{
					pChildGene->Abl[i][0] = ChaoGrade_A;
					pChildGene->Abl[i][1] = ChaoGrade_A;
					count++;
				}
				if (count >= 3)
					break;
			}
		}
	}
	pChildGene->Negative[0] = pMotherTask->Data1.Chao->pParamGC->Negative;
	pChildGene->Negative[1] = pFatherTask->Data1.Chao->pParamGC->Negative;
	pChildGene->MotherID = pMotherTask->Data1.Chao->pParamGC->ChaoID;
	pChildGene->FatherID = pFatherTask->Data1.Chao->pParamGC->ChaoID;
}
static void __declspec(naked) AL_CreateChildGeneHook()
{
	__asm
	{
		push edx // pChildGene
		push[esp + 0Ch] // pFatherTask
		push[esp + 0Ch] // pMotherTask

		// Call your __cdecl function here:
		call AL_CreateChildGene

		add esp, 4 // pMotherTask
		add esp, 4 // pFatherTask
		pop edx // pChildGene
		retn
	}
}

void AL_Gene_Init() {

	//WriteCall((void*)0x005506BF, InitChaoDNA_Hook); not needed for now

	//dna mother-father name restoration
	WriteJump((void*)0x00551D20, AL_CreateChildGeneHook);
	WriteCall((void*)0x005504A1, AL_GeneAnalyzeCommonHook);
	WriteCall((void*)0x0057BCDB, AL_GeneAnalyzeCommonHook);
}