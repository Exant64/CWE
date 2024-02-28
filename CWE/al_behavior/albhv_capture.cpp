#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "../AL_ModAPI.h"
#include "../ChaoMain.h"
#include <al_parts.h>

const int ParamFukidasiExecutor_LoadPtr = 0x05659E0;
void ParamFukidasiExecutor_Load(ObjectMaster* a1)
{
	__asm
	{
		mov eax, a1
		call ParamFukidasiExecutor_LoadPtr
	}
}
FunctionPointer(void, sub_6EFF10, (NJS_VECTOR* a1, NJS_VECTOR* a2, float a3), 0x6EFF10);
ObjectFunc(AL_CloseParameterFukidasiWait, 0x00565BC0);

//we split this out into a separate function for cleanness, on the original version it was inside
void ALBHV_Capture_Parts(ObjectMaster* tp, int minitype) {
	ChaoData1* work = tp->Data1.Chao;
	ChaoDataBase* pParamGC = work->pParamGC;

	if (minitype >= 21 && minitype <= 24) return;

	//actually this was inside the loop, and also duplicated in a lot of other places
	//since I have to do "unfaithful" modifications to the code already I just cleaned it up
	if (minitype < 21) {
		AL_PartsMinimalFlagOn(tp, minitype);
	}
	else if (minitype > 24) {
		//we still support dx behaviors incase we ever wanna do custom bhv
		//AL_PartsMinimalFlagOn_DX(tp, minitype - 25);
	}

	//we return after turning on the animal behavior because we don't intend to lock those
	if (gConfigVal.KeepAnimalParts) return;

	for (int part = 0; part < NB_PARTS_KIND; part++) {
		if (AL_IsChild(tp))
		{
			if (AL_IsExistPartsChild(minitype, part))
			{
				if (njRandom() < ChaoGlobal.PartsSetProb)
				{
					AL_SetMinimalParts(tp, part, minitype);
				}
			}
			else if (njRandom() < ChaoGlobal.PartsRemoveProb)
			{
				AL_RemoveMinimalParts(tp, part);
				//if (minitype == SA2BAnimal_SkeletonDog)
				//	Chao_InitAnimalParts(v1, v17, -1);
				if (part == PARTS_KIND_REG && minitype != SA2BAnimal_Bat)
				{
					pParamGC->HideFeet = 0;
				}
			}
		}
		else
		{
			if (AL_IsExistPartsAdult(minitype, part))
			{
				if (ChaoGlobal.PartsSetProb > njRandom())
				{
					//AL_PartsMinimalFlagOn(tp, minitype);
					AL_SetMinimalParts(tp, part, minitype);
				}
			}
			else if (ChaoGlobal.PartsRemoveProb > njRandom())
			{
				AL_RemoveMinimalParts(tp, part);

				if (part == 4 && minitype != 17)
				{
					pParamGC->HideFeet = 0;
				}
			}
		}
	}

	int ear;
	switch(AL_GetMinimalPart(tp, PARTS_KIND_HORN)){
	case SA2BAnimal_Dragon:
		ear = AL_GetMinimalPart(tp, PARTS_KIND_EAR);

		if (ear != -1 && ear != SA2BAnimal_Dragon) {
			AL_RemoveMinimalParts(tp, PARTS_KIND_EAR);	
		}
		break;
	case SA2BAnimal_Sheep:
		ear = AL_GetMinimalPart(tp, PARTS_KIND_EAR);

		if (ear != -1 && ear != SA2BAnimal_Sheep) {
			AL_RemoveMinimalParts(tp, PARTS_KIND_EAR);	
		}
		break;
	}
}

int ALBHV_Capture(ObjectMaster* tp) {
	ChaoData1* work = tp->Data1.Chao;

	switch (work->Behavior.Mode) {
	case 0:
		AL_SetMotionLink(tp, 203);
		work->Behavior.Mode = 1;
		work->Behavior.SubTimer = (unsigned __int16)(signed int)(njRandom() * 200.0) + 90;
		if (CurrentChaoArea == NextChaoArea)
		{
			PlaySound_XYZ(4100, &tp->Data1.Chao->entity.Position, 0, 0, 0);
		}
		LoadChildObject(LoadObj_Data1, (ObjectFuncPtr)0x566CF0, tp);
		ParamFukidasiExecutor_Load(tp);
	case 1:
		if (!AL_MoveHoldingObject(tp)) {
			return BHV_RET_FINISH;
		}
		if (AL_IsMotionEnd(tp)) {
			al_entry_work* pEntryMinimal = ALW_IsCommunication(tp);

			AL_SetMotionLink(tp, 204);
			work->Behavior.Mode = 2;
			if (CurrentChaoArea == NextChaoArea)
			{
				PlaySound_XYZ(4101, &tp->Data1.Chao->entity.Position, 0, 0, 0);
			}

			for (int i = 0; i < 40; i++) {
				NJS_POINT3* pos = &work->entity.Position;
				int ang = NJM_DEG_ANG(9) * i; //40 steps around a circle

				NJS_POINT3 ppos = {
					pos->x,
					njRandom() * 0.3f + pos->y + 0.5f,
					pos->z
				};

				NJS_VECTOR pvel = {
					njSin(ang) * 0.5f,
					njRandom() * 0.5f + 0.2f,
					njCos(ang) * 0.5f
				};

				sub_6EFF10(&ppos, &pvel, 1);
			}

			if (pEntryMinimal) {
				ChaoDataBase* pParamGC = work->pParamGC;
				ObjectMaster* pMinimal = pEntryMinimal->tp;
				EntityData1* pWorkMinimal = pMinimal->Data1.Entity;
				int minitype = pWorkMinimal->Index;
				NJS_VECTOR velo;
				float addform;

				if (minitype < 0 || minitype >= (int)ModAPI_MinimalInfluence.size()) {
					PrintDebug("ALBHV_Capture: animal type (%d) out of bounds when checking influence, exiting bhv", minitype);
					return BHV_RET_FINISH;
				}

				pWorkMinimal->Rotation.z = 1;

				velo.x = pWorkMinimal->Position.x - tp->Data1.Entity->Position.x;
				velo.y = 0;
				velo.z = pWorkMinimal->Position.z - tp->Data1.Entity->Position.z;
				njUnitVector(&velo);
				velo.x = velo.x * 0.4f;
				velo.y = 0.85f;
				velo.z = velo.z * 0.4f;
				MOV_SetVelo(pMinimal, &velo);
				
				ALBHV_Capture_Parts(tp, minitype);

				if (ChaoGlobal.PartsRemoveProb > njRandom() && minitype != SA2BAnimal_HalfFish)
				{
					pParamGC->BallType = 0;
				}
				if (minitype == SA2BAnimal_HalfFish)
				{
					if (ChaoGlobal.PartsSetProb > njRandom())
					{
						if (pParamGC->Type < (unsigned __int8)ChaoType_Neutral_Chaos)
						{
							pParamGC->BallType = 1;
						}
					}
				}
				else if (minitype == SA2BAnimal_Bat && ChaoGlobal.PartsSetProb > njRandom())
				{
					pParamGC->HideFeet = 1;
				}
				
				if (AL_IsChild(tp)) {
					addform = ChaoGlobal.MinimalAddFormChild;
					AL_ParameterAddUserLike(tp, ChaoGlobal.MinimalAddUserLike, ChaoGlobal.MinimalAddAttrChild);
				}
				else {
					addform = ChaoGlobal.MinimalAddFormAdult;
					AL_ParameterAddUserLike(tp, ChaoGlobal.MinimalAddUserLike, ChaoGlobal.MinimalAddAttrAdult);
				}

				switch (minitype)
				{
				case SA2BAnimal_YellowChaosDrive:
					IncrementFlySwim(tp, -addform * 0.5f);
					break;
				case SA2BAnimal_GreenChaosDrive:
					IncrementPowerRun(tp, -addform * 0.5f);
					break;
				case SA2BAnimal_RedChaosDrive:
					IncrementPowerRun(tp, addform * 0.5f);
					break;
				case SA2BAnimal_PurpleChaosDrive:
					IncrementFlySwim(tp, addform * 0.5f);
					break;
				default:
					switch (ModAPI_MinimalInfluence[minitype])
					{
					case COLOR_SWIM:
						IncrementFlySwim(tp, -addform);
						break;
					case COLOR_FLY:
						IncrementFlySwim(tp, addform);
						break;
					case COLOR_RUN:
						IncrementPowerRun(tp, -addform);
						break;
					case COLOR_POWER:
						IncrementPowerRun(tp, addform);
						break;
					case COLOR_RANDOM:
						{
							float random = njRandom();
							if (random < 0.25f) {
								IncrementPowerRun(tp, addform);
							}
							else if (random < 0.5f) {
								IncrementPowerRun(tp, -addform);
							}
							else if (random < 0.75f) {
								IncrementFlySwim(tp, addform);
							}
							else {
								IncrementFlySwim(tp, -addform);
							}
						}
						break;
					}
					break;
				}
				AL_ParameterGrow(tp, minitype, 2u, 0);
			}
			AL_CloseParameterFukidasiWait(tp);
			ALW_CommunicationOff(tp);
		}
		break;
	case 2:
		if (AL_IsMotionStop(tp)) {
			return BHV_RET_FINISH;
		}
		break;
	}
	return BHV_RET_CONTINUE;
}