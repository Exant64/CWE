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
#include "navsys_meshconvert.h"
#include "navsys.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
#include <cassert>

NavSysMeshConvert::NavSysMeshConvert() {
	const auto addTriangle = [this](uint32_t a, uint32_t b, uint32_t c, uint8_t area) {
		// todo: inefficient
		m_tris.push_back(a);
		m_tris.push_back(b);
		m_tris.push_back(c);

		m_areas.push_back(area);
	};

	const auto* landtable = CurrentLandTable;
	const COL* cols = landtable->ChunkModelCount + landtable->COLList;
	int tcap = 0;

	for (short i = 0; i < landtable->COLCount - landtable->ChunkModelCount; i++) {
		const uint8_t area = (cols->Flags & (1<<1)) ? NAV_AREA_WATER : NAV_AREA_GROUND;

		const auto* pObj = cols->Model;
		const auto* pMdl = cols->Model->basicmodel;

		njPushUnitMatrix();

		njTranslate(NULL, pObj->pos[0], pObj->pos[1], pObj->pos[2]);

		njRotateZ(NULL, pObj->ang[2]);
		njRotateY(NULL, pObj->ang[1]);
		njRotateX(NULL, pObj->ang[0]);

		const auto vertexStart = m_verts.size();

		m_verts.reserve(vertexStart + pMdl->nbPoint);

		for (int p = 0; p < pMdl->nbPoint; p++) {
			NJS_POINT3 point = pMdl->points[p];
			sub_426CC0(_nj_current_matrix_ptr_, &point, &point, 0);
			m_verts.push_back(point);
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
						area
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
								area
							);
						}
						else {
							addTriangle(
								vertexStart + strips[k + 1],
								vertexStart + strips[k],
								vertexStart + strips[k + 2],
								area
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
	m_normals.reserve(m_tris.size() / 3);
	for (size_t i = 0; i < m_tris.size(); i += 3)
	{
		const float* v0 = &m_verts[m_tris[i]].x;
		const float* v1 = &m_verts[m_tris[i + 1]].x;
		const float* v2 = &m_verts[m_tris[i + 2]].x;
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}

		NJS_VECTOR n;
		// cross vector
		n.x = e0[1] * e1[2] - e0[2] * e1[1];
		n.y = e0[2] * e1[0] - e0[0] * e1[2];
		n.z = e0[0] * e1[1] - e0[1] * e1[0];
		njUnitVector(&n);

		m_normals.push_back(n);
	}
}
