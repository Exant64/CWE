#pragma once
#include "../SA2ModLoader.h"
#include <cstdint>
#include <string>
#include "selectable.h"
#include <memory>
#include <vector>
#include <functional>
#include <array>
#include <optional>

static void PlaySelectSound() {
	PlaySoundProbably(0x8000, 0, 0, 0);
}

using UpdateFunc = std::function<void()>;
class UIController {
private:
	struct Layer {
		std::vector<std::shared_ptr<UISelectable>> m_elements;
		UpdateFunc m_func = {};
		std::string m_id = "";
		bool m_initialized = false;

		void AddElement(std::shared_ptr<UISelectable> selectable) {
			m_elements.push_back(selectable);
		}

		auto begin()
		{
			return m_elements.begin();
		}

		auto end()
		{
			return m_elements.end();
		}
	};

	std::array<Layer, 16> m_layers;
	std::shared_ptr<UISelectable> m_selected = 0;

	int m_alwaysDrawLayer = 0;
	int m_layer = 0;
	size_t m_layerCount = 0;

	void SelectButton(const std::shared_ptr<UISelectable>& selectable) {
		if (!selectable->IsSelectable()) return;

		if (m_selected)
			m_selected->Unselect();

		m_selected = selectable;
		m_selected->Select();
	}

	int FindLayer(const std::string& layer) const {
		for (size_t i = 0; i < m_layerCount; i++) {
			if (m_layers[i].m_initialized && m_layers[i].m_id == layer) {
				return i;
			}
		}
		return -1;
	}

public:
	UIController() {

	}

	~UIController() {
	}

	Layer& GetCurrentLayer() {
		return m_layers[m_layer];
	}

	std::optional<std::shared_ptr<UISelectable>> GetButton(const std::string& button) {
		for (auto& layer : m_layers) {
			for (auto& element : layer) {
				if (element->m_name == button)
					return element;
			}
		}

		return std::nullopt;
	}

	void SelectButton(const std::string& button) {
		//this code assumes that theres only one button with that name
		//if theres more, it will select the last button it finds
		SelectButton(*GetButton(button));
	}

	void SetCurrentLayer(const std::string& layer) {
		int layerSearch = FindLayer(layer);
		if (layerSearch < 0) return;

		m_layer = layerSearch;
	}

	void AddLayer(const std::string& layer, UpdateFunc func = {}) {
		if (m_layerCount >= m_layers.size()) return;

		m_layers[m_layerCount].m_initialized = true;
		m_layers[m_layerCount].m_id = layer;
		m_layers[m_layerCount].m_func = func;
		m_layerCount++;
	}

	void Disp() {
		*(char*)0x25EFFCC = 0;

		for (auto selectable : m_layers[m_alwaysDrawLayer]) {
			selectable->Disp();
		}

		//don't draw it twice
		if (m_layer != m_alwaysDrawLayer) {
			for (auto selectable : GetCurrentLayer()) {
				selectable->Disp();
			}
		}

		*(char*)0x25EFFCC = 1;
	}

	//the reason this is not precalculated is because some objects can change their "selectability" 
	//so they would need to call UIController to update it, which i dont wanna do if i dont have to
	bool DirectionPositionCheck(Direction direction, float aPosX, float aPosY, float bPosX, float bPosY) {
		switch (direction) {
		case Direction::Left:
			return bPosX < aPosX;
		case Direction::Right:
			return aPosX < bPosX;
		case Direction::Down:
			return aPosY < bPosY;
		case Direction::Up:
			return bPosY < aPosY;
		}
		return false;
	}
	bool IsDirectionHorizontal(Direction direction) {
		return direction == Direction::Left || direction == Direction::Right;
	}
	bool IsDirectionVertical(Direction direction) {
		return direction == Direction::Up || direction == Direction::Down;
	}
	void UpdateSelection(Direction direction) {
		//if theres nothing selected already, it wont know where to go
		if (!m_selected) return;

		if (!m_selected->CanUnselect(direction)) return;

		if (direction == Direction::Right && m_selected->m_selectForceRight) {
			SelectButton(m_selected->m_selectForceRight);
			return;
		}

		float minDistance = 10000;
		std::shared_ptr<UISelectable> selected = 0;
		for (size_t i = 0; i < m_layerCount; i++) {
			if (i != m_alwaysDrawLayer && i != m_layer) continue;

			for (auto element : m_layers[i]) {
				float posX = m_selected->m_posX;
				float posY = m_selected->m_posY;
				//check if its not trying to select itself and if its selectable at all
				if (element != m_selected && element->IsSelectable()) {
					if (!DirectionPositionCheck(direction, posX, posY, element->m_posX, element->m_posY)) 
						continue;
					if (IsDirectionHorizontal(direction) && m_selected->IsForcedHorizontal()) {
						if (posY != element->m_posY) 
							continue;
					}
					if (IsDirectionVertical(direction) && m_selected->IsForcedVertical()) {
						if (posX != element->m_posX) 
							continue;
					}

					float mPosX = element->m_posX - posX;
					float mPosY = element->m_posY - posY;
					float distance = sqrtf(mPosX * mPosX + mPosY * mPosY);
					if (distance < minDistance)
					{
						minDistance = distance;
						selected = element;
					}
				}
			}
		}
		if (selected)
			SelectButton(selected);
	}

	const std::array<std::pair<Buttons, Direction>, 4> m_buttonDirectionPair = {
		std::make_pair(Buttons_Up, Direction::Up),
		std::make_pair(Buttons_Down, Direction::Down),
		std::make_pair(Buttons_Left, Direction::Left),
		std::make_pair(Buttons_Right, Direction::Right)
	};
	void Exec() {
		//the first added layer will always run
		if (m_layers[m_alwaysDrawLayer].m_func)
			m_layers[m_alwaysDrawLayer].m_func();

		if (m_layer != m_alwaysDrawLayer && m_layers[m_layer].m_func)
			m_layers[m_layer].m_func();

		for (auto selectable : GetCurrentLayer()) {
			selectable->Exec();
		}

		for (auto& pair : m_buttonDirectionPair) {
			if (MenuButtons_Pressed[0] & pair.first) {
				UpdateSelection(pair.second);
				PlaySelectSound();
			}
		}

		if (MenuButtons_Pressed[0] & Buttons_A && m_selected)
			m_selected->Press(this);
	}

	template<typename T, typename... Args>
	void AddElement(const std::string& layer, Args... args) {
		auto selectable = std::make_shared<T>(args...);
		int layerSearch = FindLayer(layer);
		if (layerSearch < 0) return;

		m_layers[layerSearch].AddElement(selectable);
	}
};