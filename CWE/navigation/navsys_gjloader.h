#ifndef MESHLOADER_LVL
#define MESHLOADER_LVL

#include "stdafx.h"
#include <string>

class rcMeshLoaderLvl
{
public:
	rcMeshLoaderLvl();
	~rcMeshLoaderLvl();

	bool load();

	const float* getVerts() const { return m_verts; }
	const float* getNormals() const { return m_normals; }
	const int* getTris() const { return m_tris; }
	int getVertCount() const { return m_vertCount; }
	int getTriCount() const { return m_triCount; }

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	//rcMeshLoaderLvl(const rcMeshLoaderLvl&);
	//rcMeshLoaderLvl& operator=(const rcMeshLoaderLvl&);

	void addVertex(float x, float y, float z, int& cap);
	void addTriangle(int a, int b, int c, int& cap);

	float m_scale;
	float* m_verts;
	int* m_tris;
	float* m_normals;
	int m_vertCount;
	int m_triCount;
};

#endif // MESHLOADER_OBJ
