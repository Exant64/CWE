#include "stdafx.h"

#include "memory.h"
#include "al_msg_warn.h"
#include <ui/al_ortho.h>
#include <ui/al_tween.h>
#include "FunctionHook.h"
#include "al_parameter.h"
#include "ninja_functions.h"
#include "asmutil.h"
#include "ChaoMain.h"
#include "al_minimal.h"
#include "alg_kinder_he.h"
#include "al_sandhole.h"
#include "alg_kinder_ortho.h"
#include "al_behavior/al_knowledge.h"
#include "Chao.h"
#include "alo_obakehead.h"
#include "al_modelcontainer.h"
#include <api/api_tree.h>
#include <al_garden_info.h>
#include <alo_ringwin.h>
#include <windows.h>
#include "playsound.h"
#include "ui/al_tween_types.h"

enum {
	MD_UPGRADE_WARN_INIT = 0,
	MD_UPGRADE_WARN_IDLE = 1,
	MD_UPGRADE_WARN_WAIT_FOR_CLOSE = 2,
};

struct HealthCenterCWE {
	const char* pConditionMsg;
	task* pMotherChaoTask;
	task* pFatherChaoTask;
	task* pTweenTask;
	Sint8 inUpgrade;
	Sint8 upgradeWarnMode;
	Sint8 upgradeCursor;
};
#define GET_HE_CWE_WORK(work) ((HealthCenterCWE*)(work + 1))

FastcallFunctionPointer(void, AL_HealthCenter_SetMessage, (HealthCenter* a1, int a2), 0x058EB90);

DataArray(SAlgKinderOrthoQuad, HealthCenter_MenuBlue, 0x008A1DC0, 52);
DataArray(SAlgKinderOrthoQuad, HealthCenter_MenuYellow, 0x008A2150, 20);
DataArray(SAlgKinderOrthoQuad, HealthCenter_MenuPink, 0x008A2480, 20);

ASM_FUNC void DrawMedicalChartText(const char* TextPtr, float XPos, float YPos, float TextBoxLength, float TextSize, char TextAlignment) {
	// arguments
    ASM_PUSH(      ASM_ESP(6+0+0) ); // TextAlignment
    ASM_PUSH(      ASM_ESP(5+1+0) ); // TextSize
    ASM_PUSH(      ASM_ESP(4+2+0) ); // TextBoxLength
    ASM_PUSH(      ASM_ESP(3+3+0) ); // YPos
    ASM_PUSH(      ASM_ESP(2+4+0) ); // XPos
    ASM_MOVE( ecx, ASM_ESP(1+5+0) ); // TextPtr

    // call
    ASM_CALL_R( edx, 0x0058D880 );

    // end arguments
    ASM_ESP_ADD( 5 );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void sub_58DAA0(HealthCenter* work) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_MOVE( esi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x58DAA0 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

static ASM_FUNC void HE_MenuDraw(int a1, HealthCenter* TextLocation) {
    // save regs
    ASM_PUSH( esi );

    // arguments
    ASM_PUSH(      ASM_ESP(2+0+1) ); // TextLocation
    ASM_MOVE( esi, ASM_ESP(1+1+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x0058DB70 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // restore regs
    ASM_POP( esi );

    // return
    ASM_RET( 0 );
}

static const char* AL_HealthCenter_GetMsg(HealthCenter* work, const size_t id) {
	return (const char*)&work->dword60[*(int*)&work->dword60[4 * id]];
}

static NJS_TEXLIST* texlist_cwe_health_center = NULL;

static void HealthCenterItemDisp(task* a1) {
	HealthCenter* work = (HealthCenter*)a1->awp;

	if (!work->gap28) {
		return;
	}

	const auto kind = AL_GetHoldingItemKind();

	njPushMatrixEx();
	njTranslate(NULL, 0.34388199f, work->field_2C + 3.284631013870239f, -2.164542f);

	switch (AL_GetHoldingItemCategory()) {
		case ALW_CATEGORY_MINIMAL:
			if (kind >= 21 && kind <= 24) {
				DataPointer(NJS_TEXLIST, CHAOSDRIVE_TEXLIST, 0x1717DAC);
				DataPointer(float, flt_B18F54, 0xB18F54);
				DataArray(NJS_CNK_OBJECT*, dword_171A294, 0x171A294, 4);

				njScale(NULL, 1.2f, 1.2f, 1.2f);
				njTranslate(NULL, 0, flt_B18F54, 0);
				njSetTexture(&CHAOSDRIVE_TEXLIST);
				njCnkDrawObject(dword_171A294[kind - 21]);
			}
			else {
				njSetTexture(ModAPI_MinimalTexlists[kind]);
				njCnkMotion(ModAPI_MinimalModels[kind], ModAPI_MinimalMotion0[kind], work->field_34);
			}

			break;

		case ALW_CATEGORY_FRUIT:
			njTranslate(NULL, 0, 1.3f, 0);
			ObjectRegistry::DrawObject<njCnkDrawObject>(ALW_CATEGORY_FRUIT, kind);
			break;

		case ALW_CATEGORY_SEED:
			njSetTexture(ModAPI_SeedTexlists[kind]);
			njScale(NULL, 1.5f, 1.5f, 1.5f);
			njCnkDrawModel(ModAPI_SeedModels[kind]->model);
			break;

		case ALW_CATEGORY_MASK:
			switch (kind) {
				case 4:
				case 6:
				case 7:
				case 8:
				case 9:
					njTranslate(NULL, 0, 2.8f, 0);
					njRotateX(NULL, 0x8000);
					break;
				default:
					break;
			}

			ALO_ObakeHeadDraw(kind);
			break;
	}

	njPopMatrixEx();
}

//                             E    D       C    B      A        S         X 
static const int GradePurchasePrice[] = { 10000,20000,30000,40000, 9999999,9999999 ,9999999 };

static bool IsUpgradeBlockedByGuest(task* tp) {
	return AL_ParameterIsGuest(tp) && gConfigVal.GuestBlockStatChanges;
}

static bool CanStatBeUpgraded(task* tp, int stat) {
	if (IsUpgradeBlockedByGuest(tp)) {
		return false;
	}

	if (GET_CWEPARAM(tp)->UpgradeCounter < 3 &&
		GET_CHAOPARAM(tp)->Lev[stat] == 99 &&
		GET_CHAOPARAM(tp)->Abl[stat] < ChaoGrade_A &&
		gu32TotalRing >= GradePurchasePrice[GET_CHAOPARAM(tp)->Abl[stat]])
	{
		return true;
	}

	return false;
}

static bool CanChaoUpgrade(task* tp) {
	for (size_t i = 0; i < 5; ++i) {
		if (CanStatBeUpgraded(tp, i)) {
			return true;
		}
	}

	return false;
}

#define TABS_START_X 264
#define TABS_Y0 40
#define TABS_Y1 56
#define TAB_CORNER_WIDTH 8
#define TAB_MIDDLE_WIDTH (80 / 2)
#define TAB_WIDTH (2 * TAB_CORNER_WIDTH + TAB_MIDDLE_WIDTH)
#define TAB_START(i) (TABS_START_X + i * TAB_WIDTH)
#define TAB_MIDDLE_START(i) TAB_START(i) + TAB_CORNER_WIDTH

#define TAB_CORNER_L_POS(i) TAB_START(i), TAB_MIDDLE_START(i), TABS_Y0, TABS_Y1
#define TAB_MIDDLE_POS(i) TAB_MIDDLE_START(i), TAB_MIDDLE_START(i) + TAB_MIDDLE_WIDTH, TABS_Y0, TABS_Y1
#define TAB_CORNER_R_POS(i) TAB_MIDDLE_START(i) + TAB_MIDDLE_WIDTH, TAB_MIDDLE_START(i) + TAB_MIDDLE_WIDTH + TAB_CORNER_WIDTH, TABS_Y0, TABS_Y1

const static SAlgKinderOrthoQuad HE_TabsVanilla[] = {
	{ TAB_CORNER_L_POS(0), 16, 240, 16, 496 },
	{ TAB_MIDDLE_POS(0), 272, 496, 16, 496 },
	{ TAB_CORNER_R_POS(0), 528, 752, 16, 496 },

	{ TAB_CORNER_L_POS(1), 1552, 1776, 16, 496 },
	{ TAB_MIDDLE_POS(1), 1808, 2032, 16, 496 },
	{ TAB_CORNER_R_POS(1), 2064, 2288, 16, 496 },

	{ TAB_CORNER_L_POS(2), 16, 240, 2064, 2544 },
	{ TAB_MIDDLE_POS(2), 272, 496, 2064, 2544 },
	{ TAB_CORNER_R_POS(2), 528, 752, 2064, 2544 },
};

const static SAlgKinderOrthoQuad HE_TabsCWE[] = {
	{ TAB_CORNER_L_POS(3), 16, 240, 16, 496 },
	{ TAB_MIDDLE_POS(3), 272, 496, 16, 496 },
	{ TAB_CORNER_R_POS(3), 528, 752, 16, 496 },
};

constexpr static size_t HE_TabCount = (_countof(HE_TabsVanilla) + _countof(HE_TabsCWE)) / 3;
constexpr static Sint16 HE_LRIcon_Width = 32;
const static SAlgKinderOrthoQuad HE_LIcon = { TAB_MIDDLE_START(0), TAB_MIDDLE_START(0) + HE_LRIcon_Width, 40, 56, 3312,2320,  2064, 2544 };
const static SAlgKinderOrthoQuad HE_RIcon = { TAB_MIDDLE_START(3) + TAB_MIDDLE_WIDTH - HE_LRIcon_Width, TAB_MIDDLE_START(3) + TAB_MIDDLE_WIDTH, 40, 56, 2320, 3312, 2064, 2544 };

const static SAlgKinderOrthoQuad HE_ParentsMenuSpritesBase[] = {
	{ 264, TAB_START(0) + TAB_CORNER_WIDTH, 56, 72, 784, 1008, 16, 496 },
	{ 264, TAB_START(0) + TAB_CORNER_WIDTH, 72, 480, 784, 1008, 400, 496 },

	{ TAB_START(0) + TAB_CORNER_WIDTH, TAB_START(3), 56, 72, 1040, 1264, 16, 496 },
	{ TAB_START(3), TAB_START(3) + TAB_WIDTH, 56, 72, 1040, 1264, 400, 496 },
	{ TAB_START(3) + TAB_WIDTH, 616, 56, 72, 1040, 1264, 16, 496 },

	{ 616, 624, 56, 72, 1296, 1520, 16, 496 },
	{ 272, 616, 72, 480, 1040, 1264, 400, 496 },
	{ 616, 624, 72, 480, 1296, 1520, 400, 496 },

	// maybe the top genetics text
	{ 280, 288, 72, 96, 3344, 3568, 1180, 1936 },
	{ 288, 600, 72, 96, 3600, 3824, 1170, 1936 },
	{ 600, 608, 72, 96, 3856, 4080, 1180, 1936 },
};

const static SAlgKinderOrthoQuad HE_ParentsMenuSprites[] = {
	// "Grandparents" display part
	{ 280, 288, 290, 310, 784 - 784, 1008 - 784, 528, 1136 },
	{ 288, 600, 290, 310, 1040 - 784, 1264 - 784, 528, 1136 },
	{ 600, 608, 290, 310, 1296 - 784, 1520 - 784, 528, 1136 },

	// grandparents table
	{ 280, 288, 310, 390, 16, 240, 1168, 2032 }, 
	{ 288, 600, 310, 390, 272, 496, 1168, 2032 },
	{ 600, 608, 310, 390, 528, 752, 1168, 2032 },

	// name display left
	{ 280, 288, 112, 148, 3344, 3568, 16, 1136 }, //left part
	{ 288, 392, 112, 148, 3600, 3824, 16, 1136 }, //middle part
	{ 392, 400, 112, 148, 3856, 4080, 16, 1136 }, //right part

  	// name display right
	{ 488, 496, 112, 148, 3344, 3568, 16, 1136 }, //left part
	{ 496, 600, 112, 148, 3600, 3824, 16, 1136 }, //middle part
	{ 600, 608, 112, 148, 3856, 4080, 16, 1136 }, //right part

	{ 288, 392, 158, 266, 3088, 3312, 16, 240 }, //left chao display
	{ 496, 600, 158, 266, 3088, 3312, 16, 240 }, //right chao display
};

static void HE_ParentsMenu(HealthCenter* work) {
	auto pCweWork = GET_HE_CWE_WORK(work);

	sub_583C60();
	njSetTexture(texlist_cwe_health_center);
	njSetTextureNum(1, 0, 0, 0);
	AlgKinderOrthoQuadDrawArray(HE_ParentsMenuSpritesBase, _countof(HE_ParentsMenuSpritesBase), 0xFFFFFFFF);

	DrawMedicalChartText("Parents", 444, 83.5f, 999, 20.0f, 5);

	if (!pCweWork->pMotherChaoTask || !pCweWork->pFatherChaoTask) {
		DrawMedicalChartText("No data", 444, 130.0f, 999, 20.0f, 5);
		return;
	}

	auto pParam = GET_CWEPARAM(work->medicalChartChao_);

	char motherName[sizeof(AL_NAME) + 1];
	char fatherName[sizeof(AL_NAME) + 1];
	char mgmName[sizeof(AL_NAME) + 1];
	char mgfName[sizeof(AL_NAME) + 1];
	char fgmName[sizeof(AL_NAME) + 1];
	char fgfName[sizeof(AL_NAME) + 1];

	AL_ParameterConvertName(pParam->MotherName, motherName);
	AL_ParameterConvertName(pParam->MGroundMotherName, mgmName);
	AL_ParameterConvertName(pParam->MGroundFatherName, mgfName);
	AL_ParameterConvertName(pParam->FatherName, fatherName);
	AL_ParameterConvertName(pParam->FGroundMotherName, fgmName);
	AL_ParameterConvertName(pParam->FGroundFatherName, fgfName);

	sub_583C60();
	njSetTexture(texlist_cwe_health_center);
	njSetTextureNum(1, 0, 0, 0);
	AlgKinderOrthoQuadDrawArray(HE_ParentsMenuSprites, _countof(HE_ParentsMenuSprites), 0xFFFFFFFF);

	DrawMedicalChartText (
		"Grandparents",
		288,
		292,
		999,
		16,
		0
	);

	DrawMedicalChartText(motherName, 340, 130.f, 999, 24.0f, 5);
	DrawMedicalChartText(fatherName, 548, 130.f, 999, 24.0f, 5);
	DrawMedicalChartText(mgmName, 288.0, 305 + 20.0f, 999, 24.0f, 0);
	DrawMedicalChartText(mgfName, 288.0, 305 + 40.0f, 999, 24.0f, 0);
	DrawMedicalChartText(fgmName, 288.0f + 24 * 8.5f, 305 + 20.0f, 999, 24.0f, 0);
	DrawMedicalChartText(fgfName, 288.0f + 24 * 8.5f, 305 + 40.0f, 999, 24.0f, 0);

	VoidFunc(sub_42C1F0, 0x42C1F0);
	sub_42C1F0();

	DoLighting(10);

	const auto drawChao = [&work, &pCweWork](task* pChaoTask, float x, float y, float iconOffsetX) {
		njPushUnitMatrix();
		OrthoDrawBegin();
		OrthoScreenTranslate(x, y, 1.75f);
		njRotateY(NULL, NJM_ANG_DEG(180));
		work->Chao_DisplaySub(pChaoTask);
		OrthoDrawEnd();
		njPopMatrixEx();

		njPushUnitMatrix();
		ProjectToScreen(x + iconOffsetX, y, -23.f);
		work->Chao_field_20(pChaoTask);
		njPopMatrixEx();
	};

	if (pCweWork->pMotherChaoTask) {
		drawChao(pCweWork->pMotherChaoTask, 338, 272.f - 6, 0);
	}

	if (pCweWork->pFatherChaoTask) {
		drawChao(pCweWork->pFatherChaoTask, 548, 272.f - 6, -4.3f);
	}

	DoLighting(LightIndexBackupMaybe);
}

static void HE_DispTabs() {
	sub_583C60();

	njSetTexture((NJS_TEXLIST*)0x011D2ACC);
	njSetTextureNum(7, 0, 0, 0);
	AlgKinderOrthoQuadDrawArray(HE_TabsVanilla, _countof(HE_TabsVanilla), 0xFFFFFFFF);

	njSetTexture(texlist_cwe_health_center);
	njSetTextureNum(1, 0, 0, 0);
	AlgKinderOrthoQuadDrawArray(HE_TabsCWE, _countof(HE_TabsCWE), 0xFFFFFFFF);
	
	// we had to separate the R icon into our texlist so we can recolor it freely to match the last tab
	AlgKinderOrthoQuadDraw(&HE_LIcon, 0x000073FF);
	AlgKinderOrthoQuadDraw(&HE_RIcon, 0x10A010FF);
}

static void HE_DispBars(HealthCenter* work) {
	const auto pCweWork = GET_HE_CWE_WORK(work);

	const auto getColor = [&work, &pCweWork](size_t i) -> Uint32 {
		if(!pCweWork->inUpgrade) {
			return 0x009CBDFF;
		}

		CHAO_PARAM_GC* pParam = GET_CHAOPARAM(work->field_8);

		if (i == 5 || pParam->Abl[i] >= ChaoGrade_A) {
			return 0xA0A0A0FF;
		}

		if (!CanStatBeUpgraded(work->field_8, i)) {
			// return 0xC03030FF;
		}

		if (i == pCweWork->upgradeCursor) {
			// return 0xFFFF7DFF;
		}

		return 0x009CBDFF;
	};

	sub_583C60();

	njSetTexture(texlist_cwe_health_center);
	njSetTextureNum(1, 0, 0, 0);

	SAlgKinderOrthoQuad bar[] = {
		{ 296, 580, 260, 284, 1680, 1776, 2448, 2544 },
		{ 580, 596, 260, 284, 1552, 2032, 2064, 2800 }
	};

	const size_t barCount = (!pCweWork->inUpgrade) ? 6 : 5;
	for(size_t i = 0; i < barCount; ++i) {
		if (pCweWork->inUpgrade && i == pCweWork->upgradeCursor) {
			const Sint16 outlineWidth = 3;
			const SAlgKinderOrthoQuad outline[] = {
				{ 296 - outlineWidth, 580, Sint16(260 - outlineWidth + 32 * i), Sint16(284 + outlineWidth + 32 * i), 1680, 1776, 2448, 2544 },
				{ 580, 596 + 2 + outlineWidth, Sint16(260 - outlineWidth + 32 * i), Sint16(284 + outlineWidth + 32 * i), 1552, 2032, 2064, 2800 }
			};
			
			AlgKinderOrthoQuadDrawArray(outline, _countof(outline), 0xFFFF7DA0);
		}

		AlgKinderOrthoQuadDrawArray(bar, _countof(bar), getColor(i));

		bar[0].y0 += 32;
		bar[0].y1 += 32;
		bar[1].y0 += 32;
		bar[1].y1 += 32;
	}
}

static void HE_FirstTabDrawSkillInfo(HealthCenter* work) {
	DataArray(char, byte_8A274C, 0x008A274C, 8);

	for (size_t i = 0; i < 5; ++i) {
		int statIndex = byte_8A274C[i];
		if (statIndex >= 8u) {
			statIndex = 0;
		}

		const Uint16 stat = AL_ParameterGetSkill(work->field_8, statIndex);
		const Float y = 264 + float(i) * 32;

		AL_ParameterDrawSkillGauge(
			stat,
			380, 
			y + 1, 
			92, 
			14, 
			1
		);
		
		AL_ParameterDrawSkill(stat, 476, y, 1);
	}
}

static void HE_FirstTabDrawLifespanInfo(HealthCenter* work) {
	CHAO_PARAM_GC* pParam = GET_CHAOPARAM(work->field_8);

	sub_583C60();
	njSetTexture((NJS_TEXLIST*)0x011D2ACC);
	njSetTextureNum(7, 0, 0, 0);

	AL_ParameterDrawSkillGauge(
		(Uint32)((pParam->life / 3900.0f) * 1000),
		380.0f, 
		(264 + 32 * 5) + 1, 
		92.0f, 
		14.0f, 
		1.0f
	);

	AL_ParameterDrawSkill(pParam->life, 476, (264 + 32 * 5), 1.0f);

	sub_583C60();
	njSetTexture(texlist_cwe_health_center);
	njSetTextureNum(0, 0, 0, 0);

	const SAlgKinderOrthoQuad lifespan = { 305, 377, 423, 442, 85, 3655, 2790, 3400 };
	AlgKinderOrthoQuadDraw(&lifespan, 0xFFFFFFFF);

	chSetBillboardColor(1, 0.72f, 1, 0.4f);

	const Sint8 lifeTimeGene = NJM_MAX(pParam->gene.LifeTime[0], pParam->gene.LifeTime[1]);
	if(lifeTimeGene >= 0 && lifeTimeGene < 10) {
		chDrawBillboardSR(&ChaoTexCommonData[BBI_CMN_0 + lifeTimeGene], 561, (264 + 32 * 5) - 2, -1.2f, 1.25f, 1.25f, -1, -1);
	}
}

static void HE_DrawAbility(int abl, Sint16 x, Sint16 y) {
	SAlgKinderOrthoQuad gradeElem;
	gradeElem.x0 = x;
	gradeElem.x1 = x + 20;
	
	if (abl == 6 || abl == 7) {
		// X grade
		njSetTexture(texlist_cwe_health_center);
		njSetTextureNum(1, 0, 0, 0);

		gradeElem.u0 = 2850;
		gradeElem.u1 = 4096;
		gradeElem.v0 = 2800;
		gradeElem.v1 = 4096;
	}
	else {
		DataArray(Sint16, HE_GradeUVs, 0x8A2754, 1);

		njSetTexture((NJS_TEXLIST*)0x011D2ACC);
		njSetTextureNum(7, 0, 0, 0);

		gradeElem.u0 = HE_GradeUVs[2 * abl];
		gradeElem.u1 = HE_GradeUVs[2 * abl] + 624;
		gradeElem.v0 = HE_GradeUVs[(2 * abl) + 1];
		gradeElem.v1 = HE_GradeUVs[(2 * abl) + 1] + 624;
	}

	gradeElem.y0 = y;
	gradeElem.y1 = y + 20;

	AlgKinderOrthoQuadDraw(&gradeElem, -1);
}

static void HE_FirstTabDraw(HealthCenter* work) {
	if(!work->field_8) {
		return;
	}

	DataArray(char, byte_8A274C, 0x008A274C, 8);

	// UVs adjusted to fix it on HD port
	const SAlgKinderOrthoQuad HE_StatText[] = {
		{ 306, 353, 263, 280, 120, 2130, 20, 532 },
		{ 306, 335, 295, 315, 120, 1450, 585, 1170 },
		{ 306, 342, 327, 343, 120, 1750, 1200, 1680 },
		{ 306, 364, 359, 376, 120, 2650, 1720, 2220 },
		{ 305, 376, 390, 408, 108, 3400, 2220, 2760 },
	};

	auto workCwe = GET_HE_CWE_WORK(work);

	CHAO_PARAM_GC* pParam = GET_CHAOPARAM(work->field_8);
	CHAO_PARAM_CWE* pParamCwe = GET_CWEPARAM(pParam);

	VoidFunc(sub_4293B0, 0x4293B0);
	sub_4293B0();

	HE_DispBars(work);

	/* We have to draw everything above the bars ourselves instead of letting the game do it. */

	// draw stat texts
	njSetTexture(texlist_cwe_health_center);
	njSetTextureNum(0, 0, 0, 0);
	AlgKinderOrthoQuadDrawArray(HE_StatText, _countof(HE_StatText), 0xFFFFFFFF);

	if (!workCwe->inUpgrade) {
		const float textXPos = (Language != 2) ? 501 : 481;

		// "Skill" text
		DrawMedicalChartText(
			AL_HealthCenter_GetMsg(work, 11),
			textXPos,
			251,
			999,
			13,
			5
		);

		// "Ability" text
		DrawMedicalChartText(
			AL_HealthCenter_GetMsg(work, 9),
			565,
			251,
			999,
			13,
			5
		);

		HE_FirstTabDrawSkillInfo(work);

		sub_583C60();
		for (size_t i = 0; i < 5; i++) {
			int gradeIndex = byte_8A274C[i];
			if (gradeIndex >= 8u) {
				gradeIndex = 0;
			}

			HE_DrawAbility(pParam->Abl[gradeIndex], 558, 32 * i + 262);
		}

		HE_FirstTabDrawLifespanInfo(work);

		if (!IsUpgradeBlockedByGuest(work->field_8)) {
			sub_583C60();

			const Sint16 upgradeX = 251 + 36 + 6;
			const Sint16 upgradeY = 264 + 32 * 6 - 3;
			const SAlgKinderOrthoQuad yButton = { upgradeX, upgradeX + 18, upgradeY,  upgradeY + 18, 1, 4095, 1, 4095 };
			const SAlgKinderOrthoQuad upgradeText = { upgradeX + 18 + 2, upgradeX + 18 + 2 + 71, upgradeY + 2, upgradeY + 2 + 18, 80, 3640, 3450, 4056 };
			
			njSetTexture(texlist_cwe_health_center);

			njSetTextureNum(2, 0, 0, 0);
			AlgKinderOrthoQuadDraw(&yButton, 0xFFFFFFFF);

			njSetTextureNum(0, 0, 0, 0);
			AlgKinderOrthoQuadDraw(&upgradeText, 0xFFFFFFFF);
		}
	}
	else {
		const float lvlXPos = 472;
		const float digitSpace = 14.f;

		// "Ability" text
		DrawMedicalChartText(
			AL_HealthCenter_GetMsg(work, 9),
			420,
			251,
			999,
			13,
			5
		);

		DrawMedicalChartText(
			"Lv.",
			lvlXPos + digitSpace - 2,
			251,
			999,
			13,
			5
		);

		DrawMedicalChartText(
			"Price",
			568 - 1.5f * 14,
			251,
			999,
			13,
			5
		);

		for (size_t i = 0; i < 5; i++) {
			int statIndex = byte_8A274C[i];
			if (statIndex >= 8u) {
				statIndex = 0;
			}

			const Sint8 abl = pParam->Abl[statIndex];

			const Float y = 264 + float(i) * 32;

			const auto countDigits = [](int n) {
				int c = 0;

				while(n > 0) {
					c++;
					n /= 10;
				}

				return c;
			};

			int price = GradePurchasePrice[abl];
			const float width = digitSpace * countDigits(price);

			if (abl < ChaoGrade_A) {
				chSetBillboardColor(1, 1, 1, 1);

				if (gu32TotalRing < GradePurchasePrice[abl]) {
					chSetBillboardColor(1, 1, 0.25f, 0.25f);
				}
				else {
					chSetBillboardColor(1, 1, 1, 1);
				}

				int digit = 0;
				while(price > 0) {
					chDrawBillboardSR(&ChaoTexCommonData[price % 10], 568 - digitSpace * digit, y, -1.2f, 1, 1, -1, -1);

					price /= 10;
					digit++;
				}
			}
			else {
				const Sint16 dashX = Sint16(568 - digitSpace * 2.65f + 3);
				const Sint16 dashY = Sint16(y + 3);
				const SAlgKinderOrthoQuad dash = {dashX, Sint16(dashX + 35 * 0.85f), dashY, Sint16(dashY + 10 * 0.85f), 216, 1350, 2230, 2575 };

				sub_583C60();

				njSetTexture(texlist_cwe_health_center);
				njSetTextureNum(1, 0, 0, 0);

				AlgKinderOrthoQuadDraw(&dash, 0xFFFFFFFF);
			}

			const Sint8 lvl = pParam->Lev[statIndex];
			if (lvl != 99) {
				chSetBillboardColor(1, 1, 0.25f, 0.25f);
			}
			else {
				chSetBillboardColor(1, 1, 1, 1);
			}

			chDrawBillboardSR(&ChaoTexCommonData[(lvl / 10) % 10], lvlXPos, y, -1.2f, 1, 1, -1, -1);
			chDrawBillboardSR(&ChaoTexCommonData[lvl % 10], lvlXPos + digitSpace, y, -1.2f, 1, 1, -1, -1);

			const Sint16 ablBaseX = 410;
			sub_583C60();
			if (abl >= ChaoGrade_A) {
				HE_DrawAbility(abl, ablBaseX, 32 * i + 262);
			}
			else {
				const Sint16 ablY = Sint16(y) - 2;
				HE_DrawAbility(abl, ablBaseX - 25, ablY);
				HE_DrawAbility(abl + 1, ablBaseX + 25, ablY);

				const Sint16 arrowX = Sint16(ablBaseX - 3);
				const Sint16 arrowY = Sint16(ablY);
				const SAlgKinderOrthoQuad arrow = { arrowX, arrowX + 25, arrowY, Sint16(arrowY + 20), 340, 1180, 2880, 3540 };

				njSetTexture(texlist_cwe_health_center);
				njSetTextureNum(1, 0, 0, 0);

				AlgKinderOrthoQuadDraw(&arrow, 0xFFFFFFFF);
			}
		}

		char buff[32];
		sprintf_s(buff, "Upgrades left: %d", 3 - pParamCwe->UpgradeCounter);
		DrawMedicalChartText(
			buff,
			410 - 25 - 20,
			260 + 32 * 5,
			999,
			20,
			0
		);
	}
}

static void HE_SecondTabDraw(HealthCenter* work) {
	char buffer[256];

	sprintf_s(
		buffer,
		&work->dword60[*((int*)work->dword60 + 65)],
		GET_CHAOPARAM(work->field_8)->age / GET_CWEPARAM(work->field_8)->Birthday
	);
	DrawMedicalChartText(buffer, 288.0, 243.0, 999.0, 20.0, 0);

	sprintf_s(
		buffer,
		"Birthday in %d Chao days.",
		GET_CWEPARAM(work->field_8)->Birthday - (GET_CHAOPARAM(work->field_8)->age % GET_CWEPARAM(work->field_8)->Birthday)
	);
	DrawMedicalChartText(buffer, 288.0, 392, 999.0, 20.0, 0);
}

static task* CreateParentChao(const CHAO_ID& chaoID, AL_SHAPE_ELEMENT& fallbackShapeElement) {
	CHAO_PARAM_GC* pParam = NULL;
	AL_SHAPE_ELEMENT* pElement = &fallbackShapeElement;

	if (chaoID.id[0]) {
		if((pParam = AL_KW_FindChaoBasedOnId(chaoID))) {
			pElement = NULL;
		}
	}

	// if the shape element is the only data we have and 
	// its not initialized then dont create the task
	if (pElement && pElement->type < TYPE_CHILD) {
		return NULL;
	}

	NJS_POINT3 pos = {};
	task* pChaoTask = CreateChaoExtra(
		pParam,
		TRUE,
		pElement,
		&pos,
		0
	);
	
	GET_CHAOWK(pChaoTask)->ChaoFlag &= ~8u;
	GET_CHAOWK(pChaoTask)->ChaoFlag &= ~2u;
	GET_CHAOWK(pChaoTask)->ChaoFlag |= 0x2000;
	GET_CHAOWK(pChaoTask)->ChaoFlag &= ~0x10;
	GET_CHAOWK(pChaoTask)->ChaoFlag &= ~0x20000;

	GET_CHAOWK_CWE(pChaoTask)->ForceVisible = true;

	pChaoTask->disp = NULL;
	pChaoTask->disp_sort = NULL;

	return pChaoTask;
}

static void sub_58DAA0_r(HealthCenter* work) {
	sub_58DAA0(work);

	AL_GENE* pGene = &GET_CHAOPARAM(work->medicalChartChao_)->gene;
	auto pCweParam = GET_CWEPARAM(work->medicalChartChao_);
	auto pCweWork = GET_HE_CWE_WORK(work);
	
	pCweWork->pFatherChaoTask = CreateParentChao(pGene->FatherID, pCweParam->fatherData);
	pCweWork->pMotherChaoTask = CreateParentChao(pGene->MotherID, pCweParam->motherData);
}

static ASM_FUNC void sub_58DAA0_t() {
    ASM_PUSH (esi);

    ASM_PUSH (esi);
    ASM_CALL (sub_58DAA0_r);
	ASM_ESP_ADD (1);

    ASM_POP (esi);

    ASM_RET( 0 );
}

static void HE_MenuDisp_r(int a1, HealthCenter* work) {
	if (!work->openedMedicalChart) {
		return;
	}

	HE_DispTabs();

	// render the little blue corner piece for all menus other than the first
	if (work->medicalChartMenu) {
		njSetTexture((NJS_TEXLIST*)0x011D2ACC);
		njSetTextureNum(7, 0, 0, 0);
		AlgKinderOrthoQuadDraw(&HealthCenter_MenuYellow[4], 0xFFFFFFFF);
	}

	if (work->medicalChartMenu < 3) {
		HE_MenuDraw(a1, work);
	}

	switch (work->medicalChartMenu) {
		case 0:
			HE_FirstTabDraw(work);
			break;

		case 1:
			HE_SecondTabDraw(work);
			break;

		case 3:
			HE_ParentsMenu(work);
			break;
	}
}

static void ASM_FUNC HE_MenuDisp_t() {
	ASM_PUSH(esi); // a1

	ASM_PUSH(ASM_ESP(2)); // TextLocation
	ASM_PUSH(esi); // a1
	// Call your __cdecl function here:
	ASM_CALL (HE_MenuDisp_r);
	ASM_ESP_ADD( 2 ); // TextLocation

	ASM_POP(esi); // a1
	ASM_RET(0);
}

static ASM_FUNC void sub_58DB30(HealthCenter* work) {
    // save regs
    ASM_PUSH( edi );

    // arguments
    ASM_MOVE( edi, ASM_ESP(1+0+1) ); // a1

    // call
    ASM_CALL_R( edx, 0x58DB30 );

    // restore regs
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}

static void HE_MenuCleanup(HealthCenter* work) {
	auto pCweWork = GET_HE_CWE_WORK(work);

	sub_58DB30(work);

	if (pCweWork->pFatherChaoTask) {
		pCweWork->pFatherChaoTask->exec = DestroyTask;
	}

	if (pCweWork->pMotherChaoTask) {
		pCweWork->pMotherChaoTask->exec = DestroyTask;
	}
}

static void HE_MenuUpgradeExecutor(HealthCenter* work) {
	auto pCweWork = GET_HE_CWE_WORK(work);

	VoidFunc(sub_5438A0, 0x5438A0);
	VoidFunc(sub_543970, 0x543970);

	switch (pCweWork->upgradeWarnMode) {
		case MD_UPGRADE_WARN_INIT:
			sub_5438A0();
			pCweWork->upgradeWarnMode = MD_UPGRADE_WARN_IDLE;
			break;

		case MD_UPGRADE_WARN_IDLE:
			break;

		case MD_UPGRADE_WARN_WAIT_FOR_CLOSE:
			if (AL_MSGWarnKinderMessageArray[0].mWin && AL_MSGWarnKinderMessageArray[0].mWin->wordsLeftMaybe > 0) {
				return;
			}

			AlMsgWarnDelete(0);

			pCweWork->upgradeWarnMode = MD_UPGRADE_WARN_IDLE;
			break;
	}

	if (SWDATAE[0] & BTN_UP) {
		if (--pCweWork->upgradeCursor < 0) {
			pCweWork->upgradeCursor = 4;
		}

		SE_Call(TONE(8, 0), 0, 0, 0);
	}
	
	if (SWDATAE[0] & BTN_DOWN) {
		if (++pCweWork->upgradeCursor >= 5) {
			pCweWork->upgradeCursor = 0;
		}

		SE_Call(TONE(8, 0), 0, 0, 0);
	}

	if (SWDATAE[0] & BTN_A) {
		CHAO_PARAM_GC* pParam = GET_CHAOPARAM(work->field_8);
		CHAO_PARAM_CWE* pParamCwe = GET_CWEPARAM(pParam);
		const auto statIndex = pCweWork->upgradeCursor;

		if (CanStatBeUpgraded(work->field_8, statIndex)) {
			pParam->Lev[statIndex] = 1;
			pParam->Skill[statIndex] /= 100;

			pParamCwe->UpgradeCounter++;

			SE_Call_Timer(TONE(1, 5), (void*)0x616C7277, 1, 0, 120);

			ALO_RingWinAdd(-GradePurchasePrice[pParam->Abl[statIndex]]);
			pParam->Abl[statIndex]++;
		}
		else {
			AlMsgWarnCreate(0,50, 125, 200, 225);
			AlMsgWarnOpen(0);

			if (pParam->Abl[statIndex] >= ChaoGrade_A) {
				AlMsgWarnAddLineC(0,"You can't upgrade grades past grade A.");
			}
			else if (pParamCwe->UpgradeCounter >= 3) {
				AlMsgWarnAddLineC(0,"You don't have any upgrades left.");
			}
			else if (pParam->Lev[statIndex] < 99) {
				AlMsgWarnAddLineC(0,"You need to reach Level 99 for this stat to upgrade it.");
			}
			else if (gu32TotalRing < GradePurchasePrice[pParam->Abl[statIndex]]) {
				AlMsgWarnAddLineC(0,"You can't afford upgrading this stat.");
			}

			AlMsgWarnWaitClose(0);

			pCweWork->upgradeWarnMode = MD_UPGRADE_WARN_WAIT_FOR_CLOSE;
		}
	}

	if ((per[0]->press & (BTN_B | BTN_X)) && !pCweWork->pTweenTask) {
		sub_543970();

		pCweWork->pTweenTask = CreateTween(gRingWinTask, EASE_IN, INTERP_CUBIC, (float*)0x01A27870, 640.f, 20, [](task*) {
			ALO_RingWinDelete();
		});
		TweenSetTaskPointer<float>(pCweWork->pTweenTask, &pCweWork->pTweenTask);

		pCweWork->inUpgrade = FALSE;
		SE_Call(TONE(1, 10), 0, 0, 0);
	}
}

static void sub_58EA40_r (HealthCenter* work) {
	if (!work->openedMedicalChart) {
		return;
	}

	auto pCweWork = GET_HE_CWE_WORK(work);

	if (!work->medicalChartMenu && !pCweWork->inUpgrade) {
		if(!IsUpgradeBlockedByGuest(work->field_8) && (SWDATA[0] & BTN_Y) && !pCweWork->pTweenTask) {
			ALO_RingWinCreate(640, 80);

			pCweWork->pTweenTask = CreateTween(gRingWinTask, EASE_OUT, INTERP_CUBIC, (float*)0x01A27870, 400.f, 20, NULL);
			TweenSetTaskPointer<float>(pCweWork->pTweenTask, &pCweWork->pTweenTask);

			pCweWork->inUpgrade = TRUE;
			pCweWork->upgradeWarnMode = MD_UPGRADE_WARN_INIT;
			pCweWork->upgradeCursor = 0;
		}
	} 

	work->field_16 += 16;

	if (pCweWork->inUpgrade) {
		HE_MenuUpgradeExecutor(work);
		return;
	}

	if ((per[0]->press & BTN_L) || (SWDATAE[0] & BTN_LEFT)) {
		if (--work->medicalChartMenu < 0) {
			work->medicalChartMenu = 3;
		}

		SE_Call(TONE(8, 0), 0, 0, 0);
	}
	
	if ((per[0]->press & (BTN_R | BTN_A)) || (SWDATAE[0] & BTN_RIGHT)) {
		if (++work->medicalChartMenu == 4) {
			work->medicalChartMenu = 0;
		}

		SE_Call(TONE(8, 0), 0, 0, 0);
	}

	if (per[0]->press & (BTN_B | BTN_X)) {
		HE_MenuCleanup(work);

		SE_Call(TONE(1, 10), 0, 0, 0);
	}
}

static void ASM_FUNC sub_58EA40_t() {
	ASM_PUSH(eax); // a1

	// Call your __cdecl function here:
	ASM_CALL (sub_58EA40_r);

	ASM_POP(eax); // a1
	ASM_RET(0);
}

static void __fastcall InitDoctorHook(HealthCenter* work, int a2) {
	GET_HE_CWE_WORK(work)->pConditionMsg = "Your Chao is very healthy.\nPlease take good care of your Chao.";
	AL_HealthCenter_SetMessage(work, 108);
}

static void __fastcall AL_HealthCenter_SetMessageStr(HealthCenter* a1, const char* a2) {
	*(uint32_t*)0x01A267D0 = 0xFFFFFFFF;
	AlMsgWinAddLineC(a1->pkindercomessagething3C, a2, Language == 0);
}

static int AL_HealthCenter_GetSickness(task* tp) {
	AL_EMOTION* pEmotion = &GET_CHAOPARAM(tp)->emotion;
	
	size_t i = 0;
	while (pEmotion->IllState[i] >= 0) {
		if (++i >= 6) {
			return -1;
		}
	}

	pEmotion->IllState[i] = 100;

	switch (i) {
		case 0:
			return 3;
		case 1:
			return 0;
		case 2:
			return 4;
		case 3:
			return 2;
		case 4:
			return 5;
		case 5:
			return 1;
	}

	return 0;
}

static void ConditionText(HealthCenter* work) {
	DataArray(short, word_8A1DA0, 0x008A1DA0, 6);

	int sicknessIndex = AL_HealthCenter_GetSickness(work->field_8);
	if(sicknessIndex != -1) {
		DrawMedicalChartText(AL_HealthCenter_GetMsg(work, word_8A1DA0[sicknessIndex]), 288, 307, 312, 20, 0);
		return;
	}

	DrawMedicalChartText(GET_HE_CWE_WORK(work)->pConditionMsg, 288, 307, 312, 20, 0);
}

static void ASM_FUNC ConditionTextHook() {
	ASM_PUSH(eax); // a2
	ASM_CALL (ConditionText);
	ASM_POP(eax); // a2

	ASM_RET(0);
}

static void Doctor_PersonalityText(task* tp, std::vector<const char*>& messages, int emotion, const char* a1, const char* a2, const char* a3) {
	int val = AL_EmotionGetValue(tp, emotion);

	if (val >= -80 && val <= -40 && a1) {
		messages.push_back(a1);
	}
	else if (val == 0 && a2) {
		messages.push_back(a2);
	}
	else if (val >= 40 && val <= 80 && a3) {
		messages.push_back(a3);
	}
}

static void __fastcall DoctorMessage(HealthCenter* work, int a2) {
	if (!work->field_8) {
		return;
	}

	task* pChaoTask = work->field_8;
	CHAO_PARAM_GC* pParam = GET_CHAOPARAM(pChaoTask);

	if (!pParam) {
		return;
	}
	
	std::vector<const char*> messageValues;

	int age = pParam->age / GET_CWEPARAM(pChaoTask)->Birthday;
	
	messageValues.push_back("Your Chao is very healthy.\nPlease take good care of your Chao.");
	
	if (AL_IsChild(pChaoTask)) {
		messageValues.push_back("This Chao is still a baby. \n It'll be fun to see your Chao grow!");
	}
	else if (pParam->like >= 0 && pParam->like < 50) {
		messageValues.push_back("It doesn't look like you're\ntaking very good care of your Chao");
	}
	else if (pParam->like < 80) {
		messageValues.push_back("Looks like you're taking  \ngood care of your Chao.            ");
	}
	else if (pParam->like <= 100) {
		messageValues.push_back("This Chao looks very happy\nThis is how you take care of Chao! ");
	}
	
	if (pParam->nbSucceed == 1 || pParam->nbSucceed == 2) {
		messageValues.push_back("Take care of this Chao the\nway you have been so far.          ");
	}
	else if (pParam->nbSucceed == 3 || pParam->nbSucceed == 4) {
		messageValues.push_back("You really love your Chao.\nI can't believe this!              ");
	}
	else if (pParam->nbSucceed > 4) {
		messageValues.push_back("Looks like this Chao      \ntransformed so many times!         ");
	}
	else if (age >= 4 && age <= 5) {
		messageValues.push_back("This Chao has had a long  \nlife! Please take care of your Chao");
	}

	if (AL_EmotionGetValue(pChaoTask, EM_ST_BREED) > 8000) messageValues.push_back("Your chao is gonna have a \nbaby! Make sure you have space.    ");
	else if (AL_EmotionGetValue(pChaoTask, EM_ST_BREED) > 5000) messageValues.push_back("Your Chao can mate another\nChao without Heart Fruits.         ");
	
	if (AL_EmotionGetValue(pChaoTask, EM_ST_HUNGER) > 8000) messageValues.push_back("This Chao is hungry.      \nPlease feed your Chao.             ");
	if (AL_EmotionGetValue(pChaoTask, EM_ST_SLEEPY) > 8000) messageValues.push_back("Looks like this Chao is sleepy.\nLet your Chao rest.           ");
	if (AL_EmotionGetValue(pChaoTask, EM_ST_SLEEP_DEPTH) > 0) messageValues.push_back("This Chao looks tired.    \nI think this Chao needs a nap.     ");
	if (AL_EmotionGetValue(pChaoTask, EM_ST_TEDIOUS) > 4000) messageValues.push_back("Looks like this Chao is   \nbored. It wants to play.           ");

	if (AL_EmotionGetValue(pChaoTask, EM_ST_THIRSTY) > 8000) messageValues.push_back("Looks like this Chao is   \nready to swim.                     ");

	if (AL_EmotionGetValue(pChaoTask, EM_MD_SORROW) > 0 &&
		AL_EmotionGetValue(pChaoTask, EM_MD_FEAR) > 0 &&
		AL_EmotionGetValue(pChaoTask, EM_MD_PAIN) > 0) {
		messageValues.push_back("Looks like Chao is in pain\nDid you hurt your Chao?            ");
	}

	//hat
	switch (pParam->body.ObakeHead) {
		case OBAKE_HEAD_PARTS_PUMPKIN:
			messageValues.push_back("This Chao is wearing a    \npumpkin hat. You scared me!        ");
			break;
		case OBAKE_HEAD_PARTS_SKULL:
			messageValues.push_back("Are you wearing a skeleton\nhat? Oh No. What are you?          ");
			break;
		case OBAKE_HEAD_PARTS_WMELON:
			messageValues.push_back("A watermelon?             \nThat's strange...                  ");
			break;
		case OBAKE_HEAD_PARTS_APPLE:
			messageValues.push_back("Is this Chao wearing an   \napple? What happened?              ");
			break;
		case OBAKE_HEAD_PARTS_FLOWERPOT:
			messageValues.push_back("This Chao is wearing a pot\nCan it cook?                       ");
			break;
		case OBAKE_HEAD_PARTS_CAN:
			messageValues.push_back("Is that a tin can?        \nAre you okay?                      ");
			break;
		case OBAKE_HEAD_PARTS_PAPERBAG:
			messageValues.push_back("This chao is wearing a    \npaper bag. Is it embarrassed?      ");
			break;
		case OBAKE_HEAD_PARTS_CDBOX:
			messageValues.push_back("A carboard box? Don't be  \nafraid to show your face okay?     ");
			break;
		default:
			if (pParam->body.ObakeHead >= OBAKE_HEAD_PARTS_EGG && pParam->body.ObakeHead <= OBAKE_HEAD_PARTS_EGG_Env2) {
				messageValues.push_back("This chao is wearing an   \neggshell. Did it hatch properly?   ");
			}

			break;
	}
	
	if (AL_EmotionGetValue(pChaoTask, EM_ST_LONELY) > 6000) {
		messageValues.push_back("This Chao looks lonely.   \nDo you spend time with your Chao?  ");
	}
	else if (AL_KW_GetFriendCount(pChaoTask) > 12) {
		messageValues.push_back("This Chao is so popular.  \nThis Chao is friends with everybody");
	}

	if (pParam->body.FormNum == 1) {
		messageValues.push_back("This is an egg....   \nWait a minute...  ");
	}
	else if (pParam->body.FormNum == 2) {
		messageValues.push_back("This is a nice Omochao.  \nThe Chao that built this did a good job.");
	}

	if (pParam->Abl[7] >= 10) {
		messageValues.push_back("This Chao is very smart.  \nThis Chao must be a fast learner.  ");
	}
	else if (pParam->Abl[7] >= 4 && pParam->Abl[7] <= 9) {
		messageValues.push_back("This Chao is well-behaved.\nit will learn a lot.               ");
	}

	if (pParam->Abl[6] >= 10) {
		messageValues.push_back("This is a very lucky Chao.\nI'm so jealous!!                   ");
	}
	else if (pParam->Abl[6] >= 4 && pParam->Abl[6] <= 9) {
		messageValues.push_back("This Chao is lucky.       \nPlease take good care of your Chao.");
	}

	if (pParam->Lev[0] == 99 && pParam->Lev[1] == 99 &&
		pParam->Lev[2] == 99 && pParam->Lev[3] == 99 &&
		pParam->Lev[4] == 99) {
		messageValues.push_back("You really love your Chao.\nYou're an excellent Chao trainer!  ");
	}

	if (pParam->type > 2 &&
		pParam->Lev[0] < 10 && pParam->Lev[1] < 10 &&
		pParam->Lev[2] < 10 && pParam->Lev[3] < 10 &&
		pParam->Lev[4] < 10) {
		messageValues.push_back("This Chao is spoiled.     \nIs this Chao potty trained?        ");
	}

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_CURIOSITY,
		"This Chao is easily bored.\nThis Chao must keep you busy.      ",
		"This Chao is so quiet.    \nBe sure to take it outside to play.",
		"This Chao is very curious.\nThis Chao will try anything.       ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_KINDNESS,
		"This Chao gets frustrated easily.\n It's not healthy.          ",
		"This Chao seems to behave normally\nTake good care of your Chao",
		"This Chao is very kind!\nYour Chao must have a lot of friends. ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_AGRESSIVE,
		"This Chao cries a lot.    \nEveryone will make fun of you.     ",
		nullptr,
		"Such an energetic Chao!   \nTake it easy, OK?                  ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_SLEEPY_HEAD,
		"This Chao is very naive.  \nYou have to teach it!              ",
		nullptr,
		"This Chao is experienced.  \nPlease take care of your Chao.     ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_SOLITUDE,
		"This Chao seems pretty confident.\nDon't overdo it, okay?      ",
		nullptr,
		"This Chao like to be lonely.\nTake good care of your Chao.     ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_VITALITY,
		"This Chao likes to sit around a lot.\nYou have a laid back Chao",
		nullptr,
		"It seems mating season\nis coming soon for the young one!      ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_SOLITUDE,
		nullptr,
		nullptr,
		"This Chao seems to learn  \nquickly. Take care of your Chao.   ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_CHATTY,
		"This Chao is very shy.    \nIt's not very confident, is it?    ",
		"This Chao seems to behave normally\nTake good care of your Chao",
		"This Chao is very chatty!\nYour Chao must have a lot of friends");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_GLUTTON,
		"This chao doesn't eat much\nDon't over feed your Chao!         ",
		"This Chao eats very well. \nPlease take good care of your Chao.",
		"This Chao eats more than  \nusual. Make sure you feed it!      ");

	Doctor_PersonalityText(pChaoTask, messageValues, EM_PER_CALM,
		"This Chao seems to be anxious and stressed out\nTake it easy.  ",
		"This Chao has some potential.\nPlease take care of your Chao.  ",
		"This Chao is relaxed.     \nPlease take care of your Chao.     ");
	
	const char* msgVal = messageValues[(size_t)(njRandom() * ((float)messageValues.size() - 0.01f))];

	AL_HealthCenter_SetMessageStr(work, msgVal);
	GET_HE_CWE_WORK(work)->pConditionMsg = msgVal;
}

static FunctionHook<void, task*> HealthCenterExec_t(0x58F9B0);
static void HealthCenterExec_r(task* tp) {
	if (!tp->awp) {
		texlist_cwe_health_center = texCreateTexlist("CWE_HEALTH_CENTER");
	}

	HealthCenterExec_t.Original(tp);
}

static FunctionHook<void, task*> HealthCenterFree_t(0x58F940);
static void HealthCenterFree_r(task* tp) {
	if (texlist_cwe_health_center) {
		njReleaseTexture(texlist_cwe_health_center);
		FREE(texlist_cwe_health_center);

		texlist_cwe_health_center = NULL;
	}

	HealthCenterFree_t.Original(tp);
}

// we fake a pointer to a memorymanager struct here, that would have allocate as the first member
static void* HE_WorkAlloc_t() {
	return MemoryManager__Allocate(sizeof(HealthCenter) + sizeof(HealthCenterCWE) + 4, __FILE__, __LINE__);
}

static void *(*HE_WorkAlloc_t_ptr)() = &HE_WorkAlloc_t;
static void* HE_WorkAlloc_t_fakemem = &HE_WorkAlloc_t_ptr;

static void HE_WorkClear_t(void* p, int, size_t) {
	memset(p, 0, sizeof(HealthCenter) + sizeof(HealthCenterCWE));
}

void alg_kinder_he_Init() {
	// texture load and free hooks
	HealthCenterExec_t.Hook(HealthCenterExec_r);
	HealthCenterFree_t.Hook(HealthCenterFree_r);

	// expand work struct
	// due to the push instructions to allocate and memset having Sint8 args and our new struct size
	// not fitting in it, we have to hook the allocate and memset calls directly to override the args
	WriteData((void**)0x58F9BD, (void*)&HE_WorkAlloc_t_fakemem);
	WriteCall((void*)0x58F9E0, (void*)HE_WorkClear_t);

	/* We have to draw the stat texts, bars, points, and grades ourselves
	   Both because we draw the bars later now, which causes them to draw below,
	   and also because we need to be able to draw X grades */
	WriteCall((void*)0x58E01F, (void*)nullsub_1);
	WriteCall((void*)0x58DC25, (void*)nullsub_1);
	WriteCall((void*)0x58DF23, (void*)nullsub_1);
	WriteCall((void*)0x58DF44, (void*)nullsub_1);

	// kill ability and skill text so we can control when they're drawn for upgrade menu
	WriteCall((void*)0x58DCF5, (void*)nullsub_1);
	WriteCall((void*)0x58DD31, (void*)nullsub_1);

	WriteCall((void*)0x0058E8FC, (void*)nullsub_1); //killing the age call so that i can run my own 
	WriteCall((void*)0x0058F9A3, (void*)HE_MenuDisp_t);
	WriteJump((void*)0x58EA40, (void*)sub_58EA40_t);

	WriteCall((void*)0x58F67B, (void*)sub_58DAA0_t);

	if (gConfigVal.DoctorChaoInfo) {
		WriteCall((void*)0x0058F6FE, (void*)DoctorMessage);
		WriteCall((void*)0x0058F558, (void*)InitDoctorHook);
		WriteCall((void*)0x0058E9BA, (void*)ConditionTextHook);
	}

	// doctor item display
	WriteJump((void*)0x58F980, (void*)HealthCenterItemDisp);

	/* extend the memset zero for the chao displays, 
	   I have no idea why they do this in the first place */
	WriteData((int*)0x0058EFE5, (int)0x800);
	WriteData((int*)0x0058F040, (int)0x800);

	// stretch panel to fit lifespan
	HealthCenter_MenuBlue[35].y1 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[36].y1 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[37].y1 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[38].y0 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[39].y0 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[40].y0 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[38].y1 = HealthCenter_MenuBlue[38].y0 + 8;
	HealthCenter_MenuBlue[39].y1 = HealthCenter_MenuBlue[39].y0 + 8;
	HealthCenter_MenuBlue[40].y1 = HealthCenter_MenuBlue[40].y0 + 8;

	/* This removes the tabs and LR icons from each menu's sprites, and moves
	   the gap in the top of the panel according to the new tab positions. 
	   We also remove the peeking out corner of the first menu in the other
	   menus, and render it separately for all menus (other than the first). */
	
	HealthCenter_MenuBlue[12].x1 = TAB_START(1);
	HealthCenter_MenuBlue[13].x0 = HealthCenter_MenuBlue[12].x1;
	// we also remove the bars for the stats so we can draw them colored ourselves
	HealthCenter_MenuBlue[41] = HealthCenter_MenuBlue[51]; 
	WriteData((void**)0x0058DBD7, (void*)&HealthCenter_MenuBlue[11]);
	WriteData((Uint8*)0x0058DBDC, Uint8(52-11-10));

	HealthCenter_MenuYellow[13].x1 = TAB_START(1);
	HealthCenter_MenuYellow[14].x0 = HealthCenter_MenuYellow[13].x1;
	HealthCenter_MenuYellow[14].x1 = TAB_START(2);
	HealthCenter_MenuYellow[15].x0 = HealthCenter_MenuYellow[14].x1;
	WriteData((void**)0x0058E5D7, (void*)&HealthCenter_MenuYellow[12]);
	WriteData((Uint8*)0x0058E5DD, Uint8(51-12));

	HealthCenter_MenuPink[13].x1 = TAB_START(2);
	HealthCenter_MenuPink[14].x0 = HealthCenter_MenuPink[13].x1;
	HealthCenter_MenuPink[14].x1 = TAB_START(3);
	HealthCenter_MenuPink[15].x0 = HealthCenter_MenuPink[14].x1;
	WriteData((void**)0x0058E2AD, (void*)&HealthCenter_MenuPink[12]);
	WriteData((Uint8*)0x0058E2B2, Uint8(44-12));
}