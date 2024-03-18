#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "CodeParser.hpp"
#include "../IniFile.h"
#include "../al_behavior/al_intention.h"

template<typename T>
inline void PatchData(T* writeaddress, const T& data)
{
	if ((uint32_t)writeaddress < 0x87342C)
		WriteData(writeaddress, data, nullptr);
	else
		*writeaddress = data;
}

void CWE_Patch_Init(const IniFile* config)
{
	//main code
	//other chao than dark chao can get angry
	PatchData((unsigned char*)0x005A1279, (unsigned char)0xFE);
	PatchData((unsigned char*)0x005A127C, (unsigned char)23);
	PatchData((unsigned char*)0x005A1288, (unsigned char)1);

	//stg 13 write
	PatchData((unsigned short*)0x01313DF3, (unsigned short)0x3030);

	if (config->getBool("Chao World Extended", "FruitRebalance", true)) {
		//item stats
		//Hero Garden Fruit (sega changed the hero fruit in the pc port for some reason)
		PatchData((unsigned short*)0x008A6468, (unsigned short)40);

		//Strong Fruit
		PatchData((unsigned short*)0x008A6490, (unsigned short)60);

		//Tasty Fruit
		PatchData((unsigned short*)0x008A64A4, (unsigned short)30);
		PatchData((unsigned short*)0x008A64A6, (unsigned short)60);
		PatchData((unsigned short*)0x008A64A8, (unsigned short)0);

		//Hero, Dark Fruit
		PatchData((unsigned short*)0x008A64B8, (unsigned short)50);
		PatchData((unsigned short*)0x008A64CC, (unsigned short)50);

		//Round, Triangle, Square Fruit
		PatchData((unsigned short*)0x008A64E0, (unsigned short)80);
		PatchData((unsigned short*)0x008A64F4, (unsigned short)80);
		PatchData((unsigned short*)0x008A6508, (unsigned short)80);

		//Chao Fruit
		//PatchData((unsigned short*)0x008A6528, (unsigned short)100);
		//PatchData((unsigned short*)0x008A652A, (unsigned short)100);
		//PatchData((unsigned short*)0x008A652C, (unsigned short)100);
		//PatchData((unsigned short*)0x008A652E, (unsigned short)100);

		//Smart Fruit
		PatchData((unsigned short*)0x008A6544, (unsigned short)30);
		PatchData((unsigned short*)0x008A6548, (unsigned short)320);

		//Gba Fruit (Multiply by 2)
		PatchData((unsigned short*)0x008A6550, (unsigned short)60);
		PatchData((unsigned short*)0x008A6556, (unsigned short)60);
		PatchData((unsigned short*)0x008A6558, (unsigned short)20);
		PatchData((unsigned short*)0x008A6564, (unsigned short)40);
		PatchData((unsigned short*)0x008A6566, (unsigned short)100);
		PatchData((unsigned short*)0x008A656C, (unsigned short)60);
		PatchData((unsigned short*)0x008A6578, (unsigned short)80);
		PatchData((unsigned short*)0x008A657C, (unsigned short)80);
		PatchData((unsigned short*)0x008A6580, (unsigned short)40);
		PatchData((unsigned short*)0x008A6590, (unsigned short)60);
		PatchData((unsigned short*)0x008A6592, (unsigned short)80);
		PatchData((unsigned short*)0x008A6594, (unsigned short)40);
		PatchData((unsigned short*)0x008A65a2, (unsigned short)60);
		PatchData((unsigned short*)0x008A65a4, (unsigned short)60);
		PatchData((unsigned short*)0x008A65a8, (unsigned short)20);
		PatchData((unsigned short*)0x008A65B6, (unsigned short)80);
		PatchData((unsigned short*)0x008A65BA, (unsigned short)80);
		PatchData((unsigned short*)0x008A65BC, (unsigned short)40);
		PatchData((unsigned short*)0x008A65c8, (unsigned short)60);
		PatchData((unsigned short*)0x008A65cA, (unsigned short)20);
		PatchData((unsigned short*)0x008A65cC, (unsigned short)60);
		PatchData((unsigned short*)0x008A65ce, (unsigned short)40);

		//Mushroom
		PatchData((unsigned short*)0x008A65E4, (unsigned short)160);
		PatchData((unsigned short*)0x008A65E6, (unsigned short)0);
		//Mushroom Alt
		PatchData((unsigned short*)0x008A65F8, (unsigned short)320);
		PatchData((unsigned short*)0x008A65FA, (unsigned short)0);

		//Candy (Basically Chao Fruit x5)
		PatchData((unsigned short*)0x008A6604, (unsigned short)250);
		PatchData((unsigned short*)0x008A6606, (unsigned short)250);
		PatchData((unsigned short*)0x008A6608, (unsigned short)250);
		PatchData((unsigned short*)0x008A660A, (unsigned short)250);
		PatchData((unsigned short*)0x008A660C, (unsigned short)250); //edited from 50->250 at 9.3 release
		PatchData((unsigned short*)0x008A660E, (unsigned short)0); //added at 9.3 release
	}

	if (config->getBool("Advanced", "AdvancedChaosandCharactersChaoarelikeNormalChao", false))
	{
		PrintDebug("Patch AdvancedChaosandCharactersChaoarelikeNormalChao");
		PatchData((unsigned char*)0x00535A43, (unsigned char)0xFF);
		PatchData((unsigned char*)0x0056297E, (unsigned char)0x19);
		PatchData((unsigned char*)0x00562982, (unsigned char)0xFF);
		PatchData((unsigned char*)0x005628AD, (unsigned char)0x19);
		PatchData((unsigned char*)0x005628B1, (unsigned char)0xFF);
		PatchData((unsigned char*)0x00540212, (unsigned char)0xFF);
		PatchData((unsigned char*)0x0053A54B, (unsigned char)0xFF);
		PatchData((unsigned char*)0x0053A5BC, (unsigned char)0xFF);
	}

	if (config->getBool("Hard", "HardEnableSpinDashandSomersault", false))
	{
		PrintDebug("Patch HardEnableSpinDashandSomersault");
		PatchData((unsigned short*)0x007238EB, (unsigned short)0x9066);
	}

	if (config->getBool("Easy", "EasyStrongerChaosDrive", false))
	{
		PrintDebug("Patch EasyStrongerChaosDrive");
		PatchData((unsigned char*)0x008A63E8, (unsigned char)72);
		PatchData((unsigned char*)0x008A6400, (unsigned char)72);
		PatchData((unsigned char*)0x008A6416, (unsigned char)72);
		PatchData((unsigned char*)0x008A6426, (unsigned char)72);
		PatchData((unsigned short*)0x008A63f2, (unsigned short)20);
		PatchData((unsigned short*)0x008A6406, (unsigned short)20);
		PatchData((unsigned short*)0x008A641A, (unsigned short)20);
		PatchData((unsigned short*)0x008A642E, (unsigned short)20);
	}

	if (config->getBool("Misc", "OtherSADXChaoAlignment", false))
	{
		PrintDebug("Patch OtherSADXChaoAlignment");
		ChaoGlobal.FruitAddAttrChild = 0; 
		ChaoGlobal.FruitAddAttrAdult = 0;
		ChaoGlobal.FruitDislikeSubAttr = 0;
		ChaoGlobal.FruitPakuriSubAttr = 0;
		ChaoGlobal.MinimalAddAttrChild = 0;
		ChaoGlobal.MinimalAddAttrAdult = 0;
		ChaoGlobal.TimeAddAttrAdult = 0;
		ChaoGlobal.TimeAddAttrChild = 0;
		ChaoGlobal.DamageSubAttr = 0;
		ChaoGlobal.NadeAddAttr = 0;
		ChaoGlobal.ThrownSubAttr = 0;
		ChaoGlobal.DakkoAddAttr = 0;
	}

	if (config->getBool("Misc", "OtherRemoveDarkChao'sMeanEyesOverride", false))
	{
		PrintDebug("Patch OtherRemoveDarkChao'sMeanEyesOverride");
		PatchData((unsigned int*)0x0053A089, (unsigned int)0x90909090);
		PatchData((unsigned int*)0x0053A08C, (unsigned int)0x90909090);
	}

	if (config->getBool("Easy", "EasyReusableChaosDrive", false))
	{
		PrintDebug("Patch EasyReusableChaosDrive");
		PatchData((char*)0x00545012, (char)0);
		PatchData((char*)0x00544FCC, (char)0xEB);
	}

	if (config->getBool("Misc", "AdvancedRemoveBreedingLimit", false))
	{
		PrintDebug("Patch AdvancedRemoveBreedingLimit");
		PatchData((char*)0x5628F1, (char)24);
	}

	if (config->getBool("Easy", "EasyInstantGrowFruits", false))
	{
		PrintDebug("Patch EasyInstantGrowFruits");
		ChaoGlobal.TreeFruitAddGrowth = 2;
	}

	if (config->getBool("Easy", "EasyReincarnationRequires20Happiness", false))
	{
		PrintDebug("Patch EasyReincarnationRequires20Happiness");
		ChaoGlobal.SucceedBoundaryUserLike = 19;
	}

	if (config->getBool("Chao World Extended", "GardenMagnitude", true))
	{
		PrintDebug("Patch GardenMagnitude");
		ChaoGlobal.TimeAddGrowthAdult = 0.004166f;
		ChaoGlobal.TimeAddAttrChild = 0;
		ChaoGlobal.TimeAddAttrAdult = 0;
	}
}