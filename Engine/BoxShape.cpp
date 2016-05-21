#include "stdafx.h"
#include "BoxShape.h"
#include "RigidBody.h"
#include "CTransform.h"

BoxColliderGeometry::BoxColliderGeometry(const BoxColliderDef &def)
: ColliderGeometry(ColliderType::BOX)
, mDoItOnce(true)
, mCenter(def.mCenter)
, mHalfExtents(def.mHalfExtents)
, mOrientation(def.mOrientation)
, mInverseOrientation(mOrientation.Transposed())
{

}

const Vec3 BoxColliderGeometry::Support(const Vec3 &dir) const
{
	//Vec3 InversScale(1 / mHalfExtents.x, 1 / mHalfExtents.y, 1 / mHalfExtents.z);
	//InversScale *= 0.5f;
	//const Vec3 localDir = mInverseOrientation * (InversScale * dir);
	const Vec3 localDir = mInverseOrientation * dir;
	return mCenter + mOrientation * Vec3(localDir.x >= 0.0f ? mHalfExtents.x : -mHalfExtents.x,
		localDir.y >= 0.0f ? mHalfExtents.y : -mHalfExtents.y,
		localDir.z >= 0.0f ? mHalfExtents.z : -mHalfExtents.z);
}

void BoxColliderGeometry::UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Mat3 &inertiaTensor, Vec3 &localCentroid)
{
	// check it has cmesh
	GameObject* go = mParent->Parent().mParent->cphy->gameObject;
	mCmr = reinterpret_cast<CMeshRenderer*>(go->GetComponentByName("CMeshRenderer"));
	if (mCmr)
		mHasMesh = true;
	else
		mHasMesh = false;


	//TODOKai presentation
	mass = mParent->Parent().mParent->cphy->mMass * 8.0f * mHalfExtents.x * mHalfExtents.y * mHalfExtents.z;
	
	//const float ww = 4.0f * mHalfExtents.x * mHalfExtents.x;
	//const float hh = 4.0f * mHalfExtents.y * mHalfExtents.y;
	//const float dd = 4.0f * mHalfExtents.z * mHalfExtents.z;
	mass = 8.0f;
	const float ww = 4.0f * 0.5f * 0.5f;
	const float hh = 4.0f * 0.5f * 0.5f;
	const float dd = 4.0f * 0.5f * 0.5f;

	const float k_oneTwelfth = 1.0f / 12.0f;
	inertiaTensor.SetIdentity();
	inertiaTensor.m00 = k_oneTwelfth * mass * (hh + dd);
	inertiaTensor.m11 = k_oneTwelfth * mass * (ww + dd);
	inertiaTensor.m22 = k_oneTwelfth * mass * (ww + hh);

	localCentroid = mCenter;
}

const Vector3& BoxColliderGeometry::UpdateSize(const Vector3 &scale)
{
	if (!mHasMesh)
	return mHalfExtents = scale * BOXSIZE * 0.5f;
	else
	{
		AABB aabb = mCmr->GetMesh()->GetAABB();
		AABBFromModelSapceToWorldSpace(aabb, *mCmr->gameObject->transform);
		return mHalfExtents = (aabb.maxPoint - aabb.minPoint) * 0.5f;
	}
}

bool BoxColliderGeometry::Collides(const Vec3 &pos) const
{
	const Vec3 localPos = mParent->Parent().GlobalToLocal(pos) - mCenter;
	return
		localPos.x >= -mHalfExtents.x && localPos.x <= mHalfExtents.x
		&& localPos.y >= -mHalfExtents.y && localPos.y <= mHalfExtents.y
		&& localPos.z >= -mHalfExtents.z && localPos.z <= mHalfExtents.z;
}


void BoxColliderGeometry::UpdateAABB(const RigidBody &body, Proxy &proxy)
{
	//if (mHasMesh)
	//{
	//	AABB aabb = mCmr->GetMesh()->GetAABB();
	//	AABBFromModelSapceToWorldSpace(aabb, *mCmr->gameObject->transform);
	//	proxy.minPoint = aabb.minPoint;
	//	proxy.maxPoint = aabb.maxPoint;
	//}
	//else
	//{
	const Vec3 globalCenter = body.LocalToGlobal(mCenter);
	Vec3 halfExtentAbs0 = body.LocalToGlobalVecNoScale(mOrientation * Vec3(mHalfExtents.x, mHalfExtents.y, mHalfExtents.z));
	halfExtentAbs0.x = std::fabs(halfExtentAbs0.x);
	halfExtentAbs0.y = std::fabs(halfExtentAbs0.y);
	halfExtentAbs0.z = std::fabs(halfExtentAbs0.z);
	Vec3 halfExtentAbs1 = body.LocalToGlobalVecNoScale(mOrientation * Vec3(mHalfExtents.x, -mHalfExtents.y, mHalfExtents.z));
	halfExtentAbs1.x = std::fabs(halfExtentAbs1.x);
	halfExtentAbs1.y = std::fabs(halfExtentAbs1.y);
	halfExtentAbs1.z = std::fabs(halfExtentAbs1.z);
	Vec3 halfExtentAbs2 = body.LocalToGlobalVecNoScale(mOrientation * Vec3(mHalfExtents.x, mHalfExtents.y, -mHalfExtents.z));
	halfExtentAbs2.x = std::fabs(halfExtentAbs2.x);
	halfExtentAbs2.y = std::fabs(halfExtentAbs2.y);
	halfExtentAbs2.z = std::fabs(halfExtentAbs2.z);
	Vec3 halfExtentAbs3 = body.LocalToGlobalVecNoScale(mOrientation * Vec3(mHalfExtents.x, -mHalfExtents.y, -mHalfExtents.z));
	halfExtentAbs3.x = std::fabs(halfExtentAbs3.x);
	halfExtentAbs3.y = std::fabs(halfExtentAbs3.y);
	halfExtentAbs3.z = std::fabs(halfExtentAbs3.z);
	Vec3 halfExtentMax;
	halfExtentMax.x = Max(Max(Max(halfExtentAbs0.x, halfExtentAbs1.x), halfExtentAbs2.x), halfExtentAbs3.x);
	halfExtentMax.y = Max(Max(Max(halfExtentAbs0.y, halfExtentAbs1.y), halfExtentAbs2.y), halfExtentAbs3.y);
	halfExtentMax.z = Max(Max(Max(halfExtentAbs0.z, halfExtentAbs1.z), halfExtentAbs2.z), halfExtentAbs3.z);
	proxy.minPoint = globalCenter - halfExtentMax;
	proxy.maxPoint = globalCenter + halfExtentMax;
	//}
}


void BoxColliderGeometry::UpdateProxy(const RigidBody &body, Proxy &proxy)
{
	if (body.mParent->cphy->mColliderType != 0)
		UpdateAABB(body, proxy);
	else
	{
		if (mDoItOnce)
		{
			UpdateAABB(body, proxy);
			mDoItOnce = false;
		}
	}

}

void BoxColliderGeometry::Draw(PhysicsDebugDraw &debugDraw, uint32 lineColor, uint32 fillColor, float fillAlpha) const
{}

bool BoxColliderGeometry::RayCast(const Ray3 &ray, float maxDistance, float &t, Vec3 &n) const
{
	// Gino Algorithm 3.3 (p.75)
	Ray3 localRay;
	auto &body = mParent->Parent();
	localRay.pos = body.GlobalToLocal(ray.pos);
	localRay.dir = body.GlobalToLocalVec(ray.dir);
	const Vec3 source = mInverseOrientation * (localRay.pos - mCenter);
	const Vec3 target = mInverseOrientation * (localRay.pos + localRay.dir * maxDistance - mCenter);

	const Vec3 &h = mHalfExtents;
	const uint8 bitsS = OutCode(source, h);
	const uint8 bitsT = OutCode(target, h);

	if (!(bitsS & bitsT)) // none of the side planes separate the ray from the proxy
	{
		float tEnter = 0.0f;
		float tExit = 1.0f;
		uint8 bit = 1;
		uint8 hitBit = 1;
		for (uint8 i = 0; i < 3; ++i)
		{
			if (bitsS & bit) // point of intersection is entering
			{
				const float temp = (h[i] - source[i]) / (target[i] - source[i]);
				if (temp > tEnter)
				{
					tEnter = temp;
					hitBit = bit;
				}
			}
			else if (bitsT & bit) // point of intersection is exiting
				tExit = Min(tExit, (h[i] - source[i]) / (target[i] - source[i]));
			bit <<= 1;
			if (bitsS & bit) // point of intersection is entering
			{
				const float temp = (-h[i] - source[i]) / (target[i] - source[i]);
				if (temp > tEnter)
				{
					tEnter = temp;
					hitBit = bit;
				}
			}
			else if (bitsT & bit) // point of intersection is exiting
				tExit = Min(tExit, (-h[i] - source[i]) / (target[i] - source[i]));
			bit <<= 1;
		}
		if (tEnter != 0.0f && tEnter < tExit + EPSILON) // only return positive hit if ray is REALLY ENTERING
		{
			t = tEnter;
			Vec3 localN;
			switch (hitBit)
			{
			case 1u:
				localN.Set(1.0f, 0.0f, 0.0f);
				break;
			case 2u:
				localN.Set(-1.0f, 0.0f, 0.0f);
				break;
			case 4u:
				localN.Set(0.0f, 1.0f, 0.0f);
				break;
			case 8u:
				localN.Set(0.0f, -1.0f, 0.0f);
				break;
			case 16u:
				localN.Set(0.0f, 0.0f, 1.0f);
				break;
			case 32u:
				localN.Set(0.0f, 0.0f, -1.0f);
				break;
			}
			n = body.LocalToGlobalVec(mOrientation * localN);
			return true;
		}
	}

	return false;
}
