#include "stdafx.h"
#include "al_piano.h"
#include "ALifeSDK_Functions.h"
#include "ChaoMain.h"
#include "Chao.h"
#include "al_toy_move.h"
#include "al_world.h"
#include "al_stage.h"

#include "al_behavior/al_intention.h"
#include "ChaoMain.h"

bool __cdecl SetPianoWaypoint(ObjectMaster* a2, NJS_VECTOR* a1)
{
	if (a2)
	{
		EntityData1* v2 = a2->Data1.Entity;
		Angle v4 = v2->Rotation.y;

		a1->x = njSin(v4) + v2->Position.x;
		a1->z = njCos(v4) + v2->Position.z;
	}
	return 1;
}

void Piano_Display(ObjectMaster * a1)
{
	if (ScaleObjectMaster_XYZ(a1, 5.2f, 4.5f, 2.9f)) 
	{
		njPushMatrixEx();
		njSetTexture(&AL_TOY_TEXLIST);

		njTranslateEx(&a1->Data1.Entity->Position);
		
		njRotateY(NULL, a1->Data1.Entity->Rotation.y);

		switch (a1->Data1.Entity->Index)
		{
		case PIANOTYPE_PIANO:
			njRotateY(NULL, 0x8000);
			njCnkDrawObject(&Piano);
			break;

		case PIANOTYPE_ORGAN:
			njTranslate(NULL, 0, 2.7f, 0);
			njCnkDrawObject(&Organ);
			break;
		}

		if (a1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&a1->UnknownA_ptr->field_30)
		{
			njTranslate(NULL, 0, 0.4f, 0);

			switch (a1->Data1.Entity->Index)
			{
			case PIANOTYPE_PIANO:
				njScale(NULL, 1.5f, 0.7f, 1.5f);
				break;

			case PIANOTYPE_ORGAN:
				njScale(NULL, 2.25f, 0.7f, 1.5f);
				break;
			}
			
			DrawChaoWorldShadow();
		}

		njPopMatrixEx();
	}
}
VoidFunc(InitJingle, 0x004432D0);

const std::array<const char*, 5> PianoSongs = {
	"chao_g_pianotoy1.adx",
	"chao_g_pianotoy2.adx",
	"chao_g_pianotoy3.adx",
	"chao_g_pianotoy4.adx",
	"chao_g_pianotoy5.adx"
};

const std::array<const char*, 4> OrganSongs = {
	"chao_g_organ1.adx",
	"chao_g_organ2.adx",
	"chao_g_organ3.adx",
	"chao_g_organ4.adx"
};

void Piano_Main(ObjectMaster * a1)
{
	if (ALO_Field_Find_(a1,0, CI_KIND_AL_PIANO))
	{
		a1->Data1.Entity->Status &= ~0x240u;

		if (!a1->Data1.Entity->Action)
		{
			float a = njRandom();

			InitJingle();
			a1->Data1.Entity->Action++;

			int index = 0;
			switch (a1->Data1.Entity->Index)
			{	
			case PIANOTYPE_PIANO:
				a *= PianoSongs.size() - 0.00001f;
				index = (int)a;
				if (index >= (int)PianoSongs.size()) {
					index = 0;
				}
				
				PlayJingle(PianoSongs[index]);
				break;

			case PIANOTYPE_ORGAN:
				a *= OrganSongs.size() - 0.00001f;
				index = (int)a;

				if (index >= (int)OrganSongs.size()) {
					index = 0;
				}

				PlayJingle(OrganSongs[index]);
				break;
			}
		}
	}
	else 
	{
		a1->Data1.Entity->Status |= 0x240u;

		if (a1->Data1.Entity->Action)
			ResetMusic();
		a1->Data1.Entity->Action = 0;
	}

	AL_Toy_Move_Update(a1);

	AddToCollisionList(a1);
}

CollisionData ALO_RadicaseExecutor_collision[] = {
	{ 0, 3191, 32768, {  0.0,  1.0,  0.0 },  1.5,  0.0,  0.0, 0, 0, 0, 0 }
};
void ALO_Piano_Init(ObjectMaster *a1)
{
	a1->MainSub = Piano_Main;
	a1->DeleteSub = (ObjectFuncPtr)0x0057B9B0;
	AL_Toy_Move_Register(a1, ALW_KIND_PIANO);
}

void ALO_PianoCreate(int index, NJS_POINT3* pPos, Angle ang) {
	ObjectMaster *piano;

	piano = LoadObject(4, "ALO_Piano", ALO_Piano_Init, LoadObj_Data1);
	piano->DeleteSub = ALO_Delete;
	piano->DisplaySub = Piano_Display;

	piano->Data1.Entity->Index = index;
	piano->Data1.Entity->Position = *pPos;
	piano->Data1.Entity->Rotation.y = ang;

	//InitCollision(piano, (CollisionData*)&ALO_RadicaseExecutor_collision, 2, 4);
	AL_Toy_Move_Init(piano, (CCL_INFO*)ALO_RadicaseExecutor_collision);
}
