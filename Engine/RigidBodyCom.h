/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "RigidBody.h"

#include <string>

class ColliderComponent;
class PhysicsSystem3D;
class CPhysics;

class RigidBodyComponent

{
	friend class ColliderComponent;
	friend class PhysicsSystem3D;
public:

	RigidBodyComponent(void);

	void Dispose(void);
	void Update(void);
	void SyncCphysics(void);
	
	// Setters
	RigidBodyComponent &SetLinearVelocity(const Math::Vector3 &pos);
	RigidBodyComponent &SetAngularVelocity(const Math::Vector3 &pos);
	RigidBodyComponent &SetHasGravity(bool hasGravity);
	RigidBodyComponent &SetAllowRotation(bool allowRotation);
	RigidBodyComponent &SetType(int type);
	RigidBodyComponent &SetAwake(bool awake);
	RigidBodyComponent &SetPosition(const Math::Vector3 &pos);
	RigidBodyComponent &SetOrientation(const Math::Matrix3 &orientation);
	RigidBodyComponent &SetGravityFactor(const Vector3& g);
	// Getters
	int GetType(void) const;
	bool GetAwake(void) const;
	RigidBody &Body(void) { return *mBody; }
	const Math::Vector3 &GetCentroid(void) const;
	const Math::Vector3 &GetPosition(void) const;
	const Math::Matrix3 &GetOrientation(void) const;
	const Math::Vector3 GetAngularVelocity(void) const;
	const Math::Vector3 GetLinearVelocity(void) const;
	bool GetAllowRotation(void) const;
	bool GetHasGravity(void) const;

	// Helpers
	void GrabPosFromTransform(void);
	Math::Vector3 Forward(void);
	Math::Vector3 Right(void);
	Math::Vector3 Up(void);
	RigidBodyComponent &ApplyForce(const Math::Vector3 &force, const Math::Vector3 &at);
	RigidBodyComponent &ApplyLinearForce(const Math::Vector3 &force);
	RigidBodyComponent &ApplyAngularImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	RigidBodyComponent &ApplyImmediateImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	RigidBodyComponent &ApplyAngularForce(const Math::Vector3 &force, const Math::Vector3 &at);
	RigidBodyComponent &ApplyMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);
	RigidBodyComponent &ApplyLinearMasslessForce(const Math::Vector3 &force);
	RigidBodyComponent &ApplyImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	RigidBodyComponent &ApplyLinearImpulse(const Math::Vector3 &impulse);
	RigidBodyComponent &ApplyAngularMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);

	struct ContactRecord
	{
	public:
		ContactRecord(RigidBodyComponent *other = nullptr)
			: mOther(other)
		{ }
		RigidBodyComponent &Other(void) const { return *mOther; }
	private:
		RigidBodyComponent *mOther;
	};

	typedef std::vector<ContactRecord> ContactRecordList;
	const ContactRecordList &ContactData(void) const { return mContactRecords; }

	
	IN_LIST_LINK(RigidBodyComponent) mSystemLink;
	CPhysics* cphy;
	static const std::string mID;
private:

	RigidBody *mBody;
	int mType;
	bool mFirstFrame;
	Math::Vector3 mLinearVelocity;
	Math::Vector3 mAngularVelocity;
	bool mAllowRotation;
	bool mHasGravity;
	bool mIsAwake;
	ContactRecordList mContactRecords;
};