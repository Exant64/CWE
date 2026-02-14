#pragma once
#include "stdafx.h"

#include "ninja_functions.h"
#include "al_world.h"
#include "Chao.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "al_toy_move.h"

#include "al_behavior/al_intention.h"
#include "ChaoMain.h"
#include "renderfix.h"

enum
{
	MD_FLOAT_IDLE,
	MD_FLOAT_PICKEDUP,
	MD_FLOAT_SWIM,
	MD_FLOAT_FLY_UP,
	MD_FLOAT_FLY_DOWN_START,
	MD_FLOAT_FLY_DOWN
};
//extern NJS_OBJECT object_00F005A0;
void ALO_Float_Displayer_(ObjectMaster* a1)
{
	if (a1->Data1.Entity->field_6)
	{
		DoLighting(0);
		njPushMatrixEx();
		njSetTexture((NJS_TEXLIST*)0x01366AFC);

		//njControl3D_Backup();

		//njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
		//njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

		Has_texlist_batadvPlayerChara_in_it[3] = Has_texlist_batadvPlayerChara_in_it[3] & 0x3FFFFFF | 0x94000000;
		*(float*)0x01AED2D0 = (-(a1->Data1.Entity->Rotation.x / 335.0f));

		njTranslateEx(&a1->Data1.Entity->Position);
		njRotateY(NULL, a1->Data1.Entity->Rotation.y);
		njTranslate(NULL, 0, 0, -2.65f);
		chCnkDrawObject((NJS_OBJECT*)0x013005A0);//0x03898530 - coffin
		
		if (RenderFix_IsEnabled() && a1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&a1->UnknownA_ptr->field_30) {
			njTranslate(NULL, 0, 0.4f, 2.65f);
			njScale(NULL, 1.5f, 0.7f, 1.5f);

			rfapi_core->pDraw->AL_ShadowDraw();
		}

		njPopMatrixEx();

		*(float*)0x01AED2D0 = 0;
	}
}
//FunctionPointer(float, CalcCamDist, (ObjectMaster* a1), 0x00736460);
void ALO_Float_Displayer(ObjectMaster* a1)
{
	//float a3 = -1.0 - CalcCamDist(a1) * 0.000099999997;
	//DrawModelCallback_Queue((void(__cdecl*)(void*))ALO_Float_Displayer_, a1, a3, QueuedModelFlagsB_EnableZWrite);
}
//FunctionPointer(void, CCL_Enable, (EntityData1* a1, int a2), 0x41C1F0);
//FunctionPointer(void, CCL_Disable, (EntityData1* a1, int a2), 0x041C220);
void ALO_Float_Main(ObjectMaster* a1)
{
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE)
	{
		ALW_CommunicationOff(a1);
		a1->Data1.Entity->Action = MD_FLOAT_FLY_UP;
	}
	//if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE)
		//a1->Data1->Action = MD_FLOAT_FLY_UP;
	switch (a1->Data1.Entity->Action)
	{
	case MD_FLOAT_IDLE:
		AL_Toy_Move_Update(a1);

		//if chao is interacting with it, disable the pick up collision
		if (!ALW_IsCommunicating(a1))
			a1->Data1.Entity->Status |= 0x240u;
		else
			a1->Data1.Entity->Status &= ~0x240u;

		if (ALW_IsHeld(a1))
		{
			a1->Data1.Entity->Scale = a1->Data1.Entity->Position;
			a1->Data1.Entity->Action = MD_FLOAT_PICKEDUP;

		}
		break;
	case MD_FLOAT_PICKEDUP:
		//CCL_Disable(a1->Data1, 0);
		if (!AL_TraceHoldingPosition(0,a1))
		{
			a1->Data1.Entity->Action = MD_FLOAT_IDLE;
		}
		break;
	case MD_FLOAT_FLY_UP:
		a1->Data1.Entity->field_6 = 1;
		a1->Data1.Entity->Position.y += 0.15f;
		a1->Data1.Entity->Rotation.y += 4096;
		a1->Data1.Entity->Rotation.x += 5;

		if (a1->Data1.Entity->Rotation.x > 335)
			a1->Data1.Entity->Rotation.x = 335;

		if (a1->Data1.Entity->Position.y > 10)
		{
			a1->Data1.Entity->Position.y = 10;
			a1->Data1.Entity->Action = MD_FLOAT_FLY_DOWN_START;
		}
		break;
	case MD_FLOAT_FLY_DOWN_START:
		a1->Data1.Entity->Position = a1->Data1.Entity->Scale;
		a1->Data1.Entity->Position.y = 10;
		a1->Data1.Entity->Action = MD_FLOAT_FLY_DOWN;
		break;
	case MD_FLOAT_FLY_DOWN:
		a1->Data1.Entity->Position.y -= 0.15f;
		a1->Data1.Entity->Rotation.y -= 4096;
		a1->Data1.Entity->Rotation.x -= 5;
		if (a1->Data1.Entity->Rotation.x < 0)
			a1->Data1.Entity->Rotation.x = 0;
		if (a1->Data1.Entity->Position.y <= a1->Data1.Entity->Scale.y)
		{
			a1->Data1.Entity->Rotation.x = 0;
			a1->Data1.Entity->Position.y = a1->Data1.Entity->Scale.y;
			a1->Data1.Entity->Action = MD_FLOAT_IDLE;
		}
		break;
	}
	//ALO_Float_Displayer(a1);
	AddToCollisionList(a1);
}
CollisionData floatColl =
{ 0, 3191, 32768, {  0.0,  1.0,  0.0 },  2.0,  0.0,  0.0, 0, 0, 0, 0 };
void ALO_Float_Load(ObjectMaster* a1)
{
	a1->MainSub = ALO_Float_Main;
	a1->DeleteSub = (ObjectFuncPtr)0x0057B9B0;
	AL_Toy_Move_Register(a1, ALW_KIND_UKIWA);
}

void ALO_FloatCreate(NJS_POINT3* pPos, Angle ang) {
	ObjectMaster* p = LoadObject(4, "ALO_Float", ALO_Float_Load, LoadObj_Data1);
	//InitCollision(p, (CollisionData*)colli_info, 2, 4);

	p->DisplaySub = ALO_Float_Displayer_;
	p->Data1.Entity->field_6 = 1; //enable display
	p->Data1.Entity->Position = *pPos;
	p->Data1.Entity->Rotation.y = ang;
	p->Data1.Entity->Scale = p->Data1.Entity->Position; //scale = default pos
	//p->Data1->Position = { 10,0,0 };
	AL_Toy_Move_Init(p, (CCL_INFO*)&floatColl);
}