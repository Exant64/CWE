#include "SA2ModLoader.h"
#include "Chao.h"
#include <math.h>
#include "ninja_functions.h"
#include <vector>

double __fastcall njOuterProduct(NJS_VECTOR* a1, NJS_VECTOR* a2, NJS_VECTOR* a3)
{
	float  v3; // st7
	float  v4; // st6
	float  v5; // st5
	float v6; // ST00_4

	v3 = a2->z * a1->y - a2->y * a1->z;
	v4 = a2->x * a1->z - a2->z * a1->x;
	v5 = a2->y * a1->x - a2->x * a1->y;
	a3->x = v3;
	a3->y = v4;
	a3->z = v5;
	v6 = v5 * v5 + v4 * v4 + v3 * v3;
	return sqrtf(v6);
}

void  QuaternionLookRotation(NJS_QUATERNION* quaternion, Vector3* forward, Vector3* up)
{
	Vector3 crossOut;
	//forward.Normalize();
	njUnitVector(forward);
	Vector3 vector = *forward;
	njOuterProduct(up, &vector, &crossOut);
	njUnitVector(&crossOut);
	Vector3 vector2 = crossOut;
	Vector3 vector3;
	njOuterProduct(&vector, &vector2, &vector3);
	float m00 = vector2.x;
	float m01 = vector2.y;
	float m02 = vector2.z;
	float m10 = vector3.x;
	float m11 = vector3.y;
	float m12 = vector3.z;
	float m20 = vector.x;
	float m21 = vector.y;
	float m22 = vector.z;


	float num8 = (m00 + m11) + m22;
	//var quaternion = new Quaternion();
	if (num8 > 0)
	{
		float num = (float)sqrtf(num8 + 1);
		quaternion->re = num * 0.5f;
		num = 0.5f / num;
		quaternion->im[0] = (m12 - m21) * num;
		quaternion->im[1] = (m20 - m02) * num;
		quaternion->im[2] = (m01 - m10) * num;
		return;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)sqrtf(((1 + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion->im[0] = 0.5f * num7;
		quaternion->im[1] = (m01 + m10) * num4;
		quaternion->im[2] = (m02 + m20) * num4;
		quaternion->re = (m12 - m21) * num4;
		return;
	}
	if (m11 > m22)
	{
		float num6 = (float)sqrtf(((1 + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion->im[0] = (m10 + m01) * num3;
		quaternion->im[1] = 0.5f * num6;
		quaternion->im[2] = (m21 + m12) * num3;
		quaternion->re = (m20 - m02) * num3;
		return;
	}
	float num5 = (float)sqrtf(((1 + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion->im[0] = (m20 + m02) * num2;
	quaternion->im[1] = (m21 + m12) * num2;
	quaternion->im[2] = 0.5f * num5;
	quaternion->re = (m01 - m10) * num2;

}
void AL_GetBlushPos(NJS_OBJECT* obj, NJS_VECTOR* pos, NJS_QUATERNION* rot)
{
	if (!obj) return;
	if (!obj->model) return;
	struct StripChunk {
		Sint16 Index;
		Sint16 u, v;
	};
	struct CNK_VN_VERTEX
	{
		NJS_POINT3 Pos;
		NJS_POINT3 Normal;
	};
	struct al_model
	{
		int* VList;
		__int16* PList;
		NJS_POINT3 Center;
		float Radius;
		__int16 OrgTexId[4];
		int PListSize;
		int nbVertex;
		CNK_VN_VERTEX* pVertex;
	};
	std::vector<int> indices;
	al_model* mouthMdl = (al_model*)obj->model;
	CNK_VN_VERTEX* pVertex = mouthMdl->pVertex;
	Sint32* plist = (Sint32*)mouthMdl->PList;

	//turn tristrips into triangles
	while (plist)
	{
		unsigned char v5 = (*(Sint16*)plist) & 0xFF;
		if (v5 == NJD_CE) break;
		if (v5 >= NJD_STRIPOFF) {
			Sint16* poly16 = (Sint16*)plist;
			Uint8 Flags = (Uint8)(*poly16 >> 8);
			Uint16 Header = *poly16;
			poly16++;
			Uint16 Size = *poly16;
			poly16++;
			Sint16* polyNext = poly16 + Size;
			Uint16 Header2 = *poly16;
			poly16++;
			int stripCount = Header2 & 0x3FFF;
			int primitiveCount = 0;
			for (int strips = 0; strips < stripCount; strips++)
			{
				int indexCount = *poly16;
				bool flip = true;
				if ((*poly16 & 0x8000) == 0x8000) {
					indexCount = -indexCount;
					flip = false;
				}

				poly16++;
				StripChunk* strip = (StripChunk*)poly16;
				for (int i = 0; i < indexCount - 2; i++)
				{
					flip = !flip;

					if (!flip)
					{
						indices.push_back(strip[i].Index);
						indices.push_back(strip[i + 1].Index);
						indices.push_back(strip[i + 2].Index);
					}
					else
					{
						indices.push_back(strip[i + 1].Index);
						indices.push_back(strip[i].Index);
						indices.push_back(strip[i + 2].Index);
					}
				}
				poly16 += indexCount * 3;
			}
			plist = (Sint32*)polyNext;
		}
		else if (v5 >= NJD_TINYOFF)
		{
			if (v5 >= NJD_MATOFF)
				plist = plist + 3;
			else
				plist = plist + 1;
		}
	}

	//find top left tri index
	int topLeftTriIndex = 0;
	int topLeftPoint = 0;
	float topLeftPosX = 10000.0f;
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		for (int j = 0; j < 3; j++) {
			int index = indices[i + j];
			if (pVertex[index].Pos.x < topLeftPosX) {
				topLeftPosX = pVertex[index].Pos.x;
				topLeftTriIndex = i;
				topLeftPoint = i + j;
			}
		}
	}

	int belowTopLeftPoint = 0;
	//get point that shares 3 triangles
	for (int j = 0; j < 3; j++) {
		int index = indices[topLeftTriIndex + j];
		int sharedCount = 0;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			if (i != topLeftTriIndex) //if not the top left tri
			{
				for (int k = 0; k < 3; k++) {
					if (indices[i + k] == index)
						sharedCount++;
				}
			}
		}
		if (sharedCount == 3) {
			belowTopLeftPoint = topLeftTriIndex + j;
		}
	}

	*pos = pVertex[topLeftPoint].Pos;

	//quaternion calc
	NJS_VECTOR m1, m2;
	m1 = *pos;
	m2 = *(NJS_VECTOR*)obj->pos;
	NJS_QUATERNION quat;
	NJS_VECTOR up{ 0,1,0 };
	NJS_VECTOR forward;
	forward.x = m2.x - m1.x;
	forward.y = m2.y - m1.y;
	forward.z = m2.z - m1.z;
	QuaternionLookRotation(&quat, &forward, &up);
}
