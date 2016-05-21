/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "Collidar.h"
struct QuadColliderDef3D
	: public ColliderDef
{
	// normal is (0, 1, 0) in local space
	// width is in x-direction
	// height is in z-direction
	Math::Vector3 center;
	Math::Matrix3 orientation;
	float halfWidth, halfHeight;

	QuadColliderDef3D(void)
		: ColliderDef(ColliderType::QUAD)
	{
		orientation.SetIdentity();
		halfWidth = halfHeight = 0.5f;
	}

	QuadColliderDef3D(const Math::Vector3 &center, const Math::Matrix3 &orientation, float halfWidth, float halfHeight)
		: ColliderDef(ColliderType::QUAD)
		, center(center)
		, orientation(orientation)
		, halfWidth(halfWidth)
		, halfHeight(halfHeight)
	{ }
};

class QuadColliderGeometry3D
	: public ColliderGeometry
{
public:

	// normal is (0, 1, 0) in local space
	// width is in x-direction
	// height is in z-direction
	Math::Vector3 center;
	Math::Matrix3 orientation, inverseOrientation;
	float halfWidth, halfHeight;

	QuadColliderGeometry3D(const QuadColliderDef3D &def)
		: ColliderGeometry(ColliderType::QUAD)
		, center(def.center)
		, orientation(def.orientation)
		, inverseOrientation(orientation.Transposed())
		, halfWidth(def.halfWidth)
		, halfHeight(def.halfHeight)
	{ }

	virtual const Vector3& UpdateSize(const Vector3 &scale);
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	virtual bool RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const;
	virtual bool Collides(const Math::Vector3 &pos) const;
	virtual void UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid);
	virtual void UpdateProxy(const RigidBody &body, Proxy &proxy);
	virtual void Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const;
};