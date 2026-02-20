#include "stdafx.h"
#include "al_minimal.h"
#include "data/minimal/AnimalModels.h"
#include "data/minimal/AnimalAnims.h"
#include "al_behavior/al_intention.h"

#include "ninja_functions.h"
#include "ChaoMain.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "al_parts.h"
#include "al_minimal.h"
#include "al_motion.h"
#include "AL_ModAPI.h"

#include <renderfix.h>

std::vector<int> ModAPI_MinimalInfluence;
std::vector<ChaoItemStats> ModAPI_MinimalStats;
std::vector<const char*>  ModAPI_MinimalNames;
std::vector<NJS_TEXLIST*>  ModAPI_MinimalTexlists;
std::vector<NJS_OBJECT*>   ModAPI_MinimalModels;
std::vector<NJS_MOTION*>   ModAPI_MinimalMotion0;
std::vector<NJS_MOTION*>   ModAPI_MinimalMotion1;
std::vector<NJS_MOTION*>   ModAPI_MinimalMotion2;
std::vector<NJS_MOTION*>   ModAPI_MinimalMotion3;
std::map<int, std::vector<CWE_MINIMAL_FRUIT>> ModAPI_MinimalFruit;

NJS_TEXNAME GOMA_TEXNAME[3];
NJS_TEXLIST GOMA_TEXLIST = { GOMA_TEXNAME ,3 };
NJS_TEXNAME PEN_TEXNAME[5];
NJS_TEXLIST PEN_TEXLIST = { PEN_TEXNAME ,5 };
NJS_TEXNAME RAKO_TEXNAME[4];
NJS_TEXLIST RAKO_TEXLIST = { RAKO_TEXNAME ,4 };
NJS_TEXNAME KUJA_TEXNAME[5];
NJS_TEXLIST KUJA_TEXLIST = { KUJA_TEXNAME ,5 };
NJS_TEXNAME TUBA_TEXNAME[5];
NJS_TEXLIST TUBA_TEXLIST = { TUBA_TEXNAME ,5 };
NJS_TEXNAME OUM_TEXNAME[5];
NJS_TEXLIST OUM_TEXLIST = { OUM_TEXNAME ,5 };
NJS_TEXNAME BANB_TEXNAME[4];
NJS_TEXLIST BANB_TEXLIST = { BANB_TEXNAME ,4 };
NJS_TEXNAME USA_TEXNAME[4];
NJS_TEXLIST USA_TEXLIST = { USA_TEXNAME ,4 };
NJS_TEXNAME WARA_TEXNAME[4];
NJS_TEXLIST WARA_TEXLIST = { WARA_TEXNAME ,4 };
NJS_TEXNAME GORI_TEXNAME[2];
NJS_TEXLIST GORI_TEXLIST = { GORI_TEXNAME ,2 };
NJS_TEXNAME LION_TEXNAME[4];
NJS_TEXLIST LION_TEXLIST = { LION_TEXNAME ,4 };
NJS_TEXNAME ZOU_TEXNAME[3];
NJS_TEXLIST ZOU_TEXLIST = { ZOU_TEXNAME ,3 };
NJS_TEXNAME MOGU_TEXNAME[4];
NJS_TEXLIST MOGU_TEXLIST = { MOGU_TEXNAME ,4 };
NJS_TEXNAME KOAR_TEXNAME[4];
NJS_TEXLIST KOAR_TEXLIST = { KOAR_TEXNAME ,4 };
NJS_TEXNAME SUKA_TEXNAME[4];
NJS_TEXLIST SUKA_TEXLIST = { SUKA_TEXNAME ,4 };

NJS_TEXLIST* AnimalTexLists[] =
{
  &GOMA_TEXLIST,
  &PEN_TEXLIST,
  &RAKO_TEXLIST,
  &KUJA_TEXLIST,
  &TUBA_TEXLIST,
  &OUM_TEXLIST,
  &BANB_TEXLIST,
  &USA_TEXLIST,
  &WARA_TEXLIST,
  &GORI_TEXLIST,
  &LION_TEXLIST,
  &ZOU_TEXLIST,
  &MOGU_TEXLIST,
  &KOAR_TEXLIST,
  &SUKA_TEXLIST,
};

const char* AnimalPVMNames[] =
{
  "GOMA",
  "PEN",
  "RAKO",
  "KUJA",
  "TUBA",
  "OUM",
  "BANB",
  "USA",
  "WARA",
  "GORI",
  "LION",
  "ZOU",
  "MOGU",
  "KOAR",
  "SUKA"
};

NJS_OBJECT* AnimalModels[] = {
	&object_0052A2D0, &object_0052C40C,&object_00549104,&object_005308DC,&object_0052EA0C,&object_00532ADC,&object_00539B2C,&object_0053BFE4,&object_0053723C,&object_00544FD4,&object_00542F90,&object_0054043C,&object_0053E2B8,&object_00534AE0,&object_00546D4C,
};

NJS_MOTION* AnimalAnims0[] = {
	&animation_03215928,&animation_03215D88,&animation_032162D8,&animation_03216808,&animation_03216C88,&animation_032170E8,&animation_03217588,&animation_03217A68,&animation_03218088,&animation_03218368,&animation_032188E8,&animation_03218E08,&animation_032191E8,&animation_03219708,&animation_03219AC8,
};

NJS_MOTION* AnimalAnims1[] = {
	&animation_0052ACD8,&animation_0052D390,&animation_00549FB8,&animation_03213DD0,&animation_03214870,&animation_03215550,&animation_0053A8B0,&animation_0053CF68,&animation_00538520,&animation_00545958,&animation_00544088,&animation_00541600,&animation_0053ECE0,&animation_005359A8,&animation_00547890,
};

NJS_MOTION* AnimalAnims2[] = {
	&animation_032093D8,&animation_0320A078,&animation_0320AA48,&animation_0320B2F8,&animation_0320BC08,&animation_0320C488,&animation_0320D158,&animation_0320DCB8,&animation_0320ECB8,&animation_0320F318,&animation_03210058,&animation_03211118,&animation_03211EC8,&animation_03212648,&animation_03212FE8,
};

NJS_MOTION* AnimalAnims3[] = {
	0,0,0,&animation_00531420,&animation_0052F2B0,&animation_005334E0,0,0,0,0,0,0,0,0,0
};

ChaoItemStats AnimalStats[] =
{
  { 1, 0, 40, -16, 0, 8, 0, 20, 20, 0 },
  { 1, 0, 36, 12, 8, -24, 0, 20, 20, 0 },
  { 1, 0, 44, -4, 8, -16, 0, 20, 20, 0 },
  { 1, 0, 12, 48, -16, -12, 0, 20, 20, 0 },
  { 1, 0, 20, 60, -24, 16, 0, 20, 20, 0 },
  { 1, 0, 0, 48, -20, 4, 0, 20, 20, 0 },
  { 1, 0, -4, -12, 32, 16, 0, 20, 20, 0 },
  { 1, 0, 0, 8, 40, -16, 0, 20, 20, 0 },
  { 1, 0, -8, -8, 40, 8, 0, 20, 20, 0 },
  { 1, 0, -4, -8, 4, 40, 0, 20, 20, 0 },
  { 1, 0, 8, -16, 4, 36, 0, 20, 20, 0 },
  { 1, 0, -8, -16, 20, 36, 0, 20, 20, 0 },
  { 1, 0, 8, 8, 16, 16, 0, 20, 20, 0 },
  { 1, 0, 20, 16, 4, 8, 0, 20, 20, 0 },
  { 1, 0, 8, 12, 16, 12, 0, 20, 20, 0 }
};

int AnimalInfluence[] =
{
	//SA2B animals:
	COLOR_SWIM, COLOR_SWIM, COLOR_SWIM,
	COLOR_RUN,  COLOR_RUN,  COLOR_RUN,
	COLOR_POWER,COLOR_POWER,COLOR_POWER,
	COLOR_FLY,	COLOR_FLY,	COLOR_FLY,
	COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM,
	COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM,
	COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM,

	COLOR_SWIM, COLOR_RUN, COLOR_POWER, COLOR_FLY,
	//the last 4 don't get used, theyre just "placeholders" for the chaos drives
	//we can't use the influence system for the drives because they only give half the influence, so we had to hardcode it

	//SADX animals:
	COLOR_SWIM, COLOR_SWIM, COLOR_SWIM,
	COLOR_FLY,	COLOR_FLY,	COLOR_FLY,
	COLOR_RUN,  COLOR_RUN,  COLOR_RUN,
	COLOR_POWER,COLOR_POWER,COLOR_POWER,
	COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM
};


ObjectFunc(sub_57BD40, 0x0057BD40);
#pragma pack(push, 8)
struct __declspec(align(4)) ParticleData
{
	int rotY;
	float scale;
	NJS_VECTOR position;
	int color;
	float frame;
	int field_1C;
	NJS_VECTOR velocity;
	int rotYSpeed;
	float filler3;
	float filler4;
	float filler5;
};
#pragma pack(pop)
#pragma pack(push, 8)
struct __declspec(align(4)) ParticleUserData
{
	int a1;
	NJS_TEXLIST* texlist;
	int a2;
	int frames;
	float speed;
	float velocityLoss;
	float gravity;
	float scaleUp;
	int functionPtr;
	float a7;
	int fill1;
	int fill2;
	int fill3;
	int lastPointer;
};
#pragma pack(pop)
ParticleUserData stru_B0964C_ =
{
  1,
  (NJS_TEXLIST*)0xB09644,
  0,
  4,
   0.07f,
   0.98f,
  0,
   0.005f,
  0x6C42D0,
   40000,
  0,
  0,
  0,
  0
};
const int AllocateParticlePtr = 0x0492660;
int  AllocateParticle(ParticleUserData* a2)
{
	int te;
	__asm
	{
		mov ebx, a2
		call AllocateParticlePtr
		mov te, eax
	}
	return te;
}
void __cdecl sub_6EFF10_(NJS_VECTOR* a1, NJS_VECTOR* a2, float a3)
{
	ParticleData* v3; // esi
	Float v4; // eax

	v3 = (ParticleData*)AllocateParticle(&stru_B0964C_);
	if (v3)
	{
		v3->position = *a1;
		v3->velocity.x = a2->x;
		v3->velocity.y = a2->y;
		v4 = a2->z;
		v3->scale = a3;
		v3->velocity.z = v4;
		v3->rotY = (signed int)((njRandom() * 20.0f - 10.0f) * 182.0444488525391f);
		v3->rotYSpeed = (signed int)((njRandom() - 0.5f) * 182.0444488525391f);
		v3->filler5 = 0.0;
	}
}
void AL_Minimal_Timer(ObjectMaster* a1)
{
	sub_57BD40(a1);

	AL_MinimalExecutor_Data1* v1 = (AL_MinimalExecutor_Data1*)a1->Data1.Undefined;
	int* timer = (int*)((char*)&v1->field_F4 + 4);
	(*timer)++;
	if (!v1->field_F4 && (*timer & 31) == 1)
	{
		NJS_VECTOR pos = v1->entity.Position;
		NJS_VECTOR zeroVelo = { 0,0,0 };
		int ang = (int)(njRandom() * 65536.f);

		pos.x += njCos(ang) * 2;
		pos.y += njRandom() + njRandom() + 0.2f;
		pos.z += njSin(ang) * 2;
		
		sub_6EFF10_(&pos, &zeroVelo, 0.6f);
	}
}


void SetAnimalTexEntity(EntityData1* data)
{
	njSetTexture(ModAPI_MinimalTexlists[data->Index]);
}

const int sub_793F90Ptr = 0x793F90;
void sub_793F90(NJS_OBJECT* a1, MotionTableData* a2)
{
	__asm
	{
		mov ecx, a1
		mov eax, a2
		call sub_793F90Ptr
	}
}

void __cdecl AL_MinimalExecutor_Display_(ObjectMaster* a1)
{
	ObjectMaster* v1; // edi
	AL_MinimalExecutor_Data1* v2; // ebp
	float v3; // st7

	v1 = a1;
	v2 = (AL_MinimalExecutor_Data1*)a1->Data1.Undefined;
	if (ScaleObjectMaster_XYZ(a1, 3, 3, 2))
	{
		v2->field_34 = 200;
		if (v2->entity.Status >= 0)
		{
			v3 = -1.0f;
		}
		else
		{
			v3 = -1.2f;
		}
		DoLighting(LightIndex);
		SetAnimalTexEntity(a1->Data1.Entity);
		njPushMatrixEx();
		njTranslateEx(&v2->entity.Position);
		njTranslate(NULL, 0, v3, 0);
		RotateY(v2->entity.Rotation.y);
		njPushMatrixEx();

		if (ALO_Field_Find_(v1, 1, CI_KIND_AL_SHADOW))
		{
			njControl3D |= 0x2400u;
		}

		// mega jank, im in a rush rn
		if (RenderFix_IsEnabled()) {
			RF_BACKWARDS_COMPAT_GUARD();
			g_HelperFunctions->PushInterpolationFix();
			sub_793F90(ModAPI_MinimalModels[(unsigned __int8)v2->entity.Index], &v2->field_38);
			g_HelperFunctions->PopInterpolationFix();
		}
		else {
			g_HelperFunctions->PushInterpolationFix();
			sub_793F90(ModAPI_MinimalModels[(unsigned __int8)v2->entity.Index], &v2->field_38);
			g_HelperFunctions->PopInterpolationFix();
		}

		njControl3D &= ~0x2400u;
		njPopMatrixEx();


		if (!v1->UnknownA_ptr) PrintDebug("unknown_a == 0, problem incoming");

		if (RenderFix_IsEnabled() && v1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&v1->UnknownA_ptr->field_30)
		{
			if (ALO_Field_Find_(v1, 1, CI_KIND_AL_SHADOW))
			{
				njTranslate(NULL, 0, 0.01f, 0);
			}
			else
			{
				njTranslate(NULL, 0, 0.5f, 0);
			}
			rfapi_core->pDraw->AL_ShadowDraw();
		}
		njPopMatrixEx();
		DoLighting(LightIndexBackupMaybe);
	}
	else if (v2->field_F4)
	{
		v2->field_34--;
		if (v2->field_34 <= 0)
		{
			v2->entity.Action = 3;
			v2->entity.NextAction = 0;
		}
	}
}



void al_minimal_Init()
{
	//patch minimal timer to be constant
	WriteData((int*)0x00548C7D, (int)0x9090C031);

	//set the minimal behavior function array's first element to use our custom one
	*(int*)0x1316B00 = (int)AL_Minimal_Timer;

	WriteData((int*)0x00548D5F, (int)0xFC + 4);
	WriteData((int*)0x00548D69, (int)0xF8 + 4);
	
	DataArray(ChaoItemStats, ChaoItemStatArray, 0x08A6240, 26);
	DataArray(NJS_OBJECT*, BattleAnimalModels, 0x012966F0, 21);
	DataArray(NJS_MOTION*, BattleAnimalAnims0, 0x1296870, 21);
	DataArray(NJS_MOTION*, BattleAnimalAnims1, 0x12967F0, 21);
	DataArray(NJS_MOTION*, BattleAnimalAnims2, 0x1296770, 21);
	DataArray(NJS_MOTION*, BattleAnimalAnims3, 0x12968D8, 21);

	//animals
	for (int i = 0; i < 21; i++) {
		ModAPI_MinimalModels.push_back(BattleAnimalModels[i]);
		ModAPI_MinimalMotion0.push_back(BattleAnimalAnims0[i]);
		ModAPI_MinimalMotion1.push_back(BattleAnimalAnims1[i]);
		ModAPI_MinimalMotion2.push_back(BattleAnimalAnims2[i]);
		ModAPI_MinimalMotion3.push_back(BattleAnimalAnims3[i]);
		ModAPI_MinimalStats.push_back(ChaoItemStatArray[i]);
		ModAPI_MinimalTexlists.push_back(&al_minimal_texlist);
	}

	//chaos drives
	for (int i = 0; i < 4; i++) {
		ModAPI_MinimalModels.push_back(nullptr);
		ModAPI_MinimalMotion0.push_back(nullptr);
		ModAPI_MinimalMotion1.push_back(nullptr);
		ModAPI_MinimalMotion2.push_back(nullptr);
		ModAPI_MinimalMotion3.push_back(nullptr);
		ModAPI_MinimalStats.push_back(ChaoItemStatArray[21 + i]);
		ModAPI_MinimalTexlists.push_back(&al_minimal_texlist);
	}

	for (int i = 0; i < 40; i++) {
		ModAPI_MinimalInfluence.push_back(AnimalInfluence[i]);
	}

	for (int i = 0; i < 15; i++) {
		ModAPI_MinimalModels.push_back(AnimalModels[i]);
		ModAPI_MinimalMotion0.push_back(AnimalAnims0[i]);
		ModAPI_MinimalMotion1.push_back(AnimalAnims1[i]);
		ModAPI_MinimalMotion2.push_back(AnimalAnims2[i]);
		ModAPI_MinimalMotion3.push_back(AnimalAnims3[i]);
		ModAPI_MinimalStats.push_back(AnimalStats[i]);
		ModAPI_MinimalTexlists.push_back(AnimalTexLists[i]);
		CWE_API_Legacy.RegisterChaoTexlistLoad(AnimalPVMNames[i], AnimalTexLists[i]);
	}

	WriteJump(AL_MinimalExecutor_Display, AL_MinimalExecutor_Display_);
}
