#include "stdafx.h"
#include <sa2modloader.h>
#include <vector>
#include <map>
#include <AL_ModAPI.h>
#include <al_texlist.h>
#include <ChaoMain.h>
#include <Trampoline.h>
#include "api_customchao.h"
#include <al_draw.h>
#include <alg_kinder_he.h>

//default character chao, maybe change later to some "cwe_placehold" thing
static const char* DefaultCustomChao = "cwe_spartoi";

//dynamic vector of AL_BODY texnames, we expand the AL_BODY texlist runtime
//and using the functions below, adjust the texture IDs of the model to it
static std::vector <NJS_TEXNAME> AL_BODY_TEXNAMES;

//AL_RootObject buffer to store the 'type 26' model in
static std::vector<NJS_OBJECT*> AL_RootObject;

//node counter for checking the number of nodes and the current node when traversing the char chao's hierarchy in EditChunkObjectTexture
static int NodeCounter;

//global vector for entries
std::vector<CustomChaoEntry> CustomChaoTypeEntries;

const int AL_CalcIconColorPtr = 0x0053B940;
void AL_CalcIconColor(ObjectMaster* tp)
{
	__asm
	{
		mov eax, tp
		call AL_CalcIconColorPtr
	}
}
void __cdecl AL_CalcIconColorMod(ObjectMaster* tp) {
	ChaoData1* work = CHAOWK(tp);
	ChaoDataBase* pParam = work->pParamGC;
	
	//we put the monster evo fix here because iirc CalcIconColor is called right before the deform crap
	if (gConfigVal.FixMonsterEvo) {
		DataPointer(float, col_ratio_h, 0x01DB1038);
		DataPointer(float, col_ratio_v, 0x01DB1060);
		DataPointer(float, col_div_ratio_h, 0x1DB1034);
		DataPointer(float, col_div_ratio_v, 0x1DB1030);

		//col_div_ratio fix (uses col_ratio values for v8 instead of the regular "ratio" unclamped values)
		float div = 1.0f / (col_ratio_h + col_ratio_v);
		col_div_ratio_h = col_ratio_h * div;
		col_div_ratio_v = col_ratio_v * div;
	}

	if (AL_IsCustomChao(tp)) {
		//icon color offset in chaodata1
		*(Uint32*)((int)tp->Data1.Chao + 0x700) = CustomChaoTypeEntries[work->LocalCharacterChaoType].Data.IconColor;
		return;
	}

	AL_CalcIconColor(tp);
}
static void __declspec(naked) AL_CalcIconColorHook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call AL_CalcIconColorMod

		pop eax // a1
		retn
	}
}

void __cdecl AL_IconDraw_r(ObjectMaster* a1);
Trampoline AL_IconDraw_t(0x005501A0, 0x005501A7, AL_IconDraw_r);
void __cdecl AL_IconDraw_r(ObjectMaster* tp)
{
	ChaoData1* work = CHAOWK(tp);
	ChaoDataBase* pParam = work->pParamGC;
	const auto original = reinterpret_cast<decltype(AL_IconDraw_r)*>(AL_IconDraw_t.Target());

	//if not custom chao just draw the emotion ball
	if (!AL_IsCustomChao(tp)) {
		original(tp);
		return;
	}

	CustomChaoEntry& entry = CustomChaoTypeEntries[work->LocalCharacterChaoType];
	auto typeBackup = pParam->Type;
	//NJS_TEXLIST* iconTexlist = (NJS_TEXLIST*)0x01366ACC;
	//NJS_TEXNAME* texBackup = iconTexlist->textures;

	//we trick the emotion ball drawing function to think the chao is of specified alignment
	switch (entry.Data.IconType) {
	case ICON_TYPE_BALL:
		pParam->Type = ChaoType_Neutral_Normal;
		break;
	case ICON_TYPE_HALO:
		pParam->Type = ChaoType_Hero_Normal;
		break;
	case ICON_TYPE_SPIKY:
		pParam->Type = ChaoType_Dark_Normal;
		break;
	}

	/*
	//if there's a custom AL_ICON replacement and it's actually loaded, temporarily replace the texname pointer
	if (entry.Data.IconTextureName && entry.IconTexture.textures) {
		iconTexlist->textures = entry.IconTexture.textures;
	}
	*/

	original(tp);

	//restore everything
	//iconTexlist->textures = texBackup;
	pParam->Type = typeBackup;
}

static void FillAL_RootObject() {
	NJS_OBJECT** chaoModels = (NJS_OBJECT**)GetDllData("AL_RootObject");
	for (int i = 0; i < 144; i++) {
		AL_RootObject.push_back(chaoModels[i]);
	}

	//temp space for type 26
	for (int i = 0; i < 6; i++) {
		AL_RootObject.push_back(nullptr);
	}
}

//adjusts texture IDs on the model to the AL_BODY expansion start ID
static int EditChunkModelTexture(NJS_CNK_MODEL* model, int baseTexture, int texCount) {
	// checks vertex chunk type (so that its vertex+normal)
	__int16 vertChunkType = (__int16)*(int*)model->vlist;
	if (vertChunkType != NJD_CV_VN) {
		PrintDebug("--------------------------------------ERROR-----------------------------------------------");
		PrintDebug("invalid vertex chunk type (probably using vertex colors), cancelling custom chao register!");
		PrintDebug("------------------------------------------------------------------------------------------");
		return 1;
	}

	Sint16* pbuf = model->plist;
	Sint16* foundChunk = FindChunk(pbuf, NJD_CT_TID);
	int texChunkCount = 0;
	while (foundChunk != nullptr)
	{
		//if texCount out of bounds
		if ((foundChunk[1] & 0x1FFF) >= texCount) {
			PrintDebug("------------------------------ERROR------------------------------------");
			PrintDebug("texture index is out of bounds (%d), cancelling custom chao register!", (foundChunk[1] & 0x1FFF));
			PrintDebug("-----------------------------------------------------------------------");

			return 1;
		}

		texChunkCount++;
		foundChunk[1] = (unsigned short)((foundChunk[1] & ~0x1FFF) | min(baseTexture + (foundChunk[1] & 0x1FFF), (unsigned short)0x1FFF));
		foundChunk = FindChunk(foundChunk, NJD_CT_TID);
	}

	if (NodeCounter == 27 && texChunkCount != 2) {
		PrintDebug("---------------------------------ERROR---------------------------------------");
		PrintDebug("Mouth (node 27) doesn't have 2 materials, cancelling custom chao register!");
		PrintDebug("-----------------------------------------------------------------------------");

		return 1;
	}

	return 0;
}

static int EditChunkObjectTexture(NJS_OBJECT* obj, int baseTexture, int texCount)
{
	do
	{
		if (obj->chunkmodel) {
			if (EditChunkModelTexture(obj->chunkmodel, baseTexture, texCount)) {
				//if there's an error, return with error (wow what an insight)
				return 1;
			}
		}
		NodeCounter++;

		if (obj->child) {
			if (EditChunkObjectTexture(obj->child, baseTexture, texCount)) {
				return 1;
			}
		}
		obj = obj->sibling;
	} while (obj != nullptr);

	//no errors
	return 0;
}

size_t AddChaoType(CWE_API_CHAO_DATA const* pData) {
	size_t startIndex = AL_BODY_TEXNAMES.size();
	size_t newChaoType = (AL_RootObject.size() / 6) + 2;

	if (!pData->Name) {
		PrintDebug("Custom Chao's name is null!");
		return -1;
	}

	char name_buffer[sizeof(pData->ID)];
	strncpy(name_buffer, pData->ID, sizeof(pData->ID));
	name_buffer[sizeof(pData->ID) - 1] = '\0';

	if (strlen(name_buffer) == 0) {	
		PrintDebug("Custom Chao ID is an empty string, please set a proper one!");
		return -1;
	}

	//check if ID is already used
	for (const auto& entry : CustomChaoTypeEntries) {
		if (strcmp(pData->ID, entry.Data.ID) == 0) {
			PrintDebug("------------------ERROR-------------------");
			PrintDebug("Custom Chao ID %s is already used by %s", pData->ID, entry.Data.Name);
			PrintDebug("------------------------------------------");
			return -1;
		}
	}

	if (pData->TextureCount <= 0) {
		PrintDebug("-------------------------------ERROR---------------------------------------");
		PrintDebug("Custom Chao ID %s has negative (or zero) texture count, cancelling register", pData->ID);
		PrintDebug("---------------------------------------------------------------------------");
	}

	for (int i = 0; i < pData->TextureCount; i++) {
		AL_BODY_TEXNAMES.push_back({});
	}
	
	NodeCounter = AL_PART_ROOT;
	if (EditChunkObjectTexture(pData->pObject, startIndex, pData->TextureCount)) {
		return -1;
	}

	if (NodeCounter != AL_PART_END) {
		PrintDebug("-------------------------------ERROR---------------------------------------");
		PrintDebug("The number of nodes is not 40 (it's %d), cancelling character chao register!", NodeCounter);
		PrintDebug("---------------------------------------------------------------------------");
		return -1;
	}

	CustomChaoEntry entry = {
		.StartIndex = startIndex,
		.Data = *pData,
	};

	strcpy(entry.Data.ID, name_buffer);

	CustomChaoTypeEntries.push_back(entry);

	return newChaoType;
}

void AL_ModAPI_CharacterChao_Update() {
	AL_BODY.textures = AL_BODY_TEXNAMES.data();

	for (size_t i = 0; i < CustomChaoTypeEntries.size(); i++) {
		CustomChaoEntry& entry = CustomChaoTypeEntries[i];
		NJS_TEXLIST& tex = entry.BodyTexture;

		tex.nbTexture = entry.Data.TextureCount;
		tex.textures = AL_BODY_TEXNAMES.data() + entry.StartIndex;

		cweAPI.RegisterChaoTexlistLoad(entry.Data.TextureName, &tex);
	}

	g_HelperFunctions->HookExport("AL_RootObject", AL_RootObject.data());
}

static int AL_CustomChao_SearchID(const char* pID) {
	for (size_t i = 0; i < CustomChaoTypeEntries.size(); i++) {
		if (strcmp(pID, CustomChaoTypeEntries[i].Data.ID) == 0) {
			return i;
		}
	}

	return -1;
}

static int __cdecl AL_ShapeInit_r(ObjectMaster* tp);
static Trampoline AL_ShapeInit_Tramp(0x0056C9D0, 0x0056C9D7, AL_ShapeInit_r);
static int __cdecl AL_ShapeInit_r(ObjectMaster* tp) {
	ChaoData1* work = CHAOWK(tp);
	ChaoDataBase* pParam = work->pParamGC;

	work->IsCustomChaoTypeLoaded = false;

	if (AL_IsCustomChao(tp)) {
		int id = AL_CustomChao_SearchID(pParam->TypeID);
		if (id == -1) {
			PrintDebug("AL_ShapeInit_r: couldn't find type %s, using %s as default", pParam->TypeID, DefaultCustomChao);
			id = AL_CustomChao_SearchID(DefaultCustomChao);
		}

		work->LocalCharacterChaoType = id;
		work->IsCustomChaoTypeLoaded = true;

		const auto& entry = CustomChaoTypeEntries[work->LocalCharacterChaoType];

		//in our AL_RootObject vector we have a reserved space for type 26 that the original AL_ShapeInit will load here
		for (size_t i = 144; i < 150; i++) {
			AL_RootObject[i] = entry.Data.pObject;
		}

		if (entry.Data.Flags & CUSTOM_CHAO_FLAG_SECOND_EVO) {
			for (size_t i = 145; i < 150; i++) {
				AL_RootObject[i] = entry.Data.pSecondEvoList[i - 145];
			}
		}
	}

	auto original = reinterpret_cast<decltype(AL_ShapeInit_r)*>(AL_ShapeInit_Tramp.Target());
	return original(tp);
}

void __cdecl sub_58D9F0(char a1, HealthCenter* a2, float a3, float a4, float a5, float a6, float a7) {
	ChaoData1* work = CHAOWK(a2->field_8);
	ChaoDataBase* pParam = work->pParamGC;
	int msg = (int)a3;

	if (pParam->Type < ChaoType_Tails) {
		DrawMedicalChartText((char*)&a2->dword60[*(int*)&a2->dword60[4 * (int)a3]], a4, a5, a6, a7, a1);
		return;
	}

	const char* str = "Error";

	switch (pParam->Type) {
	case ChaoType_Tails:
		str = "Tails";
		break;
	case ChaoType_Knuckles:
		str = "Knuckles";
		break;
	case ChaoType_Amy:
		str = "Amy";
		break;
	case 26:
		str = CustomChaoTypeEntries[work->LocalCharacterChaoType].Data.Name;
		break;
	}

	DrawMedicalChartText(str, a4, a5, a6, a7, a1);
}

static void __declspec(naked) sub_58D9F0Hook()
{
	__asm
	{
		push[esp + 14h] // a7
		push[esp + 14h] // a6
		push[esp + 14h] // a5
		push[esp + 14h] // a4
		push[esp + 14h] // a3
		push eax // a2
		push ecx // a1

		// Call your __cdecl function here:
		call sub_58D9F0

		pop ecx // a1
		pop eax // a2
		add esp, 4 // a3
		add esp, 4 // a4
		add esp, 4 // a5
		add esp, 4 // a6
		add esp, 4 // a7
		retn
	}
}

int __cdecl sub_5366E0(ObjectMaster* a1, int a2)
{
	ChaoDataBase* v2 = a1->Data1.Chao->pParamGC;
	switch (v2->Type)
	{
	case ChaoType_Empty:
	case ChaoType_Good:
	case ChaoType_Bad:
		return -1;
	case ChaoType_Egg:
		return 0;
	case ChaoType_Child:
		return 1;
	case ChaoType_Neutral_Chaos:
	case ChaoType_Hero_Chaos:
	case ChaoType_Dark_Chaos:
		return 7;
	default:
		if (v2->Type >= ChaoType_Tails) 
			return 7;

		return ((unsigned __int8)v2->Type - 5) / 3 + 2;
	}
}

static void __declspec(naked) sub_5366E0Hook()
{
	__asm
	{
		push ecx // a2
		push eax // a1

		// Call your __cdecl function here:
		call sub_5366E0

		add esp, 4 // a1<eax> is also used for return value
		pop ecx // a2
		retn
	}
}

static void AL_ChaoParamWindowExecutorDisplay_r(ObjectMaster* tp);
static Trampoline AL_ChaoParamWindowExecutorDisplay_t(0x005928A0, 0x005928A8, AL_ChaoParamWindowExecutorDisplay_r);
DataArray(ChaoHudThingB, AL_ChaoParamWindow_HudThingB, 0x011D1658, 1);
static void AL_ChaoParamWindowExecutorDisplay_r(ObjectMaster* tp) {
#pragma pack(push, 8)
	struct __declspec(align(16)) AL_ChaoParamWindowExecutor_Data
	{
		char Action;
		char pad1[3];
		char byte4;
		char pad5;
		short word6;
		int posX;
		int posY;
		float alpha;
		float float14;
		int field_18;
		int field_1C;
		int dword20;
		int dword24;
		int field_28;
		int field_2C;
		int dword30;
		int dword34;
		ObjectMaster* PointerToChao;
		ChaoData* ChaoData;
	};
#pragma pack(pop)
	enum
	{
		BBI_CPW_WINDOW_LU = 0x0,
		BBI_CPW_WINDOW_CU = 0x1,
		BBI_CPW_WINDOW_RU = 0x2,
		BBI_CPW_WINDOW_LM = 0x3,
		BBI_CPW_WINDOW_CM = 0x4,
		BBI_CPW_WINDOW_RM = 0x5,
		BBI_CPW_WINDOW_LB = 0x6,
		BBI_CPW_WINDOW_CB = 0x7,
		BBI_CPW_WINDOW_RB = 0x8,
		BBI_CPW_SWIM = 0x9,
		BBI_CPW_FLY = 0xA,
		BBI_CPW_RUN = 0xB,
		BBI_CPW_POWER = 0xC,
		BBI_CPW_STAMINA = 0xD,
		BBI_CPW_0 = 0xE,
		BBI_CPW_1 = 0xF,
		BBI_CPW_2 = 0x10,
		BBI_CPW_3 = 0x11,
		BBI_CPW_4 = 0x12,
		BBI_CPW_5 = 0x13,
		BBI_CPW_6 = 0x14,
		BBI_CPW_7 = 0x15,
		BBI_CPW_8 = 0x16,
		BBI_CPW_9 = 0x17,
		BBI_CPW_LV = 0x18,
		BBI_CPW_GAUGE_L = 0x19,
		BBI_CPW_GAUGE_ORANGE = 0x1A,
		BBI_CPW_GAUGE_GRAY = 0x1B,
		BBI_CPW_GAUGE_R = 0x1C,
		BBI_CPW_KAKKO_L = 0x1D,
		BBI_CPW_KAKKO_R = 0x1E,
		BBI_CPW_TYPE_NORMAL = 0x1F,
		BBI_CPW_TYPE_SWIM = 0x20,
		BBI_CPW_TYPE_FLY = 0x21,
		BBI_CPW_TYPE_RUN = 0x22,
		BBI_CPW_TYPE_POWER = 0x23,
		BBI_CPW_TYPE_CHILD = 0x24,
		BBI_CPW_TYPE_CHAOS = 0x25,
		BBI_CPW_ATTR_NEUT = 0x26,
		BBI_CPW_ATTR_HERO = 0x27,
		BBI_CPW_ATTR_DARK = 0x28,
		NB_BBI_CPW = 0x29,
	};

	//trampoline
	auto original = reinterpret_cast<decltype(AL_ChaoParamWindowExecutorDisplay_r)*>(AL_ChaoParamWindowExecutorDisplay_t.Target());
	original(tp);

	auto* pData = (AL_ChaoParamWindowExecutor_Data*)tp->Data2.Undefined;
	if (pData->Action) {
		char v2 = pData->byte4;
		float posX = (float)pData->posX;
		float posY = (float)pData->posY;
		float v37 = (float)(24 * (v2 / 2));
		float v40 = (float)(12 * (v2 % 2));
		task* v5 = pData->PointerToChao;
		int v8;

		if (v5)
		{
			*(int*)0x1AED260 = 0;
			ChaoDataBase* pParam = GET_CHAOPARAM(v5);
			SetChaoHUDThingBColor(pData->alpha, 0.5f, 0.85f, 1.0f);

			ChaoHudThingB custom_type = {
				.adjust = 1,
				.wd = 119 * 0.5f,
				.ht = 41 * 0.5f,
				.s0 = 0,
				.t0 = 0,
				.s1 = 1,
				.t1 = 1,
				.pTexlist = &CWE_UI_TEXLIST,
				.TexNum = 33
			};

			switch (pParam->Type)
			{
			case ChaoType_Child:
				v8 = BBI_CPW_TYPE_CHILD;
				break;
			case ChaoType_Neutral_Chaos:
			case ChaoType_Hero_Chaos:
			case ChaoType_Dark_Chaos:
				v8 = BBI_CPW_TYPE_CHAOS;
				break;
			case ChaoType_Tails:
			case ChaoType_Knuckles:
			case ChaoType_Amy:
				v8 = -1;
				break;
			case 26:
				//custom sprite drawn here
				DrawChaoHudThingB(&custom_type, posX - 96, posY - 89, -34.5f, 1, 1, -1, -1);
				SetChaoHUDThingBColor(1.0f, 1.0f, 1.0f, 1.0f);
				return;
			default:
				v8 = ((unsigned __int8)pParam->Type - 5) / 3 + BBI_CPW_TYPE_NORMAL;
				break;
			}

			if (v8 >= 0)
			{
				float v45 = posX - 96;
				float v44 = posY - 72;
				
				DrawChaoHudThingB(&AL_ChaoParamWindow_HudThingB[v8], v45, v44, -34.5f, 1, 1, -1, -1);
			}
			
			if (pParam->Type != ChaoType_Child
				&& ((unsigned __int8)pParam->Type <= ChaoType_Dark_Chaos || (unsigned __int8)pParam->Type > ChaoType_Amy))
			{
				float v43 = posX - 96;
				float v42 = posY - 89;
				DrawChaoHudThingB(
					&AL_ChaoParamWindow_HudThingB[BBI_CPW_ATTR_NEUT + ((unsigned __int8)pParam->Type - 5) % 3],
					v43,
					v42,
					-35,
					1,
					1,
					-1,
					-1);
			}
		}
	}

	//back to default
	SetChaoHUDThingBColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void AL_ModAPI_CharacterChao_Init() {
	AL_BODY_TEXNAMES.resize(AL_BODY.nbTexture);

	//chao select menu type limit (anything above character chao was blocked out)
	WriteData((char*)0x00531249, (char)25);
	WriteData((char*)0x00531275, (char)25);
	WriteData((char*)0x005312A5, (char)25);
	WriteData((char*)0x005312D5, (char)25);
	WriteData((char*)0x00531305, (char)25);
	WriteData((char*)0x00531335, (char)25);

	WriteData((char*)0x00531377, (char)25);
	WriteData((char*)0x005313A4, (char)25);
	WriteData((char*)0x005313D5, (char)25);
	WriteData((char*)0x00531406, (char)25);
	WriteData((char*)0x00531437, (char)25);
	WriteData((char*)0x00531468, (char)25);

	WriteData((char*)0x00531049, (char)24);
	WriteData((char*)0x0053107D, (char)24);
	WriteData((char*)0x005310B8, (char)24);
	WriteData((char*)0x005310F3, (char)24);

	WriteData((char*)0x00531149, (char)24);
	WriteData((char*)0x0053117E, (char)24);
	WriteData((char*)0x005311BA, (char)24);
	WriteData((char*)0x005311F6, (char)24);

	//we hook the function that returns the chaotype message index thing
	//but we also return 7 for character chao so the code enters the switch statement
	//where the text render happens, which we hook below to overwrite the text
	WriteCall((void*)0x0058DDF4, sub_5366E0Hook);
	//and here we hook said text rendering,
	WriteCall((void*)0x0058DE84, sub_58D9F0Hook);
	
	//killing type check memset
	WriteCall((void*)0x052B35D, nullsub_1);

	//killing the chao type draw calls in AL_ChaoParamWindowExecutor_Display so we can do our own thing in the trampoline above
	WriteCall((void*)0x00592DDA, nullsub_1);
	WriteCall((void*)0x00592E74, nullsub_1);

	//jiggle buffer expansion to chunk model vertex limit
	int mallocExpand = (int)malloc(32768 * sizeof(NJS_VECTOR));
	WriteData((int*)0x0056F8E0, mallocExpand);
	WriteData((int*)0x0056F932, mallocExpand + 8);

	FillAL_RootObject();

	//emotion ball character chao check (assume all chao types starting from tails are character chao)
	WriteData((Uint8*)0x0053D331, (Uint8)0xFF);
	//AL_ShapeChangeType type >= 26 check increased
	WriteData((Uint8*)0x0056CF4F, (Uint8)0xFF); 
	//custom emotion ball colors
	WriteCall((void*)0x005A4D22, AL_CalcIconColorHook);
	WriteCall((void*)0x005A4D60, AL_CalcIconColorHook);
}