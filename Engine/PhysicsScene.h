/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include <vector>
#include "RigidBody.h"
#include "InList.h"
#include "RayCastResult.h"
#include "BroadPhasePhy.h"
class Collider;

class PhysicsWorld 
{
	friend class RigidBody;
	friend class Collider;
public:
	PhysicsWorld(void);
	~PhysicsWorld(void);
	void Step(float dt, unsigned char velocityIterations = 14, unsigned char positionIteratos = 14);
	void Draw(void);
	PhysicsDebugDraw &DebugDraw(void);

	typedef std::vector<RigidBody *> PickResult;
	typedef std::vector<Collider *> RegionQueryResult;

	// Setters
	void SetAllowSleep(bool allowSleep);
	void SetSleepThreshold(float linearVelocity, float angularVelocity, float time);
	void SetBroadphase(unique_ptr_phy<Broadphase> broadphase);
	void SetDamping(float linear, float angular);
	void SetWarmStart(bool warmStart);
	void SetListener(ContactListener *listener);
	void SetDamping(float damping);
	void SetWarmStartFraction(float fraction);
	void SetParams(const PhysicsParams &params);
	void SetDebugDraw(unique_ptr_phy<PhysicsDebugDraw> debugDraw);
	void SetGravity(const Math::Vector3 &gravity);

	// Getters
	PhysicsParams &Params(void);
	const PhysicsParams &Params(void) const;
	Math::Vector3 &Gravity(void);
	const Math::Vector3 &Gravity(void) const;
	const PhysicsStats &Stats(void) const;

	// Helper
	void WakeAll(void);
	RigidBody &CreateBody(const RigidBodyDef &def);
	const RayCastResult RayCast(const Ray3 &ray, float maxDistance);
	void QueryRegion(const Math::Vector3 &minPoint, const Math::Vector3 &maxPoint, RegionQueryResult &result);
	void DestroyBody(RigidBody &rigidBody);
	void Pick(const Math::Vector3 &pos, PickResult &result);
	Joint &CreateJoint(const JointDef &def);
	void DestroyJoint(Joint &joint);

private:

	void SimulateIslands(const TimeStep &timeStep);

	unique_ptr_phy<Broadphase> mBroadphase;
	unique_ptr_phy<PhysicsDebugDraw> mDebugDraw;
	ContactManager mContactManager;
	IN_LIST(RigidBody, mWorldLink) mRigidBodies;
	IN_LIST(Joint, mWorldLink) mJoints;

	PhysicsStats mStats;
	std::vector<RayCastResult> mRayCasts;
	PhysicsParams mParams;
	Math::Vector3 mGravity;

	Allocator mColliderGeometryAllocator;
	Allocator mJointAllocator;
	FixedSizeAllocator mBodyAllocator;
	FixedSizeAllocator mColliderAllocator;
	FixedSizeAllocator mBodyContactEdgeAllocator;
	FixedSizeAllocator mManifoldAllocator;
	FixedSizeAllocator mContactAllocator;
};