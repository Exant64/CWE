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
#include "renderfix.h"

bool __cdecl SetPianoWaypoint(task* a2, NJS_VECTOR* a1)
{
	if (a2)
	{
		taskwk* v2 = a2->twp;
		Angle v4 = v2->ang.y;

		a1->x = njSin(v4) * 1.5 *2 + v2->pos.x;
		a1->y = v2->pos.y;
		a1->z = njCos(v4) * 1.5 * 2 + v2->pos.z;
	}
	return 1;
}

void Piano_Display(task * a1)
{
	if (Scaletask_XYZ(a1, 5.2f, 4.5f, 2.9f)) 
	{
		njPushMatrixEx();
		njSetTexture(&AL_TOY_TEXLIST);

		njTranslateEx(&a1->twp->pos);
		
		njRotateY(NULL, a1->twp->ang.y);

		SaveControl3D(); 
		OffControl3D(NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL);

		switch (a1->twp->btimer)
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

		LoadControl3D();

		if (RenderFix_IsEnabled() && a1->UnknownA_ptr && ChaoGlobal.CamDistShadowCutLev2 > *(float*)&a1->UnknownA_ptr->field_30)
		{
			njTranslate(NULL, 0, 0.4f, 0);

			switch (a1->twp->btimer)
			{
			case PIANOTYPE_PIANO:
				njScale(NULL, 1.5f, 0.7f, 1.5f);
				break;

			case PIANOTYPE_ORGAN:
				njScale(NULL, 2.25f, 0.7f, 1.5f);
				break;
			}
			
			rfapi_core->pDraw->AL_ShadowDraw();
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

void Piano_Main(task * a1)
{
	if (ALO_Field_Find_(a1,0, CI_KIND_AL_PIANO))
	{
		a1->twp->flag &= ~0x240u;

		if (!a1->twp->mode)
		{
			float a = njRandom();

			InitJingle();
			a1->twp->mode++;

			int index = 0;
			switch (a1->twp->btimer)
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
		a1->twp->flag |= 0x240u;

		if (a1->twp->mode)
			ResetMusic();
		a1->twp->mode = 0;
	}

	AL_Toy_Move_Update(a1);

	AddToCollisionList(a1);
}

CollisionData ALO_RadicaseExecutor_collision[] = {
	{ 0, 3191, 32768, {  0.0,  1.0,  0.0 },  1.5,  0.0,  0.0, 0, 0, 0, 0 }
};
void ALO_Piano_Init(task *a1)
{
	a1->exec = Piano_Main;
	a1->dest = (task_exec)0x0057B9B0;
	AL_Toy_Move_Register(a1, ALW_KIND_PIANO);
}

void ALO_PianoCreate(int index, NJS_POINT3* pPos, Angle ang) {
	task *piano;

	piano = CreateElementalTask(4, "ALO_Piano", ALO_Piano_Init, LoadObj_Data1);
	piano->dest = ALW_CancelEntry;
	piano->disp = Piano_Display;

	piano->twp->btimer = index;
	piano->twp->pos = *pPos;
	piano->twp->ang.y = ang;

	//InitCollision(piano, (CollisionData*)&ALO_RadicaseExecutor_collision, 2, 4);
	AL_Toy_Move_Init(piano, (CCL_INFO*)ALO_RadicaseExecutor_collision);
}
