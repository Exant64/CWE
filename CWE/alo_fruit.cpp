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
#include <al_garden_info.h>
#include "renderfix.h"

std::vector<ChaoItemStats> ModAPI_FruitStats;


void ALO_FruitExecutor_DisplayHack(task *eax0)
{
	task *v1; // ebx
	taskwk *v2; // ebp
	
	float v13; // [esp+1Ch] [ebp-38h]
	float a2; // [esp+1Ch] [ebp-38h]

	v1 = eax0;
	v2 = eax0->twp;
	//this function is incorrectly named, its a bounds check (only used for chao stuff for some reason)
	//Rotation.x = fruit type
	//Rotation.z = fruit size (like the "bites" value)
	//rotation.y is actually the y rotation lol
	//i store the "visible onscreen" flag inside Index as you can see below here
	if (Scaletask_XYZ(eax0, 2.5, 2.5, 2.0))
	{
		v2->btimer = 1;
		v13 = v2->ang.z * 0.3f + 0.3f;
		if (v2->ang.x == 9)
		{
			v13 = v13 * 0.8f;
		}
		a2 = 0.8f * v13;

		AL_DayNightCycle_PushFallbackLight();

		// hack to fix normalization issues when scaling in current RF
		const NJS_VECTOR backupColor = LightsGC[LightIndex].lightColor;
		const float backupIntensity = Lights[LightIndex].intensity;
		if(RenderFix_IsEnabled()) {
			Lights[LightIndex].intensity /= a2;
			LightsGC[LightIndex].lightColor.x /= a2;
			LightsGC[LightIndex].lightColor.y /= a2;
			LightsGC[LightIndex].lightColor.z /= a2;
		}
		DoLighting(LightIndex);
		Lights[LightIndex].intensity = backupIntensity;
		LightsGC[LightIndex].lightColor = backupColor;
		
		njPushMatrixEx();
		njTranslateEx(&v2->pos);
		if (v2->flag >= 0)
		{
			njTranslate(NULL, 0, a2 * 0.55f + 0.8f, 0);
		}
		else
		{
			njTranslate(NULL, 0, 0.4f, 0);
		}
		
		if (v2->ang.z > 0)
		{
			RotateY(v2->ang.y);
			njPushMatrixEx();
			njScale(NULL, a2, a2, a2);
			if (ALO_Field_Find_(v1, 1, 150))
			{
				njControl3D |= 0x2400u;
			}
			ObjectRegistry::DrawObject<RenderFixBackwardsCompatibilityDrawObject>(ChaoItemCategory_Fruit, v2->ang.x);
			njControl3D &= ~0x2400u;
			njPopMatrixEx();

			if (RenderFix_IsEnabled() && v1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float *)&v1->UnknownA_ptr->field_30) {
				if (ALO_Field_Find_(v1, 1, CI_KIND_AL_SHADOW))
				{
					njTranslate(NULL, 0, -1.85f, 0);
				}
				else
				{
					njTranslate(NULL, 0, -1, 0);
				}

				njScale(NULL, a2, 0.7f, a2);
				
				rfapi_core->pDraw->AL_ShadowDraw();
			}
		}
		njPopMatrixEx();
		AL_DayNightCycle_PopFallbackLight();
		DoLighting(LightIndexBackupMaybe);
	}
	else if (!(v2->flag < 0)) { //if not being held by player
		v2->btimer = 0;
	}
}

#define RATIO(x,y) random >= (x / 100.0f) && random <= (y / 100.0f)
void ALO_FruitExecutor_Main_r(task* a1);
Trampoline ALO_FruitExecutor_Main_t(0x00545E40, 0x00545E4A, ALO_FruitExecutor_Main_r);
void ALO_FruitExecutor_Main_r(task *a1)
{
	ObjectFunc(original, ALO_FruitExecutor_Main_t.Target());
	original(a1);
	
	//if fruit ID inside api registered fruit list
	if (!ModAPI_MinimalFruit.count(a1->twp->ang.x)) return;
	
	//if not visible onscreen (animals only spawn if fruit is not visible)
	if (a1->twp->btimer) return;

	//random check (0.05% every frame it's onscreen)
	if (njRandom() > 0.0005) return;

	NJS_VECTOR velocity = { 0,1,0 };

	//find empty animal data slot, and check if garden has less than 10 animals
	void* data = sub_52F9E0(2);
	if (data && nbWorldEntry[2] < nbMaxEntry[2]) {
		//decrease bites
		a1->twp->ang.z--;

		if (a1->twp->ang.z <= 0)
		{
			//if no bites left, delete object
			ALW_SendCommand(a1, ALW_CMD_FINISH);

			ChaoSomeUnknownA *v23 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
			if (v23 && v23->saveData)
			{
				AL_ClearItemSaveInfo((ChaoObjectData*)v23->saveData);
				v23->saveData = 0;
			}

			a1->exec = DeleteObject_;
		}
		//purple chaos drive is the last vanilla animal, after that comes the new ones
		int type = SA2BAnimal_PurpleChaosDrive + 1; 
		float random = njRandom();

		for (const auto& minifruitentry : ModAPI_MinimalFruit[a1->twp->ang.x])
		{
			if(RATIO(minifruitentry.chanceMin, minifruitentry.chanceMax))
			{
				type = minifruitentry.minimalID;
				break;
			}
		}
		AL_MinimalExecutor_Load(type, &a1->twp->pos, 0, &velocity, (int)data);
	}
}

const int sub_530690Ptr = 0x530690;
signed int sub_530690(task *a1)
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
void sub_545790(task *a1, unsigned __int8 a2, int a3)
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
FastcallFunctionPointer(signed int, sub_56D170, (int a1, task *a2), 0x56D170);
void __cdecl sub_545C20(task *a1)
{
	ChaoSomeUnknownA *v1; // eax
	Data1Ptr v2; // ebx
	ChaoSomeUnknownA *v3; // ecx
	int v4; // edx
	Data1Ptr v5; // eax
	colliwk *v6; // eax
	Data1Ptr v7; // eax
	colliwk *v8; // eax
	Data1Ptr v9; // eax
	colliwk *v10; // eax
	__int16 v11; // ax
	ChaoSomeUnknownA *v12; // eax
	ChaoSomeUnknownA *v13; // edi
	Data1Ptr v14; // eax
	ObjUnknownA *v15; // eax
	task *v16; // eax
	ChaoDataBase *v17; // ecx
	ChaoSomeUnknownA *v22; // eax
	ChaoSomeUnknownA *v23; // eax
	ObjUnknownA *v25; // eax
	ChaoSomeUnknownA *v26; // eax
	ChaoSomeUnknownA *v27; // ecx

	v1 = (ChaoSomeUnknownA *)a1->UnknownA_ptr;
	v2.Entity = (taskwk *)a1->twp;

	//to stop warning 
	if (!a1->twp) return;

	v3 = 0;
	if (v1)
	{
		v3 = (ChaoSomeUnknownA *)(unsigned __int16)v1->setSomeIndex;
		v1->setSomeIndex = 0;
	}

	v4 = (int)v3;
	if (!v2.Entity->smode)
	{
		v5.Entity = (taskwk *)a1->twp;
		if (v5.Entity)
		{
			v6 = v5.Entity->cwp;
			if (v6)
			{
				v6->CollisionArray->field_4 &= 0xFFFFFFEF;
			}
		}
		v7.Entity = (taskwk *)a1->twp;
		v3 = (ChaoSomeUnknownA *)16;
		if (v7.Undefined)
		{
			v8 = v7.Entity->cwp;
			if (v8)
			{
				v8->CollisionArray[1].field_4 |= 0x10u;
			}
		}
		v9.Entity = (taskwk *)a1->twp;
		if (v9.Entity)
		{
			v10 = v9.Entity->cwp;
			if (v10)
			{
				v10->CollisionArray[2].field_4 |= 0x10u;
			}
		}
		++a1->twp->smode;
		a1->twp->wtimer = 0;
	}
	if (v4 == 2)
	{
		v11 = v2.Entity->wtimer;
		v3 = (ChaoSomeUnknownA *)180;
		v2.Entity->wtimer = v11 + 1;
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
			--v2.Entity->ang.z;
			v2.Entity->wtimer = 0;
			if (v13)
			{
				sub_545790(v13->pointerToOwner, v2.Entity->ang.x, v2.Entity->ang.z == 0); //PROBLEMATIC CALL, POSSIBLE STACK PROBLEM WATCH OUT YOURE GONNA CRASH AHHHH!!!
			}
			v14.Entity = (taskwk *)a1->twp;
			if (v14.Undefined)
			{
				v14.Entity = (taskwk *)v14.Entity->cwp;
				if (v14.Undefined)
				{
					v14.Entity = (taskwk *)(v14.Entity->ang.y + 48);
				}
			}

			if(v14.Entity)
				v14.Entity->pos.x = v2.Entity->ang.z * 0.5f;

			v15 = a1->UnknownA_ptr;
			if (v15)
			{
				v15->field_28 = 0.5f * v2.Entity->ang.z;
			}
			if (v2.Entity->ang.x >= 24 && v2.Entity->ang.x <= 28 && v13)
			{
				v16 = v13->pointerToOwner;
				v17 = GET_CHAOPARAM(v16);
				v17->EvolutionProgress -= 0.05f;
				if (v17->EvolutionProgress < 0)
					v17->EvolutionProgress = 0;
			}
			if (v2.Entity->ang.x == SA2BFruit_Mushroom || v2.Entity->ang.x == SA2BFruit_MushroomAlt)
			{
				if (v13) 
				{
					v16 = v13->pointerToOwner;
					v17 = GET_CHAOPARAM(v16);
					if (v2.Entity->ang.x == SA2BFruit_Mushroom)
						v17->EvolutionProgress += 0.05f;
					else v17->EvolutionProgress += 0.10f;
				}
			}
			if (!v2.Entity->ang.z)
			{
				if (v13)
				{
					v16 = v13->pointerToOwner;
					v17 = GET_CHAOPARAM(v16);
					
					switch (v2.Entity->ang.x)
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
						if (lastBiteFruit[v2.Entity->ang.x]) 
							lastBiteFruit[v2.Entity->ang.x]((ChaoData*)v17, a1);
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
					AL_ClearItemSaveInfo((ChaoObjectData*)v23->saveData);
					v25 = a1->UnknownA_ptr;
					if (v25)
					{
						v25->field_8 = 0;
					}
				}
				a1->exec = DeleteObject_;
				goto LABEL_54;
			}
		}
	}
LABEL_54:
	if (v2.Entity->flag < 0)
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
		v2.Entity->mode = 3;
		v2.Entity->smode = 0;
	}
	if (!sub_56D170((int)v3, a1))
	{
		v2.Entity->mode = 2;
		v2.Entity->smode = 0;
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