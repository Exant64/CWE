#include "stdafx.h"
#include "alo_fruit.h"
#include "Chao.h"
#include "ALifeSDK_Functions.h"
#include "ChaoMain.h"
#include "ninja_functions.h"
#include "al_sandhole.h"
#include "data/cwe/object_common_cnk/Mushroom2x.h"
#include "AL_ModAPI.h"
#include "al_modelcontainer.h"
#include "al_behavior/al_intention.h"
#include "Trampoline.h"
#include <al_daynight.h>

std::vector<ChaoItemStats> ModAPI_FruitStats;


void ALO_FruitExecutor_DisplayHack(ObjectMaster *eax0)
{
	ObjectMaster *v1; // ebx
	EntityData1 *v2; // ebp
	
	float v13; // [esp+1Ch] [ebp-38h]
	float a2; // [esp+1Ch] [ebp-38h]

	v1 = eax0;
	v2 = eax0->Data1.Entity;
	//this function is incorrectly named, its a bounds check (only used for chao stuff for some reason)
	//Rotation.x = fruit type
	//Rotation.z = fruit size (like the "bites" value)
	//rotation.y is actually the y rotation lol
	//i store the "visible onscreen" flag inside Index as you can see below here
	if (ScaleObjectMaster_XYZ(eax0, 2.5, 2.5, 2.0))
	{
		v2->Index = 1;
		v13 = v2->Rotation.z * 0.3f + 0.3f;
		if (v2->Rotation.x == 9)
		{
			v13 = v13 * 0.8f;
		}
		a2 = 0.8f * v13;

		AL_DayNightCycle_PushFallbackLight();
		DoLighting(LightIndex);
		
		njPushMatrixEx();
		njTranslateEx(&v2->Position);
		if (v2->Status >= 0)
		{
			njTranslate(NULL, 0, a2 * 0.55f + 0.8f, 0);
		}
		else
		{
			njTranslate(NULL, 0, 0.4f, 0);
		}
		
		if (v2->Rotation.z > 0)
		{
			RotateY(v2->Rotation.y);
			njPushMatrixEx();
			njScale(NULL, a2, a2, a2);
			if (ALO_Field_Find_(v1, 1, 150))
			{
				njControl3D |= 0x2400u;
			}
			ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Fruit, v2->Rotation.x);
			njControl3D &= ~0x2400u;
			njPopMatrixEx();
			if (v1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float *)&v1->UnknownA_ptr->field_30)
			{
				if (ALO_Field_Find_(v1, 1, CI_KIND_AL_SHADOW))
				{
					njTranslate(NULL, 0, -1.85f, 0);
				}
				else
				{
					njTranslate(NULL, 0, -1, 0);
				}

				njScale(NULL, a2, 0.7f, a2);
				DrawChaoWorldShadow();
			}
		}
		njPopMatrixEx();
		AL_DayNightCycle_PopFallbackLight();
		DoLighting(LightIndexBackupMaybe);
	}
	else if (!(v2->Status < 0)) { //if not being held by player
		v2->Index = 0;
	}
}

#define RATIO(x,y) random >= (x / 100.0f) && random <= (y / 100.0f)
void ALO_FruitExecutor_Main_r(ObjectMaster* a1);
Trampoline ALO_FruitExecutor_Main_t(0x00545E40, 0x00545E4A, ALO_FruitExecutor_Main_r);
void ALO_FruitExecutor_Main_r(ObjectMaster *a1)
{
	ObjectFunc(original, ALO_FruitExecutor_Main_t.Target());
	original(a1);
	
	//if fruit ID inside api registered fruit list
	if (!ModAPI_MinimalFruit.count(a1->Data1.Entity->Rotation.x)) return;
	
	//if not visible onscreen (animals only spawn if fruit is not visible)
	if (a1->Data1.Entity->Index) return;

	//random check (0.05% every frame it's onscreen)
	if (njRandom() > 0.0005) return;

	NJS_VECTOR velocity = { 0,1,0 };

	//find empty animal data slot, and check if garden has less than 10 animals
	void* data = sub_52F9E0(2);
	if (data && nbWorldEntry[2] < nbMaxEntry[2]) {
		//decrease bites
		a1->Data1.Entity->Rotation.z--;

		if (a1->Data1.Entity->Rotation.z <= 0)
		{
			//if no bites left, delete object
			ALW_SendCommand(a1, ALW_CMD_FINISH);

			ChaoSomeUnknownA *v23 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
			if (v23 && v23->saveData)
			{
				ResetChaoObjectData((ChaoObjectData*)v23->saveData);
				v23->saveData = 0;
			}

			a1->MainSub = DeleteObject_;
		}
		//purple chaos drive is the last vanilla animal, after that comes the new ones
		int type = SA2BAnimal_PurpleChaosDrive + 1; 
		float random = njRandom();

		for (const auto& minifruitentry : ModAPI_MinimalFruit[a1->Data1.Entity->Rotation.x])
		{
			if(RATIO(minifruitentry.chanceMin, minifruitentry.chanceMax))
			{
				type = minifruitentry.minimalID;
				break;
			}
		}
		AL_MinimalExecutor_Load(type, &a1->Data1.Entity->Position, 0, &velocity, (int)data);
	}
}

const int sub_530690Ptr = 0x530690;
signed int sub_530690(ObjectMaster *a1)
{
	int retval;
	__asm
	{
		mov eax, a1
		call sub_530690Ptr
		mov retval, eax
	}
	return retval;
}

const int sub_545790Ptr = 0x545790;
void sub_545790(ObjectMaster *a1, unsigned __int8 a2, int a3)
{
	__asm
	{
		push a3
		push dword ptr[a2]
		mov esi, a1
		call sub_545790Ptr
		add esp, 8
	}
}
FastcallFunctionPointer(signed int, sub_56D170, (int a1, ObjectMaster *a2), 0x56D170);
void __cdecl sub_545C20(ObjectMaster *a1)
{
	ChaoSomeUnknownA *v1; // eax
	Data1Ptr v2; // ebx
	ChaoSomeUnknownA *v3; // ecx
	int v4; // edx
	Data1Ptr v5; // eax
	CollisionInfo *v6; // eax
	Data1Ptr v7; // eax
	CollisionInfo *v8; // eax
	Data1Ptr v9; // eax
	CollisionInfo *v10; // eax
	__int16 v11; // ax
	ChaoSomeUnknownA *v12; // eax
	ChaoSomeUnknownA *v13; // edi
	Data1Ptr v14; // eax
	ObjUnknownA *v15; // eax
	ObjectMaster *v16; // eax
	ChaoDataBase *v17; // ecx
	ChaoSomeUnknownA *v22; // eax
	ChaoSomeUnknownA *v23; // eax
	ObjUnknownA *v25; // eax
	ChaoSomeUnknownA *v26; // eax
	ChaoSomeUnknownA *v27; // ecx

	v1 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
	v2.Entity = (EntityData1 *)a1->Data1.Entity;

	//to stop warning 
	if (!a1->Data1.Undefined) return;

	v3 = 0;
	if (v1)
	{
		v3 = (ChaoSomeUnknownA *)(unsigned __int16)v1->setSomeIndex;
		v1->setSomeIndex = 0;
	}

	v4 = (int)v3;
	if (!v2.Entity->NextAction)
	{
		v5.Entity = (EntityData1 *)a1->Data1.Entity;
		if (v5.Entity)
		{
			v6 = v5.Entity->Collision;
			if (v6)
			{
				v6->CollisionArray->field_4 &= 0xFFFFFFEF;
			}
		}
		v7.Entity = (EntityData1 *)a1->Data1.Entity;
		v3 = (ChaoSomeUnknownA *)16;
		if (v7.Undefined)
		{
			v8 = v7.Entity->Collision;
			if (v8)
			{
				v8->CollisionArray[1].field_4 |= 0x10u;
			}
		}
		v9.Entity = (EntityData1 *)a1->Data1.Entity;
		if (v9.Entity)
		{
			v10 = v9.Entity->Collision;
			if (v10)
			{
				v10->CollisionArray[2].field_4 |= 0x10u;
			}
		}
		++a1->Data1.Entity->NextAction;
		a1->Data1.Entity->field_6 = 0;
	}
	if (v4 == 2)
	{
		v11 = v2.Entity->field_6;
		v3 = (ChaoSomeUnknownA *)180;
		v2.Entity->field_6 = v11 + 1;
		if ((unsigned __int16)v11 > 0xB4u)
		{
			v3 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
			if (v3 && (v12 = v3->heldBy) != 0 && v12->heldBy == v3 && !v12->index)
			{
				v13 = v3->heldBy;
			}
			else
			{
				v13 = 0;
			}
			--v2.Entity->Rotation.z;
			v2.Entity->field_6 = 0;
			if (v13)
			{
				sub_545790(v13->pointerToOwner, v2.Entity->Rotation.x, v2.Entity->Rotation.z == 0); //PROBLEMATIC CALL, POSSIBLE STACK PROBLEM WATCH OUT YOURE GONNA CRASH AHHHH!!!
			}
			v14.Entity = (EntityData1 *)a1->Data1.Entity;
			if (v14.Undefined)
			{
				v14.Entity = (EntityData1 *)v14.Entity->Collision;
				if (v14.Undefined)
				{
					v14.Entity = (EntityData1 *)(v14.Entity->Rotation.y + 48);
				}
			}

			if(v14.Entity)
				v14.Entity->Position.x = v2.Entity->Rotation.z * 0.5f;

			v15 = a1->UnknownA_ptr;
			if (v15)
			{
				v15->field_28 = 0.5f * v2.Entity->Rotation.z;
			}
			if (v2.Entity->Rotation.x >= 24 && v2.Entity->Rotation.x <= 28 && v13)
			{
				v16 = v13->pointerToOwner;
				v17 = v16->Data1.Chao->pParamGC;
				v17->EvolutionProgress -= 0.05f;
				if (v17->EvolutionProgress < 0)
					v17->EvolutionProgress = 0;
			}
			if (v2.Entity->Rotation.x == SA2BFruit_Mushroom || v2.Entity->Rotation.x == SA2BFruit_MushroomAlt)
			{
				if (v13) 
				{
					v16 = v13->pointerToOwner;
					v17 = v16->Data1.Chao->pParamGC;
					if (v2.Entity->Rotation.x == SA2BFruit_Mushroom)
						v17->EvolutionProgress += 0.05f;
					else v17->EvolutionProgress += 0.10f;
				}
			}
			if (!v2.Entity->Rotation.z)
			{
				if (v13)
				{
					v16 = v13->pointerToOwner;
					v17 = v16->Data1.Chao->pParamGC;
					
					switch (v2.Entity->Rotation.x)
					{
					case SA2BFruit_HeroFruit:
						AL_ParameterAddAPos(v16, 0.15f);
						break;
					case SA2BFruit_DarkFruit:
						AL_ParameterAddAPos(v16, -0.15f);
						break;
					case SA2BFruit_HeartFruit:
						AL_EmotionAdd(v16, EM_ST_BREED, 10000);
						break;
					default:
						if (lastBiteFruit[v2.Entity->Rotation.x]) 
							lastBiteFruit[v2.Entity->Rotation.x]((ChaoData*)v17, a1);
						break;
					}
				}
			
				v22 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
				if (v22)
				{
					v3 = v22->heldBy;
					if (v3)
					{
						if (v3->heldBy == v22)
						{
							v3->setSomeIndex = 4;
						}
					}
				}
				v23 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
				if (v23 && v23->saveData)
				{
					ResetChaoObjectData((ChaoObjectData*)v23->saveData);
					v25 = a1->UnknownA_ptr;
					if (v25)
					{
						v25->field_8 = 0;
					}
				}
				a1->MainSub = DeleteObject_;
				goto LABEL_54;
			}
		}
	}
LABEL_54:
	if (v2.Entity->Status < 0)
	{
		v26 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
		if (v26)
		{
			v27 = v26->heldBy;
			if (v27)
			{
				if (v27->heldBy == v26)
				{
					v27->setSomeIndex = 6;
				}
			}
		}
		sub_530690(a1);
		v2.Entity->Action = 3;
		v2.Entity->NextAction = 0;
	}
	if (!sub_56D170((int)v3, a1))
	{
		v2.Entity->Action = 2;
		v2.Entity->NextAction = 0;
	}
}

void ALO_Fruit_Init()
{
	static_assert(offsetof(ChaoSomeUnknownA, pointerToOwner) == 0x3c, "pointerToOwner");
	static_assert(offsetof(ChaoSomeUnknownA, heldBy) == 0x40, "heldBy");
	static_assert(offsetof(ChaoSomeUnknownA, setSomeIndex) == 0x34, "setSomeIndex");

	FruitModels[SA2BFruit_MushroomAlt] = &object_02FC056C;
	WriteJump(ALO_FruitExecutor_Display, ALO_FruitExecutor_DisplayHack);
	WriteJump((void*)0x545C20, sub_545C20);
	
}