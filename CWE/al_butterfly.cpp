#include "stdafx.h"

#include "ALifeSDK_Functions.h"
#include "ninja_functions.h"
#include "ChaoMain.h"
#include "al_butterfly.h"
#include <cmath>
#include "Chao.h"
#include "al_draw.h"

#pragma warning(push)
#pragma warning( disable: 4838 )
Sint32 cnk_Mesh_001_vtx[] = { 0x1F0029, 0x50000, 0xB346A84Cu, 0x3F17FCFB, 0x3E88A3BD, 0x3F760919, 0xBDE9260Eu, 0x3E80E3D8, 0x3EA9BF82, 0x3F88D245, 0xBF48B19Fu, 0x3F76091B, 0xBDE92614u, 0x3E80E3D9, 0xB1457B2Fu, 0xBBC22218u, 0xBB8CA706u, 0x3F76091A, 0xBDE92613u, 0x3E80E3D8, 0x3EA9BF85, 0xBF06D944u, 0xBFC0B63Au, 0x3F760919, 0xBDE92613u, 0x3E80E3D8, 0x32B08051, 0xBF80406Au, 0xBEE8D1ECu, 0x3F760919, 0xBDE92613u, 0x3E80E3D7, 0xFF, 0x0 };

Sint16 cnk_Mesh_001_poly[] = { 0x2513, 0x4, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFF, 0x8, 0x6016, 0x1041, 0x11, 0x1, 0xFFFBu, 0x0, 0x4, 0x2, 0x1, 0xFD, 0x2, 0x2, 0x4, 0x60, 0x3, 0xFD, 0xFE, 0x4, 0x4, 0xFE, 0xFF };

NJS_CNK_MODEL cnk_Mesh_001 = { cnk_Mesh_001_vtx, cnk_Mesh_001_poly, { 0.1657696f, 0.03347558f, -0.6193435f }, 1.061469f };

NJS_OBJECT object_butterfly_wing1 = { NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_BREAK | NJD_EVAL_SHAPE_SKIP, &cnk_Mesh_001, 0, 0, 0, 0, 0, 0, 1, 1, 1, NULL, NULL };

Sint32 cnk_Mesh_vtx[] = { 0x1F0029, 0x50000, 0x31D685E1, 0xBF80406Au, 0xBEE8D1EAu, 0xBF737871u, 0xBE0262DEu, 0x3E902935, 0xBEBDDC2Bu, 0xBF081DF1u, 0xBFBF4F3Eu, 0xBF737872u, 0xBE0262DEu, 0x3E902934, 0x3186B084, 0xBBC22219u, 0xBB8CA707u, 0xBF73786Fu, 0xBE0262DEu, 0x3E902934, 0xBEBDDC2Au, 0x3F882FEE, 0xBF45E3A8u, 0xBF737871u, 0xBE0262DFu, 0x3E902934, 0x312D41FB, 0x3F17FCFA, 0x3E88A3BC, 0xBF737870u, 0xBE0262DEu, 0x3E902934, 0xFF, 0x0 };

Sint16 cnk_Mesh_poly[] = { 0x2513, 0x4, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFF, 0x8, 0x6016, 0x1041, 0x11, 0x1, 0xFFFBu, 0x0, 0x4, 0xFE, 0x1, 0xFD, 0xFE, 0x2, 0x4, 0x60, 0x3, 0xFD, 0x2, 0x4, 0x4, 0x2, 0xFF };

NJS_CNK_MODEL cnk_Mesh = { cnk_Mesh_vtx, cnk_Mesh_poly, { -0.1854102f, 0.03099847f, -0.6138658f }, 1.061469f };

NJS_OBJECT object_butterfly_wing2 = { NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_BREAK | NJD_EVAL_SHAPE_SKIP, &cnk_Mesh, 0, 0, 0, 0, 0, 0, 1, 1, 1, NULL, NULL };

NJS_OBJECT object_butterfly_root = { NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_HIDE | NJD_EVAL_SHAPE_SKIP, NULL, 0, 0, 0, 0, 0, 0, 1, 1, 1, &object_butterfly_wing2, NULL };
#pragma warning(pop)

enum {
	MD_INIT = 0,
	MD_IDLE,
	MD_FLY,
	MD_LANDING
};

enum {
	MD_FLAP_TARGET = 0,
	MD_FLAP_ANIMATE
};

struct butterflywk {
	Uint8 mode;
	Uint8 flapmode;
	int timer;

	ObjectMaster* target;
	NJS_VECTOR position;
	Angle ang;

	struct {
		NJS_ARGB color;
		Sint16 tex;
		float scl;
	} attr;
	
	float speed;
	int sine;

	Angle flap;
	Angle targetFlap;

	NJS_POINT3 targetPosition;
};

#define GET_BUTTERFLY_WK ((butterflywk*)(a1->Data1.Undefined))
void ALO_Butterfly_Display(ObjectMaster* a1)
{
	butterflywk* work = GET_BUTTERFLY_WK;

	njPushMatrixEx();
	{
		ChunkMatEnable = 1;
		ChunkMatFlag = ChunkMaterialFlagsEnum::UseChunkObjectColor;

		NJS_COLOR finalColor;
		finalColor.argb.a = (Uint8)(work->attr.color.a * 255.0f);
		finalColor.argb.r = (Uint8)(work->attr.color.r * 255.0f);
		finalColor.argb.g = (Uint8)(work->attr.color.g * 255.0f);
		finalColor.argb.b = (Uint8)(work->attr.color.b * 255.0f);
		ChunkObjectColor = finalColor.color;

		njSetTexture(&CWE_OBJECT_TEXLIST);
		njTranslateEx(&work->position);
		njRotateY(NULL, work->ang);

		njScale(NULL, work->attr.scl, work->attr.scl, work->attr.scl);



		njPushMatrixEx();
		{
			njRotateY(NULL, -work->flap);
			chCnkDrawObject(&object_butterfly_wing1);
		}
		njPopMatrixEx();
		njPushMatrixEx();
		{
			njRotateY(NULL, work->flap);
			chCnkDrawObject(&object_butterfly_wing2);
		}
		njPopMatrixEx();

		ChunkMatEnable = 0;
	}
	njPopMatrixEx();
}

int __cdecl AdjustAngle(__int16 bams_a, unsigned __int16 bams_b, int dang)
{
	int result; // eax
	__int16 v4; // cx

	result = bams_b;
	v4 = bams_b - bams_a;
	if ((signed __int16)(bams_b - bams_a) > dang || v4 < -dang)
	{
		if (v4 >= 0)
		{
			result = (unsigned __int16)(dang + bams_a);
		}
		else
		{
			result = (unsigned __int16)(bams_a - dang);
		}
	}
	return result;
}
FunctionPointer(float, CalculateFalloffPosition, (float x, float y, float z, Rotation* a4), 0x00494C30);

NJS_ARGB colors[] = {
	{1,1,1,1},
	{1,1,0,0},
	{1,0,1,0},
	{1,0,0,1}
};
const int sinIncrement = 1024 * 6;
const float speed = 0.15f;
void ALO_Butterfly_Main(ObjectMaster* a1)
{
	butterflywk* work = GET_BUTTERFLY_WK;

	switch (work->mode) {
	case MD_INIT:
		work->target = GetChaoObject(3, 1);

		sub_534F80((int)&stru_1A15938[1], &work->position, stru_1A15938[1].nbIndex);
		sub_534F80((int)&stru_1A15938[1], &work->targetPosition, stru_1A15938[1].nbIndex);

		if (work->target)
			work->targetPosition = work->target->Data1.Entity->Position;
		work->speed = 0.15f;
		work->attr.color = colors[rand() % 4];
		work->attr.scl = 0.4f + njRandom() * 0.75f; //(0.75 - 1.15)
		work->mode = MD_LANDING;
		break;
	case MD_FLY:
		{
			work->flapmode = MD_FLAP_ANIMATE;

			work->timer--;
			if (work->timer < 0) {
				work->timer = (int)(njRandom() * 120) + 30;

				work->speed = 0.15f + njRandom() * 0.15f;

				work->target = GetChaoObject(3, 1);
				work->targetPosition.y = 2.5f + njRandom() * 2.5f;

				if (njRandom() < 0.15f)
					work->mode = MD_IDLE;
			}

			float distX = work->targetPosition.x - work->position.x;
			float distZ = work->targetPosition.z - work->position.z;
			float dist = distX * distX + distZ * distZ;
			if (dist < 10.0f) {
				//if (work->target)
					//work->targetPosition = work->target->Data1.Entity->Position;
			}

			Rotation fake = { 0,0,0 };
			work->position.x += njSin(work->ang) * speed;
			work->position.y = CalculateFalloffPosition(work->position.x, 25, work->position.z, &fake) + work->targetPosition.y;
			work->position.z += njCos(work->ang) * speed;

			//rotate towards target position
			work->ang = AdjustAngle(
				work->ang, 
				NJM_RAD_ANG(atan2(distX, distZ)), 
				288
			);
		}
		break;
	case MD_LANDING:
		//normally, we would get the flower's target spots here
		//probably stored in the flower's work as a pointer to a struct that has the positions
		work->position = work->targetPosition;
		work->position.y += 3;

		work->timer = 30 + (int)(njRandom() * 120);
		work->mode = MD_IDLE;
		break;
	case MD_IDLE:
		work->flapmode = MD_FLAP_TARGET;

		work->timer--;
		if (work->timer < 0) {
			work->timer = 30 + (int)(njRandom() * 120);

			if (njRandom() < 0.5f)
				work->targetFlap = NJM_DEG_ANG(55);
			else 
				work->targetFlap = NJM_DEG_ANG(5);

			if (njRandom() < 0.15f)
				work->mode = MD_FLY;
		}

		break;
	}

	switch (work->flapmode) {
	case MD_FLAP_TARGET:
		work->flap = AdjustAngle(work->flap, work->targetFlap, 150);
		break;
	case MD_FLAP_ANIMATE:
		work->sine += sinIncrement;

		float sine = (njSin(work->sine) + 1) / 2.0f;
		float angle = 55;
		work->flap = NJM_DEG_ANG(sine * angle);
		break;
	}
	

	//shadow code
	//sub_540FD0(&a1->Data1.Entity->Position, 0.1f);
}

void ALO_Butterfly_Load()
{
	int count = (int)(njRandom() * 3.f) + 2;
	for (int i = 0; i < count; i++)
	{
		ObjectMaster* loaded = LoadObject(4, "ALO_Butterfly", ALO_Butterfly_Main, (LoadObj)0);
		loaded->field_1C = ALO_Butterfly_Display;
		loaded->Data1.Undefined = AllocateArray(sizeof(butterflywk), 1, (char*)__FILE__, __LINE__);
	}
}