/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "Collidar.h"
#include "MathIncludes.h"
#include "AdjancencyInfo.h"
class PolyhedronColliderGeometry;

struct PolyhedronColliderDef
	: public ColliderDef
{
	friend class PolyhedronColliderGeometry;
public:

	PolyhedronColliderDef(void)
		: ColliderDef(ColliderType::POLYHEDRON)
	{ }

	PolyhedronColliderDef(const AdjacencyInfo &adjacency)
		: ColliderDef(ColliderType::POLYHEDRON)
		, adjacency(adjacency)
	{ }

	// for STL containers
	template <typename T1, typename T2>
	PolyhedronColliderDef(const T1 &vertices, const T2 &indices)
		: ColliderDef(ColliderType::POLYHEDRON)
	{
		for (const Math::Vector3 &v : vertices)
			adjacency.AddVert(v);
		int faceVertIndices[3];
		int i = 0;
		for (int index : indices)
		{
			faceVertIndices[i++] = index;
			if (i == 3)
			{
				adjacency.AddFace(faceVertIndices[0], faceVertIndices[1], faceVertIndices[2]);
				i = 0;
			}
		}
	}
	
private:

	AdjacencyInfo adjacency;
};

class PolyhedronColliderGeometry
	: public ColliderGeometry
{
public:
	PolyhedronColliderGeometry(const PolyhedronColliderDef &def)
		: ColliderGeometry(ColliderType::POLYHEDRON)
		, mAdjacency(def.adjacency)
		, mDoItOnce(true)
	{ }

	virtual const Vector3& UpdateSize(const Vector3 &scale);
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	virtual bool RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const;
	virtual bool Collides(const Math::Vector3 &pos) const;
	virtual void UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid);
	virtual void UpdateProxy(const RigidBody &body, Proxy &proxy);
	virtual void Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const;
	void UpdateAABB(const RigidBody &body, Proxy &proxy);
	Math::Vector3 mScale;
private:
	AdjacencyInfo mAdjacency;
	bool mDoItOnce;
};