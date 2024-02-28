#pragma once
#include "stdafx.h"

#include "ninja_functions.h"
#include "al_world.h"
#include "Chao.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "al_toy_move.h"
enum
{
	MD_COFFIN_IDLE,
	MD_COFFIN_PICKEDUP,
	MD_COFFIN_SWIM,
	MD_COFFIN_FLY_UP,
	MD_COFFIN_FLY_DOWN_START,
	MD_COFFIN_FLY_DOWN
};
//extern NJS_OBJECT object_00F005A0;
void ALO_Coffin_Displayer_(ObjectMaster* a1)
{
	if (a1->Data1.Entity->field_6)
	{
		DoLighting(0);
		njPushMatrixEx();
		njSetTexture((NJS_TEXLIST*)0x01366AFC);

		//njControl3D_Backup();

		//njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
		//njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

		//EnableChunkMaterialFlags();
		//DisableChunkMaterialFlags();
		//SetChunkMaterialFlags(8);
		//SetChunkTextureIndexA(117);
		//SetChunkTextureIndexB(117);
		//*(int*)0x03D0856C = 0xFFFFFFFF;
		//*(unsigned char*)0x03D0856F = (255 - (a1->Data1->Rotation.x / 335.0f) * 255.0f);
		//*(float*)0x3D0857C = -0.5f;
		njTranslate(NULL, a1->Data1.Entity->Position.x, a1->Data1.Entity->Position.y, a1->Data1.Entity->Position.z - 2.65f);
		njRotateY(NULL, a1->Data1.Entity->Rotation.y);
		chCnkDrawObject((NJS_OBJECT*)0x01301BB4);//0x03898530 - coffin
		//chCnkDrawObject(&object_0349A900);
		njPopMatrixEx();

		//DisableChunkMaterialFlags();
		//njControl3D_Restore();
	}
}
//FunctionPointer(float, CalcCamDist, (ObjectMaster* a1), 0x00736460);
void ALO_Coffin_Displayer(ObjectMaster* a1)
{
	//float a3 = -1.0 - CalcCamDist(a1) * 0.000099999997;
	//DrawModelCallback_Queue((void(__cdecl*)(void*))ALO_Coffin_Displayer_, a1, a3, QueuedModelFlagsB_EnableZWrite);
}
//FunctionPointer(void, CCL_Enable, (EntityData1* a1, int a2), 0x41C1F0);
//FunctionPointer(void, CCL_Disable, (EntityData1* a1, int a2), 0x041C220);
void ALO_Coffin_Main(ObjectMaster* a1)
{
	//if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE)
		//a1->Data1->Action = MD_COFFIN_FLY_UP;
	switch (a1->Data1.Entity->Action)
	{
	case MD_COFFIN_IDLE:
		//CCL_Enable(a1->Data1, 0);
		if (ALW_IsHeld(a1))
		{
			a1->Data1.Entity->Action = MD_COFFIN_PICKEDUP;

		}
		break;
	case MD_COFFIN_PICKEDUP:
		//CCL_Disable(a1->Data1, 0);
		if (!AL_TraceHoldingPosition(0, a1))
		{
			a1->Data1.Entity->Action = MD_COFFIN_IDLE;
		}
		break;
	case MD_COFFIN_FLY_UP:
		a1->Data1.Entity->field_6 = 1;
		a1->Data1.Entity->Position.y += 0.15f;
		a1->Data1.Entity->Rotation.y += 4096;
		a1->Data1.Entity->Rotation.x += 5;

		if (a1->Data1.Entity->Rotation.x > 335)
			a1->Data1.Entity->Rotation.x = 335;

		if (a1->Data1.Entity->Position.y > 10)
		{
			a1->Data1.Entity->Position.y = 10;
			a1->Data1.Entity->Action = MD_COFFIN_FLY_DOWN_START;
		}
		break;
	case MD_COFFIN_FLY_DOWN_START:
		a1->Data1.Entity->Position = a1->Data1.Entity->Scale;
		a1->Data1.Entity->Position.y = 10;
		a1->Data1.Entity->Action = MD_COFFIN_FLY_DOWN;
		break;
	case MD_COFFIN_FLY_DOWN:
		a1->Data1.Entity->Position.y -= 0.15f;
		a1->Data1.Entity->Rotation.y -= 4096;
		a1->Data1.Entity->Rotation.x -= 5;
		if (a1->Data1.Entity->Rotation.x < 0)
			a1->Data1.Entity->Rotation.x = 0;
		if (a1->Data1.Entity->Position.y <= a1->Data1.Entity->Scale.y)
		{
			a1->Data1.Entity->Position.y = a1->Data1.Entity->Scale.y;
			a1->Data1.Entity->Action = MD_COFFIN_IDLE;
		}
		break;
	}
	//ALO_Coffin_Displayer(a1);
	AddToCollisionList(a1);
}
CollisionData coffinColl =
{ 0, 3191, 32768, {  0.0,  1.0,  0.0 },  2.0,  0.0,  0.0, 0, 0, 0, 0 };
void ALO_Coffin_Load(ObjectMaster* a1)
{
	a1->MainSub = ALO_Coffin_Main;

	AL_Toy_Move_Register(a1, 2);
}

void ALO_Coffin_Create(NJS_VECTOR* a1, int rotY)
{
	ObjectMaster* p = LoadObject(4, "ALO_Coffin", ALO_Coffin_Load, LoadObj_Data1);
	InitCollision(p, &coffinColl, 1, 4);

	p->DisplaySub = ALO_Coffin_Displayer_;
	p->Data1.Entity->field_6 = 1; //enable display
	p->Data1.Entity->Position = *a1;
	p->Data1.Entity->Rotation.y = rotY;
	p->Data1.Entity->Scale = p->Data1.Entity->Position; //scale = default pos
	//p->Data1->Position = { 10,0,0 };

}