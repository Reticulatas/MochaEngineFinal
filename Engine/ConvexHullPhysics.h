#pragma once
#include "MathIncludes.h"
#include <vector>
#include "Asset.h"

class ConvexHull
{
public:
	ConvexHull(){}
	std::vector<float> verts;
	std::vector<int> indices;
};

class ConvexHullMesh : public Asset
{
public:
	ConvexHullMesh(){}

	virtual void OnAssetFileChanged();

	std::vector< ConvexHull > hullLists;

	metadef(ConvexHullMesh)
		m_addbase(Asset)
		endmetadef;
};

class ConvexHullMeshManager
{
public:
	std::vector<ConvexHullMesh> meshLish;
};