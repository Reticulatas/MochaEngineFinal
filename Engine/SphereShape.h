/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include "Collidar.h"
struct SphereColliderDef3D
	: public ColliderDef
{
	Math::Vector3 center;
	float radius;

	SphereColliderDef3D(void)
		: ColliderDef(ColliderType::SPHERE)
		, center(0.0f, 0.0f, 0.0f)
		, radius(1.0f)
	{ }

	SphereColliderDef3D(const Math::Vector3 &center, float radius)
		: ColliderDef(ColliderType::SPHERE)
		, center(center)
		, radius(radius)
	{ }
};

class SphereColliderGeometry3D
	: public ColliderGeometry
{
public:

	Math::Vector3 center;
	float radius;

	SphereColliderGeometry3D(const SphereColliderDef3D &def)
		: ColliderGeometry(ColliderType::SPHERE)
		, center(def.center)
		, radius(def.radius)
	{ }

	virtual const Vector3& UpdateSize(const Vector3 &scale);
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	virtual bool RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const;
	virtual bool Collides(const Math::Vector3 &pos) const;
	virtual void UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid);
	virtual void UpdateProxy(const RigidBody &body, Proxy &proxy);
	virtual void Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const;
};
