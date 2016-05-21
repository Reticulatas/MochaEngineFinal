/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once

#include "MathIncludes.h"
#include "InList.h"
#include "Mass.h"
#include "PhysicsFlag.h"
#include "TimeStep.h"
#include "JointPhysics.h"
#include "Collidar.h"
class PhysicsWorld;
class ContactManager;
class ContactSolver;
class RigidBodyComponent;
struct ContactManifold;

struct RigidBodyDef
{
	unsigned char type;
	bool allowRotation : 1;
	bool hasGravity : 1;
	Math::Vector3 position;
	Math::Vector3 linearVelocity;
	Math::Vector3 angularVelocity;
	Math::Matrix3 orientation;
	void *userData;

	RigidBodyDef(unsigned char rigidBodyType)
		: type(rigidBodyType)
		, allowRotation(true)
		, hasGravity(true)
		, position(0.0f, 0.0f, 0.0f)
		, linearVelocity(0.0f, 0.0f, 0.0f)
		, angularVelocity(0.0f, 0.0f, 0.0f)
		, userData(nullptr)
	{
		orientation.SetIdentity();
	}
};
class RigidBody;
struct BodyContactEdge
{
	IN_LIST_LINK(BodyContactEdge) bodyLink;
	RigidBody *other;
	BodyContactEdge *twin;
	unsigned counter; 
	BodyContactEdge(void);
};

class RigidBody
{
	friend class PhysicsWorld;
	friend class ContactManager;
	friend class ContactSolver;
	friend class Island;
	friend class Collider;
public:

	static const unsigned char STATIC = 0;
	static const unsigned char DYNAMIC = 1;
	static const unsigned char KINEMATIC = 2;

	RigidBody(const RigidBodyDef &def);
	~RigidBody(void);

	// Setters
	void SetPosition(const Math::Vector3 &position);
	void SetOrientation(Math::Matrix3 orientation);
	void SetScale(const Math::Vector3 &scale);
	void SetLinearVelocity(const Math::Vector3 &velocity);
	void SetAngularVelocity(const Math::Vector3 &velocity);
	void SetAllowRotation(bool allowRotation);
	void SetHasGravity(bool hasGravity);
	void SetAwake(bool awake);
	__forceinline void SetType(unsigned char type) { mType = type; }

	// Getters
	bool HasGravity(void) const;
	bool AllowRotation(void) const;
	__forceinline const Math::Vector3 &Position(void) const { return mPosition; }
	__forceinline const Math::Vector3 &LinearVelocity(void) const { return mLinearVelocity; }
	__forceinline const MassData &GetMassData(void) const { return mMassData; }
	__forceinline const Math::Matrix3 &Orientation(void) const { return mOrientation; }
	__forceinline const Math::Matrix3 &InverseOrienation(void) const { return mInverseOrientation; }
	__forceinline const Math::Matrix3 &GlobalInverseInertiaTensor(void) const { return mGlobalInverseInertiaTensor; }
	__forceinline const Math::Vector3 &LocalCentroid(void) const { return mLocalCentroid; }
	__forceinline const Math::Vector3 &GlobalCentroid(void) const { return mGlobalCentroid; }
	__forceinline unsigned char Type(void) const { return mType; }
	__forceinline const Math::Vector3 &AngularVelocity(void) const { return mAngularVelocity; }
	__forceinline bool IsAwake(void) const { return mFlags.Test(PhysicsFlags::AWAKE); }
	typedef IN_LIST(BodyContactEdge, bodyLink) ContactEdgeList;
	const ContactEdgeList &ContactEdges(void) const;

	// Helpers
	void ApplyImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	void ApplyLinearImpulse(const Math::Vector3 &impulse);
	void ApplyLinearMasslessForce(const Math::Vector3 &force);
	void ApplyAngularMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);
	void ApplyForce(const Math::Vector3 &force, const Math::Vector3 &at);
	void ApplyLinearForce(const Math::Vector3 &force);
	void ApplyAngularImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	void ApplyImmediateImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	void ApplyAngularForce(const Math::Vector3 &force, const Math::Vector3 &at);
	void ApplyMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);

	__forceinline const Math::Vector3 LocalToGlobalNoScale(const Math::Vector3 &vec) const { return mPosition + mOrientation * (vec ); }
	__forceinline const Math::Vector3 LocalToGlobalVecNoScale(const Math::Vector3 &vec) const { return mOrientation * (vec ); }
	__forceinline const Math::Vector3 GlobalToLocalNoScale(const Math::Vector3 &vec) const { return mInverseOrientation * (vec - mPosition); }
	__forceinline const Math::Vector3 GlobalToLocalVecNoScale(const Math::Vector3 &vec) const { return (mInverseOrientation * vec); }

	const Math::Vector3 LocalToGlobal(const Math::Vector3 &vec) const;
	__forceinline const Math::Vector3 LocalToGlobalVec(const Math::Vector3 &vec) const { return mOrientation * (vec * mScale); }
	__forceinline const Math::Vector3 GlobalToLocal(const Math::Vector3 &vec) const { return ((mInverseOrientation * (vec - mPosition))) * mInvseScale; }
	__forceinline const Math::Vector3 GlobalToLocalVec(const Math::Vector3 &vec) const { return (mInverseOrientation * vec) * mInvseScale; }
	__forceinline const bool CanCollide(const RigidBody &rhs) const
	{
		return (this != &rhs) && ((IsAwake() && mType != STATIC) || (rhs.IsAwake() && rhs.mType != STATIC));
	}
	Collider &CreateCollider(const ColliderDef &def);
	void DestroyCollider(Collider &collider);

	void *userData;
	MassData mMassData;
	RigidBodyComponent* mParent;
	Math::Vector3 mPosition;
	Math::Matrix3 mOrientation;
	Math::Vector3 mScale;
	Vector3 mGravityFactor;
	float mSleepTimer;
private:

	void InvalidateColliders(void);
	void ValidateColliders(void);
	bool CollidersValid(void) const;

	void IntegratePosition(const TimeStep &timeStep);
	void UpdateOrientation(void);
	void UpdateProxies(void);
	void UpdateMassAndLocalCentroid(void);
	void UpdateGlobalInverseInertiaTensor(void);
	void UpdateGlobalCentroidFromPosition(void);
	void UpdatePositionFromGlobalCentroid(void);
	void Draw(PhysicsDebugDraw &debugDraw) const;

	IN_LIST_LINK(RigidBody) mIslandLink;
	IN_LIST_LINK(RigidBody) mIslandSearchLink;
	IN_LIST_LINK(RigidBody) mWorldLink;

	IN_LIST(BodyContactEdge, bodyLink) mContactEdges;
	IN_LIST(JointEdge, siblingLink) mJointEdges;
	IN_LIST(Collider, mSiblingLink) mColliders;

	PhysicsFlags mFlags;
	PhysicsWorld *mWorld;
	unsigned char mType;
	
	bool mColliderDirty : 1;
	bool mAllowRotation : 1;
	bool mHasGravity : 1;
	Math::Matrix3 mGlobalInverseInertiaTensor;
	Math::Matrix3 mInverseOrientation;
	
	Math::Vector3 mInvseScale;
	Math::Vector3 mAngularVelocity;
	Math::Vector3 mForceAccumulator;
	Math::Vector3 mLocalCentroid;
	Math::Vector3 mGlobalCentroid;

	Math::Vector3 mTorqueAccumulator;
	Math::Vector3 mLinearImpulseAccumulator;
	Math::Vector3 mAngularImpulseAccumulator;
	Math::Vector3 mLinearVelocity;
	
};

