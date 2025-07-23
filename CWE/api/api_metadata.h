#pragma once
#include "stdafx.h"

#include <cstring>
#include <string>
#include <vector>

#define METADATA_ID_SIZE 21

class ItemMetadata {
protected:
	static ItemMetadata* m_instance;
	ItemMetadata() {

	}

private:
	std::vector<std::string> m_data[ChaoItemCategory_Count];
public:
	void Add(const ChaoItemCategory category, const char id[METADATA_ID_SIZE]) {
		m_data[category].push_back(id);
	}

	size_t GetIndex(const ChaoItemCategory category, const char id[METADATA_ID_SIZE]) {
		const auto& data = m_data[category];

		for (size_t i = 0; i < data.size(); i++) {
			if (!strcmp(data[i].c_str(), id)) {
				return i;
			}
		}

		return -1;
	}

	const std::vector<std::string>& GetIDs(const ChaoItemCategory category) const {
		return m_data[category];
	}

	bool GetID(const ChaoItemCategory category, size_t index, char id[METADATA_ID_SIZE]) {
		const auto& data = m_data[category];

		if (index >= data.size()) {
			return false;
		}

		strcpy(id, data[index].c_str());

		return true;
	}

	static ItemMetadata* Get() {
		if (!m_instance)
			m_instance = new ItemMetadata();

		return m_instance;
	}
};
