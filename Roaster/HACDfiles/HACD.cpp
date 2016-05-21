#include "HACD.h"
#include "hacdHACD.h"
#include <stdlib.h>
#include <string.h>
#include "PlatformConfigHACD.h"

#include "dgMeshEffect.h"
#include "dgConvexHull3d.h"
#include "FloatMath.h"
#include "MergeHulls.h"
#include "../ChunkFileWriter.h"

#if USE_CONSTRAINT_BUILDER
#include "ConstraintBuilder.h"
#endif

#pragma warning(disable:4100 4996)

using namespace hacd;

namespace HACD
{

class MyHACD_API : public HACD_API, public UANS::UserAllocated, public HACD::CallBackFunction
{
public:
	MyHACD_API(void)
	{
		mHACD = NULL;
		mUserCallback = NULL;
	}
	virtual ~MyHACD_API(void)
	{
		releaseHACD();
	}
	bool HACD_API::exportScene(unsigned hullCount, BoostPath& outPath)
	{
		outPath = outPath.replace_extension(".hull");
		ChunkFileWriter writer;
		bool success = writer.Open(outPath.string());
		if (!success)
		{
			std::cout << "Failed to open " << outPath.string();
			return false;
		}
		const unsigned MarkHull = 'hull';
		unsigned meshStartPos = writer.StartChunk(MarkHull);
		writer.Write(hullCount);
		// hulls
		for (hacd::HaU32 i = 0; i < hullCount; i++)
		{
			const HACD_API::Hull *hull = getHull(i);
			if (hull)
			{
				// verts
				writer.Write(hull->mVertexCount * 3);
				for (hacd::HaU32 i = 0; i < hull->mVertexCount * 3; i++)
				{
					hacd::HaF32 f1 = hull->mVertices[i];
					writer.Write(f1);
		
				}
				// indices
				writer.Write(hull->mTriangleCount * 3);
				for (hacd::HaU32 j = 0; j < hull->mTriangleCount * 3; j++)
				{
					hacd::HaU32 i1 = hull->mIndices[j];
					writer.Write(i1);
				}
			}
		}
		writer.EndChunk(meshStartPos);
		return true;
	}
	virtual hacd::HaU32	performHACD(const Desc &desc) 
	{
		hacd::HaU32 ret = 0;
		releaseHACD();
		mUserCallback = desc.mCallback;
		if ( desc.mVertexCount )
		{
			if ( desc.mUseNewtonHACD )
			{
				dgMeshEffect mesh(true);

				float normal[3] = { 0,1,0 };
				float uv[2] = { 0,0 };

				hacd::HaI32 *faceIndexCount = (hacd::HaI32 *)HACD_ALLOC(sizeof(hacd::HaI32)*desc.mTriangleCount);
				hacd::HaI32 *dummyIndex = (hacd::HaI32 *)HACD_ALLOC(sizeof(hacd::HaI32)*desc.mTriangleCount*3);

				for (hacd::HaU32 i=0; i<desc.mTriangleCount; i++)
				{
					faceIndexCount[i] = 3;
					dummyIndex[i*3+0] = 0;
					dummyIndex[i*3+1] = 0;
					dummyIndex[i*3+2] = 0;
				}

				mesh.BuildFromVertexListIndexList(desc.mTriangleCount,faceIndexCount,dummyIndex,
					desc.mVertices,sizeof(hacd::HaF32)*3,(const hacd::HaI32 *const)desc.mIndices,
					normal,sizeof(hacd::HaF32)*3,dummyIndex,
					uv,sizeof(hacd::HaF32)*2,dummyIndex,
					uv,sizeof(hacd::HaF32)*2,dummyIndex);

				dgMeshEffect *result = mesh.CreateConvexApproximation(desc.mConcavity,desc.mMaxHullCount);

				if ( result )
				{
					// now we build hulls for each connected surface...
					dgPolyhedra segment;
					result->BeginConectedSurface();
					if ( result->GetConectedSurface(segment))
					{
						dgMeshEffect *solid = HACD_NEW(dgMeshEffect)(segment,*result);
						while ( solid )
						{
							dgConvexHull3d *hull = solid->CreateConvexHull(0.00001,desc.mMaxHullVertices);
							if ( hull )
							{
								Hull h;
								h.mVertexCount = hull->GetVertexCount();
								h.mVertices = (hacd::HaF32 *)HACD_ALLOC( sizeof(hacd::HaF32)*3*h.mVertexCount);
								for (hacd::HaU32 i=0; i<h.mVertexCount; i++)
								{
									hacd::HaF32 *dest = (hacd::HaF32 *)&h.mVertices[i*3];
									const dgBigVector &source = hull->GetVertex(i);
									dest[0] = (hacd::HaF32)source.m_x;
									dest[1] = (hacd::HaF32)source.m_y;
									dest[2] = (hacd::HaF32)source.m_z;
								}

								h.mTriangleCount = hull->GetCount();
								hacd::HaU32 *destIndices = (hacd::HaU32 *)HACD_ALLOC(sizeof(hacd::HaU32)*3*h.mTriangleCount);
								h.mIndices = destIndices;
			
								dgList<dgConvexHull3DFace>::Iterator iter(*hull);
								for (iter.Begin(); iter; iter++)
								{
									dgConvexHull3DFace &face = (*iter);
									destIndices[0] = face.m_index[0];
									destIndices[1] = face.m_index[1];
									destIndices[2] = face.m_index[2];
									destIndices+=3;
								}

								mHulls.push_back(h);

								// save it!
								delete hull;
							}

							delete solid;
							solid = NULL;
							dgPolyhedra nextSegment;
							hacd::HaI32 moreSegments = result->GetConectedSurface(nextSegment);
							if ( moreSegments )
							{
								solid = HACD_NEW(dgMeshEffect)(nextSegment,*result);
							}
							else
							{
								result->EndConectedSurface();
							}
						}
					}

					delete result;
				}
				ret= mHulls.size();
			}
			else
			{
				mHACD = HACD_NEW(HACD);
				// convert the input data points into doubles
				for (hacd::HaU32 i=0; i<desc.mVertexCount; i++)
				{
					Vec3<hacd::HaF32> p;
					p.X() = desc.mVertices[i*3+0];
					p.Y() = desc.mVertices[i*3+1];
					p.Z() = desc.mVertices[i*3+2];
					mPoints.push_back(p);
				}
				for (hacd::HaU32 i=0; i<desc.mTriangleCount; i++)
				{
					Vec3<hacd::HaI32> t;
					t.X() = desc.mIndices[i*3+0];
					t.Y() = desc.mIndices[i*3+1];
					t.Z() = desc.mIndices[i*3+2];
					mTriangles.push_back(t);
				}

				mHACD->SetPoints(&mPoints[0]);
				mHACD->SetNPoints(desc.mVertexCount);
				mHACD->SetTriangles(&mTriangles[0]);
				mHACD->SetNTriangles(desc.mTriangleCount);
				mHACD->SetCompacityWeight(0.1f);
				mHACD->SetVolumeWeight(0);
				mHACD->SetNClusters(desc.mMinHullCount);
				mHACD->SetCallBack(this);
				mHACD->SetAddExtraDistPoints(false);
				mHACD->SetAddNeighboursDistPoints(false);
				mHACD->SetAddFacesPoints(true);
				mHACD->SetNVerticesPerCH(desc.mMaxHullVertices);		// max of 64 vertices per convex-hull
				mHACD->SetConcavity(desc.mConcavity);
				mHACD->SetConnectDist(desc.mConnectDistance);
				mHACD->Compute();

				ret = (hacd::HaU32)mHACD->GetNClusters();
				for (hacd::HaU32 i=0; i<ret; i++)
				{
					Hull h;
					getHull(i,h);
					mHulls.push_back(h);
				}
			}
		}

		if ( ret && desc.mMergeHulls && desc.mMergeHullCount < ret )
		{
			MergeHullsInterface *mhi = createMergeHullsInterface();
			if ( mhi )
			{
				MergeHullVector inputHulls;
				MergeHullVector outputHulls;
				for (hacd::HaU32 i=0; i<ret; i++)
				{
					Hull &h = mHulls[i];
					MergeHull mh;
					mh.mTriangleCount = h.mTriangleCount;
					mh.mVertexCount = h.mVertexCount;
					mh.mVertices = h.mVertices;
					mh.mIndices = h.mIndices;
					inputHulls.push_back(mh);
				}

				mhi->mergeHulls(inputHulls,outputHulls,desc.mMergeHullCount);

				for (HaU32 i=0; i<ret; i++)
				{
					Hull &h = mHulls[i];
					releaseHull(h);
				}
				mHulls.clear();

				for (hacd::HaU32 i=0; i<outputHulls.size(); i++)
				{
					Hull h;
					const MergeHull &mh = outputHulls[i];
					h.mTriangleCount =  mh.mTriangleCount;
					h.mVertexCount = mh.mVertexCount;
					h.mIndices = (HaU32 *)HACD_ALLOC(sizeof(HaU32)*3*h.mTriangleCount);
					h.mVertices = (HaF32 *)HACD_ALLOC(sizeof(HaF32)*3*h.mVertexCount);
					memcpy((HaU32 *)h.mIndices,mh.mIndices,sizeof(HaU32)*3*h.mTriangleCount);
					memcpy((HaF32 *)h.mVertices,mh.mVertices,sizeof(HaF32)*3*h.mVertexCount);
					mHulls.push_back(h);
				}

				ret = mHulls.size();

				mhi->release();
			}
		}

		return ret;
	}

	void releaseHull(Hull &h)
	{
		HACD_FREE((void *)h.mIndices);
		HACD_FREE((void *)h.mVertices);
		h.mIndices = NULL;
		h.mVertices = NULL;
	}

	virtual const Hull		*getHull(hacd::HaU32 index)  const
	{
		const Hull *ret = NULL;
		if ( index < mHulls.size() )
		{
			ret = &mHulls[index];
		}
		return ret;
	}

	virtual void getHull(hacd::HaU32 index,Hull &h)
	{
		h.mVertexCount = (hacd::HaU32)mHACD->GetNPointsCH(index);
		h.mTriangleCount = (hacd::HaU32)mHACD->GetNTrianglesCH(index);
		h.mVertices = (hacd::HaF32 *)HACD_ALLOC(sizeof(hacd::HaF32)*3*h.mVertexCount);
		h.mIndices = (hacd::HaU32 *)HACD_ALLOC(sizeof(hacd::HaU32)*3*h.mTriangleCount);

		Vec3<hacd::HaF32> * pointsCH = (Vec3<hacd::HaF32> *)HACD_ALLOC(sizeof(Vec3<hacd::HaF32>)*h.mVertexCount);
		Vec3<hacd::HaI32> * trianglesCH = (Vec3<hacd::HaI32> *)HACD_ALLOC(sizeof(Vec3<hacd::HaI32>)*h.mTriangleCount);

		mHACD->GetCH(index, pointsCH, trianglesCH);
		hacd::HaF32 *hullVertices = (hacd::HaF32 *)h.mVertices;
		for (hacd::HaU32 i=0; i<h.mVertexCount; i++)
		{
			hullVertices[i*3+0] = (hacd::HaF32)pointsCH[i].X();
			hullVertices[i*3+1] = (hacd::HaF32)pointsCH[i].Y();
			hullVertices[i*3+2] = (hacd::HaF32)pointsCH[i].Z();
		}
		hacd::HaU32 *hullIndices = (hacd::HaU32 *)h.mIndices;
		for (hacd::HaU32 i=0; i<h.mTriangleCount; i++)
		{
			hullIndices[i*3+0] = trianglesCH[i].X();
			hullIndices[i*3+1] = trianglesCH[i].Y();
			hullIndices[i*3+2] = trianglesCH[i].Z();
		}
		HACD_FREE(pointsCH);
		HACD_FREE(trianglesCH);
	}

	virtual void	releaseHACD(void) // release memory associated with the last HACD request
	{
		if ( mHACD )
		{
			delete mHACD;
			mHACD = NULL;
		}
		mPoints.clear();
		mTriangles.clear();
		for (hacd::HaU32 i=0; i<mHulls.size(); i++)
		{
			releaseHull(mHulls[i]);
		}
		mHulls.clear();
		mUserCallback = NULL;
	}


	virtual void release(void) // release the HACD_API interface
	{
		delete this;
	}

	virtual bool progressUpdate(const char * msg, hacd::HaF32 progress, hacd::HaF32 concavity, hacd::HaU32 nVertices)
	{
		bool ret = true;
		if ( mUserCallback )
		{
			ret = mUserCallback->hacdProgressUpdate(msg,(hacd::HaF32)progress,(hacd::HaF32)concavity,nVertices);
		}
		else
		{
			printf("%s", msg );
		}
		return ret;
	}

	virtual hacd::HaU32	generateConstraints(void)
	{
		hacd::HaU32 ret = 0;
#if USE_CONSTRAINT_BUILDER
		if ( mHulls.size() != 0 )
		{
			hacd::ConstraintBuilder *cb = hacd::createConstraintBuilder();
			for (hacd::HaU32 i=0; i<(hacd::HaU32)mHulls.size(); i++)
			{
				Hull &h = mHulls[i];
				hacd::HaF32 volume = hacd::fm_computeMeshVolume(h.mVertices,h.mTriangleCount,h.mIndices);
				hacd::addConvexHull(cb,h.mVertexCount,h.mVertices,h.mTriangleCount,h.mIndices,volume,i);
			}
			ret = hacd::buildConstraints(cb);
			for (hacd::HaU32 i=0; i<ret; i++)
			{
				HACD_API::Constraint result;
				hacd::Constraint *c = hacd::getConstraint(cb,i,result.mFrom,result.mTo);
				result.mIntersect[0] = c->mIntersect[0];
				result.mIntersect[1] = c->mIntersect[1];
				result.mIntersect[2] = c->mIntersect[2];
				mConstraints.push_back(result);
			}
			hacd::releaseConstraintBuilder(cb);
		}
#endif

		return ret;
	}

	virtual const Constraint * getConstraint(hacd::HaU32 index) const
	{
		const Constraint *ret = NULL;
		if ( index < (hacd::HaU32)mConstraints.size() )
		{
			ret = &mConstraints[index];
		}
		return ret;
	}

	virtual hacd::HaU32	getHullCount(void)
	{
		return (hacd::HaU32) mHulls.size(); 
	}

	virtual hacd::HaU32	getConstraintCount(void)
	{
		return (hacd::HaU32) mConstraints.size(); 
	}


private:
	HACD					*mHACD;
	Vec3Vector				mPoints;
	TriVector				mTriangles;
	UserCallback			*mUserCallback;
	STDNAME::vector< Hull >	mHulls;
	STDNAME::vector< Constraint > mConstraints;

};

HACD_API * createHACD_API(void)
{
	MyHACD_API *m = HACD_NEW(MyHACD_API);
	return static_cast<HACD_API *>(m);
}





};



