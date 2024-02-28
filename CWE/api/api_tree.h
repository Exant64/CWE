#pragma once

//basically in these entry structs we try to store as much stuff that shouldn't be in separate vectors as possible
//try to only use separate vectors for stuff that needs to be updateptr'd
struct CWE_REG_TREE_ENTRY {
	int FruitIDs[3];
	NJS_TEXLIST* pTexlist;
};

extern std::vector<NJS_OBJECT*> ModAPI_TreeModels;
extern std::vector<NJS_OBJECT*> ModAPI_SeedModels;
extern std::vector<NJS_TEXLIST*> ModAPI_SeedTexlists;

extern std::vector<CWE_REG_TREE_ENTRY> ModAPI_TreeEntries;

size_t AddChaoTree(const CWE_API_TREE_DATA& tree_data, BlackMarketItemAttributes* attrib, const char* name, const char* description);

void AL_ModAPI_Tree_Init();
void AL_ModAPI_Tree_Update();