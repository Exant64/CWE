#include "stdafx.h"
#include "xxhash32.h"
#include "api_idhash.h"
#include <cnk_util.h>

// Finds the first vlist and plist in the object tree, expects the out variables to be NULL initialized beforehand
static void FindFirstChunkModelLists(const NJS_CNK_OBJECT* pObj, Sint32*& outVlist, Sint16*& outPlist) {
	while (!outVlist || !outPlist) {
        const NJS_CNK_MODEL* pModel = pObj->model;

        if (pModel) {
            if (!outVlist && pModel->vlist) outVlist = pModel->vlist;
            if (!outPlist && pModel->plist) outPlist = pModel->plist;
        }

		if (pObj->child) {
			FindFirstChunkModelLists(pObj->child, outVlist, outPlist);
		}

		pObj = pObj->sibling;

        if (!pObj) break;
	}
}

uint32_t GenerateHashForChunkObject(const NJS_CNK_OBJECT* pObj) {
	uint32_t myseed = 0;
	XXHash32 myhash(myseed);

    Sint32* vlist = NULL;
    Sint16* plist = NULL;

	FindFirstChunkModelLists(pObj, vlist, plist);
	if (!vlist || !plist) {
        return -1;
    }

    myhash.add(vlist, mtCnkVListSize(vlist));
	myhash.add(plist, mtCnkPListSize(plist));

	return myhash.hash();
}

uint32_t GenerateHashForLandTable(const OBJ_LANDTABLE* pLandtable) {
	XXHash32 xhash(0);

	const OBJ_LANDENTRY* cols = pLandtable->ssDispCount + pLandtable->pLandEntry;

	if(pLandtable->pPvmFileName) {
		xhash.add(pLandtable->pPvmFileName, strlen(pLandtable->pPvmFileName));
	}

	int cap = 0;
	int tcap = 0;
	for (short i = 0; i < pLandtable->ssCount - pLandtable->ssDispCount; i++) {
		const NJS_OBJECT* pObj = (NJS_OBJECT*)cols->pObject;
		const auto* pMdl = pObj->model;

		xhash.add(&pObj->evalflags, sizeof(pObj->evalflags));
		xhash.add(pObj->pos, sizeof(pObj->pos));
		xhash.add(pObj->ang, sizeof(pObj->ang));
		xhash.add(pObj->scl, sizeof(pObj->scl));

		if(pMdl) {
			if(pMdl->points) {
				xhash.add(pMdl->points, pMdl->nbPoint * sizeof(NJS_POINT3));
			}
		}
	}

	return xhash.hash();
}