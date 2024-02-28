#include "stdafx.h"
#include "al_look.h"
#include <random>
#include "c_colli.h"
#include "ALifeSDK_Functions.h"

AL_LOOK* AL_GetLook(ObjectMaster* a1)
{
	return (AL_LOOK*)&a1->Data1.Chao->SclH;
}

//#define HEAD_DEBUG
#define HEADROTSPD 0x100
#define HEADROTMAX 80

void AL_SetHeadRot(ObjectMaster* a1, short rotX, short rotY)
{
	AL_LOOK* look = AL_GetLook(a1);
	//look->AimAngX = rotX;
	look->AimAngY = rotY;
}

void AL_StartHeadRot(AL_LOOK* a1)
{
	a1->Timer = (short)(njRandom() * 300.f) * 3; //* 3 temporary
	a1->HeadRotMode = LOOK_ACTIVE;
}
void AL_LookNone(ObjectMaster* a1)
{
	AL_LOOK* look = AL_GetLook(a1);
	if (look->AimAngY != 0) 
		AL_StartHeadRot(look);
}

void AimHeadAng(__int16& angle, __int16 aim)
{
	if (aim != angle)
	{
		if (abs(aim - angle) < HEADROTSPD) //if the difference is smaller than the headRotation then just force set it
			angle = aim;
		else if (aim > 0 && angle < aim)
			angle += HEADROTSPD;
		else if (aim < 0 && angle > aim)
			angle -= HEADROTSPD;
	}
}

void AL_LookActive(ObjectMaster* a1)
{
	AL_LOOK* look = AL_GetLook(a1);

#ifdef HEAD_DEBUG
	look->AngX = look->AimAngX;
	look->AngY = look->AimAngY;
	return;
#endif

	//slowly go towards intended head aim
	//AimHeadAng(look->AngX, look->AimAngX);
	AimHeadAng(look->AngY, look->AimAngY);
	
	/*
	//clamp rotY so the chao doesn't break its neck
	if (look->AngY < NJM_DEG_ANG(-HEADROTMAX))
		look->AngY = NJM_DEG_ANG(-HEADROTMAX);
	if (look->AngY > NJM_DEG_ANG(HEADROTMAX))
		look->AngY = NJM_DEG_ANG(HEADROTMAX);
	*/

	look->Timer--;
	if (look->Timer <= 0)
	{
		look->Timer = 0;
		look->HeadRotMode = LOOK_STOP;
	}
}

//time to turn head back to original position
#define STOPTIME 30
void AL_LookStop(ObjectMaster* a1)
{
	AL_LOOK* look = AL_GetLook(a1);

//#ifdef HEAD_DEBUG
	//look->AngX = 0;
	look->AngY = 0;
	look->HeadRotMode = LOOK_NONE;
	//look->AimAngX = 0;
	look->AimAngY = 0;
	return;
//#endif
	/*
	PrintDebug("%d %d", look->AngX, look->AngY);

	//if the angle is between the rotation value then just set it to 0
	if ((look->AngX > -HEADROTSPD && look->AngX < 0)
		|| (look->AngX < HEADROTSPD && look->AngX > 0))
		look->AngX = 0;
	if ((look->AngY > -HEADROTSPD && look->AngY < 0)
		|| (look->AngY < HEADROTSPD && look->AngY > 0))
		look->AngY = 0;


	if (look->AngX < 0)
		look->AngX += HEADROTSPD;
	if (look->AngX > 0)
		look->AngX -= HEADROTSPD;

	if (look->AngY < 0)
		look->AngY += HEADROTSPD;
	if (look->AngY > 0)
		look->AngY -= HEADROTSPD;


	if (look->AngX == 0 && look->AngY == 0)
	{
		look->AimAngX = 0;
		look->AimAngY = 0;
		look->HeadRotMode = LOOK_NONE;
	}
	*/
}

#define LOOKCONTROLCOUNT 3
LOOK_FUNC AL_LookControlFunctions[LOOKCONTROLCOUNT] = { AL_LookNone, AL_LookActive, AL_LookStop };

void AL_LookControl(ObjectMaster *a1)
{
	AL_LOOK* look = AL_GetLook(a1);
	if (look->HeadRotMode >= 0 && look->HeadRotMode < LOOKCONTROLCOUNT)
		AL_LookControlFunctions[look->HeadRotMode](a1);
	else
		PrintDebug("invalid look mode");
}

FunctionPointer(void, sub_590680, (ObjectMaster* a1), 0x590680);
ObjectMaster* GetTopChaoObject(ObjectMaster* a1)
{
	AL_PERCEPTION_INFO* v1; // eax
	__int16 v2; // cx
	al_perception_link* v3; // eax
	al_entry_work* v4; // eax
	ObjectMaster* result; // eax

	v1 = &a1->Data1.Chao->ChaoObjects;
	if (!v1->InSightFlag
		|| (v2 = v1->NearestNum, v2 < 0)
		|| (v3 = &v1->field_18[v2]) == 0
		|| (v4 = v3->pEntry) == 0
		|| (result = v4->tp) == 0)
	{
		result = 0;
	}
	return result;
}

ObjectMaster* GetClosestChao(ObjectMaster* a1)
{
	ObjectMaster* chaoFound = nullptr;
	float smallestDist = 100000;
	int v3 = *(int*)0x1DC0F80;
	if (*(int*)0x1DC0F80 > 0)
	{
		do
		{
			--v3;
			ObjectMaster* v5 = GetChaoObject(0, v3);
			//v5 = MainCharacter[0];
			if (v5)
			{
				if (v5 != a1)
				{
					EntityData1* data1 = v5->Data1.Entity;
					if (data1)
					{
						unsigned __int16 minRot = NJM_DEG_ANG(145);
						float maxDist = 10;
						float dist = CheckDistance(&data1->Position, &a1->Data1.Entity->Position);
						if (dist < maxDist)
						{
							float v15 = atan2(
								data1->Position.z - a1->Data1.Entity->Position.z,
								data1->Position.x - a1->Data1.Entity->Position.x
							);

							//basically abs
							int v9 = a1->Data1.Entity->Rotation.y - NJM_RAD_ANG(v15);
							if (v9 < 0)
								v9 = NJM_RAD_ANG(v15) - a1->Data1.Entity->Rotation.y;




							float v8 = (float)NJM_ANG_DEG(v9);
							for (; v8 > 180.0f; v8 = v8 - 360.0f)
							{
								;
							}
							for (; v8 < -180.0f; v8 = v8 + 360.0f)
							{
								;
							}
							PrintDebug("final %f", v8);
							//if (NJM_DEG_ANG(v8) < minRot)
							if (v8 > 0)
							{
								if (smallestDist > (double)dist)
								{
									smallestDist = dist;
									chaoFound = v5;
								}

							}
						}
					}
				}
			}
		} while (v3 > 0);
	}
	return chaoFound;
}

void __cdecl WalkControlHeadRotation(ObjectMaster* a1)
{
	MOV_TurnToAim2(a1, 288);

	ObjectMaster* otherChao = ALO_Field_Find_(a1, 5, 0x81);
	if (!otherChao)
	{
		AL_SetHeadRot(a1, 0, 0);
		return;
	}
	NJS_VECTOR pos = a1->Data1.Entity->Position;
	NJS_VECTOR tgt = otherChao->Data1.Entity->Position;

	float v2 = (float)(pos.x - tgt.x);
	float v3 = (float)(pos.z - tgt.z);
	int angY = NJM_RAD_ANG(atan2(v2, v3));
	float v4 = sqrtf((v3 * v3) + (v2 * v2));
	int angX = NJM_RAD_ANG(atan2((tgt.y - pos.y), v4));

	AL_SetHeadRot(a1, angX, angY);
}
static void __declspec(naked) WalkControlHeadRotationHook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call WalkControlHeadRotation

		pop eax // a1
		retn
	}
}
//

int __cdecl AL_BehaviorCheck(ObjectMaster* a1)
{
	AL_BEHAVIOR* v1; // eax
	int v2; // ecx
	BHV_FUNC v3; // edx

	AL_LOOK* look = AL_GetLook(a1);
	look->HeadRotMode = LOOK_STOP;

	v1 = &a1->Data1.Chao->Behavior;
	v1->Mode = 0;
	v1->Timer = 0;
	v1->SubTimer = 0;
	v2 = v1->CurrBhvFuncNum;
	v1->SubMode = 0;
	v1->LimitTimer = 3600;
	v3 = v1->BhvFuncList[v2];
	--v1->nbBhvFuncEntry;
	v1->PrevFunc = v3;
	v1->CurrBhvFuncNum = ++v2;
	return v1->BhvFuncList[v2] != 0;
}
static void __declspec(naked) AL_BehaviorCheckHook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call AL_BehaviorCheck

		add esp, 4 // a1<eax> is also used for return value
		retn
	}
}

//perfection
#define col_size_thing 15
CCL_INFO addColli =
{
	(char)145,
	0,//sphere
	0x70,
	0u,
	0x2400, // //0x782008
	{  0.0,  0,  col_size_thing - 0.3f },
	 20,
	 3,
	 col_size_thing,
	 0.0,
	0,
	0,
	0
};


//doing it this way should let others hack the collision without cwe overwriting it
signed int __cdecl Chao_InitCollision(ObjectMaster* obj, CollisionData* collision, int count, unsigned __int8 a4)
{
	CollisionData colli[6];

	memcpy(colli, collision, sizeof(CollisionData) * count);
	memcpy(&colli[5], &addColli, sizeof(CollisionData));

	//init collision copies the colli to ram so its not a problem that the colli array is temp
	return InitCollision(obj, colli, count + 1, a4);

}
static void __declspec(naked) Chao_InitCollisionHook()
{
	__asm
	{
		push[esp + 0Ch] // __int8 a4
		push[esp + 0Ch] // count
		push[esp + 0Ch] // collision
		push eax // obj

		// Call your __cdecl function here:
		call Chao_InitCollision

		add esp, 4 // obj<eax> is also used for return value
		add esp, 4 // collision
		add esp, 4 // count
		add esp, 4 // __int8 a4
		retn
	}
}

void AL_Look_Init() {
	
	//UNFINISHED

	//head rotation
	//we do a little trolling (it makes all chao walk if youre wondering lol)
	*(__int16*)(0x01312C48 + 0x126) = 0;
	//*(float*)0x1312C68 = 0.0f;
	WriteCall((void*)0x00550334, Chao_InitCollisionHook);
	WriteCall((void*)0x0053DC8E, AL_BehaviorCheckHook);
	WriteCall((void*)0x005A30D1, WalkControlHeadRotationHook);
}