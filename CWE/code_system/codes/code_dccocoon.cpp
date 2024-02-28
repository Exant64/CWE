#include "stdafx.h"
#include "include/code_dccocoon.h"

void DCCocoon::Init() {
	NJS_OBJECT** mayu = (NJS_OBJECT * *)GetDllData("MayuObjectList");
	mayu[0] = mayu[2];
	mayu[1] = mayu[2];
}