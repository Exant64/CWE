#pragma once

#include "stdafx.h"

#include <vector>
#include <string>

// this can probably be integrated into generator, but whatever

enum {
	NAV_AREA_GROUND = 0,
	NAV_AREA_WATER = 1
};

enum {
	NAV_FLAGS_WALK = BIT_0,
	NAV_FLAGS_SWIM = BIT_1
};

class NavSysMeshConvert
{
public:
	NavSysMeshConvert();

	const std::vector<NJS_POINT3>& getVerts() const { return m_verts; }
	const std::vector<NJS_VECTOR>& getNormals() const { return m_normals; }
	const std::vector<int>& getTris() const { return m_tris; }
	const std::vector<uint8_t>& getAreas() const { return m_areas; }
	int getVertCount() const { return m_verts.size(); }
	int getTriCount() const { return m_tris.size() / 3; }

private:
	std::vector<NJS_POINT3> m_verts;
	std::vector<NJS_VECTOR> m_normals;
	std::vector<int> m_tris;
	std::vector<uint8_t> m_areas;
};
