#include "stdafx.h"
#include "ninja_functions.h"
#include "renderfix.h"
#include <assert.h>

RFAPI_CORE* rfapi_core = NULL;

bool RenderFix_IsEnabled() {
	return rfapi_core != NULL;
}

static void SetRenderFixBackwardsCompatibilityConstantAttr() {
	SaveConstantAttr();
	SaveControl3D();

	OnControl3D(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);

	rfapi_core->pChunk->CnkSetControl(~RJD_CNK_CTRL_DBLIGHT, 0);
	OnConstantAttr(0, NJD_FST_IS | NJD_FST_DB);
	OffConstantAttr(NJD_FST_IA, 0);
}

static void RestoreRenderFixBackwardsCompatibilityAttr() {
	rfapi_core->pChunk->CnkSetControl(-1, RJD_CNK_CTRL_DBLIGHT);
	LoadConstantAttr();
	LoadControl3D();
}

void RenderFixBackwardsCompatibilityDrawObject(NJS_OBJECT* pObject) {
	if (!RenderFix_IsEnabled()) {
		njCnkDrawObject(pObject);
		return;
	}

	SetRenderFixBackwardsCompatibilityConstantAttr();
	rfapi_core->pChunk->CnkTransformObject(pObject, rfapi_core->pChunk->CnkNormalDrawModel);
	RestoreRenderFixBackwardsCompatibilityAttr();
}

void RenderFixBackwardsCompatibilityDrawModel(NJS_CNK_MODEL* pModel) {
	if (!RenderFix_IsEnabled()) {
		sub_42E660(pModel);
		return;
	}

	SetRenderFixBackwardsCompatibilityConstantAttr();
	rfapi_core->pChunk->CnkNormalDrawModel(pModel);
	RestoreRenderFixBackwardsCompatibilityAttr();
}

void rfCnkNormalDrawObject(NJS_OBJECT* pObject) {
	assert(RenderFix_IsEnabled());

	rfapi_core->pChunk->CnkTransformObject(pObject, rfapi_core->pChunk->CnkNormalDrawModel);
}

void rfCnkNormalDrawModel(NJS_CNK_MODEL* pModel) {
	assert(RenderFix_IsEnabled());

	rfapi_core->pChunk->CnkNormalDrawModel(pModel);
}

void RenderFix_Init(const HelperFunctions& helper) {
	auto* rf = helper.Mods->find("sa2-render-fix");
	if (!rf) return;

	rfapi_core = rf->GetDllExport<RFAPI_CORE*>("rfapi_core");
}