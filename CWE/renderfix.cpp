#include "stdafx.h"
#include "ninja_functions.h"
#include "renderfix.h"
#include <assert.h>

RFAPI_CORE* rfapi_core = NULL;

bool RenderFix_IsEnabled() {
	return rfapi_core != NULL;
}

static void PreRenderFixBackwardsCompatibilityDraw() {
	rfapi_core->pApiRenderState->SetCnkSpecMode(RFRS_CNKSPECMD_NONE);

	SaveConstantAttr();
	OnConstantAttr(~NJD_FST_IA, 0);
}

static void PostRenderFixBackwardsCompatibilityDraw() {
	LoadConstantAttr();

	rfapi_core->pApiRenderState->SetCnkSpecMode(RFRS_CNKSPECMD_END);
}

void RenderFixBackwardsCompatibilityDrawObject(NJS_OBJECT* pObject) {
	if (!RenderFix_IsEnabled()) {
		njCnkDrawObject(pObject);
		return;
	}

	PreRenderFixBackwardsCompatibilityDraw();
	// todo: what even is the difference between this and njCnkDrawObject?
	rfapi_core->pApiDraw->CnkDrawObject(pObject);
	PostRenderFixBackwardsCompatibilityDraw();
}

void RenderFixBackwardsCompatibilityDrawModel(NJS_CNK_MODEL* pModel) {
	if (!RenderFix_IsEnabled()) {
		sub_42E660(pModel);
		return;
	}

	PreRenderFixBackwardsCompatibilityDraw();
	// todo: what even is the difference between this and njCnkDrawObject?
	rfapi_core->pApiDraw->CnkDrawModel(pModel);
	PostRenderFixBackwardsCompatibilityDraw();
}

void rfCnkNormalDrawObject(NJS_OBJECT* pObject) {
	assert(RenderFix_IsEnabled());

	rfapi_core->pApiRenderState->SetCnkFuncMode(RFRS_CNKFUNCMD_NORMAL);
	rfapi_core->pApiDraw->CnkDrawObject(pObject);
	rfapi_core->pApiRenderState->SetCnkFuncMode(RFRS_CNKFUNCMD_END);
}

void rfCnkNormalDrawModel(NJS_CNK_MODEL* pModel) {
	assert(RenderFix_IsEnabled());

	rfapi_core->pApiRenderState->SetCnkFuncMode(RFRS_CNKFUNCMD_NORMAL);
	rfapi_core->pApiDraw->CnkDrawModel(pModel);
	rfapi_core->pApiRenderState->SetCnkFuncMode(RFRS_CNKFUNCMD_END);
}

void RenderFix_Init(const HelperFunctions& helper) {
	auto* rf = helper.Mods->find("sa2-render-fix");
	if (!rf) return;

	rfapi_core = rf->GetDllExport<RFAPI_CORE*>("rfapi_core");
}