#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "BoxShape.h"

CBoxColliderComponent::CBoxColliderComponent(void)
: mCenter(0.0f, 0.0f, 0.0f)
, mHalfExtents(1.0f, 1.0f, 1.0f)
, mBoxSize(1,1,1)
, mID("BoxColliderComponent")
{
	mOrientation.SetIdentity();
}

void CBoxColliderComponent::OnFree(void)
{
	ColliderComponent::OnFree();
}

void CBoxColliderComponent::OnInit(void)
{
	ColliderComponent::OnInit();
}

void CBoxColliderComponent::OnStart(void)
{
	if (!gameObject->HasComponent<CPhysics>())
	{
		mCphy = gameObject->AddComponent<CPhysics>();
		mCphy->collidarCom = this;
	}
	else
	{
		mCphy = static_cast<CPhysics*>(gameObject->GetComponentByName("CPhysics"));
		mCphy->collidarCom = this;
	}

	
	CMeshRenderer*mesh = reinterpret_cast<CMeshRenderer*>(gameObject->GetComponentByName("CMeshRenderer"));
	if (mesh)
	{
		mBoxSize = mesh->GetMesh()->GetAABB().maxPoint - mesh->GetMesh()->GetAABB().minPoint;
		mCenter =  mesh->GetMesh()->GetOrigin();
	}

	SetHalfExtents(gameObject->transform->scale * mBoxSize * 0.5f);

	ColliderComponent::OnStart();
}

void CBoxColliderComponent::OnUpdate(void)
{
	ColliderComponent::OnUpdate();
}

// Getters

const Math::Matrix3 &CBoxColliderComponent::GetOrientation(void) const
{
	return mOrientation;
}

const Math::Vector3 &CBoxColliderComponent::GetCenter(void) const
{
	return mCenter;
}

const Math::Vector3 &CBoxColliderComponent::GetHalfExtents(void) const
{
	return mHalfExtents;
}

CBoxColliderComponent &CBoxColliderComponent::SetOrientation(const Math::Matrix3 &orientation)
{
	mOrientation = orientation;
	InvalidateCollider();
	return *this;
}

CBoxColliderComponent &CBoxColliderComponent::SetCenter(const Math::Vector3 &center)
{
	mCenter = center;
	InvalidateCollider();
	return *this;
}

CBoxColliderComponent &CBoxColliderComponent::SetHalfExtents(const Math::Vector3 &halfExtents)
{
	mHalfExtents = halfExtents;
	InvalidateCollider();
	return *this;
}

Collider *CBoxColliderComponent::CreateCollider(RigidBody &body)
{
	BoxColliderDef def;
	def.mCenter.Set(mCenter.x, mCenter.y, mCenter.z);
	def.mOrientation = mOrientation;
	def.mHalfExtents.Set(mHalfExtents.x, mHalfExtents.y, mHalfExtents.z);

	return &body.CreateCollider(def);
}

const Vector3& CBoxColliderComponent::UpdateSize(const Vector3 &halfext)
{
	return mHalfExtents = halfext;
}



meta_define(CBoxColliderComponent);
___COMP_REGISTER(CBoxColliderComponent);
