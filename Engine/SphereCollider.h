#pragma once
#include "ColliderCom.h"

class CSphereColliderComponent
	: public ColliderComponent
{
public:
	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);

	virtual const Vector3& UpdateSize(const Vector3 &halfext);
	const static std::string ID;

	CSphereColliderComponent(void);
	//virtual SphereColliderComponent3D * Clone(void) const;

	const Vec3 &GetCenter(void) const;
	CSphereColliderComponent &SetCenter(const Vec3 &c);

	float GetRadius(void) const;
	CSphereColliderComponent &SetRadius(float r);

protected:

	virtual Collider *CreateCollider(RigidBody &body);

public:
	___BEGIN_REGISTERED___
	Vec3 mCenter;
	float mRadius;
	___END_REGISTERED___
		___SERIALIZE_BEGIN___
		___DEFBASE(ColliderComponent)
		___DEFSER(mCenter, 1)
		___DEFSER(mRadius, 1)
	___SERIALIZE_END___
	virtual void CopyFrom(const ::CSphereColliderComponent* c)
	{
		mCenter = c->mCenter;
		mRadius = c->mRadius;
	}
	metadef(CSphereColliderComponent)
		endmetadef;
	COMPONENTCLONE(CSphereColliderComponent);
};
___SERIALIZE_CLASS(CSphereColliderComponent, 1);