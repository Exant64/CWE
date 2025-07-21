#include "stdafx.h"
#include "xxhash32.h"
#include "api_idhash.h"

static size_t mtCnkVListSize(const Sint32* pVList)
{
    const Sint32* start = pVList;
    const Sint32* vlist = pVList;

    while (CNK_GET_OFFTYPE(vlist) != 0xFF)
    {
        /** Next data chunk **/
        vlist += ((uint16_t*)vlist)[1] + 1;
    }

    return ((uintptr_t)vlist - (uintptr_t)start) + 4;
}

static size_t mtCnkPListSize(const Sint16* pPList)
{
    const Sint16* start = pPList;
    const Sint16* plist = pPList;

    for (; ; )
    {
        const int type = CNK_GET_OFFTYPE(plist);

        if (type == NJD_CE)
        {
            /** NJD_ENDOFF **/
            break;
        }

        if (type == NJD_CN)
        {
            /** NJD_NULLOFF **/

            /** Next offset **/
            ++plist;
            continue;
        }

        if (type <= CNK_BITSOFF_MAX)
        {
            /** NJD_BITSOFF **/

            /** Next offset **/
            ++plist;
            continue;
        }

        if (type <= CNK_TINYOFF_MAX)
        {
            /** NJD_TINYOFF **/

            /** Next offset **/
            plist += 2;
            continue;
        }

        if (type <= CNK_MATOFF_MAX)
        {
            /** NJD_MATOFF **/

            /** Next offset **/
            plist += ((uint16_t*)plist)[1] + 2;
            continue;
        }

        if (type <= CNK_STRIPOFF_MAX) // and volume
        {
            /** NJD_STRIPOFF **/

            /** Next offset **/
            plist += ((uint16_t*)plist)[1] + 2;
            continue;
        }

        /** An error occured, stop **/
        break;
    }

    return ((uintptr_t)plist - (uintptr_t)start) + 2;
}

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