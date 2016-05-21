/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include <vector>
#include <map>
class AdjacencyInfo
{
public:

	AdjacencyInfo(void) { Clear(); }
	struct Face
	{
		int edge;
		int freeLink;
		bool active;
		Face(void) : edge(-1), freeLink(-1), active(false) { }
	};
	struct Vert
	{
		Vec3 position;
		int edge;
		int freeLink;
		bool active;
		Vert(void) : edge(-1), freeLink(-1), active(false) { }
		Vert(const Vec3 &position) : position(position), edge(-1), freeLink(-1) { }
	};
	struct Edge
	{
		int vert;
		int face;
		int next;
		int prev;
		int twin;
		int freeLink;
		bool active;
		Edge(void)
			: vert(-1)
			, face(-1)
			, next(-1)
			, prev(-1)
			, twin(-1)
			, freeLink(-1)
			, active(false)
		{ }
	};

	typedef std::vector<Vert> VertList;
	typedef std::vector<Edge> EdgeList;
	typedef std::vector<Face> FaceList;

	// Getters
	uint NumEdges(void) const { return mNumEdges; }
	uint NumFaces(void) const { return mNumFaces; }
	uint NumOrphanVerts(void) const { return mNumOrphanVerts; }
	uint NumBoundaryEdges(void) const { return mNumBoundaryEdges; }
	bool IsManifold(void) const { return mNumBoundaryEdges == 0; } 
	const VertList &Verts(void) const { return mVerts; }
	const EdgeList &Edges(void) const { return mEdges; }
	const FaceList &Faces(void) const { return mFaces; }
	const Vec3 &Centroid(void) const { return mCentroid; }
	uint NumVerts(void) const { return mNumVerts; }

	// Helpers
	void RemoveFace(int f);
	void Clear(void);
	int AddVert(const Vec3 &position);
	void RemoveVert(int v);
	int AddFace(int v0, int v1, int v2);
	int FindVertEdge(int v) const;
	const Vec3 Support(const Vec3 &dir) const; 
	void RemoveOrphanVerts(void);
	Vec3 mCentroid;
	bool isCreated;
private:

	VertList mVerts;
	EdgeList mEdges;
	FaceList mFaces;

	int mFreeVerts;
	int mFreeEdges;
	int mFreeFaces;

	uint mNumVerts;
	uint mNumEdges;
	uint mNumFaces;

	uint mNumOrphanVerts;
	uint mNumBoundaryEdges;

	typedef std::pair<int, int> VertPair;
	typedef std::map<VertPair, int> EdgeMap;
	EdgeMap mEdgeMap;

	// for lowering floating point error in support function calculation
	
};