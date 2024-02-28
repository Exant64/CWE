#pragma once

#include "Chao.h"

enum eLookMode : __int16 {
	LOOK_NONE = 0,
	LOOK_ACTIVE = 1,
	LOOK_STOP = 2
};

//#define X_HEAD_ROT
//struct size is limited to 12 bytes, but preferably use 8 to not mess with camdist which might get overwritten, we dont know yet
struct AL_LOOK {
	//	float SclH;
#ifdef X_HEAD_ROT
	__int16 AngX;
	__int16 AimAngX;
#endif

	__int16 AngY;

	//float SclV;
	__int16 HeadRotMode;
	
	//float CamDist;
	__int16 AimAngY;
	__int16 Timer;
};
typedef void(__cdecl* LOOK_FUNC)(ObjectMaster*);

AL_LOOK* AL_GetLook(ObjectMaster* a1);
void AL_LookControl(ObjectMaster* a1);
void AL_SetHeadRot(ObjectMaster* a1, short rotX, short rotY);