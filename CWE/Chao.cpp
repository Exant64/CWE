#include "stdafx.h"

#include "ninja_functions.h"
#include "ChaoMain.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "al_piano.h"
#include "al_parts.h"
#include "AL_ModAPI.h"
#include "al_parameter.h"

#include "Trampoline.h"

#include <ctime>
#include "al_console.h"
#include "Chao.h"
#include "alo_obakehead.h"
#include "alg_kinder_he.h"
#include "al_world.h"
#include "al_social.h"

#include "al_behavior/albhv.h"
#include "al_behavior/alsbhv.h"
#include "al_behavior/al_intention.h"

#include "al_palette.h"

#include "define.h"
#include "al_tv.h"
#include "al_draw.h"

#include "al_face.h"
#include "al_animation.h"

#include "al_look.h"
#include "al_modelcontainer.h"
#include "al_parts.h"
#include "al_name.h"
#include "c_colli.h"

#include "define.h"
#include "al_gene.h"
#include "al_behavior/al_behavior.h"
#include "al_shape.h"
#include "code_system/code_manager.h"
#include <api/api_metadata.h>

const int AL_IconSetPtr = 0x53D660;
void AL_IconSet(ObjectMaster* a4, char a2, int a3)
{
	__asm 
	{
		mov ecx, a4
		mov dl, a2
		push a3
		call AL_IconSetPtr
		add esp, 4

	}
}

const int sub_534F80Ptr = 0x534F80;
void sub_534F80alt(int a1, NJS_VECTOR* a2, signed int a3)
{
	__asm
	{
		mov eax, a1
		mov edi, a2
		push a3
		call sub_534F80Ptr
		add esp, 4
	}
}
void AL_GetRandomAttrPos_0(ObjectMaster* a1)
{
	if (stru_1A15938[1].nbIndex > 0)
	{
		sub_534F80alt((int)&stru_1A15938[1], &a1->EntityData2->Waypoint, stru_1A15938[1].nbIndex);
	}
}
const int sub_55A920Ptr = 0x55A920;
ObjectMaster* sub_55A920(int a1, NJS_VECTOR* a2, ObjectMaster* parent, Uint8 a4, float a5)
{
	__asm
	{
		push a5
		push dword ptr[a4]
		push parent
		mov edi, a2
		mov ebx, a1
		call sub_55A920Ptr
		add esp, 12
	}
}
void ALOField_Load(ObjectMaster* a1, Uint8 a2, NJS_VECTOR* a3, float a4, int timer)
{
	sub_55A920(timer, a3, a1, a2, a4);
}

#pragma pack(push, 8)
struct __declspec(align(16)) KarateMainExecData
{
	char gap0;
	char field_1;
	char field_2;
	char field_3;
	char field_4;
	char byte5;
	char byte6;
	char Round;
	char opponent;
	char field_9;
	char MenuSelection;
	char field_B;
	ObjectMaster* playerChao;
	ObjectMaster* pointerToSaveThing;
	ObjectMaster* PointerToOtherSaveThing;
	ChaoData* field_18;
	ChaoData* field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	float field_30;
	int field_34;
	int field_38;
	ObjectMaster* field_3C;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(8)) KarateChaoExec_Data2
{
	__int16 char0;
	short word2;
	short flag;
	short word6;
	ObjectMaster* pointerToChao;
	void* dwordC;
	ChaoData* chaoDataPointer;
	int field_14;
	NJS_VECTOR* BaseTranslationPos;
	NJS_VECTOR* HeadTranslationPos;
	float SwimStat;
	float FlyStat;
	float RunStat;
	float PowerStat;
	int field_30;
	float field_34;
	float zeal;
	float StaminaStat;
	float LuckStat;
	float float44;
};
#pragma pack(pop)

DataPointer(KarateMainExecData*, KarateMainExec_Ptr, 0x01A5D148);
ObjectMaster* KarateCreateChao(ChaoData* chaoData, int a2, KarateOpponent* a3, NJS_VECTOR* position, Angle angle)
{
	KarateOpponent* opponent = a3;

	if (opponent->StatPoints[7] == 1)
	{
		ObjectMaster* player = KarateMainExec_Ptr->pointerToSaveThing;
		KarateChaoExec_Data2* data2 = (KarateChaoExec_Data2*)player->Data2.Undefined;
		opponent->Alignment = (short)(data2->chaoDataPointer->data.Alignment * 10000.0f);
		opponent->Magnitude = (short)(data2->chaoDataPointer->data.EvolutionProgress * 10000.0f);
		opponent->FlySwim = (short)(data2->chaoDataPointer->data.FlySwim * 10000.0f);
		opponent->PowerRun = (short)(data2->chaoDataPointer->data.PowerRun * 10000.0f);
		opponent->ChaoType = data2->chaoDataPointer->data.Type;
		for (int i = 0; i < 7; i++)
		{
			if (data2->chaoDataPointer->data.StatPoints[i] >= 1000)
				opponent->StatPoints[i] = data2->chaoDataPointer->data.StatPoints[i] - 200;
			else opponent->StatPoints[i] = data2->chaoDataPointer->data.StatPoints[i];
		}
	}

	ObjectMaster* chao = CreateChao(chaoData, a2, opponent, position, angle);
	ChaoDataBase* fullData = chao->Data1.Chao->pParamGC;

	for (int i = 0; i < 7; i++)
		fullData->partsDX.MinimalParts[i] = opponent->Name[i];
	fullData->partsDX.MinimalParts[7] = SADXAnimal_Seal;

	if (opponent->Shiny == 3)
	{
		fullData->Shiny = 1;
		fullData->ShinyJewelMonotone = 1;
	}

	if (opponent->f13 == 0)
	{
		fullData->EyeAlignment = 0;
		fullData->EyeColor = 0;
	}
	else if (opponent->f13 >= 1 && opponent->f13 <= 6)
	{
		fullData->EyeAlignment = 1;
		fullData->EyeColor = opponent->f13 - 1;
	}
	else if (opponent->f13 >= 7 && opponent->f13 <= 12)
	{
		fullData->EyeAlignment = 2;
		fullData->EyeColor = opponent->f13 - 7;
	}
	else if (opponent->f13 >= 13 && opponent->f13 <= 18)
	{
		fullData->EyeAlignment = 3;
		fullData->EyeColor = opponent->f13 - 13;
	}
	else if (opponent->f13 >= 20 && opponent->f13 <= 22)
	{
		fullData->EyeAlignment = opponent->f13 - 20;
		fullData->EyeColor = 7;
	}
	else if (opponent->f13 >= 23 && opponent->f13 <= 25)
	{
		fullData->EyeAlignment = (opponent->f13 - 23) + 1;
		fullData->EyeColor = 8;
	}
	sub_566B80(chao);
	return chao;
}

void sub_54A690(ObjectMaster* a1)
{
	Data1Ptr v1; // eax
	CollisionInfo* v2; // eax

	if (a1 && (v1.Undefined = (EntityData1*)a1->Data1.Entity) != 0 && (v2 = v1.Entity->Collision) != 0)
	{
		v2->CollisionArray[1].field_2 |= 0x70u;
	}
}
void sub_54A730(ObjectMaster* tp)
{
	tp->Data1.Entity->Status &= ~0x240u;
	CCL_Disable(tp, 0);
	CCL_Disable(tp, 2);
}

std::random_device rd;
std::mt19937 e{ rd() }; // or std::default_random_engine e{rd()};
std::uniform_int_distribution<Uint32> dist{ 0, UINT32_MAX };

Uint32 __cdecl GenerateRandomSeed()
{
	return dist(e);
}

static void Chao_Main_r(ObjectMaster* a1);
static Trampoline Chao_Main_Tramp(0x0054FE20, 0x0054FE28, Chao_Main_r);
static void __cdecl Chao_Main_r(ObjectMaster* a1)
{
	chaowk* work = GET_CHAOWK(a1);
	ChaoDataBase* pParam = a1->Data1.Chao->pParamGC;
	if (!pParam) return;

	//some timer
	if (a1->Data1.Chao->gap_30 <= 2u)
	{
		if (pParam->field_19 == 2)
			pParam->field_19 = 0;

		//upgrade to 9.4
		if (pParam->IsInitializedAccessory == 0)
		{
			for (int i = 0; i < 4; i++)
				pParam->Accessories_[i] = pParam->Accessories_old[i];
			pParam->IsInitializedAccessory = 1;
		}

		if (!(pParam->Flags & AL_PARAM_FLAG_ACCESSORIES_NEW)) {
			for (size_t i = 0; i < _countof(pParam->Accessories_); ++i) {
				memset(&pParam->Accessories[i], 0, sizeof(pParam->Accessories[i]));

				char id[METADATA_ID_SIZE];
				bool foundID = ItemMetadata::Get()->GetID(ChaoItemCategory_Accessory, pParam->Accessories_[i] - 1, id);
				if (!foundID) {
					// TODO: error
					continue;
				}

				strcpy_s(pParam->Accessories[i].ID, id);
			}

			pParam->Flags |= AL_PARAM_FLAG_ACCESSORIES_NEW;
		}

		if (!(pParam->Flags & AL_PARAM_FLAG_NAME_NEW))
		{
			AL_GENE& Gene = pParam->Gene;

			memcpy(pParam->Name, pParam->Name_, sizeof(pParam->Name_));
			memcpy(pParam->MotherName, Gene.MotherName, sizeof(Gene.MotherName));
			memcpy(pParam->FatherName, Gene.FatherName, sizeof(Gene.FatherName));
			memcpy(pParam->MGroundMotherName, Gene.MGroundMotherName, sizeof(Gene.MGroundMotherName));
			memcpy(pParam->MGroundFatherName, Gene.MGroundFatherName, sizeof(Gene.MGroundFatherName));
			memcpy(pParam->FGroundMotherName, Gene.FGroundMotherName, sizeof(Gene.FGroundMotherName));
			memcpy(pParam->FGroundFatherName, Gene.FGroundFatherName, sizeof(Gene.FGroundFatherName));

			pParam->Flags |= AL_PARAM_FLAG_NAME_NEW;
		}


		if (!pParam->Birthday)
		{
			pParam->Birthday = 60 + (int)(njRandom() * 30); //randrange(60,90)
		}

		if (!pParam->ChaoID.id[0]) //if doesnt have UID
		{
			//give em one
			pParam->ChaoID.id[0] = GenerateRandomSeed();
		}
	}

	// new accessory stuff
	for (size_t i = 0; i < _countof(pParam->Accessories); ++i) {
		auto& data = pParam->Accessories[i];

		// if it's an empty string, set no index, check next accessory
		if (!data.ID[0]) {
			work->AccessoryCalculatedID[i][0] = 0;
			work->AccessoryIndices[i] = -1;
			continue;
		}

		// first off, check if the id changed since spawn
		// we wouldn't need to handle this but KCE is a thing
		// if there was something non-empty set already and it didn't change then skip
		if (work->AccessoryCalculatedID[i][0] && !strcmp(data.ID, work->AccessoryCalculatedID[i])) {
			continue;
		}

		strcpy_s(work->AccessoryCalculatedID[i], data.ID);

		const size_t index = ItemMetadata::Get()->GetIndex(ChaoItemCategory_Accessory, data.ID);
		if (index == -1) {
			// error object (should be the last registered i think)
			work->AccessoryIndices[i] = ObjectRegistry::Get(ChaoItemCategory_Accessory)->Size() - 1;
		}
		else {
			work->AccessoryIndices[i] = index;
		}
	}

	*(int*)pParam->Name_ = *(int*)pParam->Name;
	*(short*)(&pParam->Name_[4]) = *(short*)(&pParam->Name[4]);
	pParam->Name_[6] = pParam->Name[6];

	if (!(pParam->Flags & AL_PARAM_FLAG_OLD_GUEST_CHECK)) {
		if ((Uint8)pParam->Name[6] == 0xFB) {
			pParam->field_19 = 1;
			pParam->Name[6] = 0;
		}

		pParam->Flags |= AL_PARAM_FLAG_OLD_GUEST_CHECK;
	}

	//these are ported xml codes
	//immortality
	if (*(char*)0x0053FD6C == 1) //new challenge enabled check
	{
		if (pParam->Medal == 8)
		{
			if (pParam->Lifespan < 1000) pParam->Lifespan = 1000;
			if (pParam->Lifespan2 < 1000) pParam->Lifespan2 = 1000;
		}
	}

	//new power climbing
	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_CLIMB && a1->Data1.Chao->Behavior.SubTimer == 240)
	{
		if (pParam->StatPoints[3] >= 150) //USE GETSTAT THING LATER
		{
			a1->Data1.Chao->Behavior.SubTimer = 0;
		}
	}

	//monster transparency fix
	if (pParam->FlySwim < -1 || pParam->FlySwim > 1 ||
		pParam->PowerRun < -1 || pParam->PowerRun > 1)
	{
		if (a1->Data1.Chao->Face.pMouthObject && a1->Data1.Chao->Face.pMouthObject->chunkmodel)
		{
			__int32* color = (__int32*)a1->Data1.Chao->Face.pMouthObject->chunkmodel->plist;
			color[1] = 0xFFFFFFFF;
		}
	}

	if (gConfigVal.MoreAnimation)
		Chao_ExtraAnimations(a1);

	if (gConfigVal.MoreSound)
		Chao_ExtraSounds(a1);

	for (auto& c : CodeManager::Instance()) {
		c->ChaoObject(a1);
	}

	//trampoline
	auto original = reinterpret_cast<decltype(Chao_Main_r)*>(Chao_Main_Tramp.Target());
	original(a1);
}

const int ALO_RakugakiExecutor_LoadPtr = 0x05AB200;
void ALO_RakugakiExecutor_Load_(int a1, char a2, int a3)
{
	__asm
	{
		mov ebx, a1
		push a3
		push dword ptr [a2]
		call ALO_RakugakiExecutor_LoadPtr
		add esp, 8 
	}
}

void __cdecl NewDrawings(int a1, char a2, int a3)
{
	char ID = a2;
	if (ID >= 13 && ID <= 15 && njRandom() > 0.5f)
		ID += 3;
	ALO_RakugakiExecutor_Load_(a1, ID, a3);
}

static void __declspec(naked) NewDrawingsHook()
{
	__asm
	{
		push[esp + 08h] // a3
		push[esp + 08h] // a2
		push ebx // a1

		// Call your __cdecl function here:
		call NewDrawings

		pop ebx // a1
		add esp, 4 // a2
		add esp, 4 // a3
		retn
	}
}

void Chao_Init()
{	
	static_assert(offsetof(ChaoDataBase, XGradeValue) == 0x59e, "ChaoDataBase: XGradeValue not in right location");

	static_assert(offsetof(ChaoData1, pParamGC) == 0x5C, "ChaoData1:pParamGC got shuffled");
	static_assert(offsetof(ChaoData1, MotionTable) == 0x0AC, "ChaoData1:MotionTable got shuffled");
	static_assert(offsetof(ChaoData1, Behavior) == 0x1B4, "ChaoData1:Behavior got shuffled");
	static_assert(offsetof(ChaoData1, field_510) == 0x510, "ChaoData1:field_510 (AL_SHAPE) got shuffled");
	static_assert(offsetof(ChaoData1, Face) == 0x68C, "ChaoData1:Face got shuffled");
	static_assert(offsetof(ChaoData1, gap6EC) == 0x6EC, "ChaoData1:gap6EC (AL_ICON) got shuffled");
	static_assert(offsetof(ChaoData1, ObjectListInfo) == 0x7CC, "ChaoData1:ObjectListInfo got shuffled");
	static_assert(offsetof(ChaoData1, LocalCharacterChaoType) == 0x1D74, "ChaoData1: new fields dont start at vanilla size");
	static_assert(sizeof(ChaoData1) >= 0x1D74, "ChaoData1 size lower than vanilla, something's up");

	AL_Name_Init();

	//expand ChaoData1 alloc
	WriteData((int*)0x00550288, (int)sizeof(ChaoData1) + 4);
	WriteData((int*)0x00550294, (int)sizeof(ChaoData1));
	//memalloc = sizeof(ChaoData)
	WriteData((short*)0x55040E, (short)0x200);
	WriteData((short*)0x5503DD, (short)0x804);
	WriteData((short*)0x5503E7, (short)0x800);
	WriteData((short*)0x55043E, (short)0x804);
	WriteData((short*)0x550448, (short)0x800);

	AL_Gene_Init();

	AL_FaceInit();

	//new drawings
	WriteCall((void*)0x005AB3D2, NewDrawingsHook);
	WriteData((int*)0x5AB091, (int)&AL_DRAWING_TEXLIST);
	WriteData((int*)0x5AB0D8, (int)&AL_DRAWING_TEXLIST);

	AL_Behavior_Init();

	AL_Draw_Init();

	AL_Shape_Init();

	//minda karate chao
	WriteCall((void*)0x0057902D, KarateCreateChao);

	AL_Parts_Init();
}
