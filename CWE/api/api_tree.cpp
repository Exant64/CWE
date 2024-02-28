#include "stdafx.h"
#include "SA2ModLoader.h"
#include <vector>
#include <al_marketattr.h>
#include "api_tree.h"

std::vector<NJS_TEXLIST*> ModAPI_SeedTexlists;
std::vector<NJS_OBJECT*> ModAPI_TreeModels;
std::vector<NJS_OBJECT*> ModAPI_SeedModels;

std::vector<CWE_REG_TREE_ENTRY> ModAPI_TreeEntries;

static int FruitLeafCounter = 0;

static inline Sint16 GetVertexCount(NJS_CNK_MODEL* pModel) {
	Sint16* vertexChunk = (Sint16*)pModel->vlist;
	return vertexChunk[1];
}

//just a quick util thing for doing the null checks
static inline bool NoneNullOrAll(void* p1, void* p2, void* p3) {
	if (!p1 || !p2 || !p3) {
		return !p1 && !p2 && !p3;
	}
	
	return p1 && p2 && p3;
}

static int CheckTreeVertexCount(NJS_OBJECT* pSaplingObj, NJS_OBJECT* pAdultObj, NJS_OBJECT* pDeadObj) {
	//if only one of them is null then that means it has an extra child/sibling
	bool null_check = NoneNullOrAll(pSaplingObj, pAdultObj, pDeadObj);
	if (!null_check) {
		PrintDebug("AddChaoTree: hierarchy of all 3 tree types doesn't match!");
		return 1;
	}

	//we technically only need to check for one of them because of the above check but the warning won't shut up lol
	if (!pSaplingObj || !pAdultObj || !pDeadObj) {
		return 0;
	}
	
	bool model_null_check = NoneNullOrAll(pSaplingObj->model, pAdultObj->model, pDeadObj->model);
	if (!model_null_check) {
		PrintDebug("AddChaoTree: one of the tree models has an extra chunkmodel on a node!");
		return 1;
	}

	//same thing here
	if (pSaplingObj->model) {
		Sint16 sapling_vtx_count = GetVertexCount(pSaplingObj->chunkmodel);
		Sint16 adult_vtx_count = GetVertexCount(pAdultObj->chunkmodel);
		Sint16 dead_vtx_count = GetVertexCount(pDeadObj->chunkmodel);

		if (sapling_vtx_count != adult_vtx_count || adult_vtx_count != dead_vtx_count || sapling_vtx_count != dead_vtx_count) {
			PrintDebug("AddChaoTree: one or more tree types don't have matching vertex counts!");
			return 1;
		}
	}
	else {
		FruitLeafCounter++;
	}

	if (CheckTreeVertexCount(pSaplingObj->sibling, pAdultObj->sibling, pDeadObj->sibling)) {
		return 1;
	}

	return CheckTreeVertexCount(pSaplingObj->child, pAdultObj->child, pDeadObj->child);
}

size_t AddChaoTree(const CWE_API_TREE_DATA& tree_data, BlackMarketItemAttributes* attrib, const char* name, const char* description) {
	FruitLeafCounter = 0;

	if (CheckTreeVertexCount(tree_data.pSaplingObj, tree_data.pAdultObj, tree_data.pDeadObj)) {
		PrintDebug("AddChaoTree: CheckTreeVertexCount failed");
		return -1;
	}

	if (FruitLeafCounter != 3) {
		PrintDebug("AddChaoTree: Tree has more or less than 3 fruit nodes (%d)", FruitLeafCounter);
		return -1;
	}

	ModAPI_SeedModels.push_back(tree_data.pSeedObj);
	ModAPI_SeedTexlists.push_back(tree_data.pTexlist);

	ModAPI_TreeModels.push_back(tree_data.pSaplingObj);
	ModAPI_TreeModels.push_back(tree_data.pAdultObj);
	ModAPI_TreeModels.push_back(tree_data.pDeadObj);

	CWE_REG_TREE_ENTRY entry = {
		.FruitIDs = {
			tree_data.FruitIDs[0],
			tree_data.FruitIDs[1],
			tree_data.FruitIDs[2]
		},

		.pTexlist = tree_data.pTexlist
	};

	ModAPI_TreeEntries.push_back(entry);

	BlackMarketAttributes::Get()->Add(ChaoItemCategory_Seed, attrib, name, description);

	return ModAPI_SeedModels.size();
}

void RegisterChaoTreeFruit(int treeID, int f1, int f2, int f3) {
	//ModAPI_FruitTrees[treeID].fID[0] = f1;
	//ModAPI_FruitTrees[treeID].fID[1] = f2;
	//ModAPI_FruitTrees[treeID].fID[2] = f3;
}

void AL_ModAPI_Tree_Update() {
	WriteData((int*)0x005485DF, (int)ModAPI_TreeModels.data());
	WriteData((int*)0x00548609, (int)ModAPI_TreeModels.data());
	WriteData((int*)0x00548602, (int)(&ModAPI_TreeModels[1]));

	WriteData((int*)0x54630A, (int)ModAPI_TreeModels.data());

	WriteData((int*)0x00546AD7, (int)(&ModAPI_TreeModels[1]));
	WriteData((int*)0x00546AE2, (int)ModAPI_TreeModels.data());

	WriteData((int*)0x00546BC0, (int)(&ModAPI_TreeModels[1]));
	WriteData((int*)0x0546BCD, (int)ModAPI_TreeModels.data());

	WriteData((int*)0x0054732B, (int)(&ModAPI_TreeModels[2]));
	WriteData((int*)0x00547337, (int)(&ModAPI_TreeModels[1]));

	WriteData((int*)0x00547F3C, (int)ModAPI_SeedModels.data());
}

void AL_ModAPI_Tree_Init() {
	DataArray(NJS_OBJECT*, Seeds, 0x012E537C, 7);
	DataArray(NJS_OBJECT*, TreeModels, 0x132913C, 24);

	//seed
	for (int i = 0; i < BlackMarketCategories[ChaoItemCategory_Seed].Count; i++) {
		ModAPI_SeedTexlists.push_back(&AL_OBJECT_TEXLIST);
		ModAPI_SeedModels.push_back(Seeds[i]);
	}

	//"local" tree's seed slot
	ModAPI_SeedTexlists.push_back(nullptr);
	ModAPI_SeedModels.push_back(nullptr);
	BlackMarketAttributes::Get()->Add(ChaoItemCategory_Seed, 0, 0, 0);

	//the TreeModels array only has 8 actual entries, the 9th entry would be the "local tree"/garden tree, lucky for us its 3 nullptrs which is perfect
	for (int i = 0; i < 9; i++) {
		const CWE_REG_TREE_ENTRY vanilla_entry = {
			.FruitIDs = {i + 2,  i + 2,  i + 2 },
			.pTexlist = &AL_OBJECT_TEXLIST
		};

		ModAPI_TreeEntries.push_back(vanilla_entry);

		ModAPI_TreeModels.push_back(TreeModels[i * 3 + 0]);
		ModAPI_TreeModels.push_back(TreeModels[i * 3 + 1]);
		ModAPI_TreeModels.push_back(TreeModels[i * 3 + 2]);
	}
}