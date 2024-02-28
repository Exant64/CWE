#pragma once


#include <vector>

class ChaoInfo {
private:
	std::vector<ChaoDataBase*> m_vector;
	size_t m_size;

protected:
	ChaoInfo(){
		for (int i = 0; i < ChaoSlots_Length; i++) {
			m_vector.push_back(&ChaoSlots[i].data);
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

	ChaoDataBase& operator[](std::size_t idx) { 
		return *m_vector[idx]; 
	}

	std::vector<ChaoDataBase*>::iterator begin()
	{
		return m_vector.begin();
	}

	std::vector<ChaoDataBase*>::iterator end()
	{
		return m_vector.end();
	}
};

FunctionPointer(int, AL_GetAllChaoCount, (), 0x531A20);
static int AL_GetMaxChao() {
	return ChaoInfo::Instance().Count();
}