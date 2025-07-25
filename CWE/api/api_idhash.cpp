#include "stdafx.h"
#include "xxhash32.h"
#include "api_idhash.h"
#include <cnk_util.h>

// Finds the first vlist and plist in the object tree, expects the out variables to be NULL initialized beforehand
static void FindFirstChunkModelLists(const NJS_OBJECT* pObj, Sint32*& outVlist, Sint16*& outPlist) {
	while (!outVlist || !outPlist) {
        const NJS_CNK_MODEL* pModel = pObj->chunkmodel;

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

uint32_t GenerateHashForChunkObject(const NJS_OBJECT* pObj) {
	uint32_t myseed = 0;
	XXHash32 myhash(myseed);

    Sint32* vlist = NULL;
    Sint16* plist = NULL;

	FindFirstChunkModelLists(pObj, vlist, plist);
	if (!vlist || !plist) {
        // todo: feedback
        return -1;
    }

    myhash.add(vlist, mtCnkVListSize(vlist));
	myhash.add(plist, mtCnkPListSize(plist));

	return myhash.hash();
}