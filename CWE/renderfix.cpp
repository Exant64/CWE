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
	rfapi_core->pChunk->CnkSetControl(~RJD_CNK_CTRL_DBLIGHT, 0);
	OnConstantAttr(0, NJD_FST_IS | NJD_FST_DB);
	OffConstantAttr(NJD_FST_IA, 0);
}

static void RestoreRenderFixBackwardsCompatibilityAttr() {
	rfapi_core->pChunk->CnkSetControl(-1, RJD_CNK_CTRL_DBLIGHT);
	LoadConstantAttr();
}

RenderFixBackwardsCompatibilityGuard::RenderFixBackwardsCompatibilityGuard() {
	SetRenderFixBackwardsCompatibilityConstantAttr();
}

RenderFixBackwardsCompatibilityGuard::~RenderFixBackwardsCompatibilityGuard() {
	RestoreRenderFixBackwardsCompatibilityAttr();
}

void RenderFixBackwardsCompatibilityDrawMotion(NJS_OBJECT* pObject, NJS_MOTION* pMotion, float frame) {
	if (!RenderFix_IsEnabled()) {
		njCnkMotion(pObject, pMotion, frame);
		return;
	}

	RF_BACKWARDS_COMPAT_GUARD();
	rfapi_core->pChunk->CnkTransformMotion(pObject, pMotion, frame, rfapi_core->pChunk->CnkSimpleDrawModel);
}

void RenderFixBackwardsCompatibilityDrawObject(NJS_OBJECT* pObject) {
	if (!RenderFix_IsEnabled()) {
		njCnkDrawObject(pObject);
		return;
	}

	RF_BACKWARDS_COMPAT_GUARD();
	rfapi_core->pChunk->CnkTransformObject(pObject, rfapi_core->pChunk->CnkNormalDrawModel);
}

void RenderFixBackwardsCompatibilityDrawModel(NJS_CNK_MODEL* pModel) {
	if (!RenderFix_IsEnabled()) {
		sub_42E660(pModel);
		return;
	}

	RF_BACKWARDS_COMPAT_GUARD();
	rfapi_core->pChunk->CnkNormalDrawModel(pModel);
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