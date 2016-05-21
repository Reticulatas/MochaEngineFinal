#include "stdafx.h"
#include "AdjancencyInfo.h"
#include "List_Assert.h"
template <typename T, typename Container>
static int Allocate(int &freeList, Container &container)
{
	int index = -1;
	if (freeList < 0)
	{
		// free list empty, just push
		index = static_cast<int>(container.size());
		container.resize(index + 1);
	}
	else
	{
		index = freeList;
		T &feature = container[freeList]; // grab free feature
		freeList = feature.freeLink;      // update free list
		feature.freeLink = -1;            // remove feature from free list
	}
	GV_ASSERT_MESSAGE(index >= 0, "Invalid allocated index.");
	T &feature = container[index];
	GV_ASSERT_MESSAGE(!feature.active, "Invalid feature activeness.");
	feature.active = true;
	return index;
}

template <typename T, typename Container>
static void Free(int index, int &freeList, Container &container)
{
	T &feature = container[index];
	GV_ASSERT_MESSAGE(feature.active, "Removing a feature that was already free.");
	feature.freeLink = freeList;
	feature.active = false;
	freeList = index;
}

int AdjacencyInfo::AddVert(const Vec3 &position)
{
	const int v = Allocate<Vert>(mFreeVerts, mVerts);
	auto &vert = mVerts[v];
	vert.position = position; // update position
	vert.edge = -1;           // initialize edge index

	// update centroid
	mCentroid += (position - mCentroid) / (mNumVerts + 1.0f);

	++mNumVerts;
	++mNumOrphanVerts;

	return v;
}

void AdjacencyInfo::RemoveVert(int v)
{
	auto &vert = mVerts[v];

	// remove faces this vert belongs to
	const int startEdge = vert.edge;
	if (startEdge >= 0)
	{
		int e = startEdge;
		do
		{
			auto &edge = mEdges[e];
			RemoveFace(edge.face);
			e = mEdges[edge.prev].twin;
		} while (e != startEdge);
	}

	// dispose vert
	if (vert.edge >= 0)
		vert.edge = -1;
	else
		--mNumOrphanVerts;

	// update free list
	Free<Vert>(v, mFreeVerts, mVerts);

	--mNumVerts;
}

void AdjacencyInfo::RemoveOrphanVerts(void)
{
	for (int v = 0, n = mVerts.size(); v < n; ++v)
	{
		auto &vert = mVerts[v];
		if (vert.active && vert.edge < 0)
			Free<Vert>(v, mFreeVerts, mVerts);
	}

	mNumVerts -= mNumOrphanVerts;
	mNumOrphanVerts = 0;
}

int AdjacencyInfo::AddFace(int v0, int v1, int v2)
{
	const int faceVerts[3] = { v0, v1, v2 };

	// allocate face
	const int f = Allocate<Face>(mFreeFaces, mFaces);
	auto &face = mFaces[f];

	// create edges
	{
		// iterate face edges
		int faceEdgeIndices[3] = { -1, -1, -1 };
		for (uint i = 2, j = 0; j < 3; i = j++)
		{
			const uint v0 = faceVerts[i]; 
			const uint v1 = faceVerts[j]; 
			auto &vert0 = mVerts[v0];
			auto &vert1 = mVerts[v1];

			// check existence of half edge pair
			const auto edgeIter = mEdgeMap.find(VertPair(v0, v1));
			const bool edgePairExists = edgeIter != mEdgeMap.end();
			int e01 = -1;
			int e10 = -1;
			if (edgePairExists)
			{
				e01 = edgeIter->second;
				e10 = mEdges[e01].twin;
			}
			else
			{
				// allocate & init half edge pair
				e01 = Allocate<Edge>(mFreeEdges, mEdges);
				e10 = Allocate<Edge>(mFreeEdges, mEdges);
				GV_ASSERT_MESSAGE(mEdges[e01].next < 0, "Edge link not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e10].next < 0, "Edge link not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e01].prev < 0, "Edge link not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e10].prev < 0, "Edge link not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e01].face < 0, "Edge face not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e10].face < 0, "Edge face not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e01].vert < 0, "Edge vert not initialized.");
				GV_ASSERT_MESSAGE(mEdges[e10].vert < 0, "Edge vert not initialized.");

				// link twins
				mEdges[e01].twin = e10;
				mEdges[e10].twin = e01;

				// record edge existence
				mEdgeMap[VertPair(v0, v1)] = e01;
				mEdgeMap[VertPair(v1, v0)] = e10;

				mNumEdges += 2;
				mNumBoundaryEdges += 2;
			} // end of edge allocation

			auto &edge01 = mEdges[e01];
			auto &edge10 = mEdges[e10];

			// link vert to edges
			if (edge01.vert < 0)
				edge01.vert = v1;
			if (edge10.vert < 0)
				edge10.vert = v0;

			// link face to edge
			//GV_ASSERT_MESSAGE(edge01.face < 0, "Edge already belongs to a face.");
			if (edge01.face < 0)
			{
				edge01.face = f;
				--mNumBoundaryEdges;
			}

			// link edge to vert
			if (vert0.edge < 0)
			{
				vert0.edge = e01;
				--mNumOrphanVerts;
			}

			// link edge to face
			if (face.edge < 0)
				face.edge = e01;

			// record face edges
			faceEdgeIndices[i] = e01;
		}

		// link face edges
		for (unsigned i = 2, j = 0; j < 3; i = j++)
		{
			const int eI = faceEdgeIndices[i];
			const int eJ = faceEdgeIndices[j];
			mEdges[eI].next = eJ;
			mEdges[eJ].prev = eI;
		}

	} // end of edge creation

	GV_ASSERT_MESSAGE(f >= 0, "Invalid face index.");
	++mNumFaces;
	return f;
}

int AdjacencyInfo::FindVertEdge(int v) const
{
	// POSSIBLE OPTIMIZATION: this is currently a linear operation
	for (auto &pair : mEdgeMap)
	{
		if (v == pair.first.first)
			return pair.second;
	}
	return -1;
}

void AdjacencyInfo::RemoveFace(int f)
{
	// remove adjacent edges
	const int startEdge = mFaces[f].edge;
	GV_ASSERT_MESSAGE(startEdge >= 0, "Invalid face edge.");

	int faceVertIndices[3] = { -1, -1, -1 };
	int faceEdgeIndices[3] = { -1, -1, -1 };
	int e = startEdge;
	int i = 0;
	do
	{
		auto &edge = mEdges[e];
		const int t = edge.twin;
		const int n = edge.next;
		const int p = edge.prev;
		auto &twin = mEdges[t];
		auto &next = mEdges[n];
		auto &prev = mEdges[p];

		faceVertIndices[i] = edge.vert;
		faceEdgeIndices[i] = e;
		++i;

		// free both edges if twin face does not exist
		if (twin.face < 0)
		{
			mEdgeMap.erase(VertPair(edge.vert, prev.vert));
			mEdgeMap.erase(VertPair(prev.vert, edge.vert));

			edge.twin = -1;
			twin.twin = -1;

			Free<Edge>(e, mFreeEdges, mEdges);
			Free<Edge>(t, mFreeEdges, mEdges);

			mNumEdges -= 2;
			mNumBoundaryEdges -= 2;
		}

		++mNumBoundaryEdges;

		e = n;
	} while (e != startEdge);

	// unlink everything from edges
	for (const int e : faceEdgeIndices)
	{
		// okay to access data members after fake "free"
		auto &edge = mEdges[e];
		edge.next = -1;
		edge.prev = -1;
		edge.vert = -1;
		edge.face = -1;
	}

	// update vert edge
	for (const int v : faceVertIndices)
	{
		auto &vert = mVerts[v];
		vert.edge = FindVertEdge(v);
		if (vert.edge < 0)
			++mNumOrphanVerts;
	}

	// unlink edge from face
	auto &face = mFaces[f];
	face.edge = -1;

	// finally, free face
	Free<Face>(f, mFreeFaces, mFaces);

	--mNumFaces;
}

void AdjacencyInfo::Clear(void)
{
	mVerts.clear();
	mEdges.clear();
	mFaces.clear();

	mNumVerts = 0;
	mNumEdges = 0;
	mNumFaces = 0;

	mNumOrphanVerts = 0;
	mNumBoundaryEdges = 0;

	mFreeVerts = -1;
	mFreeEdges = -1;
	mFreeFaces = -1;

	mEdgeMap.clear();

	mCentroid.ZeroOut();

	isCreated = false;
}

const Vec3 AdjacencyInfo::Support(const Vec3 &dir) const
{
	// TODOKai
	//GV_ASSERT_MESSAGE(IsManifold(), "Can't perform support function on a non-manifold.");
	GV_ASSERT_MESSAGE(mNumVerts > 0, "No verticies.");
	
	if (mNumVerts == 1)
		return mVerts[0].position;
	
	// start from first point
	int bestVertIndex = 0; 
	auto bestVert = &mVerts[bestVertIndex];
	float bestDot = dir.Dot(bestVert->position - mCentroid);
	
	bool end = false;
	
	for (unsigned i = 0; i < mNumVerts; ++i)
	{
		auto vert = &mVerts[i];
		const float dot = dir.Dot(vert->position - mCentroid);
		if (dot >= bestDot + EPSILON)
		{
			// record new best vert
			bestVertIndex = bestVertIndex;
			bestVert = vert;
			bestDot = dot;
		}
	}
	
	
	return bestVert->position;


	//if (mNumVerts == 1)
	//	return mVerts[0].position;
	//
	//// start from first point
	//int bestVertIndex = 0; //std::rand() % m_numVerts;
	//auto bestVert = &mVerts[bestVertIndex];
	//float bestDot = dir.Dot(bestVert->position - mCentroid);
	//
	//bool end = false;
	//while (!end)
	//{
	//	// loop through all edges leaving best vert
	//	int e = bestVert->edge;
	//	int startEdge = e;
	//	const float oldBestDot = bestDot;
	//	do
	//	{
	//		auto &edge = mEdges[e];
	//		const int vertIndex = edge.vert;
	//		auto vert = &mVerts[vertIndex];
	//		const float dot = dir.Dot(vert->position - mCentroid);
	//
	//		if (dot >= bestDot + EPSILON)
	//		{
	//			// record new best vert
	//			bestVertIndex = bestVertIndex;
	//			bestVert = vert;
	//			bestDot = dot;
	//		}
	//
	//		e = mEdges[edge.twin].next;
	//	} while (startEdge != e);
	//
	//	// end if no new best vert is found
	//	end = (bestDot == oldBestDot);
	//}
	//
	//return bestVert->position;

}