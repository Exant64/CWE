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

enum
{
	MD_BOAT_IDLE,
	MD_BOAT_PICKEDUP,
	MD_BOAT_SWIM,
	MD_BOAT_FLY_UP,
	MD_BOAT_FLY_DOWN_START,
	MD_BOAT_FLY_DOWN
};

void ALO_Boat_Displayer(ObjectMaster* a1)
{
	DoLighting(0);
	njPushMatrixEx();
	njSetTexture((NJS_TEXLIST*)0x01366AFC);

	//EnableChunkMaterialFlags();
	//SetChunkMaterialFlags(8);
	Has_texlist_batadvPlayerChara_in_it[3] = Has_texlist_batadvPlayerChara_in_it[3] & 0x3FFFFFF | 0x94000000;
	//*(int*)0x03D0856C = 0xFFFFFFFF;
	*(float *)0x01AED2D0 = (-(a1->Data1.Entity->Rotation.x / 335.0f));

	njTranslateEx(&a1->Data1.Entity->Position);
	njRotateY(NULL, a1->Data1.Entity->Rotation.y);
	chCnkDrawObject((NJS_OBJECT*)0x0130111C);

	if (a1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&a1->UnknownA_ptr->field_30)
	{
		njTranslate(NULL, 0, 0.4f, 0);

		njScale(NULL, 1.5f, 0.7f, 2.25f);
		DrawChaoWorldShadow();
	}

	njPopMatrixEx();

	*(float*)0x01AED2D0 = 0;
	//DisableChunkMaterialFlags();
	
	DoLighting(LightIndexBackupMaybe);
}



void ALO_Boat_Main(ObjectMaster* a1)
{
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE)
	{
		ALW_CommunicationOff(a1);
		a1->Data1.Entity->Action = MD_BOAT_FLY_UP;
	}

	switch (a1->Data1.Entity->Action)
	{
	case MD_BOAT_IDLE:
		AL_Toy_Move_Update(a1);
		
		//if chao is interacting with it, disable the pick up collision
		if (!ALW_IsCommunicating(a1)) 
			a1->Data1.Entity->Status |= 0x240u;
		else
			a1->Data1.Entity->Status &= ~0x240u;

		if (ALW_IsHeld(a1)) 
		{
			a1->Data1.Entity->Scale = a1->Data1.Entity->Position;
			a1->Data1.Entity->Action = MD_BOAT_PICKEDUP;
		}
		break;	
	case MD_BOAT_PICKEDUP:
		CCL_Disable(a1, 1);
		if (!AL_TraceHoldingPosition(0, a1))
		{
			a1->Data1.Entity->Action = MD_BOAT_IDLE;
		}
		break;
	case MD_BOAT_FLY_UP:
		a1->Data1.Entity->Position.y += 0.15f;
		a1->Data1.Entity->Rotation.y += 4096;
		a1->Data1.Entity->Rotation.x += 5;

		if (a1->Data1.Entity->Rotation.x > 335)
			a1->Data1.Entity->Rotation.x = 335;

		if (a1->Data1.Entity->Position.y > 10)
		{
			a1->Data1.Entity->Position.y = 10;
			a1->Data1.Entity->Action = MD_BOAT_FLY_DOWN_START;
		}
		break;
	case MD_BOAT_FLY_DOWN_START:
		a1->Data1.Entity->Position = a1->Data1.Entity->Scale;
		a1->Data1.Entity->Position.y = 10;
		a1->Data1.Entity->Action = MD_BOAT_FLY_DOWN;
		break;
	case MD_BOAT_FLY_DOWN:
		a1->Data1.Entity->Position.y -= 0.15f;
		a1->Data1.Entity->Rotation.y -= 4096;
		a1->Data1.Entity->Rotation.x -= 5;
		if (a1->Data1.Entity->Rotation.x < 0)
			a1->Data1.Entity->Rotation.x = 0;
		if (a1->Data1.Entity->Position.y <= a1->Data1.Entity->Scale.y)
		{
			a1->Data1.Entity->Rotation.x = 0;
			a1->Data1.Entity->Position.y = a1->Data1.Entity->Scale.y;
			a1->Data1.Entity->Action = MD_BOAT_IDLE;
		}
		break;
	}
	//ALO_Boat_Displayer(a1);
	AddToCollisionList(a1);
}


CollisionData baotColli =
{ 0, 3191, 32768, {  0.0,  1.0,  0.0 },  2.0,  0.0,  0.0, 0, 0, 0, 0 };


void ALO_Boat_Init(ObjectMaster *a1)
{
	a1->MainSub = ALO_Boat_Main;
	a1->DeleteSub = (ObjectFuncPtr)0x0057B9B0;
	AL_Toy_Move_Register(a1, ALW_KIND_BOAT);
}


void ALO_BoatCreate(NJS_POINT3* pPos, Angle ang) {
	ObjectMaster* p = LoadObject(4, "ALO_Boat", ALO_Boat_Init, LoadObj_Data1);
	//InitCollision(p, (CollisionData*)&boatColliT, 2, 4);

	p->DisplaySub = ALO_Boat_Displayer;
	p->Data1.Entity->Position = *pPos;
	p->Data1.Entity->Rotation.y = ang;
	p->Data1.Entity->Scale = p->Data1.Entity->Position; //scale = default pos
	//p->Data1->Position = { 10,0,0 };
	AL_Toy_Move_Init(p, (CCL_INFO*)&baotColli);

}