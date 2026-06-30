#pragma once
#include "stdafx.h"

#include <vector>
#include "ChaoMain.h"
#include <functional>
#include <array>
#include "ninja_functions.h"
#include "alg_kinder_bl.h"
#include "api/api_msg.h"

class BlackMarketAttributes {
protected:
	static BlackMarketAttributes* m_instance;
	BlackMarketAttributes() {
		for (int i = 0; i < ChaoItemCategory_Count; i++) {
			for (int j = 0; j < BlackMarketCategories[i].Count; j++) {
				m_attributes[i].push_back(BlackMarketCategories[i].attrib[j]);
			}
		}
	}
private:
	std::unordered_map<size_t, size_t> m_modIndexMap[ChaoItemCategory_Count];
	std::vector<BlackMarketItemAttributes> m_attributes[ChaoItemCategory_Count];
	
	void UpdatePointers(ChaoItemCategory cat) {
		if (m_attributes[cat].size() > 0) {
			BlackMarketCategories[cat].attrib = m_attributes[cat].data();
			BlackMarketCategories[cat].Count = m_attributes[cat].size();
		}
	}
public:
	BlackMarketItemAttributes* Attrib(ChaoItemCategory cat, int index) {
		if (cat < 0 || cat > ChaoItemCategory_Count) return nullptr;

		if (index < 0 || (size_t)index > m_attributes[cat].size()) return nullptr;

		return &m_attributes[cat][index];
	}

	int Size(ChaoItemCategory cat) const {
		return m_attributes[cat].size();
	}

	void SetModIndex(ChaoItemCategory category, size_t index, size_t modIndex) {
		m_modIndexMap[category][index] = modIndex;
	}

	size_t GetModIndex(ChaoItemCategory category, size_t index) {
		return m_modIndexMap[category][index];
	}

	void Add(ChaoItemCategory category, const BlackMarketItemAttributes* attrib, const char* name, const char* description) {
		BlackMarketItemAttributes newAttrib = { 0,0,999,0,0,0 };
		if (attrib)
		{
			newAttrib = *attrib;
			if (newAttrib.Name == -1)
				newAttrib.Name = RegisterAlItemString(name);
			if (newAttrib.Descriptions == -1)
				newAttrib.Descriptions = RegisterAlItemString(description);
		}

		m_attributes[category].push_back(newAttrib);

		UpdatePointers(category);
	}

	static BlackMarketAttributes* Get() {
		if (!m_instance)
			m_instance = new BlackMarketAttributes();

		return m_instance;
	}

};
