//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "stdafx.h"

#include "ninja_functions.h"
#include "navsys_gjloader.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <cassert>

rcMeshLoaderLvl::rcMeshLoaderLvl() :
	m_scale(1.0f),
	m_verts(0),
	m_tris(0),
	m_normals(0),
	m_vertCount(0),
	m_triCount(0)
{
}

rcMeshLoaderLvl::~rcMeshLoaderLvl()
{
	// delete[] m_verts;
	// delete[] m_normals;
	// delete[] m_tris;
}

void rcMeshLoaderLvl::addVertex(float x, float y, float z, int& cap)
{
	if (m_vertCount + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		float* nv = new float[cap * 3];
		if (m_vertCount)
			memcpy(nv, m_verts, m_vertCount * 3 * sizeof(float));
		delete[] m_verts;
		m_verts = nv;
	}
	float* dst = &m_verts[m_vertCount * 3];
	*dst++ = x * m_scale;
	*dst++ = y * m_scale;
	*dst++ = z * m_scale;
	m_vertCount++;
}

void rcMeshLoaderLvl::addTriangle(int a, int b, int c, int& cap)
{
	if (m_triCount + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		int* nv = new int[cap * 3];
		if (m_triCount)
			memcpy(nv, m_tris, m_triCount * 3 * sizeof(int));
		delete[] m_tris;
		m_tris = nv;
	}
	int* dst = &m_tris[m_triCount * 3];
	*dst++ = a;
	*dst++ = b;
	*dst++ = c;
	m_triCount++;
}

bool rcMeshLoaderLvl::load()
{
	const auto* landtable = CurrentLandTable;
	const COL* cols = landtable->ChunkModelCount + landtable->COLList;
	int cap = 0;
	int tcap = 0;
	for (short i = 0; i < landtable->COLCount - landtable->ChunkModelCount; i++) {
		const auto* pObj = cols->Model;
		const auto* pMdl = cols->Model->basicmodel;

		njPushUnitMatrix();

		njTranslate(NULL, pObj->pos[0], pObj->pos[1], pObj->pos[2]);

		njRotateZ(NULL, pObj->ang[2]);
		njRotateY(NULL, pObj->ang[1]);
		njRotateX(NULL, pObj->ang[0]);

		const auto vertexStart = m_vertCount;
		for (int p = 0; p < pMdl->nbPoint; p++) {
			NJS_POINT3 point = pMdl->points[p];
			sub_426CC0(_nj_current_matrix_ptr_, &point, &point, 0);
			addVertex(point.x, point.y, point.z, cap);
		}

		njPopMatrixEx();

		for (Uint16 m = 0; m < pMdl->nbMeshset; m++) {
			const auto& meshset = pMdl->meshsets[m];
			const auto type = meshset.type_matId >> 0xE;

			assert(type == 0 || type == 3);

			if (type == 0) {
				for (size_t ind = 0; ind < meshset.nbMesh; ind += 3) {
					addTriangle(
						vertexStart + meshset.meshes[ind],
						vertexStart + meshset.meshes[ind + 1],
						vertexStart + meshset.meshes[ind + 2],
						tcap
					);
				}
			}
			else if (type == 3) {
				const Sint16* strips = meshset.meshes;
				for (size_t ind = 0; ind < meshset.nbMesh; ind++) {
					const Sint16 lenRev = *(strips++);
					const Sint16 length = lenRev & 0x7FFF;

					bool flip = !((lenRev & 0x8000) == 0x8000);
					for (Sint16 strip = 0; strip < length - 2; strip++) {
						Sint16 k = strip;

						flip = !flip;
						if (!flip) {
							addTriangle(
								vertexStart + strips[k],
								vertexStart + strips[k + 1],
								vertexStart + strips[k + 2],
								tcap
							);
						}
						else {
							addTriangle(
								vertexStart + strips[k + 1],
								vertexStart + strips[k],
								vertexStart + strips[k + 2],
								tcap
							);
						}
					}

					strips += length;
				}
			}
		}
	
		cols++;
	}

	// Calculate normals.
	m_normals = new float[m_triCount * 3];
	for (int i = 0; i < m_triCount * 3; i += 3)
	{
		const float* v0 = &m_verts[m_tris[i] * 3];
		const float* v1 = &m_verts[m_tris[i + 1] * 3];
		const float* v2 = &m_verts[m_tris[i + 2] * 3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &m_normals[i];
		n[0] = e0[1] * e1[2] - e0[2] * e1[1];
		n[1] = e0[2] * e1[0] - e0[0] * e1[2];
		n[2] = e0[0] * e1[1] - e0[1] * e1[0];
		float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		if (d > 0)
		{
			d = 1.0f / d;
			n[0] *= d;
			n[1] *= d;
			n[2] *= d;
		}
	}
	return true;
}
