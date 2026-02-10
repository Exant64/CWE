#include "stdafx.h"
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <data/al_fonames.h>

#pragma warning(push)
#pragma warning( disable: 4838 )
#include <data/chaotree.nja>
#include <data/cwe/object_common_cnk/ali_chaoseed.nja>
#include <data/al_model/al_spartoi.nja>
#pragma warning(pop)

const char* TwotoneEggs[] = {
	"Yellow twotone egg",
	"White twotone egg",
	"Brown twotone egg",
	"Sky Blue twotone egg",
	"Pink twotone egg",
	"Blue twotone egg",
	"Grey twotone egg",
	"Green twotone egg",
	"Red twotone egg",
	"Lime Green twotone egg",
	"Purple twotone egg",
	"Orange twotone egg",
	"Black twotone egg",
};
const char* ShinyTwotoneEggs[] = {
	"Shiny yellow twotone egg",
	"Shiny white twotone egg",
	"Shiny brown twotone egg",
	"Shiny sky blue twotone egg",
	"Shiny pink twotone egg",
	"Shiny blue twotone egg",
	"Shiny grey twotone egg",
	"Shiny green twotone egg",
	"Shiny red twotone egg",
	"Shiny lime green twotone egg",
	"Shiny purple twotone egg",
	"Shiny orange twotone egg",
	"Shiny black twotone egg",
};

const char* JewelEggs[] = {
	"Gold egg",
	"Silver egg",
	"Ruby egg",
	"Sapphire egg",
	"Emerald egg",
	"Amethyst egg",
	"Aquamarine egg",
	"Garnet egg",
	"Onyx egg",
	"Peridot egg",
	"Topaz egg",
	"Pearl egg",
	"Metal egg",
	"Metal egg",
	"Glass egg",
};

void ThemeUnlockFunc(int ID)
{
	if (ID)
	{
		if (ID == 1)
		{
			*(&SonicLightShoesGot + 100) = 1;
		}
		else if (ID == 2)
		{
			*(&SonicLightShoesGot + 101) = 1;
		}
	}
	else
	{
		*(&SonicLightShoesGot + 103) = 1;
	}
}

//todo: nasty
extern size_t SpartoiLensID;
extern void ALAPI_RegisterFruit(CWE_REGAPI_LEGACY* cwe_api);
extern void ALAPI_RegisterAccessory(CWE_REGAPI_LEGACY* cwe_api);
extern void ALAPI_RegisterMinimal(CWE_REGAPI_LEGACY* cwe_api);
extern void ALAPI_RegisterMotion(CWE_REGAPI_LEGACY* cwe_api);
extern void ALAPI_RegisterSpecial(CWE_REGAPI_LEGACY* cwe_api);

void ALAPI_Register(CWE_REGAPI_LEGACY* cwe_api) {
	cwe_api->RegisterSaveLoad("CWE", &cweSaveFile, sizeof(cweSaveFile));

	CWE_RegisterTexlists(cwe_api);

	for (size_t i = 0; i < LengthOfArray(TwotoneEggs); i++)
	{
		BlackMarketCategories[1].attrib[14 + i].Name = cwe_api->RegisterAlItemString(TwotoneEggs[i]);
		BlackMarketCategories[1].attrib[41 + i].Name = cwe_api->RegisterAlItemString(ShinyTwotoneEggs[i]);
	}

	for (size_t i = 0; i < LengthOfArray(JewelEggs); i++)
	{
		BlackMarketCategories[1].attrib[54 + i].Name = cwe_api->RegisterAlItemString(JewelEggs[i]);
	}

	for (size_t i = 0; i < LengthOfArray(NewFortuneTellerNames); i++)
		cwe_api->RegisterFoName((char*)NewFortuneTellerNames[i]);

	BlackMarketItemAttributes chao_seed_attrib = { 2500,500,0,-1,-1,0 };
	const CWE_API_TREE_DATA chao_tree_data = {
		.pSeedObj = &object_ali_chaoseed,

		.pSaplingObj = &treebaby,
		.pAdultObj = &tree,
		.pDeadObj = &treedead,

		.pTexlist = &CWE_OBJECT_TEXLIST,

		.ID = "cwe_chaotree",
		.Flags = 0,

		.FruitIDs = {SA2BFruit_ChaoFruit,SA2BFruit_ChaoFruit,SA2BFruit_ChaoFruit}
	};
	cwe_api->AddChaoTree(chao_tree_data, &chao_seed_attrib, "Chao Seed", "Grows a Chao Fruit tree.");
	
	//set all seeds to be rebuyable as it is in vanilla
	for (int i = 0; i < 7; i++)
		cwe_api->SetRebuyFlag(ChaoItemCategory_Seed, i, true);

	ALAPI_RegisterFruit(cwe_api);
	ALAPI_RegisterAccessory(cwe_api);
	ALAPI_RegisterMinimal(cwe_api);
	ALAPI_RegisterMotion(cwe_api);
	ALAPI_RegisterSpecial(cwe_api);

	//spartoi fix attempt, in the future we'll properly reexport lol
	//head
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->child->child->pos[0] = 0.049f;
	//left wing
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->child->pos[0] = 0.22f;
	//forehead
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->pos[0] = -0.062f;
	//left ear
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->pos[0] = 0.65f;
	//right ear
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling->pos[0] = -0.77f;
	//left horn
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->pos[0] = 0.83f;
	//right horn
	object_al_spartoi.child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->pos[0] = -0.87f;

	static const CWE_API_CHAO_DATA spartoi_data = {
		.pObject = &object_al_spartoi,
		.TextureName = "spartoi",
		.TextureCount = 5,
		.IconColor = 0xFFFFCC00,
		.IconType = ICON_TYPE_SPIKY,

		.pEvolveFunc = [](ObjectMaster* tp) {
			ChaoDataBase* pParam = GET_CHAOPARAM(tp);
		
			return pParam->Type == ChaoType_Dark_Power && pParam->EyeColor == SpartoiLensID;
		},

		.Flags = 0,
		.Name = "Spartoi",
		.ID = "cwe_spartoi",
		
	};
	cwe_api->AddChaoType(&spartoi_data);

	//todo: move these into the "setup crap" where it copies vanilla stuff, cuz only custom cwe content should be here imo
	cwe_api->RegisterOtherItemFunc(0, ThemeUnlockFunc);
	cwe_api->RegisterOtherItemFunc(1, ThemeUnlockFunc);
	cwe_api->RegisterOtherItemFunc(2, ThemeUnlockFunc);
}