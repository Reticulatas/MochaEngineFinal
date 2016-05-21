#include "stdafx.h"
#include "SphereShape.h"
#include "RigidBody.h"
const Math::Vector3 SphereColliderGeometry3D::Support(const Math::Vector3 &dir) const
{
	//if (dir != Vector3().cZero)
		return center + radius * dir.Normalized();
	//else
		//return center;
}

bool SphereColliderGeometry3D::RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const
{
	// Ray-Sphere Intersection (sphere at origin)
	// http://www.csee.umbc.edu/~olano/435f02/ray-sphere.html

	// offset ray to center around sphere
	const Math::Vector3 globalCenter = mParent->Parent().LocalToGlobal(center);
	const Math::Vector3 source = ray.pos - globalCenter;
	const Math::Vector3 target = source + ray.dir * maxDistance;

	float rayDot0 = source.Dot(ray.dir);
	float rayDot1 = target.Dot(ray.dir);
	if (rayDot0 > rayDot1)
		std::swap(rayDot0, rayDot1);
	if (rayDot0 > radius || rayDot1 < -radius) // early out
		return false;

	const float a = 1.0f;
	const float b = 2.0f * ray.dir.Dot(source);
	const float c = source.LengthSq() - radius * radius;
	const float d = b * b - 4.0f * a * c;
	if (d >= 0.0f)
	{
		// compute t
		t = (-b - std::sqrtf(d)) / (2.0f * a * maxDistance);

		if (t >= 0.0f && t <= 1.0f) // validate t range
		{
			// compute normal
			n = source + ray.dir * (t * maxDistance);
			n.Normalize();
			return true;
		}
	}

	return false;
}

bool SphereColliderGeometry3D::Collides(const Math::Vector3 &pos) const
{
	return (mParent->Parent().GlobalToLocal(pos) - center).LengthSq() < radius * radius;
}

void SphereColliderGeometry3D::UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid)
{
	mass = material.mDensity * (4.0f / 3.0f) * (float)PI * radius * radius * radius;
	unitInertiaTensor.SetIdentity() *= (2.0f / 5.0f) * radius * radius;
	localCentroid = center;
}

void SphereColliderGeometry3D::UpdateProxy(const RigidBody &body, Proxy &proxy)
{
	const Math::Vector3 globalCenter = body.LocalToGlobal(center);
	proxy.minPoint.Set(globalCenter.x - radius, globalCenter.y - radius, globalCenter.z - radius);
	proxy.maxPoint.Set(globalCenter.x + radius, globalCenter.y + radius, globalCenter.z + radius);
}

void SphereColliderGeometry3D::Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const
{
	const RigidBody &body = mParent->Parent();
	debugDraw.DrawSphere(body.Position() + body.Orientation() * center, radius, body.Orientation(), lineColor);
}

const Vector3& SphereColliderGeometry3D::UpdateSize(const Vector3 &scale)
{
	//TODOKai
	return scale;
}
