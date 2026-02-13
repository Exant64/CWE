#pragma once

#include "SA2ModLoader.h"

//ignore this, this is for our side of things
#ifdef CWEV9

#include <alg_kinder_bl.h>
#include <alg_kinder_ortho.h>
#include <api/api_metadata.h>

#else

#define METADATA_ID_SIZE 21

struct BlackMarketItemAttributes
{
	int PurchasePrice;
	int SalePrice;
	__int16 RequiredEmblems;
	__int16 Name;
	__int16 Descriptions;
	__int16 Unknown;
};

#endif
