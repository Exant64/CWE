#pragma once
#include "stdafx.h"
#include <SA2ModLoader.h>


class Code {
public:
	const int m_param;
	
	Code(int param) : m_param(param) {}

	virtual void Init() {};
	virtual void OnFrame() {};
	virtual void ChaoObject(ObjectMaster* tp) {};
	virtual void OnALControl(ObjectMaster* tp) {};
	virtual void OnChaoData(ChaoDataBase& chao) {};
};

//for codes that only use init
#define PATCH_CODE(name) \
class name : public Code { \
	public: \
		using Code::Code; \
		void Init() override; \
};

//for directly ported codes
#define ONFRAME_CODE(name) \
class name : public Code { \
	public: \
		using Code::Code; \
		void OnFrame() override; \
};