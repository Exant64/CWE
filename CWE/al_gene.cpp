#include "stdafx.h"

#include "al_behavior/al_knowledge.h"
#include "Chao.h"

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

// this is where you would add anything that gets applied to the chaodata from the dna
void AL_GeneAnalyzeCommonAdd(AL_GENE* pGene, ChaoDataBase* pParam)
{
	AL_GeneAnalyzeCommon(pGene, pParam);

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

void AL_CreateChildGene(ObjectMaster* pMotherTask, ObjectMaster* pFatherTask, AL_GENE* pChildGene)
{
	AL_GENE FatherGene = pFatherTask->Data1.Chao->pParamGC->Gene;
	AL_GENE MotherGene = pMotherTask->Data1.Chao->pParamGC->Gene;

	if (gConfigVal.MedalChaoCanMakeColorChao) {
		AL_GetMedalGene(pFatherTask->Data1.Chao->pParamGC, FatherGene);
		AL_GetMedalGene(pMotherTask->Data1.Chao->pParamGC, MotherGene);
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