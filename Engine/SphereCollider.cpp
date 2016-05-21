#include "stdafx.h"
#include "SphereCollider.h"
#include "SphereShape.h"
const std::string CSphereColliderComponent::ID = "SphereColliderComponent3D";

CSphereColliderComponent::CSphereColliderComponent(void)
: mCenter(0.0f, 0.0f, 0.0f)
, mRadius(1.0f)
{ }

//SphereColliderComponent3D * SphereColliderComponent3D::Clone(void) const
//{
//	SphereColliderComponent3D * component = new SphereColliderComponent3D;
//	component->m_center = m_center;
//	component->m_radius = m_radius;
//	return component;
//}

const Vec3 &CSphereColliderComponent::GetCenter(void) const
{
	return mCenter;
}
CSphereColliderComponent &CSphereColliderComponent::SetCenter(const Vec3 &c)
{
	mCenter = c;
	InvalidateCollider();
	return *this;
}

float CSphereColliderComponent::GetRadius(void) const
{
	return mRadius;
}
CSphereColliderComponent &CSphereColliderComponent::SetRadius(float r)
{
	mRadius = r;
	InvalidateCollider();
	return *this;
}

Collider *CSphereColliderComponent::CreateCollider(RigidBody &body)
{
	SphereColliderDef3D def;
	def.center = mCenter;
	def.radius = mRadius;

	return &body.CreateCollider(def);
}

const Vector3& CSphereColliderComponent::UpdateSize(const Vector3 &halfext)
{
	//TODOKai
	return halfext;
}

void CSphereColliderComponent::OnFree(void)
{

}

void CSphereColliderComponent::OnInit(void)
{

}

void CSphereColliderComponent::OnStart(void)
{

}

void CSphereColliderComponent::OnUpdate(void)
{

}

meta_define(CSphereColliderComponent);
___COMP_REGISTER(CSphereColliderComponent);
