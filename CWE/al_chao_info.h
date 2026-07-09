#pragma once


#include <vector>

class ChaoInfo {
private:
	std::vector<CHAO_PARAM_GC*> m_vector;
	size_t m_size;

protected:
	ChaoInfo(){
		// todo: unsure if AL_GetCurrGardenList would return
		// the first list when this is initialized
		auto pGardenInfo = &GardenInfoList[0];

		for (size_t i = 0; i < _countof(pGardenInfo->chao); i++) {
			m_vector.push_back(&pGardenInfo->chao[i].param);
		}

		m_size = m_vector.size();
	}

public:
	static ChaoInfo& Instance() {
		static ChaoInfo _val;
		return _val;
	}

	size_t Count() const { 
		return m_size; 
	}

	CHAO_PARAM_GC& operator[](std::size_t idx) { 
		return *m_vector[idx]; 
	}

	std::vector<CHAO_PARAM_GC*>::iterator begin()
	{
		return m_vector.begin();
	}

	std::vector<CHAO_PARAM_GC*>::iterator end()
	{
		return m_vector.end();
	}
};

FunctionPointer(int, AL_GetAllChaoCount, (), 0x531A20);
static int AL_GetMaxChao() {
	return ChaoInfo::Instance().Count();
}