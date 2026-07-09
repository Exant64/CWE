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
	if (AL_IsOnScreen2(eax0, 2.5, 2.0)) {
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
			SaveControl3D();

			RotateY(v2->ang.y);
			njPushMatrixEx();
			njScale(NULL, a2, a2, a2);
			if (AL_IsHitKindWithNum(v1, 1, CI_KIND_AL_SHADOW))
			{
				_nj_control_3d_flag_ |= 0x2400u;
			}
			ObjectRegistry::DrawObject<RenderFixBackwardsCompatibilityDrawObject>(ALW_CATEGORY_FRUIT, v2->ang.x);
			_nj_control_3d_flag_ &= ~0x2400u;
			LoadControl3D();
			njPopMatrixEx();

			if (RenderFix_IsEnabled() && v1->fwp && ChaoGlobal.CamDistShadowCutLev2 > GET_ALW_ENTRY_WORK(eax0)->CamDist) {
				if (AL_IsHitKindWithNum(v1, 1, CI_KIND_AL_SHADOW))
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

			if (AL_GetItemSaveInfo(a1))
			{
				AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)AL_GetItemSaveInfo(a1));
				AL_ClearItemSaveInfoPtr(a1);
			}

			a1->exec = DestroyTask;
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
	ALW_ENTRY_WORK *v1; // eax
	taskwk* v2; // ebx
	int v3; // ecx
	int v4; // edx
	colliwk *v6; // eax
	colliwk *v8; // eax
	colliwk *v10; // eax
	__int16 v11; // ax
	ALW_ENTRY_WORK *v12; // eax
	ALW_ENTRY_WORK *v13; // edi
	taskwk* v14; // eax
	ObjUnknownA *v15; // eax
	task *v16; // eax
	CHAO_PARAM_GC *v17; // ecx
	ALW_ENTRY_WORK *v22; // eax
	ALW_ENTRY_WORK *v23; // eax
	ObjUnknownA *v25; // eax
	ALW_ENTRY_WORK *v26; // eax
	ALW_ENTRY_WORK *v27; // ecx

	v1 = (ALW_ENTRY_WORK *)a1->fwp;
	v2 = a1->twp;

	//to stop warning 
	if (!a1->twp) return;

	v3 = ALW_RecieveCommand(a1);

	v4 = (int)v3;
	if (!v2->smode)
	{
		if (v2)
		{
			v6 = v2->cwp;
			if (v6)
			{
				v6->info->attr &= 0xFFFFFFEF;
			}
		}
		if (v2)
		{
			v8 = v2->cwp;
			if (v8)
			{
				v8->info[1].attr |= 0x10u;
			}
		}
		if (v2)
		{
			v10 = v2->cwp;
			if (v10)
			{
				v10->info[2].attr |= 0x10u;
			}
		}
		++a1->twp->smode;
		a1->twp->wtimer = 0;
	}
	if (v4 == 2)
	{
		v11 = v2->wtimer;
		v2->wtimer = v11 + 1;
		if ((unsigned __int16)v11 > 180)
		{
			v13 = ALW_IsCommunicationEx(a1, 0);

			--v2->ang.z;
			v2->wtimer = 0;

			if (v13)
			{
				sub_545790(v13->tp, v2->ang.x, v2->ang.z == 0); //PROBLEMATIC CALL, POSSIBLE STACK PROBLEM WATCH OUT YOURE GONNA CRASH AHHHH!!!
			}

			v14 = (taskwk *)a1->twp;
			if (v2)
			{
				v14 = (taskwk *)v2->cwp;
				if (v14)
				{
					v14 = (taskwk *)(v14->ang.y + 48);
				}
			}

			if(v14)
				v14->pos.x = v2->ang.z * 0.5f;

			ALW_SetHeldRadius(a1, 0.5f * v2->ang.z);

			if (v2->ang.x >= 24 && v2->ang.x <= 28 && v13)
			{
				v16 = v13->tp;
				v17 = GET_CHAOPARAM(v16);
				v17->body.growth -= 0.05f;
				if (v17->body.growth < 0)
					v17->body.growth = 0;
			}
			if (v2->ang.x == ChaoFruit_Mushroom || v2->ang.x == ChaoFruit_MushroomAlt)
			{
				if (v13) 
				{
					v16 = v13->tp;
					v17 = GET_CHAOPARAM(v16);
					if (v2->ang.x == ChaoFruit_Mushroom)
						v17->body.growth += 0.05f;
					else v17->body.growth += 0.10f;
				}
			}
			if (!v2->ang.z)
			{
				if (v13)
				{
					v16 = v13->tp;
					v17 = GET_CHAOPARAM(v16);
					
					switch (v2->ang.x)
					{
					case ChaoFruit_HeroFruit:
						AL_ParameterAddAPos(v16, 0.15f);
						break;
					case ChaoFruit_DarkFruit:
						AL_ParameterAddAPos(v16, -0.15f);
						break;
					case ChaoFruit_HeartFruit:
						AL_EmotionAdd(v16, EM_ST_BREED, 10000);
						break;
					default:
						if (lastBiteFruit[v2->ang.x]) 
							lastBiteFruit[v2->ang.x](v16, (CHAO_SAVE_INFO*)v17, a1);
						break;
					}
				}
			
				ALW_SendCommand(a1, 4);

				if (AL_GetItemSaveInfo(a1)) {
					AL_ClearItemSaveInfo((ITEM_SAVE_INFO*)AL_GetItemSaveInfo(a1));
					AL_ClearItemSaveInfoPtr(a1);
				}

				a1->exec = DestroyTask;
				goto LABEL_54;
			}
		}
	}
LABEL_54:
	if (v2->flag < 0)
	{
		ALW_SendCommand(a1, 6);
		sub_530690(a1);
		v2->mode = 3;
		v2->smode = 0;
	}
	if (!sub_56D170((int)v3, a1))
	{
		v2->mode = 2;
		v2->smode = 0;
	}
}

void ALO_Fruit_Init()
{
	FruitModels[ChaoFruit_MushroomAlt] = &object_02FC056C;
	WriteJump(ALO_FruitExecutor_Display, ALO_FruitExecutor_DisplayHack);
	WriteJump((void*)0x545C20, sub_545C20);
}