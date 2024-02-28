#include "stdafx.h"
#include <random>

#include "ninja_functions.h"
#include "ALifeSDK_Functions.h"
#include "al_tv.h"
#include "al_console.h"
#include "Chao.h"
//#include "TVGameObject.h"
//#include "../../CWE/CWE/CWE/ninja_functions.h"
//#include "Console.h"
DataPointer(NJS_OBJECT, TV, 0x01302C1C);
DataPointer(ObjectMaster*, TVObject, 0x1AED288);
#define SLOWORD(x)   (*((short*)&(x)))
//FunctionPointer(void, __cdecl SetChunkTextureID, (NJS_CNK_MODEL *a1, __int16 a2), 0x75B360);
template<class T> char __SETS__(T x)
{
	if (sizeof(T) == 1)
		return char(x) < 0;
	if (sizeof(T) == 2)
		return short(x) < 0;
	if (sizeof(T) == 4)
		return int(x) < 0;
	return INT64(x) < 0;
}
template<class T, class U> char __OFSUB__(T x, U y)
{
	if (sizeof(T) < sizeof(U))
	{
		U x2 = x;
		char sx = __SETS__(x2);
		return (sx ^ __SETS__(y)) & (sx ^ __SETS__(x2 - y));
	}
	else
	{
		T y2 = y;
		char sx = __SETS__(x);
		return (sx ^ __SETS__(y2)) & (sx ^ __SETS__(x - y2));
	}
}


//CollisionData tvCollision = { 0, 0xC77, 0, 32768,{ 0,1,0},{2,0,0},  0,0 };
void  SetChunkTextureID(NJS_CNK_MODEL* a1, __int16 a2)
{
	signed int v2; // esi
	int v3; // eax
	NJS_CNK_MODEL* v4; // edx
	unsigned __int8 v5; // cl
	__int16 v6; // cx
	short* v7; // eax

	v2 = 0;
	if (a1)
	{
		v3 = (int)a1->plist;
		if (v3)
		{
			v4 = a1 + 1;
			while (1)
			{
				do
				{
					while (1)
					{
						while (1)
						{
							while (1)
							{
							LABEL_4:
								while (1)
								{
									v5 = *(char*)v3;
									if (*(char*)v3 >= 1u)
									{
										break;
									}
									v3 += 2;
								}
								if (v5 >= 8u)
								{
									break;
								}
								v3 += 2;
							}
							if (v5 >= 0x10u)
							{
								break;
							}
							if (v5 == 8)
							{
								v6 = *(short*)(v3 + 2);
								v7 = (short*)(v3 + 2);
								if (v2 < 4 && SLOWORD(v4->vlist) < 0)
								{
									*(short*)(v4->vlist) = v6 & 0x1FFF;
									++v2;
									v4 = (NJS_CNK_MODEL*)((char*)v4 + 2);
								}
								*v7 = a2 & 0x1FFF | v6 & 0xC000;
								v3 = (int)(v7 + 1);
							}
							else
							{
							LABEL_16:
								v3 += 4;
							}
						}
						if (v5 >= 0x20u)
						{
							break;
						}
						v3 += 4;
						switch (v5)
						{
						case 17u:
						case 18u:
						case 20u:
							goto LABEL_16;
						case 19u:
						case 21u:
						case 22u:
							v3 += 8;
							break;
						case 23u:
							v3 += 12;
							break;
						default:
							goto LABEL_4;
						}
					}
				} while (v5 < 0x38u || v5 < 0x40u);
				if (v5 >= 0xFFu)
				{
					break;
				}
				v3 += 2 + 2 * (unsigned __int16)(*(short*)(v3 + 2) + 1);
			}
		}
	}
}

enum
{
	MD_TV_SLEEP = 0x0,
	MD_TV_ON = 0x1,
	MD_TV_ON_MAIN = 0x2,
	MD_TV_OFF = 0x3,
	MD_TV_OFF_MAIN = 0x4,
	MD_TV_NOISE = 0x5,
	MD_TV_NOISE_MAIN = 0x6,
	MD_TV_SPORTS = 0x7,
	MD_TV_SPORTS_MAIN = 0x8,
	MD_TV_ANIME = 0x9,
	MD_TV_ANIME_MAIN = 0xA,
	MD_TV_DRAMA = 0xB,
	MD_TV_DRAMA_MAIN = 0xC,
	//CWE STARTS HERE
	MD_TV_CONSOLE = 0xD, 
	MD_TV_CONSOLE_MAIN = 0xE,
};


DataPointer(NJS_TEXLIST, AL_TOY_TEXLIST, 0x01366AFC);
short word_12C1EF4[] = { 1,2,3,4,5,0 };
short word_12C226C[] = { 0x10, 0x11 };
short word_13047C4[] = { 6,7,8,0 };
short word_13047CC[] = { 9,0x0A, 0x0B, 0 };
short word_13047D4[] = { 0x0F, 0x0E, 0x0D, 0x0C };
short word_13047DC[] = { 0x0C, 0x0D, 0x0E, 0x0F };
void TV_Main(ObjectMaster* a1)
{

	ObjectMaster* v1; // esi
	EntityData1* v2; // ebp
	char v3; // al
	char v4; // zf
	char v5; // sf
	unsigned __int8 v6; // of
	int v7; // eax
	double v9; // st7
	char v10; // al
	unsigned __int16 v11; // ax
	char v12; // al
	__int16 v14; // di
	ObjectMaster* v15; // eax
	char v16; // al
	char v17; // al
	ObjectMaster* v18; // eax
	char v19; // al
	int v20; // eax
	ObjectMaster* v21; // eax
	ObjectMaster* v22; // eax
	float a1a; // [esp+24h] [ebp+4h]

	v1 = a1;
	v2 = a1->Data1.Entity;
	;
	switch (v2->Action)
	{
	case MD_TV_ON:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, word_13047D4[0]);
		}
		++v2->Action;
		v2->NextAction = 0;
		goto LABEL_5;
	case MD_TV_ON_MAIN:
	LABEL_5:
		v3 = v2->Index++;
		if ((unsigned __int8)v3 > 4u)
		{
			v6 = __OFSUB__(++v2->Rotation.x, 3);
			v4 = v2->Rotation.x == 3;
			v5 = v2->Rotation.x - 3 < 0;
			v2->Index = 0;
			if (!((unsigned __int8)(v5 ^ v6) | (unsigned __int8)v4))
			{
				v22 = 0;// (ObjectMaster*)CCL_IsHitKindEx(a1, 304);
				if (v22) //is playing wii
				{
					v2->Action = MD_TV_CONSOLE;
					v2->field_6 = 0;
					float test = (njRandom() * 4.0f);
					v2->Status = test;
				}
				else
				{
					v7 = rand();
					v2->Rotation.x = 3;
					v2->NextAction = 0;
					v2->Index = 0;
					a1a = njRandom();
					v9 = a1a;
					if (a1a >= 0.30000001)
					{
						if (v9 >= 0.60000002)
						{
							if (v9 >= 0.8999999761581421)
							{
								v2->Action = 5;
								v2->field_6 = 0;
							}
							else
							{
								v2->Action = 11;
								v2->field_6 = 0;
								//PlayJingle(TVChannel1);
							}
						}
						else
						{
							v2->Action = 9;
							v2->field_6 = 0;
							//PlayJingle(TVChannel2);
						}
					}
					else
					{
						v2->Action = 7;
						v2->field_6 = 0;
						//PlayJingle(TVChannel3);
					}
				}
			}
			//v2->Action = 9;

			if (TV.child->chunkmodel)
			{
				SetChunkTextureID(TV.child->chunkmodel, word_13047D4[v2->Rotation.x]);
			}
		}
		goto LABEL_24;
	case MD_TV_OFF:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, word_13047DC[0]);
		}
		//InitJingle();
		++v2->Action;
		v2->NextAction = 0;
		goto LABEL_19;
	case MD_TV_OFF_MAIN:
	LABEL_19:
		v10 = v2->Index++;
		if ((unsigned __int8)v10 > 4u)
		{
			v6 = __OFSUB__(++v2->Rotation.x, 3);
			v4 = v2->Rotation.x == 3;
			v5 = v2->Rotation.x - 3 < 0;
			v2->Index = 0;
			if (!((unsigned __int8)(v5 ^ v6) | (unsigned __int8)v4))
			{
				v2->Rotation.x = 3;
				v2->Action = 0;
				v2->NextAction = 0;
				v2->field_6 = 0;
				v2->Index = 0;
			}
			if (TV.child->chunkmodel)
			{
				SetChunkTextureID(TV.child->chunkmodel, word_13047DC[v2->Rotation.x]);
			}
		}
		goto LABEL_24;
	case MD_TV_NOISE:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, word_12C226C[0]);
		}
		++v2->Action;
		v2->NextAction = 0;
		goto LABEL_30;
	case MD_TV_NOISE_MAIN:
	LABEL_30:
		v12 = v2->Index++;
		if ((unsigned __int8)v12 <= 1u)
		{
			goto LABEL_36;
		}
		v6 = __OFSUB__(++v2->Rotation.x, 1);
		v4 = v2->Rotation.x == 1;
		v5 = v2->Rotation.x - 1 < 0;
		v2->Index = 0;
		if (!((unsigned __int8)(v5 ^ v6) | (unsigned __int8)v4))
		{
			v2->Rotation.x = 0;
		}

		if (!TV.child->chunkmodel)
		{
			goto LABEL_36;
		}
		v14 = word_12C226C[v2->Rotation.x];
		goto LABEL_35;
	case MD_TV_SPORTS:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, word_12C1EF4[0]);
		}
		++v2->Action;
		v2->NextAction = 0;
		goto LABEL_43;
	case MD_TV_SPORTS_MAIN:
	LABEL_43:
		v16 = v2->Index++;
		if ((unsigned __int8)v16 > 8u)
		{
			v6 = __OFSUB__(++v2->Rotation.x, 4);
			v4 = v2->Rotation.x == 4;
			v5 = v2->Rotation.x - 4 < 0;
			v2->Index = 0;
			if (!((unsigned __int8)(v5 ^ v6) | (unsigned __int8)v4))
			{
				v2->Rotation.x = 0;
			}

			if (TV.child->chunkmodel)
			{
				v14 = word_12C1EF4[v2->Rotation.x];
			LABEL_35:
				SetChunkTextureID(TV.child->chunkmodel, v14);
			}
		}
	LABEL_36:
		v15 = (ObjectMaster*)CCL_IsHitKindEx(a1, 0xB1u);
		if (!v15 || !*((int*)v15 + 1))
		{
			goto LABEL_24;
		}
		v2->Action = 9;
		goto LABEL_39;
	case MD_TV_ANIME:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, word_13047C4[0]);
		}
		++v2->Action;
		v2->NextAction = 0;
		goto LABEL_51;
	case MD_TV_ANIME_MAIN:
	LABEL_51:
		v17 = v2->Index++;
		if ((unsigned __int8)v17 > 0xCu)
		{
			v6 = __OFSUB__(++v2->Rotation.x, 2);
			v4 = v2->Rotation.x == 2;
			v5 = v2->Rotation.x - 2 < 0;
			v2->Index = 0;
			if (!((unsigned __int8)(v5 ^ v6) | (unsigned __int8)v4))
			{
				v2->Rotation.x = 0;
			}
			if (TV.child->chunkmodel)
			{
				SetChunkTextureID(TV.child->chunkmodel, word_13047C4[v2->Rotation.x]);
			}
		}
		v18 = (ObjectMaster*)CCL_IsHitKindEx(a1, 0xB1u);
		if (!v18 || !*((int*)v18 + 1))
		{
			goto LABEL_24;
		}
		v2->Action = 11;
		goto LABEL_39;
	case MD_TV_DRAMA:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, word_13047CC[0]);
		}
		++v2->Action;
		v2->NextAction = 0;
		goto LABEL_62;
	case MD_TV_DRAMA_MAIN:
	LABEL_62:
		v19 = v2->Index++;
		if ((unsigned __int8)v19 > 0x96u)
		{
			v2->Index = 0;
			v20 = (unsigned __int16)(signed int)(njRandom() * 2.9f);
			v2->Rotation.x = v20;
			if (TV.child->chunkmodel)
			{
				SetChunkTextureID(TV.child->chunkmodel, word_13047CC[v2->Rotation.x]);
			}
		}
		v21 = (ObjectMaster*)CCL_IsHitKindEx(a1, 0xB1u);
		if (!v21 || !*((int*)v21 + 1))
		{
			goto LABEL_24;
		}
		v2->Action = 7;
	LABEL_39:
		v2->NextAction = 0;
		v2->field_6 = 0;
		v2->Index = 0;
		goto LABEL_25;
	case MD_TV_CONSOLE:
		v2->Rotation.x = 0;
		if (TV.child->chunkmodel)
		{
			SetChunkTextureID(TV.child->chunkmodel, 121 + v2->Rotation.x + 4 * v2->Status);
		}
		++v2->Action;
		v2->NextAction = 0;
		break;
	case MD_TV_CONSOLE_MAIN:
		v12 = v2->Index++;
		if ((unsigned __int8)v12 <= 30)
		{
			goto LABEL_36;
		}
		v6 = __OFSUB__(++v2->Rotation.x, 3);
		v4 = v2->Rotation.x == 3;
		v5 = v2->Rotation.x - 3 < 0;
		v2->Index = 0;
		if (!((unsigned __int8)(v5 ^ v6) | (unsigned __int8)v4))
		{
			v2->Rotation.x = 0;
		}

		if (!TV.child->chunkmodel)
		{
			goto LABEL_36;
		}
		v14 = 121 + v2->Rotation.x + 4 * v2->Status;
		goto LABEL_35;
	default:
	LABEL_24:
		if (v2->Action)
		{
		LABEL_25:
			v11 = v2->field_6;
			v2->field_6 = v11 + 1;
			if (v11 > 0xE10u)
			{
				v2->Action = 3;
				v2->NextAction = 0;
				v2->field_6 = 0;
				v2->Index = 0;

				return;
			}
		}
		else
		{
			v22 = (ObjectMaster*)CCL_IsHitKindEx(a1, 175);
			if (v22)
			{
				v22 = (ObjectMaster*)CCL_IsHitKindEx(a1, 303);
				if (v22) //is playing wii
					sub_55A920(3600, &v2->Position, a1, 304, 25.5);

				v2->Action = 1;
				v2->NextAction = 0;
				v2->field_6 = 0;
				v2->Index = 0;

				ObjectMaster* field = sub_55A920(3600, &v2->Position, a1, 201, 25.5);

			}
		}


	}
	AddToCollisionList(a1);
	//Chao_AddToCollisionList(a1);
	//RunObjectChildren(a1);
	//a1->DisplaySub(a1);
}

void TV_load_hook()
{
	NJS_VECTOR v31;
	v31.x = 30.0;
	v31.y = 1.5;
	v31.z = -83.0;
	ALO_TVExecutor_Load(&v31);
	PlaceConsole(&v31);
}

void CWETV_Init()
{
	WriteCall((void*)0x0054CADB, TV_load_hook);
	WriteJump(ALO_TVExecutor_Main, TV_Main);
}