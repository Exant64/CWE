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
#include "Chao.h"
#include "alo_obakehead.h"
#include "alg_kinder_he.h"
#include "alg_karate_main.h"
#include "al_world.h"
#include "al_social.h"

#include "al_behavior/albhv.h"
#include "al_behavior/alsbhv.h"
#include "al_behavior/al_intention.h"

#include "al_palette.h"

#include "define.h"
#include "al_draw.h"

#include "al_face.h"
#include "al_animation.h"

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
#include "FunctionHook.h"

const int AL_IconSetPtr = 0x53D660;
void AL_IconSet(task* a4, char a2, int a3)
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
void AL_GetRandomAttrPos_0(task* a1)
{
	if (stru_1A15938[1].nbIndex > 0)
	{
		sub_534F80alt((int)&stru_1A15938[1], &a1->EntityData2->AimPos, stru_1A15938[1].nbIndex);
	}
}
const int sub_55A920Ptr = 0x55A920;
task* sub_55A920(int a1, NJS_VECTOR* a2, task* parent, Uint8 a4, float a5)
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
void ALOField_Load(task* a1, Uint8 a2, NJS_VECTOR* a3, float a4, int timer)
{
	sub_55A920(timer, a3, a1, a2, a4);
}

task* KarateCreateChao(CHAO_SAVE_INFO* chaoData, int a2, KarateOpponent* a3, NJS_VECTOR* position, Angle angle)
{
	KarateOpponent* opponent = a3;

	if (opponent->StatPoints[7] == 1)
	{
		task* player = KarateMainExec_Ptr->pointerToSaveThing;
		KarateChaoExec_Data2* data2 = (KarateChaoExec_Data2*)player->Data2.Undefined;
		opponent->Alignment = (short)(data2->chaoDataPointer->data.Alignment * 10000.0f);
		opponent->Magnitude = (short)(data2->chaoDataPointer->data.EvolutionProgress * 10000.0f);
		opponent->FlySwim = (short)(data2->chaoDataPointer->data.FlySwim * 10000.0f);
		opponent->PowerRun = (short)(data2->chaoDataPointer->data.PowerRun * 10000.0f);
		opponent->ChaoType = data2->chaoDataPointer->data.type;
		for (int i = 0; i < 7; i++)
		{
			if (data2->chaoDataPointer->data.Skill[i] >= 1000)
				opponent->StatPoints[i] = data2->chaoDataPointer->data.Skill[i] - 200;
			else opponent->StatPoints[i] = data2->chaoDataPointer->data.Skill[i];
		}
	}

	task* chao = CreateChao(chaoData, a2, opponent, position, angle);
	CHAO_PARAM_GC* fullData = GET_CHAOPARAM(chao);
	CHAO_PARAM_CWE* pParamCwe = GET_CWEPARAM(chao);

	for (int i = 0; i < 7; i++)
		fullData->partsDX.MinimalParts[i] = opponent->Name[i];
	fullData->partsDX.MinimalParts[7] = SADXAnimal_Seal;

	if (opponent->Shiny == 3)
	{
		fullData->Shiny = 1;
		pParamCwe->ShinyJewelMonotone = 1;
	}

	if (opponent->f13 == 0)
	{
		pParamCwe->EyeAlignment = 0;
		pParamCwe->EyeColor = 0;
	}
	else if (opponent->f13 >= 1 && opponent->f13 <= 6)
	{
		pParamCwe->EyeAlignment = 1;
		pParamCwe->EyeColor = opponent->f13 - 1;
	}
	else if (opponent->f13 >= 7 && opponent->f13 <= 12)
	{
		pParamCwe->EyeAlignment = 2;
		pParamCwe->EyeColor = opponent->f13 - 7;
	}
	else if (opponent->f13 >= 13 && opponent->f13 <= 18)
	{
		pParamCwe->EyeAlignment = 3;
		pParamCwe->EyeColor = opponent->f13 - 13;
	}
	else if (opponent->f13 >= 20 && opponent->f13 <= 22)
	{
		pParamCwe->EyeAlignment = opponent->f13 - 20;
		pParamCwe->EyeColor = 7;
	}
	else if (opponent->f13 >= 23 && opponent->f13 <= 25)
	{
		pParamCwe->EyeAlignment = (opponent->f13 - 23) + 1;
		pParamCwe->EyeColor = 8;
	}
	sub_566B80(chao);
	return chao;
}

void sub_54A690(task* a1)
{
	Data1Ptr v1; // eax
	colliwk* v2; // eax

	if (a1 && (v1.Undefined = (taskwk*)a1->twp) != 0 && (v2 = v1.Entity->cwp) != 0)
	{
		v2->CollisionArray[1].field_2 |= 0x70u;
	}
}
void sub_54A730(task* tp)
{
	tp->twp->flag &= ~0x240u;
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

// to handle KCE being able to change accessory ids in param at will
// and to not have to match up the id to the index every frame
// we store the rendered accessory index in AccessoryIndices,
// and the "approved" id in AccessoryCalculatedID to not check needlessly if it didnt change
// we also store the index back in the old slots, to provide compatibility checks
void AL_ChaoAccessoryMainCheck(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	auto pParam = GET_CWEPARAM(tp);

	// new accessory stuff
	for (size_t i = 0; i < _countof(pParam->Accessories); ++i) {
		auto& data = pParam->Accessories[i];

		// if it's an empty string, set no index, check next accessory
		if (!data.ID[0]) {
			work->AccessoryCalculatedID[i][0] = 0;
			work->AccessoryIndices[i] = -1;
			pParam->Accessories_[i] = 0;
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

		// compatibility with old evolution checks
		pParam->Accessories_[i] = work->AccessoryIndices[i] + 1;
	}
}

// handles converting the old index-based accessories to the new structs
// this is called per chaodata in alw_control (to handle all in-garden chao as soon as possible)
// and in the race chao main function to fix them using index 0 in work->AccessoryIndices
void AL_ChaoAccessoryConversion(CHAO_PARAM_CWE* pParam) {
	if (!pParam) return;

	if (!(pParam->Flags & AL_PARAM_FLAG_ACCESSORIES_NEW)) {
		for (size_t i = 0; i < _countof(pParam->Accessories_); ++i) {
			memset(&pParam->Accessories[i], 0, sizeof(pParam->Accessories[i]));

			int accIndex = int(pParam->Accessories_[i]) - 1;
			const bool pinkHoodiePatch = accIndex == 8; // was pink hoodie
			
			// shift everything from pink hoodie backwards, pink hoodie id was deleted
			// so its just a regular hoodie now, and using "pinkHoodiePatch" we set the color
			if (accIndex >= 8) accIndex -= 1;

			char id[METADATA_ID_SIZE];
			bool foundID = ItemMetadata::Get()->GetID(ChaoItemCategory_Accessory, accIndex, id);
			if (!foundID) {
				// TODO: error
				continue;
			}

			// hacky way to make pink hoodie a regular hoodie and recolor it to resemble the pink one
			if (pinkHoodiePatch) {
				pParam->Accessories[i].ColorFlags |= BIT_0;

				// pink color
				NJS_COLOR* colorSlot = (NJS_COLOR*)&pParam->Accessories[i].ColorSlots[0];
				colorSlot->argb.a = 255;
				colorSlot->argb.r = 255;
				colorSlot->argb.g = 121;
				colorSlot->argb.b = 213;
			}

			strcpy_s(pParam->Accessories[i].ID, id);
		}

		pParam->Flags |= AL_PARAM_FLAG_ACCESSORIES_NEW;
	}
}

// I wanted the call to this logic to stay in this file, but the buyobuyocontrol hook is in api_customchao
// look there to see where this is called
// we had to move this out of chao_main hook so that it runs in chao-partner too
void AL_BuyoBuyo_AccessoryHandling(task* tp) {
	AL_ChaoAccessoryMainCheck(tp);
}

static void Chao_Main_r(task* a1);
static FunctionHook<void, task*> Chao_Main_hook(0x0054FE20, Chao_Main_r);
static void Chao_Main_r(task* a1)
{
	chaowk* work = GET_CHAOWK(a1);
	CHAO_PARAM_GC* pParam = GET_CHAOPARAM(a1);
	CHAO_PARAM_CWE* pParamCwe = GET_CWEPARAM(a1);

	if (!pParam) return;

	//some timer
	if (GET_CHAOWK(a1)->Timer <= 2u)
	{
		if (pParam->field_19 == 2)
			pParam->field_19 = 0;

		//upgrade to 9.4
		if (pParamCwe->IsInitializedAccessory == 0)
		{
			for (int i = 0; i < 4; i++)
				pParamCwe->Accessories_[i] = pParamCwe->Accessories_old[i];
			pParamCwe->IsInitializedAccessory = 1;
		}

		
		if (!(pParamCwe->Flags & AL_PARAM_FLAG_NAME_NEW))
		{
			AL_GENE& Gene = pParam->Gene;

			memcpy(pParamCwe->Name, pParam->name, sizeof(pParam->name));
			memcpy(pParamCwe->MotherName, Gene.MotherName, sizeof(Gene.MotherName));
			memcpy(pParamCwe->FatherName, Gene.FatherName, sizeof(Gene.FatherName));
			memcpy(pParamCwe->MGroundMotherName, Gene.MGroundMotherName, sizeof(Gene.MGroundMotherName));
			memcpy(pParamCwe->MGroundFatherName, Gene.MGroundFatherName, sizeof(Gene.MGroundFatherName));
			memcpy(pParamCwe->FGroundMotherName, Gene.FGroundMotherName, sizeof(Gene.FGroundMotherName));
			memcpy(pParamCwe->FGroundFatherName, Gene.FGroundFatherName, sizeof(Gene.FGroundFatherName));

			pParamCwe->Flags |= AL_PARAM_FLAG_NAME_NEW;
		}


		if (!pParamCwe->Birthday)
		{
			pParamCwe->Birthday = 60 + (int)(njRandom() * 30); //randrange(60,90)
		}

		if (!pParam->ChaoID.id[0]) //if doesnt have UID
		{
			//give em one
			pParam->ChaoID.id[0] = GenerateRandomSeed();
		}
	}

	memcpy(pParam->name, pParamCwe->Name, sizeof(pParam->name));

	if (!(pParamCwe->Flags & AL_PARAM_FLAG_OLD_GUEST_CHECK)) {
		if ((Uint8)pParamCwe->Name[6] == 0xFB) {
			pParam->field_19 = 1;
			pParamCwe->Name[6] = 0;
		}

		pParamCwe->Flags |= AL_PARAM_FLAG_OLD_GUEST_CHECK;
	}

	//these are ported xml codes
	//immortality
	if (*(char*)0x0053FD6C == 1) //new challenge enabled check
	{
		if (pParam->Medal == 8)
		{
			if (pParam->life < 1000) pParam->life = 1000;
			if (pParam->LifeMax < 1000) pParam->LifeMax = 1000;
		}
	}

	//new power climbing
	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_CLIMB && GET_CHAOWK(a1)->Behavior.SubTimer == 240)
	{
		if (pParam->Skill[3] >= 150) //USE GETSTAT THING LATER
		{
			GET_CHAOWK(a1)->Behavior.SubTimer = 0;
		}
	}

	//monster transparency fix
	if (pParam->FlySwim < -1 || pParam->FlySwim > 1 ||
		pParam->PowerRun < -1 || pParam->PowerRun > 1)
	{
		if (GET_CHAOWK(a1)->Face.pMouthObject && GET_CHAOWK(a1)->Face.pMouthObject->chunkmodel)
		{
			__int32* color = (__int32*)GET_CHAOWK(a1)->Face.pMouthObject->chunkmodel->plist;
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

	Chao_Main_hook.Original(a1);

	// if disable jiggle was enabled or disabled rerun fittobaseobject
	// to prevent the jiggled vertices to be frozen
	if(work->JiggleFlagChanged) {
		AL_FitToBaseObject(a1, work->Shape.pObject);
		work->JiggleFlagChanged = false;
	}
}

// once the jiggle and everything has been ran
// if jiggle is disabled reset the normals
// unfortunately the chao normals being sane depends on jiggle basing it off
// pOrgVertexNormal
// luckily adult chao set pVertex->Normal instead of pOrgVertexNormal
// note: since child chao set pOrgVertexNormal itself, it might not be enough
// and we need to normalize for them
static void AL_Deform_r(task* tp);
static FunctionHook<void, task*> AL_Deform_t(0x5A49C0, AL_Deform_r);
static void AL_Deform_r(task* tp) {
	AL_Deform_t.Original(tp);

	chaowk* work = GET_CHAOWK(tp);
	if(!(work->ChaoFlag & 0x20)) {
		AL_FitToBaseObject(tp, work->Shape.pObject);
	}
}


static void RaceChaoExecutor(task* tp);
static Trampoline RaceChaoExecutor_t(0x0053FC10, 0x0053FC19, RaceChaoExecutor);
static void RaceChaoExecutor(task* tp) {
	AL_ChaoAccessoryConversion(GET_CWEPARAM(tp));

	auto original = reinterpret_cast<decltype(RaceChaoExecutor)*>(RaceChaoExecutor_t.Target());
	original(tp);
}

enum {
    RAKUGAKI_SONIC,
    RAKUGAKI_SHADOW,
    RAKUGAKI_TAILS,
    RAKUGAKI_EGGMAN,
    RAKUGAKI_KNUCKLES,
    RAKUGAKI_ROUGE,
    RAKUGAKI_N_CHAO,
    RAKUGAKI_H_CHAO,
    RAKUGAKI_D_CHAO,
    RAKUGAKI_CAKE,
    RAKUGAKI_CAR,
    RAKUGAKI_FLOWER,
    RAKUGAKI_BONE,
    RAKUGAKI_AMY,
    RAKUGAKI_TIKAL,
    RAKUGAKI_BIG,
	RAKUGAKI_CWE_START,
	RAKUGAKI_CWE_COCO = RAKUGAKI_CWE_START,
	RAKUGAKI_CWE_PUYO,
	RAKUGAKI_CWE_NIGHTS,
	RAKUGAKI_CWE_AMIGO, // this is unused atm
};

const int ALO_RakugakiExecutor_LoadPtr = 0x05AB200;
void ALO_RakugakiExecutor_Load_(int a1, int a2, int a3)
{
	__asm
	{
		mov ebx, a1
		push a3
		push a2
		call ALO_RakugakiExecutor_LoadPtr
		add esp, 8 
	}
}

void __cdecl NewDrawings(int a1, int ID, int a3)
{
	if (ID >= RAKUGAKI_AMY && ID <= RAKUGAKI_BIG && njRandom() > 0.5f)
		ID += RAKUGAKI_CWE_START - RAKUGAKI_AMY;

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
	static_assert(offsetof(chaowk, pParamGC) == 0x5C, "chaowk:pParamGC got shuffled");
	static_assert(offsetof(chaowk, ChaoFlag) == 0xA0, "chaowk:ChaoFlag got shuffled");
	static_assert(offsetof(chaowk, MotionCtrl) == 0x0AC, "chaowk:MotionCtrl got shuffled");
	static_assert(offsetof(chaowk, Behavior) == 0x1B4, "chaowk:Behavior got shuffled");
	static_assert(offsetof(chaowk, Shape) == 0x510, "chaowk:Shape.pObject (AL_SHAPE) got shuffled");
	static_assert(offsetof(chaowk, Face) == 0x68C, "chaowk:Face got shuffled");
	static_assert(offsetof(chaowk, Icon) == 0x6EC, "chaowk:Icon got shuffled");
	static_assert(offsetof(chaowk, Perception) == 0x7CC, "chaowk:Perception got shuffled");
	static_assert(offsetof(chaowk, LocalCharacterChaoType) == 0x1D74, "chaowk: new fields dont start at vanilla size");
	static_assert(sizeof(chaowk) >= 0x1D74, "chaowk size lower than vanilla, something's up");

	AL_Name_Init();

	//expand chaowk alloc
	WriteData((int*)0x00550288, (int)sizeof(chaowk) + 4);
	WriteData((int*)0x00550294, (int)sizeof(chaowk));
	//memalloc = sizeof(ChaoData)
	WriteData((short*)0x55040E, (short)0x200);
	WriteData((short*)0x5503DD, (short)0x804);
	WriteData((short*)0x5503E7, (short)0x800);
	WriteData((short*)0x55043E, (short)0x804);
	WriteData((short*)0x550448, (short)0x800);

	// egg ParamCopy alloc size (2 malloc, 2 memset, and memcpy)
	WriteData((uint32_t*)0x0057BC0E, uint32_t(sizeof(CHAO_SAVE_INFO) + 4));
	WriteData((uint32_t*)0x0057BC61, uint32_t(sizeof(CHAO_SAVE_INFO) + 4));
	WriteData((uint32_t*)0x0057BC18, uint32_t(sizeof(CHAO_SAVE_INFO)));
	WriteData((uint32_t*)0x0057BC6B, uint32_t(sizeof(CHAO_SAVE_INFO)));
	WriteData((uint32_t*)0x0057BC32, uint32_t(sizeof(CHAO_SAVE_INFO) / 4));

	AL_Gene_Init();
	AL_ParameterInit();
	AL_FaceInit();

	// new drawings
	// note: there's an extra texture at texture 0 because that's how it is in AL_OBJECT too
	// i used an aiai texture which managed to mislead me earlier, hence this comment lol
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
