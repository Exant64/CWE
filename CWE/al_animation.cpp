#include "stdafx.h"

#include "Chao.h"
#include "ChaoMain.h"
#include "ALifeSDK_Functions.h"
#include "al_parameter.h"
#include "al_behavior/albhv.h"
#include "iostream"
#include "al_behavior/al_behavior.h"
#include <al_face.h>

#define RandomChance (njRandom() < 0.5f)

#define SOUNDBANK1(a1) 0x1000 + a1
#define SOUNDBANK6(a1) 0x6000 + a1

void Chao_ExtraAnimations(ObjectMaster* a1)
{
	ChaoData1* data = a1->Data1.Chao;

	//Reset to 0 each Behavior
	if (data->Behavior.Mode == 0) data->AnimRandomized = 0;

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_THINK)
	{
		if (data->AnimRandomized == 0)
		{
			if (data->MotionTable.TransitionToID == 0 || data->MotionTable.TransitionToID == 1 || data->MotionTable.TransitionToID == 238 || data->MotionTable.TransitionToID == 239)
			{
				if (data->MotionTable.AnimID == 534)
				{
					AL_SetMotionLink(a1, 538);
					data->Behavior.Timer += (rand() % 30 + 30);
					AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
				}
				else if (data->MotionTable.AnimID == 536)
				{
					AL_SetMotionLink(a1, 541);
				}
				else if (data->MotionTable.AnimID == 128)
				{
					AL_SetMotionLink(a1, 279);
					data->Behavior.Timer += (rand() % 30 + 30);
					AL_FaceChangeEye(a1, ChaoEyes_Painful);
				}
				else if (data->MotionTable.AnimID == 398)
				{
					AL_SetMotionLink(a1, 399);
				}
				else if (data->MotionTable.AnimID == 220)
				{
					AL_SetMotionLink(a1, 454);
					data->Behavior.Timer += (rand() % 30 + 30);
					AL_FaceChangeEye(a1, ChaoEyes_ClosedUp);
				}
				else if (data->MotionTable.AnimID == 201 || data->MotionTable.AnimID == 192)
				{
					int HeroIdle = 483;
					if (AL_IsHero(data->pParamGC->Type))
					{
						HeroIdle = 484;
					}

					const int NoAnim[] = { 513, HeroIdle };
					int NoRNG = rand() % std::size(NoAnim);

					AL_SetMotionLink(a1, NoAnim[NoRNG]);
					data->AnimRandomized++;

					AL_FaceChangeEye(a1, ChaoEyes_ClosedStraight);
				}
				else if (data->MotionTable.AnimID >= 100 && data->MotionTable.AnimID <= 102)
				{
					int HeroIdle = 0;
					int DarkIdle = 1;
					if (AL_IsHero(data->pParamGC->Type))
					{
						HeroIdle = 554;
					}
					else if (AL_IsDark(data->pParamGC->Type))
					{
						const int DarkAnim[] = { 570, 571, 572 };
						int DarkRNG = rand() % std::size(DarkAnim);

						DarkIdle = DarkAnim[DarkRNG];
					}

					int FlyIdle = 132;
					if (data->pParamGC->StatPoints[1] >= 100)
					{
						FlyIdle = 131;
					}

					const int Idle2Anim[] = { 0, 1, 238, 239, 215, 457, 510, HeroIdle, DarkIdle, FlyIdle };
					int Idle2RNG = rand() % std::size(Idle2Anim);

					AL_SetMotionLink(a1, Idle2Anim[Idle2RNG]);
					data->AnimRandomized++;
				}
				else
				{
					int vitality = AL_EmotionGetValue(a1, EM_PER_VITALITY);
					int ExtraIdle = 0;

					if (vitality < -30)
					{
						ExtraIdle = 485;
					}
					else if (vitality > 30)
					{
						ExtraIdle = 477;
					}

					const int IdleAnim[] = { 0, 1, 238, 239, 215, 419, 421, ExtraIdle };
					int IdleRNG = rand() % std::size(IdleAnim);

					AL_SetMotionLink(a1, IdleAnim[IdleRNG]);
					data->AnimRandomized++;
				}
			}
		}
		else if (data->AnimRandomized == 1)
		{
			if (data->MotionTable.AnimID == 216)
			{
				const int LookUpAnim[] = { 216, 217, 218 };
				int LookUpRNG = rand() % std::size(LookUpAnim);

				AL_SetMotionLinkStep(a1, LookUpAnim[LookUpRNG], 0x23u);
				data->Behavior.Timer += (rand() % 30 + 30);
				data->AnimRandomized++;
			}
			else if (data->MotionTable.AnimID == 477)
			{
				const int ShyAnim[] = { 477, 478 };
				int ShyRNG = rand() % std::size(ShyAnim);

				AL_SetMotionLinkStep(a1, ShyAnim[ShyRNG], 0x23u);
				data->Behavior.Timer += (rand() % 30 + 30);
				data->AnimRandomized++;
			}
			else if (data->MotionTable.AnimID == 555)
			{
				const int HeroAnim[] = { 555, 556, 559 };
				int HeroRNG = rand() % std::size(HeroAnim);

				AL_SetMotionLinkStep(a1, HeroAnim[HeroRNG], 0x23u);
				data->Behavior.Timer += (rand() % 30 + 30);
				data->AnimRandomized++;
			}
		}


		if (AL_EmotionGetValue(a1, EM_ST_HUNGER) < 5500 && data->MotionTable.TransitionToID == 62)
		{
			if (data->MotionTable.AnimID == 175 || data->MotionTable.AnimID == 179)
			{
				AL_SetMotionLinkStep(a1, 166, 0x23u);
				data->Behavior.Timer += (rand() % 30 + 30);

				AL_FaceChangeEye(a1, ChaoEyes_ClosedUp);
				AL_FaceChangeMouth(a1, ChaoMouth_OpenSmile);
			}
		}

		if (data->MotionTable.TransitionToID == 130)
		{
			AL_SetBehavior(a1, (BHV_FUNC)ChaoBehaviour_FLY);
			AL_GetRandomAttrPos_0(a1);
		}
		else if (data->MotionTable.TransitionToID == 539)
		{
			AL_SetMotionLink(a1, 540);
		}

		//Reset Behaviour if it play those unique animations (i should probably learn how to make my own behaviour)
		if (data->Behavior.Timer == 1)
		{
			if (data->MotionTable.AnimID == 166 || data->MotionTable.AnimID == 455 || data->MotionTable.AnimID == 279 || data->MotionTable.AnimID == 540)
			{
				data->Behavior.Mode = 0;
			}
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_SITTHINK)
	{
		if (data->MotionTable.TransitionToID == 62 && data->MotionTable.AnimID == 62 && data->Behavior.Timer > 300)
		{
			data->MotionTable.TransitionToID = 493; //For some reason it broke with AL_SetMotionLink
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WATCHINGTV || AL_GetBehavior(a1) == ALBHV_ToyMoveCheck<(BHV_FUNC)ChaoBehaviour_WATCHINGTV> || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WATCHINGRADIO || AL_GetBehavior(a1) == ALBHV_ToyMoveCheck<(BHV_FUNC)ChaoBehaviour_WATCHINGRADIO> || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WATCHINGSING || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WATCHINGDANCE || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WATCHINGMUSIC)
	{
		if (data->AnimRandomized == 0)
		{
			if (data->MotionTable.TransitionToID == 573)
			{
				int ToothyClap = 573;
				if (data->pParamGC->MouthType == ChaoMouth_ToothySmile || data->pParamGC->MouthType == ChaoMouth_ToothyFrown)
				{
					const int ToothyAnim[] = { 465, 466, 467 };
					int ToothyRNG = rand() % std::size(ToothyAnim);

					ToothyClap = ToothyAnim[ToothyRNG];
				}

				const int GiggleAnim[] = { ToothyClap, 472 };
				int GiggleRNG = rand() % std::size(GiggleAnim);

				AL_SetMotionLink(a1, GiggleAnim[GiggleRNG]);
				data->AnimRandomized++;
			}
			else if (data->MotionTable.TransitionToID == 574)
			{
				const int Giggle2Anim[] = { 574, 474, 475 };
				int Giggle2RNG = rand() % std::size(Giggle2Anim);

				AL_SetMotionLink(a1, Giggle2Anim[Giggle2RNG]);
				data->AnimRandomized++;
			}
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_SLEEP && data->Behavior.Timer == (rand() % 500 + 250))
	{
		if (data->MotionTable.TransitionToID != 30) //Frame Perfect why are you like this
		{
			if (data->MotionTable.AnimID == 19)
			{
				const int MidSleepAnim[] = { 73, 74, 75, 76, 85, 87 };
				int MidSleepRNG = rand() % std::size(MidSleepAnim);

				AL_SetMotionLinkStep(a1, MidSleepAnim[MidSleepRNG], 0x28u);
			}
			else if (data->MotionTable.AnimID == 25)
			{
				AL_SetMotionLink(a1, 77);
			}
			else if (data->MotionTable.AnimID == 53)
			{
				const int MidSleep2Anim[] = { 90, 91, 92, 93 };
				int MidSleep2RNG = rand() % std::size(MidSleep2Anim);

				AL_SetMotionLinkStep(a1, MidSleep2Anim[MidSleep2RNG], 0x28u);
			}
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WALK)
	{
		if (data->MotionTable.TransitionToID == 100)
		{
			if (data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_ANGER)
			{
				AL_SetMotionLink(a1, 126);
			}
			if (data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_URGETOCRY)
			{
				AL_SetMotionLink(a1, 105);
			}
			if (data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_URGETOCRY2)
			{
				const int CryWalkAnim[] = { 534, 536 };
				int CryWalkRNG = rand() % std::size(CryWalkAnim);

				AL_SetMotionLink(a1, CryWalkAnim[CryWalkRNG]);
			}
			if (data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_DIZZY)
			{
				AL_SetMotionLink(a1, 127);
			}
		}

	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_ANGER || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_URGETOCRY || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_URGETOCRY2 || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_DIZZY)
	{
		if (data->Behavior.nbBhvFuncEntry == 1 && data->Behavior.Timer == 1 && RandomChance)
		{
			Chao_BehaviourQueue(a1, ChaoBehaviour_WALK);
			AL_GetRandomAttrPos_0(a1);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTOPLAYER || AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTOBALL)
	{
		if (data->AnimRandomized == 0)
		{
			if (data->MotionTable.TransitionToID == 119 && data->pParamGC->StatPoints[2] >= 1333)
			{
				const int GotoTarget2Anim[] = { 119, 129 };
				int GotoTarget2RNG = rand() % std::size(GotoTarget2Anim);

				AL_SetMotionLink(a1, GotoTarget2Anim[GotoTarget2RNG]);
				data->AnimRandomized++;
			}
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_NOTICEITEM)
	{
		if (data->Behavior.Timer == 0 && AL_EmotionGetValue(a1, EM_PER_SOLITUDE) > 30 && RandomChance)
		{
			AL_SetMotionLink(a1, 302);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTOITEM)
	{
		if (data->MotionTable.AnimID == 302 && data->MotionTable.TransitionToID == 100)
		{
			AL_SetMotionLink(a1, 104);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_SWIM)
	{
		if (data->Face.EyeCurrNum == ChaoEyes_Painful && data->MotionTable.TransitionToID == 148)
		{
			AL_SetMotionLink(a1, 149);
		}
	}
	else if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_SUPERSWIM)
	{
		if (data->Face.EyeCurrNum == ChaoEyes_Painful && data->MotionTable.TransitionToID == 145)
		{
			AL_SetMotionLink(a1, 144);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_CLIMB && data->Behavior.SubTimer == 120)
	{
		AL_SetMotionLink(a1, 157);
		AL_FaceChangeEye(a1, ChaoEyes_Painful);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_EATING && data->MotionTable.TransitionToID == 186)
	{
		AL_FaceChangeEye(a1, ChaoEyes_Painful);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_COMEOUTEGG)
	{
		if (data->pParamGC->Reincarnations > 1 && (data->MotionTable.TransitionToID == 205 || data->MotionTable.TransitionToID == 206) && RandomChance)
		{
			AL_SetMotionLinkStep(a1, 207, 15);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_TIRED)
	{
		if (data->MotionTable.TransitionToID == 0)
		{
			const int TiredAnim[] = { 276, 488 };
			int TiredRNG = rand() % std::size(TiredAnim);

			AL_SetMotionLink(a1, TiredAnim[TiredRNG]);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GETPET)
	{
		if (data->MotionTable.TransitionToID == 0)
		{
			if (data->Behavior.PrevFunc != (BHV_FUNC)ChaoBehaviour_THINK && RandomChance)
			{
				int naive = AL_EmotionGetValue(a1, EM_PER_SLEEPY_HEAD);
				if (naive < -70 && RandomChance)
				{
					AL_SetMotionLinkStep(a1, 519, 0x14u);
					AL_FaceChangeEye(a1, ChaoEyes_Tiny);
				}
				else if (naive < -30)
				{
					AL_SetMotionLinkStep(a1, 518, 0x14u);
					AL_FaceChangeEye(a1, ChaoEyes_Normal);
				}
			}
			else if (data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_THINKSEED || data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_WALKWITHSEED || data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_FOUNDSEEDSPOT || data->Behavior.PrevFunc == (BHV_FUNC)ChaoBehaviour_HOLDINGITEM)
			{
				AL_SetMotionLinkStep(a1, 194, 0x14u);
				AL_FaceChangeEye(a1, ChaoEyes_Normal);
			}
		}
		else if (data->MotionTable.TransitionToID == 42)
		{
			AL_FaceChangeEye(a1, ChaoEyes_Tiny);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WARTHOG && data->Behavior.Timer == 60)
	{
		AL_SetMotionLink(a1, 122);
		AL_FaceChangeEye(a1, ChaoEyes_Painful);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_FOUNDSEEDSPOT && data->MotionTable.TransitionToID == 0)
	{
		AL_SetMotionLinkStep(a1, 83, 15);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_MATEWALK)
	{
		if (data->MotionTable.TransitionToID == 120)
		{
			int charm = AL_EmotionGetValue(a1, EM_PER_CHARM);
			if (charm > 30)
			{
				AL_SetMotionLinkStep(a1, 121, 15);
			}
			else if (charm < -70)
			{
				AL_SetMotionLinkStep(a1, 100, 15);
			}
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_RACEWAIT)
	{
		if (data->MotionTable.TransitionToID == 6)
		{
			int HeroIdle = 11;
			if (AL_IsHero(data->pParamGC->Type))
			{
				HeroIdle = 8;
			}

			const int RaceWaitAnim[] = { 7, HeroIdle };
			int RaceWaitRNG = rand() % std::size(RaceWaitAnim);

			AL_SetMotionLink(a1, RaceWaitAnim[RaceWaitRNG]);
		}
	}

	//This Part is busted, might fix later.
	/*
	const int JumpScaredAnim[] = { 568, 161 };
	int JumpScaredRNG = rand() % std::size(JumpScaredAnim);

	if (data->MotionTable.AnimID == 161)
	{
		AL_SetMotionLink(a1, 162);
	}

	WriteData((unsigned int*)0x005647D0, (unsigned int)JumpScaredAnim[JumpScaredRNG]);

	const int ThrowdAnim[] = { 271, 274 };
	int ThrowdRNG = rand() % std::size(ThrowdAnim);

	if (data->MotionTable.TransitionToID == 159)
	{
		if (data->MotionTable.AnimID == 271)
		{
			AL_SetMotionLinkStep(a1, 272, 0xFu);
		}
		else if (data->MotionTable.AnimID == 274)
		{
			AL_SetMotionLinkStep(a1, 275, 0xFu);
		}
	}

	if (data->MotionTable.AnimID == 272)
	{
		AL_SetMotionLinkStep(a1, 273, 0xFu);
	}

	WriteData((unsigned int*)0x005644F7, (unsigned int)ThrowdAnim[ThrowdRNG]);
	WriteData((unsigned int*)0x00563F5F, (unsigned int)ThrowdAnim[ThrowdRNG]);
	*/

	//skillful animation speed
	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTANIMAL)
	{
		int skillful = AL_EmotionGetValue(a1, EM_PER_SKILLFUL);
		if (skillful > 70)
		{
			a1->Data1.Chao->MotionTable.frameIncreaseSpeed_ = 1.25f;
		}
		else if (skillful > 30)
		{
			a1->Data1.Chao->MotionTable.frameIncreaseSpeed_ = 1.125f;
		}
		else if (skillful < -30)
		{
			a1->Data1.Chao->MotionTable.frameIncreaseSpeed_ = 0.875f;
		}
		else if (skillful < -70)
		{
			a1->Data1.Chao->MotionTable.frameIncreaseSpeed_ = 0.75f;
		}
	}

	//Anger Face
	if (AL_EmotionGetValue(a1, EM_MD_ANGER) > 80 && AL_EmotionGetValue(a1, EM_PER_AGRESSIVE) > 30)
	{
		a1->Data1.Chao->Face.EyeDefaultNum = ChaoEyes_Mean;
	}
	else
	{
		a1->Data1.Chao->Face.EyeDefaultNum = a1->Data1.Chao->pParamGC->EyeType;
	}
}

void Chao_ExtraSounds(ObjectMaster* a1)
{
	ChaoData1* data = a1->Data1.Chao;

	//Reset to 0 each Behavior
	if (data->Behavior.Mode == 0) data->ExtraSound = 0;

	if (data->ExtraSound == 0)
	{
		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_THINK)
		{
			if (data->MotionTable.TransitionToID == 399)
			{
				const int BegSound[] = { 137, 138 };
				int BegRNG = rand() % std::size(BegSound);

				PlaySound_XYZ(SOUNDBANK6(BegSound[BegRNG]), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
			else if (data->MotionTable.TransitionToID == 199)
			{
				const int HatSuccesSound[] = { 149, 150, 151 };
				int HatSuccesRNG = rand() % std::size(HatSuccesSound);

				PlaySound_XYZ(SOUNDBANK6(HatSuccesSound[HatSuccesRNG]), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
			else if (data->MotionTable.TransitionToID == 454)
			{
				PlaySound_XYZ(SOUNDBANK6(152), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_HUNGRY)
		{
			PlaySound_XYZ(SOUNDBANK1(18), &data->entity.Position, 0, 0, 0);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GETTINGTIRED)
		{
			if (data->MotionTable.TransitionToID == 21)
			{
				PlaySound_XYZ(SOUNDBANK6(7), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GETPET)
		{
			if (data->MotionTable.TransitionToID == 194)
			{
				PlaySound_XYZ(SOUNDBANK6(27), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
			else if (data->MotionTable.TransitionToID == 519)
			{
				PlaySound_XYZ(SOUNDBANK6(32), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_JUMP)
		{
			const int JumpSound[] = { 30, 52, 139 };
			int JumpRNG = rand() % std::size(JumpSound);

			PlaySound_XYZ(SOUNDBANK6(JumpSound[JumpRNG]), &data->entity.Position, 0, 0, 55);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_EATING && data->gap6EC[0] == ChaoIcon_Heart)
		{
			const int GotFoodSound[] = { 42, 65 };
			int GotFoodRNG = rand() % std::size(GotFoodSound);

			PlaySound_XYZ(SOUNDBANK6(GotFoodSound[GotFoodRNG]), &data->entity.Position, 0, 0, 110);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_WALK)
		{
			if (data->MotionTable.TransitionToID == 126)
			{
				PlaySound_XYZ(SOUNDBANK6(68), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTDAMAGE)
		{
			const int DamageSound[] = { 109, 41 };
			int DamageRNG = rand() % std::size(DamageSound);

			PlaySound_XYZ(SOUNDBANK6(DamageSound[DamageRNG]), &data->entity.Position, 0, 0, 110);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTTHROWND)
		{
			PlaySound_XYZ(SOUNDBANK6(141), &data->entity.Position, 0, 0, 110);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_PUTHAT)
		{
			PlaySound_XYZ(SOUNDBANK6(143), &data->entity.Position, 0, 0, 110);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_DRAWING)
		{
			if (data->MotionTable.TransitionToID == 325)
			{
				PlaySound_XYZ(SOUNDBANK6(154), &data->entity.Position, 0, 0, 110);
				data->ExtraSound++;
			}
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_MATEWALK)
		{
			PlaySound_XYZ(SOUNDBANK6(156), &data->entity.Position, 0, 0, 110);
			data->ExtraSound++;
		}

		if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GOTOBALL)
		{
			PlaySound_XYZ(SOUNDBANK6(159), &data->entity.Position, 0, 0, 110);
			data->ExtraSound++;
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_BUNNY)
	{
		if (data->MotionTable.frame == 1.0f)
		{
			PlaySound_XYZ(SOUNDBANK1(2), &data->entity.Position, 0, 0, 0);
		}
		else if (data->MotionTable.frame == 7.0f)
		{
			PlaySound_XYZ(SOUNDBANK1(3), &data->entity.Position, 0, 0, 0);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_GORILLA && data->Behavior.Timer % 16 == 0)
	{
		if (data->MotionTable.AnimID == 283)
		{
			PlaySound_XYZ(SOUNDBANK1(36), &data->entity.Position, 0, 0, 0);
		}
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_BROOMSTICK && data->Behavior.Timer % 34 == 0)
	{
		PlaySound_XYZ(SOUNDBANK1(37), &data->entity.Position, 0, 0, 0);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_POGOSTICK && data->MotionTable.frame == 5.0f)
	{
		PlaySound_XYZ(SOUNDBANK1(41), &data->entity.Position, 0, 0, 0);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_URGETOCRY && data->Behavior.Timer == 100 && RandomChance)
	{
		PlaySound_XYZ(SOUNDBANK6(0), &data->entity.Position, 0, 0, 110);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_TIRED && data->Behavior.Timer == 0)
	{
		const int TiredSound[] = { 4, 158 };
		int TiredRNG = rand() % std::size(TiredSound);

		PlaySound_XYZ(SOUNDBANK6(TiredSound[TiredRNG]), &data->entity.Position, 0, 0, 110);
	}

	if (AL_GetBehavior(a1) == (BHV_FUNC)ChaoBehaviour_TRIP && data->Behavior.Timer == 40 && RandomChance)
	{
		PlaySound_XYZ(SOUNDBANK6(24), &data->entity.Position, 0, 0, 110);
	}
}

static void AL_MoreAnimData() {
	const int ThinkAnim[] = { 3, 4, 241 };
	int ThinkRNG = rand() % std::size(ThinkAnim);

	WriteData((unsigned int*)0x00565C97, (unsigned int)ThinkAnim[ThinkRNG]);
	WriteData((unsigned int*)0x00567739, (unsigned int)ThinkAnim[ThinkRNG]);

	const int TireAnim[] = { 47, 80, 81 };
	int TireRNG = rand() % std::size(TireAnim);

	WriteData((unsigned int*)0x0055C47E, (unsigned int)TireAnim[TireRNG]);

	const int MoveAnim[] = { 38, 48, 82, 48 };
	int MoveRNG = rand() % std::size(MoveAnim);

	WriteData((unsigned int*)0x0055C40E, (unsigned int)MoveAnim[MoveRNG]);

	const int GotoTargetAnim[] = { 106, 108 };
	int GotoTargetRNG = rand() % std::size(GotoTargetAnim);

	WriteData((unsigned int*)0x005632C0, (unsigned int)GotoTargetAnim[GotoTargetRNG]);
	WriteData((unsigned int*)0x005638A4, (unsigned int)GotoTargetAnim[GotoTargetRNG]);

	const int JumpAnim[] = { 138, 135 };
	int JumpRNG = rand() % std::size(JumpAnim);

	WriteData((unsigned int*)0x005A0FC1, (unsigned int)JumpAnim[JumpRNG]);

	const int HandingAnim[] = { 202, 251, 401 };
	int HandingRNG = rand() % std::size(HandingAnim);

	WriteData((unsigned int*)0x005606ED, (unsigned int)HandingAnim[HandingRNG]);

	const int HungryAnim[] = { 94, 95, 96, 97 };
	int HungryRNG = rand() % std::size(HungryAnim);

	WriteData((unsigned int*)0x0059EDC1, (unsigned int)HungryAnim[HungryRNG]);

	const int EatTooFastAnim[] = { 183, 178 };
	int EatTooFastRNG = rand() % std::size(EatTooFastAnim);

	WriteData((unsigned int*)0x00560CF5, (unsigned int)EatTooFastAnim[EatTooFastRNG]);

	const int HateAnim[] = { 447, 448 };
	int HateRNG = rand() % std::size(HateAnim);

	WriteData((unsigned int*)0x005A1828, (unsigned int)HateAnim[HateRNG]);

	const int SadAnim[] = { 502, 487 };
	int SadRNG = rand() % std::size(SadAnim);

	WriteData((unsigned int*)0x0059F9B4, (unsigned int)SadAnim[SadRNG]);

	const int CryAnim[] = { 523, 480 };
	int CryRNG = rand() % std::size(CryAnim);

	WriteData((unsigned int*)0x0059FD32, (unsigned int)CryAnim[CryRNG]);

	const int FearAnim[] = { 498, 501 };
	int FearRNG = rand() % std::size(FearAnim);

	WriteData((unsigned int*)0x0059F3A4, (unsigned int)FearAnim[FearRNG]);

	const int BeingCalledAnim[] = { 492, 453 };
	int BeingCalledRNG = rand() % std::size(BeingCalledAnim);

	WriteData((unsigned int*)0x005636F1, (unsigned int)BeingCalledAnim[BeingCalledRNG]);

	const int WatchTVAnim[] = { 459, 461 };
	int WatchTVRNG = rand() % std::size(WatchTVAnim);

	WriteData((unsigned int*)0x0059896D, (unsigned int)WatchTVAnim[WatchTVRNG]);
	WriteData((unsigned int*)0x00598C6D, (unsigned int)WatchTVAnim[WatchTVRNG]);
}

static void AL_MoreSoundData() {
	const int SleepSound[] = { 15, 12, 13, 16 };
	int SleepRNG = rand() % std::size(SleepSound);

	WriteData((unsigned int*)0x0054F495, (unsigned int)SOUNDBANK6(SleepSound[SleepRNG]));

	const int Sleep2Sound[] = { 17, 18, 19 };
	int Sleep2RNG = rand() % std::size(Sleep2Sound);

	WriteData((unsigned int*)0x0054F4A9, (unsigned int)SOUNDBANK6(Sleep2Sound[Sleep2RNG]));

	const int CrawlSound[] = { 21, 22 };
	int CrawlRNG = rand() % std::size(CrawlSound);

	WriteData((unsigned int*)0x005A3327, (unsigned int)SOUNDBANK6(CrawlSound[CrawlRNG]));

	const int ThinkSound[] = { 28, 29 };
	int ThinkRNG = rand() % std::size(ThinkSound);

	WriteData((unsigned int*)0x00565CCB, (unsigned int)SOUNDBANK6(ThinkSound[ThinkRNG]));

	const int SwimIdleSound[] = { 37, 39 };
	int SwimIdleRNG = rand() % std::size(SwimIdleSound);

	WriteData((unsigned int*)0x005621F7, (unsigned int)SOUNDBANK6(SwimIdleSound[SwimIdleRNG]));

	const int RashSound[] = { 44, 43 };
	int RashRNG = rand() % std::size(RashSound);

	WriteData((unsigned int*)0x0059A1EF, (unsigned int)SOUNDBANK6(RashSound[RashRNG]));

	const int HandingSound[] = { 55, 70 };
	int HandingRNG = rand() % std::size(HandingSound);

	WriteData((unsigned int*)0x005607AA, (unsigned int)SOUNDBANK6(HandingSound[HandingRNG]));

	const int HateFruitSound[] = { 58, 62 };
	int HateFruitRNG = rand() % std::size(HateFruitSound);

	WriteData((unsigned int*)0x005609F8, (unsigned int)SOUNDBANK6(HateFruitSound[HateFruitRNG]));

	const int EatTooFastSound[] = { 61, 63, 64 };
	int EatTooFastRNG = rand() % std::size(EatTooFastSound);

	WriteData((unsigned int*)0x00560D66, (unsigned int)SOUNDBANK6(EatTooFastSound[EatTooFastRNG]));

	const int FinishEatSound[] = { 66, 40 };
	int FinishEatRNG = rand() % std::size(FinishEatSound);

	WriteData((unsigned int*)0x00560C97, (unsigned int)SOUNDBANK6(FinishEatSound[FinishEatRNG]));

	const int AngrySound[] = { 102, 103 };
	int AngryRNG = rand() % std::size(AngrySound);

	WriteData((unsigned int*)0x005A15B5, (unsigned int)SOUNDBANK6(AngrySound[AngryRNG]));

	const int HeroHiSound[] = { 145, 134 };
	int HeroHiRNG = rand() % std::size(HeroHiSound);

	WriteData((unsigned int*)0x005A041B, (unsigned int)SOUNDBANK6(HeroHiSound[HeroHiRNG]));

	const int GorillaSound[] = { 135, 136 };
	int GorillaRNG = rand() % std::size(GorillaSound);

	WriteData((unsigned int*)0x005A21AA, (unsigned int)SOUNDBANK6(GorillaSound[GorillaRNG]));
}

static int ShuffleTimer = 0;
void AL_MoreAnimSound()
{
	if (++ShuffleTimer % 5 != 0) return;
	
	if (gConfigVal.MoreAnimation) {
		AL_MoreAnimData();
	}

	if (gConfigVal.MoreSound) {
		AL_MoreSoundData();
	}
}

int ALBHV_PickUpLockOn_MoreAnim(ObjectMaster* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	switch (bhv->Mode) {
	case 0:
		if (gConfigVal.MoreAnimation && njRandom() < 0.5f) {
			AL_SetMotionLink(tp, 187);

			if (gConfigVal.MoreSound) {
				if (CurrentChaoArea == NextChaoArea) {
					PlaySound_XYZ(SOUNDBANK6(129), &work->entity.Position, 0, 0, 110);
				}
			}
		}
		else {
			AL_SetMotionLink(tp, 188);

			if (gConfigVal.MoreSound) {
				if (CurrentChaoArea == NextChaoArea) {
					if (njRandom() < 0.5f) {
						PlaySound_XYZ(SOUNDBANK6(147), &work->entity.Position, 0, 0, 110);
					}
					else {
						PlaySound_XYZ(SOUNDBANK6(148), &work->entity.Position, 0, 0, 110);
					}
				}
			}
		}

		if (CurrentChaoArea == NextChaoArea) {
			PlaySound_XYZ(4097, &work->entity.Position, 0, 0, 0);
		}

		bhv->Mode++;
		bhv->LimitTimer = 1800;
		break;
	case 1:
		if (AL_IsMotionEnd(tp)) {
			ObjectMaster* pLockOn = ALW_GetLockOnTask(tp);
			if (pLockOn && !ALW_IsSheAttentionOtherOne(tp, pLockOn)) {
				AL_GrabObjectBothHands(tp, pLockOn);
				AL_MoveHoldingObject(tp);

				bhv->Mode++;

				bhv->Timer = 0;
				bhv->SubTimer = 0;
			}
		}
	case 2:
		AL_MoveHoldingObject (tp);
		if (AL_IsMotionEnd(tp)) {
			return BHV_RET_FINISH;
		}
		break;
	}
	
	return --bhv->LimitTimer > 0 ? BHV_RET_CONTINUE : BHV_RET_BREAK;
}

//this should be moved to config folder type code
void AL_MoreAnimSound_Init() {
	WriteData((int*)0x005615DA, (int)ALBHV_PickUpLockOn_MoreAnim);
}