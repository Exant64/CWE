#pragma once
#include "stdafx.h"

// functions borrowed from Shaddatic's SAMT, thanks shad
#define CNK_NULLOFF_MAX    (NJD_CN        ) /* max null type                        */
#define CNK_BITSOFF_MAX    (NJD_CB_DP     ) /* max bits type                        */
#define CNK_TINYOFF_MAX    (NJD_CT_TID2   ) /* max bits type                        */
#define CNK_MATOFF_MAX     (NJD_CM_DAS2   ) /* max material type                    */
#define CNK_VERTOFF_MAX    (NJD_CV_NF_D8  ) /* max vertex type                      */
#define CNK_VOLOFF_MAX     (NJD_CO_ST     ) /* max volume type                      */
#define CNK_STRIPOFF_MAX   (NJD_CS_UVH2   ) /* max strip type                       */

#define CNK_GET_OFFTYPE(pcnk_)      (((Uint8*)(pcnk_))[0])

uint32_t GenerateHashForChunkObject(const NJS_OBJECT* pObj);