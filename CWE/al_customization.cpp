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

static int vectorInitFlag = 0;
static std::vector<ITEM_SAVE_INFO*> HatList;
static std::vector<ItemSaveInfoBase*> AccessoryList;
static void UpdateHatAccVector() {
	vectorInitFlag = 0;
}
static void SubMenuUpdate() {
	UpdateHatAccVector();
}
static void CheckHatAccVectorUpdate() {
	if (!vectorInitFlag)
	{
		HatList.clear();
		AccessoryList.clear();

		ITEM_SAVE_INFO* items = (ITEM_SAVE_INFO*)ChaoHatSlots;
		for (int j = 0; j < 24; j++)
		{
			if (items[j].Type > 0) {
				HatList.push_back(&items[j]);
			}
		}

		for (auto& item : AccessoryItemList) {
			if (item.IndexID == -1) continue;

			AccessoryList.push_back(&item);
		}

		vectorInitFlag = 1;
	}
}

class LeftBarButton : public UISelectable {
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
		if (IsSelected())
			DrawChaoHudThingB(&MainButtons[m_buttonType + LeftBarType::Count], m_posX, m_posY, -1.2f, 1, 1, -1, -1);
		else
			DrawChaoHudThingB(&MainButtons[m_buttonType], m_posX, m_posY, -1.2f, 1, 1, -1, -1);
	}
	LeftBarButton(const std::string& layerEnter, float posX, float posY, int buttonType) : UISelectable(layerEnter, SelectFlag::ForceVertical), m_layerEnter(layerEnter), m_buttonType(buttonType) {
		m_posX = posX;
		m_posY = posY;
	}
};

class BaseCustomizeBox : public UISelectable {
private:
	ChaoHudThingB m_sprite = { 1, 0x40 ,0x40, 0, 0, 0.5 ,0.5, &AL_OSAMENU, 1 };
public:
	float m_sclX = 1;
	float m_sclY = 1;
	float GetWidth() { return m_sprite.wd; }
	float GetHeight() { return m_sprite.ht; }
	void OverrideSprite(ChaoHudThingB& hud) {
		m_sprite = hud;
	}
	virtual void BoxContentDisp() {}
	virtual void ColorSet() {}
	void ColorSetSelected() {
		if (!IsSelected())
			SetChaoHUDThingBColor(1, 0x58 / 255.0f, 0xB0 / 255.0f, 0xC0 / 255.0f); //yellow, selected
	}
	void Disp() override {
		SetChaoHUDThingBColor(1, 1, 1, 1);
		ColorSet();

		DrawChaoHudThingB(&m_sprite, m_posX, m_posY, -1.2f, m_sclX, m_sclY, -1, -1);

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
		ColorSetSelected();
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

class HatAccRender : public BaseCustomizeBox {
private:
	bool m_useNullItem = false;
	float m_offX = 0;
	float m_offY = 0;
public:
	int m_rotY = 0;
	virtual bool IsAccessorySlot() const {
		return false;
	}

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
	bool AL_Customization_CreateAcc(int ID, int Garden)
	{
		if (ID == -1) return false;

		auto save = CWE_GetNewItemSaveInfo(ChaoItemCategory_Accessory);
		if (!save) {
			PrintDebug("HatAccRender: no hat slots left");
			return false;
		}

		save->Garden = Garden;
		save->IndexID = ID;
		save->Position = ProbablyChaoSpawnPoints[Garden * 16 + (int)(njRandom() * 15.f)];
		UpdateHatAccVector();

		return true;
	}
	void Exec() override {
		m_rotY += 256;
	}
	void ColorSet() override {
		ColorSetSelected();
	}
	virtual int GetItem() {
		return -1;
	}
	virtual void AdditionalSpriteDraw() {

	}
	void BoxContentDisp() override {
		AdditionalSpriteDraw();

		int item = GetItem();
		if (item < 0 && !m_useNullItem) return;

		if (item < 0 && m_useNullItem) {
			DoLighting(0);
			njPushMatrixEx();

			OrthoScreenTranslate(m_offX, m_offY); //-0.6 0.4
			if (IsSelected())
				RotateY(m_rotY);
			njTranslate(NULL, 0, 1.5f, 0);
			njSetTexture((NJS_TEXLIST*)0x1366AD4);
			OrthoDraw(&NoMedal_Main);

			njPopMatrixEx();
		}
		else {
			Rotation rot = { 0, IsSelected() ? m_rotY : 0, 0 };

			SAlItem _item = { 
				IsAccessorySlot() ? ChaoItemCategory_Accessory : ChaoItemCategory_Hat,
				(Uint16)item 
			};

			DrawItem(m_offX, m_offY, 1, rot, _item);
		}

		SetChaoHUDThingBColor(1, 1, 1, 1);
	}
	HatAccRender(float posX, float posY, bool useNullItem = false, float sclX = 1, float sclY = 1, int flags = 0) : BaseCustomizeBox("hataccbox", posX, posY, flags) {
		m_useNullItem = useNullItem;
		m_sclX = sclX;
		m_sclY = sclY;
		m_offX = (posX + (GetWidth() * m_sclX * (1 / 2.f))); // ((posX + (GetWidth() * m_sclX * (1 / 2.f)) - 320.0f) / 640.0f)* (4.0f / 3.0f);
		m_offY = (posY + (GetHeight() * m_sclY * (1/2.f))); // -((posY + (GetHeight() * m_sclY * 4 / 5.f) - 240) / 480.0f);
	}
};
class HatAccCustomizeBox : public HatAccRender {
private:
	const size_t m_index;
public:
	bool IsAccessorySlot() const override {
		return m_index >= HatList.size();
	}

	void Press(UIController* controller) override {
		if (m_index >= HatList.size() + AccessoryList.size()) return;

		ChaoDataBase* pParam = GBAManager_GetChaoDataPointer();

		if (!IsAccessorySlot()) {
			ITEM_SAVE_INFO* saveInfo = HatList[m_index];

			bool canEquip = true;
			if (pParam->Headgear > 0)
				canEquip = AL_Customization_CreateHat(pParam->Headgear, pParam->Garden);
			if(canEquip) {
				PlaySoundProbably(0x1007, 0, 0, 0);
				pParam->Headgear = (char)saveInfo->Type;
				AL_ClearItemSaveInfo(saveInfo);
				UpdateHatAccVector();
			}
		}
		else {
			auto* pSaveInfo = AccessoryList[m_index - HatList.size()];
			const int accType = GetAccessoryType(pSaveInfo->IndexID);
			
			bool canEquip = true;
			if (pParam->Accessories[accType].ID[0])
				canEquip = AL_Customization_CreateAcc(GET_CHAOWK(pChao)->AccessoryIndices[accType], pParam->Garden);
			if (canEquip) {
				PlaySoundProbably(0x1007, 0, 0, 0);
				AL_SetAccessory(pChao, pSaveInfo->IndexID);
				AL_ClearItemSaveInfo(pSaveInfo);
				UpdateHatAccVector();
			}
			
		}
	}
	int GetItem() override{
		if (m_index >= HatList.size() + AccessoryList.size()) return -1;

		if (!IsAccessorySlot()) {
			return HatList[m_index]->Type;
		}

		return AccessoryList[m_index - HatList.size()]->IndexID;
	}
	HatAccCustomizeBox(size_t index, float posX, float posY, int flags = 0) 
		: HatAccRender(posX, posY, false,1,1, flags), 
		m_index(index) 
	{}
};

class HatAccSlot : public HatAccRender {
private:
	int colorSine = 0;
	const int m_slot;
	bool HasHeadgear() {
		return m_slot == 0 && GBAManager_GetChaoDataPointer()->Headgear;
	}
	bool HasAccessory() {
		return m_slot > 0 && GBAManager_GetChaoDataPointer()->Accessories[m_slot - 1].ID[0];
	}
public:
	bool IsAccessorySlot() const override {
		return m_slot > 0;
	}

	int GetItem() override {
		if (HasHeadgear())
			return GBAManager_GetChaoDataPointer()->Headgear;
		else if (HasAccessory())
			return GET_CHAOWK(pChao)->AccessoryIndices[m_slot - 1];
		return -1;
	}
	void ColorSet() override {
		float c = (njSin(colorSine) + 1) / 2.0f;
		if (colorSine == 0) c = 1;

		if(IsSelected())
			SetChaoHUDThingBColor(1, 
				(120 + c * (255 - 120)) / 255.0f,
				1, 
				(120 + c * (255 - 120)) / 255.0f
				);
		else if(!HasHeadgear() && !HasAccessory())
			SetChaoHUDThingBColor(1, 0.5f, 0.5f, 0.5f);
		else
			SetChaoHUDThingBColor(1, 1, 1, 1);
	}
	void Press(UIController* controller) override {
		ChaoDataBase* pParam = GBAManager_GetChaoDataPointer();
		PlaySoundProbably(0x100A, 0, 0, 0);
		if (!IsAccessorySlot())
		{
			Uint8& headgear = pParam->Headgear;
			if (headgear && AL_Customization_CreateHat(headgear, pParam->Garden)) {
				headgear = 0;
			}
		}
		else {
			const auto accessoryIndex = AL_GetAccessory(pChao, m_slot - 1);
			if (accessoryIndex != -1 && AL_Customization_CreateAcc(accessoryIndex, pParam->Garden)) {
				AL_ParameterClearAccessory(pChao, m_slot - 1);
			}
		}
	}
	void AdditionalSpriteDraw() override {

	}
	void Exec() override {
		SetSelectable(HasHeadgear() || HasAccessory());

		m_rotY += 256;

		if (IsSelected()) {
			colorSine += 1024;
		}
		else 
			colorSine = 0;
	}
	HatAccSlot(int slot, float posX, float posY) : HatAccRender(posX,posY,false, 0.85f,0.85f, ForceHorizontal), m_slot(slot) {
		ChaoHudThingB element = { 1, 0x40 ,0x40, 0, 0, 1 ,1, &CWE_UI_TEXLIST, 28 + m_slot };
		OverrideSprite(element);
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

UIController* customizationController = nullptr;
ObjectMaster* largeBar = 0;

static void AL_OdekakeCustomization(ODE_MENU_MASTER_WORK* a1) {	
	NJS_VECTOR posIn = { 130, 190, -30 }, posOut;
	switch (a1->mode)
	{
	case 0:
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
				
				auto& exitButton = customizationController->GetButton("exit");
				if (exitButton->IsSelected()) {
					exitButton->Press(customizationController);
				}
				else {
					customizationController->SelectButton("exit");
				}
				PlaySelectSound();
			});
		customizationController->AddElement<LeftBarButton>(baseLayerName, hataccLayerName, 100.f, 200.f, LeftBarButton::LeftBarType::HatAcc);
		customizationController->AddElement<LeftBarButton>(baseLayerName, medalLayerName, 100.f, 250.f, LeftBarButton::LeftBarType::Medal);
		customizationController->AddElement<LeftBarButton>(baseLayerName, "exit", 100.f, 300.f, LeftBarButton::LeftBarType::Exit);
		customizationController->AddElement<BaseCustomizeBox>(baseLayerName, "chaobox", 100.f, 125.f, UISelectable::CantSelect);

		customizationController->AddLayer(medalLayerName);
		for (int i = 0; i < 16; i++) {
			int posX = (75 * (i % 6)) + 180;
			int posY = (75 * (i / 6)) + 100;
			customizationController->AddElement<MedalCustomizeBox>(medalLayerName, i, (float)posX, (float)posY);
		}

		customizationController->AddLayer(hataccLayerName, CheckHatAccVectorUpdate);

		for (int i = 0; i < HatsSubMenuCount; i++) {
			customizationController->AddElement<HatAccSlot>(hataccLayerName, i,  220.f + (float)i * 75, 380.f);
		}

		for (int i = 0; i < 24; i++) {
			float posX = (float)(75 * (i % 6)) + 180;
			float posY = (float)(75 * (i / 6)) + 100;
			int flags = 0;
			if (i >= 18)
				flags = UISelectable::SelectFlag::ForceHorizontal;
			customizationController->AddElement<HatAccCustomizeBox>(hataccLayerName, i, posX, posY, flags);
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