/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include "Collidar.h"
#define BOXSIZE 1.f
struct BoxColliderDef
	: public ColliderDef
{
	BoxColliderDef(void)
	: ColliderDef(ColliderType::BOX)
	, mHalfExtents(0.5f, 0.5f, 0.5f)
	, mCenter(0.0f, 0.0f, 0.0f)
	, mOrientation(Matrix3::cIdentity)
	{}
	
	BoxColliderDef(const Vector3 &center, const Vector3 &halfExtents, const Mat3 &orientation)
		: ColliderDef(ColliderType::BOX)
		, mCenter(center)
		, mHalfExtents(halfExtents)
		, mOrientation(orientation)
	{}

	Vector3 mHalfExtents;
	Vector3 mCenter;
	Matrix3 mOrientation;
};

class BoxColliderGeometry
	: public ColliderGeometry
{
public:

	BoxColliderGeometry(const BoxColliderDef &def);
	void UpdateAABB(const RigidBody &body, Proxy &proxy);
	virtual void UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Matrix3 &unitInertiaTensor, Vector3 &localCentroid);
	virtual void UpdateProxy(const RigidBody &body, Proxy &proxy);
	virtual bool RayCast(const Ray3 &ray, float maxDistance, float &t, Vector3 &n) const;
	virtual bool Collides(const Vector3 &pos) const;
	virtual void Draw(PhysicsDebugDraw &debugDraw, uint32 lineColor, uint32 fillColor, float fillAlpha) const;
	virtual const Vector3 Support(const Vector3 &dir) const;
	virtual const Vector3& UpdateSize(const Vector3 &scale);


	bool mDoItOnce;

	Vector3 mCenter;
	Vector3 mHalfExtents;
	Matrix3 mOrientation, mInverseOrientation;

	bool mHasMesh;
	CMeshRenderer* mCmr;
};
