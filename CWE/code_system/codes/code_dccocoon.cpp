#include "stdafx.h"
#include "include/code_dccocoon.h"

void DCCocoon::Init() {
	NJS_CNK_OBJECT** mayu = (NJS_CNK_OBJECT * *)GetDataDllProcAddr("MayuObjectList");
	mayu[0] = mayu[2];
	mayu[1] = mayu[2];
}