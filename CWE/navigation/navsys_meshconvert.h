#pragma once

#include "stdafx.h"

#include <vector>
#include <string>

// this can probably be integrated into generator, but whatever
class NavSysMeshConvert {
	public:
		NavSysMeshConvert();

		void addModel(const NJS_MODEL* pMdl, const uint8_t area);
		void addObject(const NJS_OBJECT* pObj, const uint8_t area);

		const std::vector<NJS_POINT3>& getVerts() const { return m_verts; }
		const std::vector<NJS_VECTOR>& getNormals() const { return m_normals; }
		const std::vector<int>& getTris() const { return m_tris; }
		const std::vector<uint8_t>& getAreas() const { return m_areas; }
		int getVertCount() const { return m_verts.size(); }
		int getTriCount() const { return m_tris.size() / 3; }
		
	private:
		NJS_VECTOR calcNormal(int a, int b, int c);
		void addTriangle(int a, int b, int c, uint8_t area);
		std::vector<NJS_POINT3> m_verts;
		std::vector<NJS_VECTOR> m_normals;
		std::vector<int> m_tris;
		std::vector<uint8_t> m_areas;
};
