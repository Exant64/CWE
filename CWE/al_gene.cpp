#include "stdafx.h"

#include "al_behavior/al_knowledge.h"
#include "Chao.h"

#include "data/color_mix_table.h"
#include "data/more_faces.h"

#include <random>
#include <unordered_map>
#include "ChaoMain.h"
#include "api/api_accessory.h"
#include "al_behavior/al_behavior.h"

const int sub_536450Ptr = 0x536450;
void sub_536450(KarateOpponent* a1, CHAO_PARAM_GC* a2)
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
void AL_GeneAnalyzeCommon(AL_GENE* a1, CHAO_PARAM_GC* a2)
{
	__asm
	{
		mov edi, a1
		push a2
		call AL_GeneAnalyzeCommmonPtr
		add esp, 4
	}
}

static void AL_GeneColorMixing(AL_GENE* pGene, CHAO_PARAM_GC* pParam) {
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

static void AL_GeneVaryingShades(AL_GENE* pGene, CHAO_PARAM_GC* pParam) {
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

static void AL_GeneAnalyzeMoreFaces(CHAO_PARAM_GC* pParam) {
	if(gConfigVal.MoreFaces == CFG_MORE_FACE_RANDOM) {
		pParam->EyeType = Uint8(njRandom() * (float(ChaoEyes_Mean) + 0.999f));
		pParam->MouthType = Uint8(njRandom() * (float(0x10) + 0.999f));

		return;
	}

	uint32_t acceptedEntryIndices[_countof(NewFaceEntries)];
	uint32_t acceptedEntryCount = 0;

	for(size_t i = 0; i < _countof(NewFaceEntries); ++i) {
		const auto& entry = NewFaceEntries[i];

		bool check = true;

		for(size_t j = 0; j < entry.PersonalityCount; ++j) {
			const auto persKind = entry.PersonalityKinds[j];
			const auto persRange = entry.PersonalityCheckRanges[j];
	
			const auto pers = pParam->Emotion.Personality[persKind - EM_PER_CURIOSITY];
			const bool persLow = pers < -40;
			const bool persHigh = pers > 40;

			bool localCheck = false;

			switch(persRange) {
				case GENE_FACE_CHECK_LOW:
					localCheck = persLow;
					break;
				case GENE_FACE_CHECK_MID:
					localCheck = !persLow && !persHigh;
					break;
				case GENE_FACE_CHECK_HIGH:
					localCheck = persHigh;
					break;
			}

			if(!localCheck) {
				check = false;
				break;
			}
		}

		if(check) {
			acceptedEntryIndices[acceptedEntryCount++] = i;
		}
	}

	if(acceptedEntryCount > 0) {
		// added 1 + because index 0 resembles the existing vanilla face
		const auto index = Uint32(njRandom() * (1 + float(acceptedEntryCount) - 0.0001f));

		if(!index) return;

		const auto& entry = NewFaceEntries[acceptedEntryIndices[index - 1]];

		pParam->EyeType = entry.Eye;
		pParam->MouthType = entry.Mouth;
	}
}

// this is where you would add anything that gets applied to the chaodata from the dna
void AL_GeneAnalyzeCommonAdd(AL_GENE* pGene, CHAO_PARAM_GC* pParam) {
	auto pParamCwe = GET_CWEPARAM(pParam);

	AL_GeneAnalyzeCommon(pGene, pParam);

	if(gConfigVal.ColorMixing) {
		AL_GeneColorMixing(pGene, pParam);
	}

	if(gConfigVal.NewbornVaryingShades) {
		AL_GeneVaryingShades(pGene, pParam);
	}

	if (gConfigVal.MoreFaces) {
		AL_GeneAnalyzeMoreFaces(pParam);
	}

	if (gConfigVal.LuckyChao) {
		const float luckyChaoChance = 1.f / 60.f;
		if(njRandom() < luckyChaoChance) {
			pParam->Headgear = 1 + Uint8(njRandom() * (14 - 1 + 0.999f));
		}

		for(size_t i = 0; i < _countof(pParamCwe->Accessories); ++i) {
			if(njRandom() < 1.f - luckyChaoChance) {
				continue;
			}

			// filtering for type
			std::vector<size_t> accessoryIndices;
			for(size_t a = 0; a < GetAccessoryCount(); ++a) {
				extern NJS_OBJECT object_alo_missing;
				const auto& accData = GetAccessoryData(a);

				if(accData.pObject == &object_alo_missing) {
					continue;
				}
				
				if(accData.SlotType == i) {
					accessoryIndices.push_back(a);
				}
			}

			if(accessoryIndices.empty()) {
				continue;
			}

			AL_SetAccessory(
				pParamCwe, 
				accessoryIndices[Uint32(njRandom() * (float(accessoryIndices.size()) - 0.001f))]
			);
		}
	}

	if (gConfigVal.EyeColorsForNewbornChao) {
		pParamCwe->EyeColor = Uint8(njRandom() * 5.999f);
	}

	pParamCwe->Flags |= AL_PARAM_FLAG_NEWBORN;

	pParamCwe->Negative = pGene->Negative[0] || pGene->Negative[1];

	if (pGene->MotherID.id[0] != 0) {
		auto motherParam = AL_KW_FindChaoBasedOnId(pGene->MotherID);
		auto dataID = GET_CWEPARAM(motherParam);

		memcpy(pParamCwe->MotherName, dataID->Name, sizeof(AL_NAME));
		memcpy(pParamCwe->MGroundFatherName, dataID->FatherName, sizeof(AL_NAME));
		memcpy(pParamCwe->MGroundMotherName, dataID->MotherName, sizeof(AL_NAME));
		sub_536450(&pParamCwe->motherData, motherParam);
	}

	if (pGene->FatherID.id[0] != 0) {
		auto fatherParam = AL_KW_FindChaoBasedOnId(pGene->FatherID);
		auto dataID = GET_CWEPARAM(fatherParam);

		memcpy(pParamCwe->FatherName, dataID->Name, sizeof(AL_NAME));
		memcpy(pParamCwe->FGroundFatherName, dataID->FatherName, sizeof(AL_NAME));
		memcpy(pParamCwe->FGroundMotherName, dataID->MotherName, sizeof(AL_NAME));
		sub_536450(&pParamCwe->fatherData, fatherParam);
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

static void AL_GetMedalGene(const CHAO_PARAM_GC* param, AL_GENE& gene) {
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
	if(param->Color || param->type < 5 || param->type >= ChaoType_Neutral_Chaos) {
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

	const auto& entry = ColorEntries[param->type - 5];

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

void AL_CreateChildGene(task* pMotherTask, task* pFatherTask, AL_GENE* pChildGene)
{
	AL_GENE FatherGene = GET_CHAOPARAM(pFatherTask)->Gene;
	AL_GENE MotherGene = GET_CHAOPARAM(pMotherTask)->Gene;

	if (gConfigVal.MedalChaoCanMakeColorChao) {
		AL_GetMedalGene(GET_CHAOPARAM(pFatherTask), FatherGene);
		AL_GetMedalGene(GET_CHAOPARAM(pMotherTask), MotherGene);
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
		if (GET_CHAOPARAM(pMotherTask)->field_19 != 1 && GET_CHAOPARAM(pMotherTask)->Medal == 7)
			Chance += 0.38f;

		if (GET_CHAOPARAM(pFatherTask)->field_19 != 1 && GET_CHAOPARAM(pFatherTask)->Medal == 7)
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
	
	pChildGene->Negative[0] = GET_CWEPARAM(pMotherTask)->Negative;
	pChildGene->Negative[1] = GET_CWEPARAM(pFatherTask)->Negative;
	pChildGene->MotherID = GET_CHAOPARAM(pMotherTask)->ChaoID;
	pChildGene->FatherID = GET_CHAOPARAM(pFatherTask)->ChaoID;
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