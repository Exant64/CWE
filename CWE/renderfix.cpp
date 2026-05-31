#include "stdafx.h"
#include "ninja_functions.h"
#include "renderfix.h"
#include <assert.h>

const RFAPI_CORE* rfapi_core = NULL;

bool RenderFix_IsEnabled() {
	return rfapi_core != NULL;
}

static void SetRenderFixBackwardsCompatibilityConstantAttr() {
	SaveConstantAttr();
	rfapi_core->pChunk->CnkSetControl(~RJD_CNK_CTRL_DBLIGHT, 0);
	OnConstantAttr(0, NJD_FST_IS | NJD_FST_DB);
	// 0xFF hack added to temporarily fix RF bug
	// this will be fixed soon after this commit most likely,
	// but let's keep the workaround to be safe for now
	OffConstantAttr(0xFF | NJD_FST_IA, 0);
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

extern "C" __declspec(dllexport)
int32_t RFAPI_Init(const RFAPI_CORE* pApi, const char* puPath, const HelperFunctions* pHelpFuncs, size_t ixMod) {
    if ( RFAPI_CHECKVER( pApi, 1,5,3,0 ) ) {
		rfapi_core = pApi;
	}
	
    return 0;
}

void RenderFix_VersionCheck(const HelperFunctions& helper) {
	auto* rf = helper.Mods->find("sa2-render-fix");
	if (!rf) return;
	
	// if RF was found but RFAPI_Init didn't run or was early exited
	// then rfapi_core didn't get set, and we know version wasn't new enough
	if(rfapi_core) return;

	MessageBoxA(
		0, 
		"RenderFix is out of date, please update to 1.5.3 or above.", 
		"Chao World Extended", 
		MB_ICONWARNING
	);
}