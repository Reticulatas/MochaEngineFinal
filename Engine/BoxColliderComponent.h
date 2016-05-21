/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "ColliderCom.h"
#include <string>
class CBoxColliderComponent
	: public ColliderComponent
{
public:
	void OnFree(void);
	void OnInit(void) override;
	void OnStart(void);
	void OnUpdate(void);

	CBoxColliderComponent(void);

	virtual const Vector3& UpdateSize(const Vector3 &halfext);
	
	const Math::Vector3 &GetCenter(void) const;
	CBoxColliderComponent &SetCenter(const Math::Vector3 &center);

	const Math::Vector3 &GetHalfExtents(void) const;
	CBoxColliderComponent &SetHalfExtents(const Math::Vector3 &halfExtents);

	const Math::Matrix3 &GetOrientation(void) const;
	CBoxColliderComponent &SetOrientation(const Math::Matrix3 &orientation);

protected:

	virtual Collider *CreateCollider(RigidBody &body);

public:
	___BEGIN_REGISTERED___
	___END_REGISTERED___
	const std::string mID;
	Math::Vector3 mCenter;
	Math::Vector3 mHalfExtents;
	Vector3 mBoxSize;
	Math::Matrix3 mOrientation;
	___SERIALIZE_BEGIN___
		___DEFBASE(ColliderComponent)
		___DEFSER(mCenter, 1)
		___DEFSER(mHalfExtents, 1)
		___DEFSER(mOrientation, 1)
		___DEFSER(mBoxSize, 2)
	___SERIALIZE_END___
	virtual void CopyFrom(const ::CBoxColliderComponent* c)
	{
		mCenter = c->mCenter;
		mHalfExtents = c->mHalfExtents;
		mOrientation = c->mOrientation;
		mBoxSize = c->mBoxSize;

	}
	metadef(CBoxColliderComponent)
		endmetadef;
	COMPONENTCLONE(CBoxColliderComponent);

};

___SERIALIZE_CLASS(CBoxColliderComponent, 2);