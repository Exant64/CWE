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

NJS_VECTOR NavSysMeshConvert::calcNormal(int a, int b, int c) {
	const float* v0 = &m_verts[a].x;
	const float* v1 = &m_verts[b].x;
	const float* v2 = &m_verts[c].x;
	NJS_VECTOR n;

	float e0[3], e1[3];
	for (int j = 0; j < 3; ++j)
	{
		e0[j] = v1[j] - v0[j];
		e1[j] = v2[j] - v0[j];
	}

	// cross vector
	n.x = e0[1] * e1[2] - e0[2] * e1[1];
	n.y = e0[2] * e1[0] - e0[0] * e1[2];
	n.z = e0[0] * e1[1] - e0[1] * e1[0];

	njUnitVector(&n);

	return n;
}

void NavSysMeshConvert::addTriangle(int a, int b, int c, uint8_t area) {
	m_tris.insert(m_tris.end(), { a, b, c });
	m_areas.push_back(area);

	m_normals.push_back(calcNormal(a, b, c));
}

void NavSysMeshConvert::addModel(const NJS_MODEL* pMdl, const uint8_t area) {
	const auto vertexStart = m_verts.size();
	m_verts.reserve(vertexStart + pMdl->nbPoint);

	for (int p = 0; p < pMdl->nbPoint; p++) {
		NJS_POINT3 point = pMdl->points[p];
		sub_426CC0(_nj_current_matrix_ptr_, &point, &point, 0);
		m_verts.push_back(point);
	}
	
	for (Uint16 m = 0; m < pMdl->nbMeshset; m++) {
		const auto& meshset = pMdl->meshsets[m];
		const auto type = meshset.type_matId & NJD_MESHSET_MASK;

		switch(type) {
			default:
				assert(false);
				break;

			case NJD_MESHSET_3:
				for (size_t ind = 0; ind < meshset.nbMesh; ind += 3) {
					addTriangle(
						vertexStart + meshset.meshes[ind],
						vertexStart + meshset.meshes[ind + 1],
						vertexStart + meshset.meshes[ind + 2],
						area
					);
				}
				break;

			case NJD_MESHSET_4:
				for (size_t ind = 0; ind < meshset.nbMesh; ind += 4) {
					addTriangle(
						vertexStart + meshset.meshes[ind],
						vertexStart + meshset.meshes[ind + 1],
						vertexStart + meshset.meshes[ind + 2],
						area
					);

					addTriangle(
						vertexStart + meshset.meshes[ind + 2],
						vertexStart + meshset.meshes[ind + 1],
						vertexStart + meshset.meshes[ind + 3],
						area
					);
				}
				break;

			case NJD_MESHSET_TRIMESH: {
				const Sint16* strips = meshset.meshes;
				for (size_t ind = 0; ind < meshset.nbMesh; ind++) {
					const Sint16 lenRev = *(strips++);
					const Sint16 length = lenRev & 0x7FFF;

					bool flip = !((lenRev & 0x8000) == 0x8000);
					for (Sint16 k = 0; k < length - 2; k++) {
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
			} break;
		}
	}
}

void NavSysMeshConvert::addObject(const NJS_OBJECT* pObj, const uint8_t area) {
	njPushUnitMatrix();

	njTranslate(NULL, pObj->pos[0], pObj->pos[1], pObj->pos[2]);

	njRotateZ(NULL, pObj->ang[2]);
	njRotateY(NULL, pObj->ang[1]);
	njRotateX(NULL, pObj->ang[0]);

	addModel(pObj->model, area);

	njPopMatrixEx();
}

NavSysMeshConvert::NavSysMeshConvert() {
	const auto* pLand = pObjLandTable;
	const OBJ_LANDENTRY* pEntries = pLand->ssDispCount + pLand->pLandEntry;

	for (short i = 0; i < pLand->ssCount - pLand->ssDispCount; i++) {
		const auto& entry = pEntries[i];

		// check for "wall"/"cant stand" flag
		if(entry.slAttribute & (1 << 12)) {
			continue;
		}
		
		const uint8_t area = (entry.slAttribute & (1<<1)) ? NAV_AREA_WATER : NAV_AREA_GROUND;
		addObject((NJS_OBJECT*)entry.pObject, area);
	}
}
