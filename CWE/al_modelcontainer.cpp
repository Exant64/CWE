#include "stdafx.h"
#include "al_modelcontainer.h"

extern NJS_OBJECT object_alo_missing;

std::array<ObjectRegistry*, ChaoItemCategory_Count> ObjectRegistry::m_instance;

int ObjectRegistry::Size() const {
	return m_objects.size();
}

NJS_OBJECT* ObjectRegistry::GetObj(int index)const {
	if (index < 0 || (size_t)index > m_objects.size())
		return &object_alo_missing;

	return m_objects[index];
}
NJS_TEXLIST* ObjectRegistry::GetTex(int index) const {
	if (index < 0 || (size_t)index > m_texlists.size())
		return &CWE_OBJECT_TEXLIST;

	return m_texlists[index];
}

void ObjectRegistry::Set(int index, NJS_OBJECT* obj) {
	if (!obj) return;
	
	m_objects[index] = obj;
}

void ObjectRegistry::Set(int index, NJS_TEXLIST* tex) {
	if (!tex) return;

	m_texlists[index] = tex;
}

void ObjectRegistry::Add(NJS_OBJECT* obj, NJS_TEXLIST* tex) {
	if (!obj || !tex) {
		obj = &object_alo_missing;
		tex = &CWE_OBJECT_TEXLIST;
	}
	m_objects.push_back(obj);
	m_texlists.push_back(tex);
}
