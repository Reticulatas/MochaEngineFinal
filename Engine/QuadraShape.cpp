#include "stdafx.h"
#include "QuadraShape.h"
#include "RigidBody.h"
const Math::Vector3 QuadColliderGeometry3D::Support(const Math::Vector3 &dir) const
{
	const Math::Vector3 localDir = inverseOrientation * dir;
	return center + orientation * Math::Vector3(localDir.x >= 0.0f ? halfWidth : -halfWidth,
		0.0f,
		localDir.z >= 0.0f ? halfHeight : -halfHeight);
}

bool QuadColliderGeometry3D::RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const
{
	// everything is in local space (normal is (0, 1, 0))
	Ray3 localRay;
	auto &body = mParent->Parent();
	localRay.pos = body.GlobalToLocal(ray.pos);
	localRay.dir = body.GlobalToLocalVec(ray.dir);
	const Math::Vector3 source = inverseOrientation * (localRay.pos - center);
	const Math::Vector3 target = inverseOrientation * (localRay.pos + localRay.dir * maxDistance - center);

	if (source.y * target.y > EPSILON) // ray on one side of quad, early out
		return false;

	// compute t
	t = source.y / (source.y - target.y);
	const Math::Vector3 intersection = Lerp(source, target, t);
	GV_ASSERT_MESSAGE(std::fabs(intersection.y) <= EPSILON, "Incorrect intersection.");

	if (std::fabs(intersection.x) <= halfWidth && std::fabs(intersection.z) <= halfHeight)
	{
		// compute normal
		n.ZeroOut();
		n.y = source.y >= 0.0f ? 1.0f : -1.0f;
		n = body.LocalToGlobalVec(orientation * n);
		return true;
	}
	else
		return false;
}

bool QuadColliderGeometry3D::Collides(const Math::Vector3 &pos) const
{
	return false;
}

void QuadColliderGeometry3D::UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid)
{
	mass = material.mDensity * 4.0f * halfWidth * halfHeight;

	const float c = 1.0f / 12.0f;
	const float ww = 4.0f * halfWidth * halfWidth;
	const float hh = 4.0f * halfHeight * halfHeight;
	unitInertiaTensor.SetIdentity();
	unitInertiaTensor.m00 = c * ww;
	unitInertiaTensor.m11 = c * (ww + hh);
	unitInertiaTensor.m22 = c * hh;
	unitInertiaTensor = unitInertiaTensor * inverseOrientation;

	localCentroid = center;
}
void QuadColliderGeometry3D::UpdateProxy(const RigidBody &body, Proxy &proxy)
{
	const Math::Vector3 globalCenter = body.LocalToGlobal(center);
	Math::Vector3 halfExtentAbs0 = body.LocalToGlobalVec(orientation * Math::Vector3(halfWidth, 0.0f, halfHeight));
	halfExtentAbs0.x = std::fabs(halfExtentAbs0.x);
	halfExtentAbs0.y = std::fabs(halfExtentAbs0.y);
	halfExtentAbs0.z = std::fabs(halfExtentAbs0.z);
	Math::Vector3 halfExtentAbs1 = body.LocalToGlobalVec(orientation * Math::Vector3(halfWidth, 0.0f, -halfHeight));
	halfExtentAbs1.x = std::fabs(halfExtentAbs1.x);
	halfExtentAbs1.y = std::fabs(halfExtentAbs1.y);
	halfExtentAbs1.z = std::fabs(halfExtentAbs1.z);
	Math::Vector3 halfExtents;
	halfExtents.x = (halfExtentAbs0.x > halfExtentAbs1.x) ? halfExtentAbs0.x : halfExtentAbs1.x;
	halfExtents.y = (halfExtentAbs0.y > halfExtentAbs1.y) ? halfExtentAbs0.y : halfExtentAbs1.y;
	halfExtents.z = (halfExtentAbs0.z > halfExtentAbs1.z) ? halfExtentAbs0.z : halfExtentAbs1.z;
	proxy.minPoint = globalCenter - halfExtents;
	proxy.maxPoint = globalCenter + halfExtents;
}

void QuadColliderGeometry3D::Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const
{
	const RigidBody &body = mParent->Parent();
	const Math::Matrix3 t = body.Orientation() * orientation;
	const Math::Vector3 v0 = body.LocalToGlobal(center) + t * Math::Vector3(halfWidth, 0.0f, halfHeight);
	const Math::Vector3 v1 = body.LocalToGlobal(center) + t * Math::Vector3(-halfWidth, 0.0f, halfHeight);
	const Math::Vector3 v2 = body.LocalToGlobal(center) + t * Math::Vector3(-halfWidth, 0.0f, -halfHeight);
	const Math::Vector3 v3 = body.LocalToGlobal(center) + t * Math::Vector3(halfWidth, 0.0f, -halfHeight);
	debugDraw.DrawLine(v0, v1, lineColor);
	debugDraw.DrawLine(v1, v2, lineColor);
	debugDraw.DrawLine(v2, v3, lineColor);
	debugDraw.DrawLine(v3, v0, lineColor);
	debugDraw.DrawLine(0.5f * (v0 + v1), 0.5f * (v2 + v3), lineColor);
	debugDraw.DrawLine(0.5f * (v0 + v3), 0.5f * (v1 + v2), lineColor);
}

const Vector3& QuadColliderGeometry3D::UpdateSize(const Vector3 &scale)
{
	//TODOKai
	return scale;
}
