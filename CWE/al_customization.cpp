#pragma once
#include "stdafx.h"

#include "ninja_functions.h"
#include "al_world.h"
#include "Chao.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "ChaoMain.h"
#include "alo_obakehead.h"
#include "AL_ModAPI.h"

#pragma warning(push)
#pragma warning( disable: 4838 )
#include "data/al_model/ali_medal_ame.nja"
#include "data/al_model/ali_medal_eme.nja"
#include "data/al_model/ali_medal_pal.nja"
#include "data/al_model/ali_medal_rub.nja"
#include "data/al_model/ali_model_saf.nja"
#include "data/al_model/ali_medal_none.nja"
#pragma warning(pop)

#include "alg_kinder_he.h"
#include "al_odekake.h"
#include <functional>
#include <array>
#include "ui/controller.h"
#include "ui/selectable.h"
#include "ui/al_ortho.h"
#include "al_ode_guide.h"

#include "al_ode_menu.h"
#include <api/api_accessory.h>
#include <al_behavior/al_behavior.h>
#include <al_garden_info.h>
#include <ui/al_tween.h>
#include <al_name.h>
#include <set>
#include <bit>

// the menu entry
static void AL_OdekakeCustomization(ODE_MENU_MASTER_WORK* a1);

CWE_API_ODEKAKE_ENTRY OdekakeCustomizationEntry = {
	AL_OdekakeCustomization, 
	nullptr, 
	ODE_FLAGS_REQUIRE_CHAO, 
	&stru_11BA528[32],
	&stru_11BA528[8],
	&stru_11BA528[9],
	&stru_11BA528[22],
	&stru_11BA528[23], 
	1.0, 1.0, 0.5, 0.5 
};

//constants
const int HatsSubMenuCount = AccessoryTypeCount + 1; //(accessories + vanilla hat)
const std::string baseLayerName = "base";
const std::string medalLayerName = "medal";
const std::string hataccLayerName = "hatacc";

//models
DataArray(NJS_OBJECT*, BigMedals, 0x012E58F8, 16);
extern NJS_OBJECT object_ala_full_mannequin;
extern NJS_OBJECT object_alo_mannequin;

static ObjectMaster* pChao = NULL;

static bool ColorMenuLastFrame = false;
static bool ColorMenuOpened = false;
static NJS_POINT2 HatAccMenuOffset;
static std::vector<ITEM_SAVE_INFO*> HatList;

static UIController* customizationController = nullptr;
static ObjectMaster* largeBar = 0;

struct AccessorySaveComparator {
	bool operator()(const AccessorySaveInfo* a, const AccessorySaveInfo* b) const {
		return a->IndexID < b->IndexID || 
			(a->IndexID == b->IndexID && a->UsedColors < b->UsedColors) || 
			(a->IndexID == b->IndexID && a->UsedColors == b->UsedColors && memcmp(a->Colors, b->Colors, sizeof(a->Colors)) < 0);
	}
};

static std::map<AccessorySaveInfo*, size_t, AccessorySaveComparator> AccessoryListCount;
static std::vector<AccessorySaveInfo*> AccessoryList;

static void UpdateHatAccVector() {
	HatList.clear();
	AccessoryList.clear();
	AccessoryListCount.clear();

	ITEM_SAVE_INFO* items = (ITEM_SAVE_INFO*)ChaoHatSlots;
	for (int j = 0; j < 24; j++)
	{
		if (items[j].Type > 0) {
			HatList.push_back(&items[j]);
		}
	}

	for (auto& item : AccessoryItemList) {
		if (item.IndexID == -1) continue;

		AccessoryListCount[&item]++;
	}

	for (auto& item : AccessoryListCount) {
		AccessoryList.push_back(item.first);
	}
}

bool AL_Customization_CreateAcc(int ID, AL_PARAM_ACCESSORY_INFO& accInfo, int Garden) {
	if (ID == -1) return false;

	auto save = (AccessorySaveInfo*)CWE_GetNewItemSaveInfo(ChaoItemCategory_Accessory);
	if (!save) {
		PrintDebug("HatAccRender: no hat slots left");
		return false;
	}

	save->UsedColors = accInfo.ColorFlags;
	memcpy(save->Colors, accInfo.ColorSlots, sizeof(save->Colors));

	save->Garden = Garden;
	save->IndexID = ID;
	save->Position = ProbablyChaoSpawnPoints[Garden * 16 + (int)(njRandom() * 15.f)];
	UpdateHatAccVector();

	return true;
}

static const void* const njDrawTextureEx_p = (void*)0x0077F6B0;
static void njDrawTextureEx(const NJS_TEXTURE_VTX* polygon, Int count, Int trans)
{
	count;

	__asm
	{
		push[trans]
		mov eax, [polygon]
		call njDrawTextureEx_p
		add esp, 4
	}
}

class LeftBarButtonOld : public UISelectable {
private:
	const std::string m_layerEnter;
	int m_buttonType;
public:
	enum LeftBarType : int {
		HatAcc = 0,
		Medal = 1,
		Exit = 2,
		Count
	};
	ChaoHudThingB MainButtons[LeftBarType::Count * 2] = {
		{1,59,25,0,   0,   0.3333333333f, 0.5f,  &CWE_UI_TEXLIST, 5},
		{1,59,25,0.3333333333f,0,   0.666666666f, 0.5f,  &CWE_UI_TEXLIST, 5},
		{1,59,25,0.666666666f,0,   1,    0.5f, &CWE_UI_TEXLIST, 5},
		{1,59,25,0,   0.5f, 0.3333333333f, 1,  &CWE_UI_TEXLIST, 5},
		{1,59,25,0.3333333333f,0.5f, 0.666666666f, 1,  &CWE_UI_TEXLIST, 5},
		{1,59,25,0.666666666f,0.5f, 1,    1, &CWE_UI_TEXLIST, 5},
	};

	void Press(UIController* controller) override {
		if (m_buttonType == Exit) {
			AL_OdekakeMenuMaster_Data_ptr->mode = 2;
			AL_OdekakeMenuMaster_Data_ptr->EndFlag = 1;
		}
		else {
			controller->SetCurrentLayer(m_layerEnter);
		}
	}
	void Disp() override {
		return;
		if (IsSelected())
			DrawChaoHudThingB(&MainButtons[m_buttonType + LeftBarType::Count], m_posX, m_posY, -1.2f, 1, 1, -1, -1);
		else
			DrawChaoHudThingB(&MainButtons[m_buttonType], m_posX, m_posY, -1.2f, 1, 1, -1, -1);
	}
	LeftBarButtonOld(const std::string& layerEnter, float posX, float posY, int buttonType) : UISelectable(layerEnter, SelectFlag::ForceVertical), m_layerEnter(layerEnter), m_buttonType(buttonType) {
		m_posX = posX;
		m_posY = posY;
	}
};

class LeftBarButton : public UISelectable {
private:
	enum class ButtonMode {
		Unselected,
		Selected,
		ToUnselected,
		ToSelected
	};

	ButtonMode m_mode;

	const std::string m_layerEnter;
	int m_buttonType;
	task* m_gbaTween = NULL;
	ChaoHudThingB m_leftPart = {1, 23, 46, 0.f, 41.f / 256.f, 22.f / 256.f, 88 / 256.f, &CWE_UI_TEXLIST, 36};
	ChaoHudThingB m_middlePart = { 1, 1, 46, 31.f / 256.f, 41.f / 256.f, 33.f / 256.f, 88 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_rightPart = { 1, 23, 46, 66.f / 256.f + 0.005f, 41.f / 256.f, 88.f / 256.f, 88 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedLeftPart = { 1, 23, 46, 0.f, 97.f / 256.f, 22.f / 256.f, 144 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedMiddlePart = { 1, 1, 46, 31.f / 256.f, 97.f / 256.f, 33.f / 256.f, 144 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedRightPart = { 1, 23, 46, 66.f / 256.f, 97.f / 256.f, 88.f / 256.f, 144 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_gba = { 1, 51, 42, 0, 214 / 256.f, 51 / 256.f, 1, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_text = { 1, 83, 27, 0, 0, 83 / 256.f, 27 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_icon = { 1, 46, 46, 0, 62.f / 256.f, 46 / 256.f, (62.f + 46) / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedIcon = { 1, 46, 46, 54 / 256.f, 62.f / 256.f, (54 + 46) / 256.f, (62.f + 46) / 256.f, &CWE_UI_TEXLIST, 36 };
public:
	enum LeftBarType : int {
		HatAcc = 0,
		Medal = 1,
		Exit = 2,
		Count
	};

	Angle m_ang = 0;
	float m_iconX = 0.f;
	float m_sclX = 1;
	float m_sclY = 1;
	float m_progress = 0.f;
	float m_textProgress = 0.f;
	Uint32 m_timer = 0;
	task* m_tween = NULL;
	task* m_textTween = NULL;
	float GetWidth() { return m_leftPart.wd; }
	float GetHeight() { return m_rightPart.ht; }

	void Press(UIController* controller) override {
		if (m_buttonType == Exit) {
			AL_OdekakeMenuMaster_Data_ptr->mode = 2;
			AL_OdekakeMenuMaster_Data_ptr->EndFlag = 1;
		}
		else {
			controller->SetCurrentLayer(m_layerEnter);
		}
	}

	void Exec() override {
	
	}

	static void DrawAsSprite(ChaoHudThingB& pHud, const float posX, const float posY, const float sclX, const float sclY, const Angle angle, const float z) {
		NJS_TEXANIM texanim = {
			pHud.wd,
			pHud.ht,
			pHud.wd / 2.f,
			pHud.ht / 2.f,
			Sint16(pHud.s0 * 256.f), Sint16(pHud.t0 * 256.f),
			Sint16(pHud.s1 * 256.f), Sint16(pHud.t1 * 256.f),
			pHud.TexNum,
			NJD_SPRITE_ALPHA
		};

		NJS_SPRITE sprite = {
			.p = {posX + pHud.wd / 2.f, posY + pHud.ht / 2.f, 0},
			.sx = sclX,
			.sy = sclY,
			.ang = angle,
			.tlist = pHud.pTexlist,
			.tanim = &texanim
		};

		njDrawSprite2D(&sprite, 0, z, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA);
	}

	void Disp() override {
		switch (m_mode) {
			case ButtonMode::Unselected:
				if (!IsSelected()) {
					m_timer = 0;
					m_ang = 0;
					if (!m_gbaTween && m_progress > 0.f)
					{
						m_gbaTween = CreateTween(NULL, EASE_TYPE::EASE_IN, INTERP_TYPE::INTERP_QUAD, &m_progress, 0.f, 15, NULL);
						TweenSetTaskPointer<float>(m_gbaTween, &m_gbaTween);
					}
					if (!m_textTween && m_textProgress > 0.f)
					{
						m_textTween = CreateTween(NULL, EASE_TYPE::EASE_IN, INTERP_TYPE::INTERP_CIRC, &m_textProgress, 0.f, 15, NULL);
						TweenSetTaskPointer<float>(m_textTween, &m_textTween);
					}
				}
				else {
					m_mode = ButtonMode::ToSelected;
				}
				break;
			case ButtonMode::ToSelected:
				if (m_gbaTween) {
					DeleteObject_(m_gbaTween);
					m_gbaTween = NULL;
				}

				if (m_textTween) {
					DeleteObject_(m_textTween);
					m_textTween = NULL;
				}
				
				m_mode = ButtonMode::Selected;
				break;
			case ButtonMode::Selected:
				if (!IsSelected()) {
					m_mode = ButtonMode::ToUnselected;
				}
				else {
					if (!m_timer++) {
						if (!m_gbaTween && m_progress < 1.f)
						{
							m_gbaTween = CreateTween(NULL, EASE_TYPE::EASE_OUT, INTERP_TYPE::INTERP_QUAD, &m_progress, 1.f, 25, NULL);
							TweenSetTaskPointer<float>(m_gbaTween, &m_gbaTween);
						}
						if (!m_textTween && m_textProgress < 1.f)
						{
							m_textTween = CreateTween(NULL, EASE_TYPE::EASE_OUT, INTERP_TYPE::INTERP_CIRC, &m_textProgress, 1.f, 40, NULL);
							TweenSetTaskPointer<float>(m_textTween, &m_textTween);
						}
					}
					m_ang += 256;
				}
				break;
			case ButtonMode::ToUnselected:
				if (m_gbaTween) {
					DeleteObject_(m_gbaTween);
					m_gbaTween = NULL;
				}

				if (m_textTween) {
					DeleteObject_(m_textTween);
					m_textTween = NULL;
				}

				m_mode = ButtonMode::Unselected;
				break;
			
		}

		SetChaoHUDThingBColor(1, 1, 1, 1);

		const float middle_width = 140;
		auto& originalIconSprite = IsSelected() ? m_selectedIcon : m_icon;

		NJS_TEXANIM texanim = {
			originalIconSprite.wd,
			originalIconSprite.ht,
			originalIconSprite.wd / 2.f,
			originalIconSprite.ht / 2.f,
			Sint16(originalIconSprite.s0 * 256.f), Sint16(originalIconSprite.t0 * 256.f),
			Sint16(originalIconSprite.s1 * 256.f), Sint16(originalIconSprite.t1 * 256.f),
			originalIconSprite.TexNum,
			NJD_SPRITE_ALPHA
		};

		NJS_SPRITE sprite = {
			.p = {m_posX + originalIconSprite.wd / 2.f, m_posY + originalIconSprite.ht / 2.f, 0},
			.sx = m_sclX,
			.sy = m_sclY,
			.ang = m_ang,
			.tlist = originalIconSprite.pTexlist,
			.tanim = &texanim
		};

		njDrawSprite2D(&sprite, 0, -2, NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA);

		DrawAsSprite(
			m_gba, 
			m_posX - m_progress * 20, 
			(1 - m_progress) * -7.f + m_posY, 
			m_sclX,
			m_sclY,
			m_progress * NJM_DEG_ANG(-10 + njSin(m_ang * 2) * 5), 
			-0.5f
		);

		m_text.s1 = m_text.s0 + m_textProgress * (m_text.wd / 256.f);
		DrawAsSprite(
			m_text, 
			m_posX + 13.f, 
			m_posY + 7.f, 
			m_sclX * m_textProgress,
			m_sclY,
			m_textProgress * NJM_DEG_ANG(25), 
			-0.5f
		);
	}

	LeftBarButton(const std::string& layerEnter, float posX, float posY, int buttonType) : UISelectable(layerEnter, 0), m_layerEnter(layerEnter), m_buttonType(buttonType) {
		m_posX = posX;
		m_posY = posY;
		m_mode = ButtonMode::Unselected;
	}
};

class BaseCustomizeBox : public UISelectable {
private:
	ChaoHudThingB m_sprite = { 1, 51 ,51, 94 / 256.f, 0, (94 + 51) / 256.f, (51.f) / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedSprite = { 1, 51 ,51, 154 / 256.f, 0, (154 + 51) / 256.f, (51.f) / 256.f, &CWE_UI_TEXLIST, 36 };
public:
	float m_sclX = 1;
	float m_sclY = 1;
	float GetWidth() { return m_sprite.wd; }
	float GetHeight() { return m_sprite.ht; }
	virtual void BoxContentDisp() {}
	virtual void ColorSet() {}
	virtual bool IsBoxSelected() const { return IsSelected(); }
	void Disp() override {
		SetChaoHUDThingBColor(1, 1, 1, 1);
		ColorSet();

		DrawChaoHudThingB(IsBoxSelected() ? &m_selectedSprite : & m_sprite, m_posX, m_posY, -2.2f, m_sclX, m_sclY, -1, -1);

		BoxContentDisp();
	}
	BaseCustomizeBox(const std::string& name, float posX, float posY, uint32_t flags) : UISelectable(name, flags) {
		m_posX = posX;
		m_posY = posY;
	}
};

class MedalCustomizeBox : public BaseCustomizeBox {
private:
	int m_index;
	int m_rotY = 0;

	bool AL_Customization_MedalUnlocked(int index)
	{
		int medal = GBAManager_GetChaoDataPointer()->DoctorMedal;
		int karateInfo = GBAManager_GetChaoDataPointer()->KarateInfo;
		switch (index)
		{
		case 0:
			return true;
		case 8:
			return medal & 1;
		case 7:
			return medal & 2;
		case 9:
			return medal & 4;
		case 10:
			return medal & 8;
		case 11:
			return medal & (1 << 10);
		case 12:
			return karateInfo >= 1;
		case 13:
			return karateInfo >= 6;
		case 14:
			return karateInfo >= 11;
		case 15:
			return karateInfo >= 16;
		default:
			return ((1 << (index + 3)) & medal);
		}
	}
public:
	void Press(UIController* controller) override {
		if (AL_Customization_MedalUnlocked(m_index)) {
			GBAManager_GetChaoDataPointer()->Medal = m_index;
		}
	}
	void Exec() override {
		m_rotY += 256;
	}

	void ColorSet() override {
		if (GBAManager_GetChaoDataPointer()->Medal == m_index)
			SetChaoHUDThingBColor(1, 1, 0x96 / 255.0f, 0x7F / 255.0f); //light green, current medal
		if (!AL_Customization_MedalUnlocked(m_index))
			SetChaoHUDThingBColor(1, 0x49 / 255.0f, 0x5E / 255.0f, 0xAF / 255.0f); //light grey, not available
	}
	void BoxContentDisp() override {
		DoLighting(0);

		njPushMatrixEx();
		//njUnitMatrix(0);

		ProjectToScreen(m_posX + 30, m_posY + 30, -32);
//njTranslate(NULL, m_posX + 30, m_posY + 50 , 0.2);
		//OrthoTranslate(m_posX + 30, m_posY, 1);
		if (IsSelected())
			RotateY(m_rotY);


		njSetTexture((NJS_TEXLIST*)0x1366AD4);
		//OrthoDraw(BigMedals[m_index]);
		chCnkDrawObject(BigMedals[m_index]);
		njPopMatrixEx();
	}
	MedalCustomizeBox(int index, float posX, float posY) : BaseCustomizeBox("medalbox", posX, posY, 0) {
		m_index = index;
	}
};

class ScrollArea : public UISelectable {
private:
	const size_t m_horizItemCount = 4;
	const size_t m_verticalItemCount = 3;
	const size_t m_scrollAt = 2;

	float m_scrollOffsetY = 0.f;
	float m_scrollAlpha = 1.f;

	int m_uiSelectX = 0;
	int m_uiSelectY = 0;

	ChaoHudThingB m_sprite = { 1, 51 ,51, 94 / 256.f, 0, (94 + 51) / 256.f, (52.f) / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedSprite = { 1, 51 ,51, 154 / 256.f, 0, (154 + 51) / 256.f, (52.f) / 256.f, &CWE_UI_TEXLIST, 36 };

	ChaoHudThingB m_scrollTop = { 1, 10, 4, 227 / 256.f, 0, 237 / 256.f, 3 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_scrollMiddle = { 1, 10, 1, 227 / 256.f, 6.5f / 256.f, 237 / 256.f, 6.5f / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_scrollBottom = { 1, 10, 5, 227 / 256.f, 9.5f / 256.f, 237 / 256.f, 13 / 256.f, &CWE_UI_TEXLIST, 36 };

	const size_t GetItemCount() const {
		return AccessoryList.size();
	}

	void ScrollDisp(float posX, float posY, float length){
		DrawChaoHudThingB(
			&m_scrollTop,
			posX,
			posY,
			-2.2f,
			1,
			1,
			-1,
			-1
		);

		auto scrollMiddle = m_scrollMiddle;
		scrollMiddle.ht = length;

		DrawChaoHudThingB(
			&scrollMiddle,
			posX,
			posY + m_scrollTop.ht,
			-2.3f,
			1,
			1,
			-1,
			-1
		);

		DrawChaoHudThingB(
			&m_scrollBottom,
			posX,
			posY + m_scrollTop.ht + scrollMiddle.ht,
			-2.2f,
			1,
			1,
			-1,
			-1
		);

	}

public:
	float m_alpha = 1;
	float m_visualPosX, m_visualPosY;

	bool CanUnselect(Direction direction) const override {
		switch (direction) {
		case Direction::Left:
			return m_uiSelectX == 0;
		case Direction::Right:
		case Direction::Up:
		case Direction::Down:
			return false;
		}
	}

	void Press(UIController* controller) override {
		if (!IsSelected()) return;

		ChaoDataBase* pParam = GBAManager_GetChaoDataPointer();
		const size_t index = m_uiSelectX + m_uiSelectY * m_horizItemCount;
		if (index >= AccessoryList.size()) return;

		auto* pSaveInfo = AccessoryList[index];
		const int accType = GetAccessoryType(pSaveInfo->IndexID);

		bool canEquip = true;
		if (pParam->Accessories[accType].ID[0])
			canEquip = AL_Customization_CreateAcc(GET_CHAOWK(pChao)->AccessoryIndices[accType], GET_CHAOPARAM(pChao)->Accessories[accType], pParam->Garden);

		if (canEquip) {
			PlaySoundProbably(0x1007, 0, 0, 0);
			AL_SetAccessory(pChao, pSaveInfo, pSaveInfo->IndexID);
			AL_ClearItemSaveInfo(pSaveInfo);
			UpdateHatAccVector();
		}
	}

	void Disp() override {
		if (IsSelected()) {
			bool doTween = false;
			if (MenuButtons_Pressed[0] & Buttons_Up) {
				if (m_uiSelectY >= 1) {
					m_uiSelectY--;
					doTween = true;
				}
			}
			if (MenuButtons_Pressed[0] & Buttons_Down) {
				if (m_uiSelectY < GetItemCount() - 1) {
					m_uiSelectY++;
					doTween = true;
				}
			}

			if (MenuButtons_Pressed[0] & Buttons_Left) {
				if (m_uiSelectX >= 1) m_uiSelectX--;
			}
			if (MenuButtons_Pressed[0] & Buttons_Right) {
				if (m_uiSelectX < m_horizItemCount - 1) m_uiSelectX++;
			}

			if (GetItemCount() > 0) {
				const size_t index = m_uiSelectX + m_uiSelectY * m_horizItemCount;
				if (index >= GetItemCount()) {
					const size_t lastItemIndex = GetItemCount() - 1;
					m_uiSelectX = lastItemIndex % m_horizItemCount;
					m_uiSelectY = lastItemIndex / m_horizItemCount;
				}
			}
			else {
				m_uiSelectX = m_uiSelectY = 0;
			}

			if (doTween) {
				if (m_uiSelectY >= m_scrollAt) {
					CreateTween(NULL, EASE_OUT, INTERP_EXPO, &m_scrollOffsetY, float(m_scrollAt) - float(m_uiSelectY), 15, NULL);
				}
			}
		}

		const auto setColor = [this](float a, float r, float g, float b) {
			SetChaoHUDThingBColor(a * m_alpha, r, g, b);
		};

		setColor(1, 1, 1, 1);

		const float horizSpacing = m_sprite.wd + 15;
		const float verticalSpacing = m_sprite.ht + 15;
		const float scrollBarLength = verticalSpacing * (m_verticalItemCount + 1) - 25;
		const int verticalItemCount = (Sint32(GetItemCount()) - 1) / Sint32(m_horizItemCount);

		if (verticalItemCount > m_scrollAt) {
			setColor(0.3f, 0, 0, 0);
			ScrollDisp(m_visualPosX + horizSpacing * m_horizItemCount, m_visualPosY, scrollBarLength);

			const float scrollCursorLength = scrollBarLength / (verticalItemCount + 1);
			setColor(1, 1, 1, 1);
			ScrollDisp(m_visualPosX + horizSpacing * m_horizItemCount, m_visualPosY + m_uiSelectY * scrollCursorLength, scrollCursorLength);
		}

		for (size_t y = 0; y < 1 + GetItemCount() / m_horizItemCount; ++y) {
			for (size_t x = 0; x < m_horizItemCount; ++x) {
				const size_t index = x + y * m_horizItemCount;
				if (index >= GetItemCount()) break;

				const bool selected = IsSelected() && m_uiSelectX == x && m_uiSelectY == y;
				const auto& sprite = selected ? &m_selectedSprite : &m_sprite;

				const float scrolledY = float(y) + m_scrollOffsetY;

				if (scrolledY < -1.f) 
					continue;
				if (scrolledY - m_verticalItemCount > 1.f) 
					continue;

				float alpha = 1;
				if (scrolledY - m_verticalItemCount > 0) {
					alpha = 1 - (scrolledY - float(m_verticalItemCount));
				}
				else if (scrolledY < 0) {
					alpha = 1 + scrolledY;
				}

				if (alpha < 1)
					setColor(alpha, 1, 1, 1);
				else 
					setColor(1, 1, 1, 1);

				DrawChaoHudThingB(sprite, m_visualPosX + x * horizSpacing, m_visualPosY + (float(y) + m_scrollOffsetY) * verticalSpacing, -2.2f, 1, 1, -1, -1);

				if (index >= AccessoryList.size()) continue;

				char nums[6];
				sprintf_s(nums, "%d", int(AccessoryListCount[AccessoryList[index]]));

				for (size_t str = 0; str < strlen(nums); str++) {
					if (!nums[str]) break;

					nums[str] -= '0';
					nums[str] += 0x10;
				}
				
				NJS_COLOR nameColor = (NJS_COLOR)0xFFFFFFFF;
				nameColor.argb.a = m_alpha * 255;
				if(alpha < 1) nameColor.argb.a = m_alpha * alpha * 255;
				
				DisplayChaoName_NewFont((char*)(nums - offsetof(ChaoDataBase, Name) + 0x12), m_visualPosX + x * horizSpacing + sprite->wd, m_visualPosY + (y + m_scrollOffsetY) * verticalSpacing + sprite->ht - 15, 22, 22, nameColor, 0, DrawAncorV_Center);

				Rotation rot = { 0, 0, 0 };

				SAlItem _item = {
					ChaoItemCategory_Accessory,
					AccessoryList[index]->IndexID
				};
				
				float scl = 0.9f * m_alpha;
				if (alpha < 1) {
					scl *= alpha;
				}

				AccessorySetupDraw(AccessoryList[index]->IndexID, AccessoryList[index]->Colors, AccessoryList[index]->UsedColors);
				DrawItem(m_visualPosX + x * horizSpacing + (sprite->wd) / 2.f, m_visualPosY + (y + m_scrollOffsetY) * verticalSpacing + (sprite->ht) / 2.f, scl, rot, _item);
			}
		}
	}

	void Exec() override {
		SetSelectable(m_alpha == 1);
	}

	ScrollArea(const std::string& name, float posX, float posY) : UISelectable(name, 0) {
		m_visualPosX = posX;
		m_visualPosY = posY;

		m_posX = 400;
		m_posY = m_visualPosY;
	}
};

class ColorEditor : public UISelectable {
private:
	std::optional<EAccessoryType> m_accessoryType;

	ChaoHudThingB m_colorPanel = { 1, 211, 213, 0, 0, 1, 1, &CWE_UI_TEXLIST, 37 };

	ChaoHudThingB m_colorSlot = { 1, 37, 36, 132 / 256.f, 115 / 256.f, 169 / 256.f, 151 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_editedColorSlot = { 1, 39, 38, 125 / 256.f, 156 / 256.f, 167 / 256.f, 196 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_selectedColorSlot = { 1, 40, 39, 172 / 256.f, 155 / 256.f, 214 / 256.f, 196 / 256.f, &CWE_UI_TEXLIST, 36 };

	ChaoHudThingB m_genericSliderLeft = { 1, 9, 17, 106 / 256.f, 91 / 256.f, 114 / 256.f, 108 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_genericSliderMiddle = { 1, 1, 17, 117 / 256.f, 91 / 256.f, 117 / 256.f, 108 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_genericSliderRight = { 1, 11, 17, 236 / 256.f, 91 / 256.f, 246 / 256.f, 108 / 256.f, &CWE_UI_TEXLIST, 36 };

	ChaoHudThingB m_hueColors = { 1, 29, 13, 219 / 256.f, 114 / 256.f, 248 / 256.f, 126 / 256.f, &CWE_UI_TEXLIST, 36 };
	ChaoHudThingB m_overlayColor = { 1, 29, 13, 219 / 256.f, 132 / 256.f, 248 / 256.f, 144 / 256.f, &CWE_UI_TEXLIST, 36 };

	ChaoHudThingB m_sliderPicker = { 1, 8, 23, 228 / 256.f, 24 / 256.f, 236 / 256.f, 46 / 256.f, &CWE_UI_TEXLIST, 36 };

	ChaoHudThingB m_hueText = { 1, 137, 29, 0, 0, 1, 29 / 130.f, &CWE_UI_TEXLIST, 5 };
	ChaoHudThingB m_satText = { 1, 137, 29, 0, 29 / 130.f, 1, 58 / 130.f, &CWE_UI_TEXLIST, 5 };
	ChaoHudThingB m_ligText = { 1, 137, 29, 0, 63 / 130.f, 1, 90 / 130.f, &CWE_UI_TEXLIST, 5 };
	ChaoHudThingB m_colorsText = { 1, 137, 29, 0, 99 / 130.f, 1, 1, &CWE_UI_TEXLIST, 5 };

	int m_colorSlotIndex = 0;

	int m_selectionX = 0;
	int m_selectionY = 0;

	int m_sineAng = 0;

	float m_hsl[3] = {0, 0, 0};

	size_t GetColorSlotCount() const {
		if (!m_accessoryType) return 0;

		const auto index = GET_CHAOWK(pChao)->AccessoryIndices[*m_accessoryType];
		if (index == -1) return 0;
		
		return GetAccessoryColorCount(index);
	}

	NJS_COLOR GetSlotColor(size_t slot) const {
		if (!m_accessoryType) return NJS_COLOR(0xFFFFFFFF);

		const auto& accData = GET_CHAOPARAM(pChao)->Accessories[*m_accessoryType];
		const auto accIndex = GET_CHAOWK(pChao)->AccessoryIndices[*m_accessoryType];

		if (accIndex == -1) return NJS_COLOR(0xFFFFFFFF);
		if (accData.ColorFlags & (1 << slot)) return NJS_COLOR(accData.ColorSlots[slot]);

		return NJS_COLOR(GetAccessoryDefaultColors(accIndex)[slot]);
	}

	void SelectColorSlot(size_t slot) {
		m_colorSlotIndex = slot;
		
		NJS_COLOR color = GetSlotColor(slot);
		RGBtoHSL(color.argb.r / 255.f, color.argb.g / 255.f, color.argb.b / 255.f);
	}

	void UpdateColorSlot() const {
		if (!m_accessoryType) return;

		auto& accData = GET_CHAOPARAM(pChao)->Accessories[*m_accessoryType];
		NJS_COLOR& accessoryColor = *(NJS_COLOR*)&accData.ColorSlots[m_colorSlotIndex];

		const NJS_ARGB hslColor = HSLtoRGB(m_hsl[0], m_hsl[1], m_hsl[2]);

		accessoryColor.argb.a = 255;
		accessoryColor.argb.r = Uint8(hslColor.r * 255.f);
		accessoryColor.argb.g = Uint8(hslColor.g * 255.f);
		accessoryColor.argb.b = Uint8(hslColor.b * 255.f);

		accData.ColorFlags |= (1 << m_colorSlotIndex);
	}

	size_t GetVerticalSlotCount() const {
		return (GetColorSlotCount() > 4) ? 2 : 1;
	}

	float GetSliderLength() const {
		return 100.f;
	}

	void SliderDisp(
		float posX, 
		float posY, 
		float sliderVal, 
		bool selected, 
		const NJS_ARGB& overlayColors, 
		const NJS_ARGB& rightColors,
		ChaoHudThingB& overlaySprite
	) {
		const float sliderLength = GetSliderLength();
		DrawChaoHudThingB(&m_genericSliderLeft, posX, posY, -2.2, 1, 1, -1, -1);
		DrawChaoHudThingB(&m_genericSliderMiddle, posX + m_genericSliderLeft.wd, posY, -2.2, sliderLength, 1, -1, -1);

		SetChaoHUDThingBColor(rightColors.a, rightColors.r, rightColors.g, rightColors.b);
		DrawChaoHudThingB(&m_genericSliderRight, posX + m_genericSliderLeft.wd + sliderLength, posY, -2.2, 1, 1, -1, -1);

		SetChaoHUDThingBColor(overlayColors.a, overlayColors.r, overlayColors.g, overlayColors.b);
		overlaySprite.wd = 1;
		DrawChaoHudThingB(&overlaySprite, posX + m_genericSliderLeft.wd, posY + 1.5f, -2.1, sliderLength, 1, -1, -1);

		if (selected) {
			SetChaoHUDThingBColor(m_alpha, 0, 1, 0);
		}
		else {
			SetChaoHUDThingBColor(m_alpha, 1, 1, 1);
		}
		const float sliderPos = m_genericSliderLeft.wd + sliderLength + m_genericSliderRight.wd - m_sliderPicker.wd;
		DrawChaoHudThingB(&m_sliderPicker, posX + sliderPos * sliderVal, posY - 2, -0.5f, 1, 1, -1, -1);
	}
	void SliderShadowDisp(
		float posX,
		float posY
	) {
		SetChaoHUDThingBColor(m_alpha * 0.3f, 0, 0, 0);
		const float sliderLength = GetSliderLength();
		DrawChaoHudThingB(&m_genericSliderLeft, posX, posY, -2.4f, 1, 1, -1, -1);
		DrawChaoHudThingB(&m_genericSliderMiddle, posX + m_genericSliderLeft.wd, posY, -2.4f, sliderLength, 1, -1, -1);
		DrawChaoHudThingB(&m_genericSliderRight, posX + m_genericSliderLeft.wd + sliderLength, posY, -2.4f, 1, 1, -1, -1);
	}

	void RGBtoHSL(float r, float g, float b) {
		const float cMax = max(max(r, g), b);
		const float cMin = min(min(r, g), b);

		const float delta = cMax - cMin;

		m_hsl[2] = (cMax + cMin) / 2.f;

		if (delta == 0) {
			m_hsl[0] = 0;
			m_hsl[1] = 0;
		}
		else {
			m_hsl[1] = (m_hsl[2] > 0.5) ? delta / (2 - cMax - cMin) : delta / (cMax + cMin);

			if (cMax == r) {
				m_hsl[0] = (g - b) / delta + (g < b ? 6 : 0);
			}
			else if (cMax == g) {
				m_hsl[0] = (b - r) / delta + 2;
			}
			else if (cMax == b) {
				m_hsl[0] = (r - g) / delta + 4;
			}

			m_hsl[0] /= 6.f;
		}
	}

	NJS_ARGB HSLtoRGB(float h, float s, float l) const {
		NJS_ARGB result;

		const auto hueToRgb = [](float p, float q, float t) {
			if (t < 0) t += 1;
			if (t > 1) t -= 1;
			if (t < 1.f / 6) return p + (q - p) * 6 * t;
			if (t < 1.f / 2) return q;
			if (t < 2.f / 3) return p + (q - p) * (2.f / 3 - t) * 6;
			return p;
		};

		float r, g, b;

		if (!s) {
			r = g = b = l;
		}
		else {
			const float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
			const float p = 2 * l - q;
			r = hueToRgb(p, q, h + 1.f / 3);
			g = hueToRgb(p, q, h);
			b = hueToRgb(p, q, h - 1.f / 3);
		}

		result.a = m_alpha;
		result.r = r;
		result.g = g;
		result.b = b;

		return result;
	}

	void DrawColorSlots() {
		SetChaoHUDThingBColor(m_alpha, 1, 1, 1);

		const auto slotCount = GetColorSlotCount();

		const auto index = m_selectionY * min(slotCount, 4) + m_selectionX;

		const float slotPosY1 = 45;
		const float slotPosY2 = 85;

		const float slotPosX1 = 0;
		const float slotPosX2 = m_colorPanel.wd;
		const auto calcSlotX = [&](size_t slotCount, size_t index) {
			return float(index + 1) * (slotPosX2 - slotPosX1) / float(slotCount + 1);
		};

		const float colorSlotPad = m_colorSlot.wd + 10;
		for (size_t i = 0; i < slotCount; ++i) {
			float y;
			if (slotCount <= 4) {
				y = (slotPosY1 + slotPosY2) / 2.f;
			}
			else if (i < 4) {
				y = slotPosY1;
			}
			else {
				y = slotPosY2;
			}

			float x;
			if (i >= 4) {
				x = calcSlotX(slotCount - 4, i - 4);
			}
			else {
				x = calcSlotX(min(slotCount, 4), i);
			}

			float scl = 1.f;
			if (m_selectionY < GetVerticalSlotCount() && index == i) {
				scl = (0.5 * njSin(m_sineAng) + 0.5f) * 0.3f + 0.7f;
			}

			// shadow
			SetChaoHUDThingBColor(m_alpha * 0.3f, 0, 0, 0);
			DrawChaoHudThingB((i == m_colorSlotIndex) ? &m_editedColorSlot : &m_colorSlot, m_posX + x + 5, m_posY + y + 5, -2.5f, scl, scl, DrawAncorV_Center, DrawAncorV_Center);

			if (m_selectionY < GetVerticalSlotCount() && index == i) {
				SetChaoHUDThingBColor(m_alpha, 1, 1, 1);
				const float scl = (i == m_colorSlotIndex) ? 1.2f : 1.05f;
				//DrawChaoHudThingB(&m_selectedColorSlot, m_posX + x + m_colorSlot.wd / 2.f, m_posY + y + m_colorSlot.ht / 2.f, -2.0f, scl, scl, DrawAncorV_Center, DrawAncorV_Center);
			}

			if (i == m_colorSlotIndex) {
				SetChaoHUDThingBColor(m_alpha, 1, 1, 1);
				DrawChaoHudThingB(&m_editedColorSlot, m_posX + x, m_posY + y, -2.0f, scl * 1.05f, scl * 1.05f, DrawAncorV_Center, DrawAncorV_Center);
			}

			NJS_COLOR color = GetSlotColor(i);
			SetChaoHUDThingBColor(m_alpha, color.argb.r / 255.f, color.argb.g / 255.f, color.argb.b / 255.f);
			DrawChaoHudThingB(&m_colorSlot, m_posX + x, m_posY + y, -2.1f, scl, scl, DrawAncorV_Center, DrawAncorV_Center);
		}
	}
public:
	float m_alpha = 0.f;

	void InitEditorSelectSlot(const EAccessoryType accType) {
		m_accessoryType = accType;

		SelectColorSlot(0);
		m_selectionX = 0;
		m_selectionY = 0;
	}

	void Disp() override {
		if (!m_alpha) return;

		SetChaoHUDThingBColor(m_alpha, 1, 1, 1);
		DrawChaoHudThingB(&m_colorPanel, m_posX, m_posY, -2.4f, 1, 1, -1, -1);
		DrawChaoHudThingB(&m_colorsText, m_posX + m_colorPanel.wd / 2.f, m_posY, -2.3f, 1, 1, DrawAncorV_Center, DrawAncorV_Center);
		DrawColorSlots();

		const size_t sliderSelectionStart = GetVerticalSlotCount();
		const float sliderTextStartY = 85;
		const float sliderX = m_posX + m_colorPanel.wd / 2.f - 2 * GetSliderLength() / 3.f;
		const float sliderStartY = sliderTextStartY + 35;
		const float sliderPad = 30.f;

		SliderShadowDisp(sliderX + 5, m_posY + sliderStartY + 5);

		SetChaoHUDThingBColor(m_alpha, 1, 0, 0);
		SliderDisp(sliderX, m_posY + sliderStartY, m_hsl[0], m_selectionY == sliderSelectionStart, {m_alpha,1,1,1}, {m_alpha,1,0,0}, m_hueColors);
		
		const NJS_ARGB satSliderStartColor = HSLtoRGB(m_hsl[0], 0, m_hsl[2]);
		const NJS_ARGB satSliderEndColor = HSLtoRGB(m_hsl[0], 1, m_hsl[2]);

		const auto argbToColor = [this](const NJS_ARGB& argb) {
			NJS_COLOR color;
			color.argb.a = Uint8(m_alpha * argb.a * 255.f);
			color.argb.r = Uint8(argb.r * 255.f);
			color.argb.g = Uint8(argb.g * 255.f);
			color.argb.b = Uint8(argb.b * 255.f);
			return color;
		};

		const auto lightSliderStartColor = argbToColor(HSLtoRGB(m_hsl[0], m_hsl[1], 0));
		const auto lightSliderMiddleColor = argbToColor (HSLtoRGB(m_hsl[0], m_hsl[1], 0.5f));
		const auto lightSliderEndColor = argbToColor(HSLtoRGB(m_hsl[0], m_hsl[1], 1));

		SliderShadowDisp(sliderX + 5, m_posY + sliderStartY + sliderPad + 5);
		SetChaoHUDThingBColor(m_alpha, satSliderStartColor.r, satSliderStartColor.g, satSliderStartColor.b);
		SliderDisp(sliderX, m_posY + sliderStartY + sliderPad, m_hsl[1], m_selectionY == sliderSelectionStart + 1, satSliderEndColor, satSliderEndColor, m_overlayColor);

		SliderShadowDisp(sliderX + 5, m_posY + sliderStartY + sliderPad * 2 + 5);
		SetChaoHUDThingBColor(m_alpha, lightSliderStartColor.argb.r / 255.f, lightSliderStartColor.argb.g / 255.f, lightSliderStartColor.argb.b / 255.f);
		DrawChaoHudThingB(&m_genericSliderLeft, sliderX, m_posY + sliderStartY + sliderPad * 2, -2.2, 1, 1, -1, -1);
		SetChaoHUDThingBColor(m_alpha, lightSliderEndColor.argb.r / 255.f, lightSliderEndColor.argb.g / 255.f, lightSliderEndColor.argb.b / 255.f);
		DrawChaoHudThingB(&m_genericSliderRight, sliderX + m_genericSliderLeft.wd + GetSliderLength(), m_posY + sliderStartY + sliderPad * 2, -2.2, 1, 1, -1, -1);

		const float u0 = 117 / 256.f;
		const float v0 = 91 / 256.f;
		const float u1 = 117 / 256.f;
		const float v1 = 108 / 256.f;

		NJS_TEXTURE_VTX vertices[4];
		vertices[0] = {sliderX + m_genericSliderLeft.wd, m_posY + sliderStartY + sliderPad * 2, 0.99f, u0, v0, lightSliderStartColor.color };
		vertices[1] = {vertices[0].x + GetSliderLength() / 2.f, vertices[0].y, 0.99f, u1, v0, lightSliderMiddleColor.color };
		vertices[2] = {vertices[0].x, vertices[0].y + m_genericSliderMiddle.ht, 0.99f, u0, v1, vertices[0].col };
		vertices[3] = {vertices[1].x, vertices[2].y, 0.99f, u1, v1, vertices[1].col };

		njSetTexture(&CWE_UI_TEXLIST);
		njSetTextureNum(36);
		njDrawTextureEx(vertices, 4, 1);

		for (size_t i = 0; i < 4; ++i) {
			vertices[i].x += GetSliderLength() / 2.f;
		}
		vertices[0].col = vertices[2].col = lightSliderMiddleColor.color;
		vertices[1].col = vertices[3].col = lightSliderEndColor.color;
		njDrawTextureEx(vertices, 4, 1);

		if (m_selectionY == sliderSelectionStart + 2) {
			SetChaoHUDThingBColor(m_alpha, 0, 1, 0);
		}
		else {
			SetChaoHUDThingBColor(m_alpha, 1, 1, 1);
		}
		const float sliderPos = m_genericSliderLeft.wd + GetSliderLength() + m_genericSliderRight.wd - m_sliderPicker.wd;
		DrawChaoHudThingB(&m_sliderPicker, sliderX + sliderPos * m_hsl[2], m_posY + sliderStartY + sliderPad * 2 - 2, -0.5f, 1, 1, -1, -1);
	}

	void Exec() override {
		if (!ColorMenuOpened) return;

		m_sineAng += 512;

		if (MenuButtons_Pressed[0] & Buttons_B) {
			ColorMenuOpened = false;
			return;
		}

		const size_t sliderSelectionStart = GetVerticalSlotCount();
		const size_t maxSelectionY = sliderSelectionStart + 3 - 1; // color slots + 3 sliders - 1 (max not count)

		if (MenuButtons_Pressed[0] & Buttons_Up) {
			if (--m_selectionY < 0) m_selectionY = 0;
			m_selectionX = 0;
		}

		if (MenuButtons_Pressed[0] & Buttons_Down) {
			if (++m_selectionY > maxSelectionY) m_selectionY = maxSelectionY;
			m_selectionX = 0;
		}

		if (MenuButtons_Pressed[0] & Buttons_Left) {
			if (m_selectionY >= sliderSelectionStart) {
				float& val = m_hsl[m_selectionY - sliderSelectionStart];
				val -= 0.05f;
				if (val < 0) val = 0;
				UpdateColorSlot();
			}
			else {
				if (--m_selectionX < 0) m_selectionX = 0;
			}
		}

		if (MenuButtons_Pressed[0] & Buttons_Right) {
			if (m_selectionY >= sliderSelectionStart) {
				float& val = m_hsl[m_selectionY - sliderSelectionStart];
				val += 0.05f;
				if (val > 1) val = 1;
				UpdateColorSlot();
			}
			else {
				const auto colorCount = GetColorSlotCount();

				size_t maxSelectionX;
				if (colorCount > 4) maxSelectionX = !m_selectionY ? 4 : (colorCount - 4);
				else maxSelectionX = colorCount;

				if (++m_selectionX >= maxSelectionX) m_selectionX = maxSelectionX - 1;
			}
		}

		if (MenuButtons_Pressed[0] & Buttons_A) {
			if (m_selectionY < sliderSelectionStart) {
				const auto slotCount = GetColorSlotCount();
				SelectColorSlot(m_selectionY * min(slotCount, 4) + m_selectionX);
			}
		}
	}

	ColorEditor() : UISelectable("coloredit", CantSelect), m_accessoryType(EAccessoryType::Generic1) {
		m_posX = 100;
		m_posY = 125;
	}
};

class HatAccSlot : public BaseCustomizeBox {
private:
	const float m_basePosX, m_basePosY;
	ChaoHudThingB m_slotSprite;

	float m_editAnim = 0.f;

	const int m_slot;
	bool m_editSelected = false;
	bool m_preventRightSelect = false;

	int GetItem() {
		if (HasHeadgear())
			return GBAManager_GetChaoDataPointer()->Headgear;
		else if (HasAccessory())
			return GET_CHAOWK(pChao)->AccessoryIndices[m_slot - 1];
		return -1;
	}

	bool HasHeadgear() {
		return m_slot == 0 && GBAManager_GetChaoDataPointer()->Headgear;
	}
	bool HasAccessory() {
		return m_slot > 0 && GBAManager_GetChaoDataPointer()->Accessories[m_slot - 1].ID[0];
	}
public:
	int m_rotY = 0;

	bool AL_Customization_CreateHat(int ID, int Garden)
	{
		if (ID < 1) return false;

		ITEM_SAVE_INFO* v9 = (ITEM_SAVE_INFO*)AL_GetNewItemSaveInfo(9);
		if (v9 == nullptr)
		{
			PrintDebug("HatAccRender: no hat slots left");
			return false;
		}

		v9->Garden = Garden;
		v9->Type = ID;
		v9->position = ProbablyChaoSpawnPoints[Garden * 16 + (int)(njRandom() * 15.f)];
		UpdateHatAccVector();

		return true;
	}

	void BoxContentDisp() override {
		DrawChaoHudThingB(&m_slotSprite, m_posX, m_posY, -2.f, m_sclX, m_sclY, -1, -1);

		ChaoHudThingB m_sprite = { 1, 51 ,51, 94 / 256.f, 0, (94 + 51) / 256.f, (51.f) / 256.f, &CWE_UI_TEXLIST, 36 };
		ChaoHudThingB m_selectedSprite = { 1, 51 ,51, 154 / 256.f, 0, (154 + 51) / 256.f, (51.f) / 256.f, &CWE_UI_TEXLIST, 36 };
		auto sprite = m_editSelected ? m_selectedSprite : m_sprite;
		SetChaoHUDThingBColor(m_editAnim, 1, 1, 1);
		DrawChaoHudThingB(&sprite, m_posX - sprite.wd * m_editAnim, m_posY, -2.2f, .75f, .75f, -1, -1);

		SetChaoHUDThingBColor(1, 1, 1, 1);

		int item = GetItem();
		if (item < 0) return;

		Rotation rot = { 0, IsSelected() ? m_rotY : 0, 0 };

		SAlItem _item = {
			(m_slot > 0) ? ChaoItemCategory_Accessory : ChaoItemCategory_Hat,
			(Uint16)item
		};

		if (m_slot > 0) {
			const auto index = GET_CHAOWK(pChao)->AccessoryIndices[m_slot - 1];
			if (index != -1) {
				const auto& accData = GET_CHAOPARAM(pChao)->Accessories[m_slot - 1];
				AccessorySetupDraw(index, accData.ColorSlots, accData.ColorFlags);
			}
		}

		DrawItem(m_posX + 20, m_posY + 20, 0.75f, rot, _item);

		SetChaoHUDThingBColor(1, 1, 1, 1);
	}

	bool CanUnselect(Direction direction) const override {
		if (ColorMenuOpened) return false;
		if (direction == Direction::Left) return false;
		if (direction == Direction::Right && m_preventRightSelect) {
			return false;
		}
		return true;
	}

	void ColorSet() override {
		if (!HasHeadgear() && !HasAccessory())
			SetChaoHUDThingBColor(1, 0.5f, 0.5f, 0.5f);
		else
			SetChaoHUDThingBColor(1, 1, 1, 1);
	}

	void Press(UIController* controller) override {
		if (!IsBoxSelected()) {
			return;
		}

		ChaoDataBase* pParam = GBAManager_GetChaoDataPointer();
		PlaySoundProbably(0x100A, 0, 0, 0);
		if (!m_slot)
		{
			Uint8& headgear = pParam->Headgear;
			if (headgear && AL_Customization_CreateHat(headgear, pParam->Garden)) {
				headgear = 0;
			}
		}
		else {
			const auto accessoryIndex = AL_GetAccessory(pChao, m_slot - 1);
			if (accessoryIndex != -1 && AL_Customization_CreateAcc(accessoryIndex, GET_CHAOPARAM(pChao)->Accessories[m_slot - 1], pParam->Garden)) {
				AL_ParameterClearAccessory(pChao, m_slot - 1);
			}
		}
	}

	bool IsBoxSelected() const override {
		if (m_editSelected) return false;
		return IsSelected();
	}

	void Exec() override {
		m_preventRightSelect = false;
		
		m_rotY += 256;

		m_posX = m_basePosX + HatAccMenuOffset.x;
		m_posY = m_basePosY + HatAccMenuOffset.y;
	
		SetSelectable(HasHeadgear() || HasAccessory());

		const auto slotAccIndex = GET_CHAOWK(pChao)->AccessoryIndices[m_slot - 1];
		const bool hasColorSlots = slotAccIndex != -1 && GetAccessoryColorCount(slotAccIndex) > 0;
		if (IsSelected() && slotAccIndex != -1) {
			if (hasColorSlots && !ColorMenuOpened) {
				if (MenuButtons_Pressed[0] & Buttons_Left) {
					m_editSelected = true;
				}
				if (MenuButtons_Pressed[0] & Buttons_Right) {
					m_editSelected = false;
					m_preventRightSelect = true;
				}

				if (m_editSelected) {
					if (!ColorMenuOpened && MenuButtons_Pressed[0] & Buttons_A) {
						ColorMenuOpened = true;
						auto colorEditElement = customizationController->GetButton("coloredit");
						if (colorEditElement) {
							auto colorEditor = std::dynamic_pointer_cast<ColorEditor, UISelectable>(*colorEditElement);
							colorEditor->InitEditorSelectSlot(EAccessoryType(m_slot - 1));
						}
					}
				}

				if (!m_editAnim) {
					m_editAnim += 0.00001f;
					CreateTween(NULL, EASE_OUT, INTERP_CIRC, &m_editAnim, 1.f, 15, NULL);
				}
			}
		}
		else {
			m_editSelected = false;

			if (m_editAnim >= 1.f) {
				m_editAnim -= 0.00001f;
				CreateTween(NULL, EASE_IN, INTERP_CIRC, &m_editAnim, 0.f, 15, NULL);
			}
		}
	}
	HatAccSlot(int slot, float posX, float posY) : BaseCustomizeBox("hatacc", posX, posY, 0), m_slot(slot), m_basePosX(posX), m_basePosY(posY) {
		m_slotSprite = { 1, 0x40 / 1.25f ,0x40 / 1.25f, 0, 0, 1 ,1, &CWE_UI_TEXLIST, 28 + m_slot };
		m_sclX = m_sclY = .75f;
	}
};


const int someUIProjectionCodePtr = 0x0055A060;
void someUIProjectionCode(NJS_VECTOR* a1, NJS_VECTOR* a2)
{
	__asm
	{
		mov edi, a1
		mov esi, a2
		call someUIProjectionCodePtr
	}
}

static void AL_OdekakeCustomization(ODE_MENU_MASTER_WORK* a1) {	
	NJS_VECTOR posIn = { 150, 300 + 50, -12 }, posOut;
	switch (a1->mode)
	{
	case 0:
		UpdateHatAccVector();

		//setup sadx medal models
		BigMedals[0] = &NoMedal_Main;
		BigMedals[11] = (NJS_OBJECT*)0x012E365C;
		BigMedals[12] = (NJS_OBJECT*)0x012E390C;
		BigMedals[13] = (NJS_OBJECT*)0x012E43C4;
		BigMedals[14] = (NJS_OBJECT*)0x012e4e1c;
		BigMedals[15] = (NJS_OBJECT*)0x012e586c;

		if (customizationController) {
			while (1) PrintDebug("customizationController not 0, what happened?");
		}

		someUIProjectionCode(&posIn, &posOut);
		pChao = CreateChao((ChaoData*)GBAManager_GetChaoDataPointer(), 0, 0, &posOut, 0);
		pChao->Data1.Chao->field_B0 &= ~8u;
		pChao->Data1.Chao->field_B0 &= ~2u;
		pChao->Data1.Chao->field_B0 &= ~0x10u;

		customizationController = new UIController();

		customizationController->AddLayer(baseLayerName,
			[]() {
				if (!(MenuButtons_Pressed[0] & Buttons_B)) {
					return;
				}
				
				auto exitButton = customizationController->GetButton("exit");

				if (!exitButton) return;

				if ((*exitButton)->IsSelected()) {
					(*exitButton)->Press(customizationController);
				}
				else {
					customizationController->SelectButton("exit");
				}
				PlaySelectSound();
			}
		);

		#define spread 100.f
		customizationController->AddElement<LeftBarButton>(baseLayerName, hataccLayerName, 295 - spread, 385, LeftBarButton::LeftBarType::HatAcc);
		customizationController->AddElement<LeftBarButton>(baseLayerName, medalLayerName, 295, 385, LeftBarButton::LeftBarType::Medal);
		customizationController->AddElement<LeftBarButton>(baseLayerName, "exit", 295 + spread, 385, LeftBarButton::LeftBarType::Exit);
		//customizationController->AddElement<BaseCustomizeBox>(baseLayerName, "chaobox", 100.f, 125.f, UISelectable::CantSelect);

		customizationController->AddLayer(medalLayerName);
		for (int i = 0; i < 16; i++) {
			int posX = (75 * (i % 6)) + 180;
			int posY = (75 * (i / 6)) + 100;
			customizationController->AddElement<MedalCustomizeBox>(medalLayerName, i, (float)posX, (float)posY);
		}

		ColorMenuOpened = ColorMenuLastFrame = false;
		customizationController->AddLayer(hataccLayerName, []() {
			if (ColorMenuLastFrame != ColorMenuOpened) {
				NJS_VECTOR posIn, posOut;
				NJS_POINT2 targetMenuOffset;
				float targetAlpha;

				if (ColorMenuOpened) {
					targetAlpha = 0;
					posIn = { 400, 300 + 50, -12 };
					targetMenuOffset = { 400 - 140, 0 };
				}
				else {
					targetAlpha = 1;
					posIn = { 150, 300 + 50, -12 };
					targetMenuOffset = { 0, 0 };
				}
				someUIProjectionCode(&posIn, &posOut);
				
				CreateTween(pChao, EASE_OUT, INTERP_CIRC, &pChao->Data1.Entity->Position, posOut, 30, NULL);
				CreateTween(NULL, EASE_OUT, INTERP_CIRC, &HatAccMenuOffset, targetMenuOffset, 30, NULL);

				auto scrollAreaElement = customizationController->GetButton("scrollarea");
				if (scrollAreaElement) {
					auto scroll = std::dynamic_pointer_cast<ScrollArea, UISelectable>(*scrollAreaElement);
					CreateTween(NULL, EASE_OUT, INTERP_EXPO, &scroll->m_alpha, targetAlpha, 25, NULL);
				}

				auto colorEditElement = customizationController->GetButton("coloredit");
				if (colorEditElement) {
					auto scroll = std::dynamic_pointer_cast<ColorEditor, UISelectable>(*colorEditElement);
					CreateTween(NULL, EASE_OUT, INTERP_EXPO, &scroll->m_alpha, 1 - targetAlpha, 25, NULL);
				}
			}
		
			ColorMenuLastFrame = ColorMenuOpened;
		});

		customizationController->AddElement<ColorEditor>(hataccLayerName);
		customizationController->AddElement<ScrollArea>(hataccLayerName, "scrollarea", 340, 100);

		for (int i = 0; i < HatsSubMenuCount; i++) {
			customizationController->AddElement<HatAccSlot>(hataccLayerName, i, 240 - spread + 140, 100 + (float)i * 55);
		}

		customizationController->SelectButton(hataccLayerName);
		customizationController->SetCurrentLayer(hataccLayerName);

		CreateButtonGuide(SELECT | CONFIRM | BACK);
		LargeTitleBarExecutor_Load(AL_OdekakeMenuMaster_Data_ptr->CurrStage, 650.0, 66.0);

		largeBar = LoadObject(3,
			"CustomizeUIController",
			[](ObjectMaster*) {
				customizationController->Exec();
			},
			LoadObj_Data1);
		largeBar->DisplaySub = [](ObjectMaster*) {
			customizationController->Disp();
		};

		a1->mode++;
		break;
	case 1:
		break;
	case 2:
		a1->timer++;
		if (a1->timer > 90)
		{
			a1->timer = 0;
			a1->mode++;
		}
		break;
	case 3:
		if (a1->state == 0)
		{
			a1->mode++;
		}
		break;
	case 4:
		DeleteObject_(pChao); pChao = 0;
		DeleteObject_(largeBar); largeBar = 0;

		delete customizationController;
		customizationController = 0;

		AL_OdeMenuSetNextStage(0);
		AL_OdeMenuChangeStage();
		break;
	}
}