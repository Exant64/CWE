#pragma once
#include "stdafx.h"

#include <vector>
#include "ChaoMain.h"
#include <functional>
#include <array>
#include "ninja_functions.h"
using DrawObjectFunc = void(*)(NJS_CNK_OBJECT*);
class ObjectRegistry {
private:
	std::vector<NJS_CNK_OBJECT*> m_objects;
	std::vector<NJS_TEXLIST*> m_texlists;

public:
	NJS_CNK_OBJECT* GetObj(int index) const;
	NJS_TEXLIST* GetTex(int index) const;
	int Size() const;
	void Set(int index, NJS_CNK_OBJECT* obj);
	void Set(int index, NJS_TEXLIST* tex);
	void Add(NJS_CNK_OBJECT* obj, NJS_TEXLIST* tex);
	
	static ObjectRegistry* Get(Sint8 cat) {
		if (cat < 0 || (size_t)cat >= m_instance.size()) throw;

		if (!m_instance[cat])
			m_instance[cat] = new ObjectRegistry();

		return m_instance[cat];
	}

	template<DrawObjectFunc func = chCnkDrawObject>
	static void DrawObject(Sint8 cat, int index) {
		auto* models = Get(cat);

		njSetTexture(models->GetTex(index));
		func(models->GetObj(index));
	}

	template<void (*func) (NJS_CNK_MODEL*) = chCnkDrawModelWrapper>
	static void DrawModel(Sint8 cat, int index) {
		auto* models = Get(cat);

		njSetTexture(models->GetTex(index));
		if (models->GetObj(index)->model)
			func(models->GetObj(index)->model);
		else {

		}

	}
protected:
	static std::array<ObjectRegistry*, NB_CWE_CATEGORY> m_instance;

};
