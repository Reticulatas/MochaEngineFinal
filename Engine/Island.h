/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "RigidBody.h"
#include "InList.h"
struct TimeStep;
class ContactSolver;
class PhysicsWorld;

class Island 
{
	friend class ContactSolver;
	friend class PhysicsWorld;
public:

	Island(const Math::Vector3 &gravity, float linearDampingFactor, float angularDampingFactor);

	void Add(RigidBody &body);
	void Remove(RigidBody &body);
	void Add(Joint &joint);
	void Remove(Joint &joint);
	void Add(ContactManifold &manifold);
	void Remove(ContactManifold &manifold);
	void Clear(void);

	void Sleep(const TimeStep &timeStep);
	void Step(const TimeStep &timeStep);

private:

	const Math::Vector3 &mGravity;
	const float mLinearDamp;
	const float mAngularDamp;
	IN_LIST(RigidBody, mIslandLink) mRigidBodies;
	IN_LIST(Joint, mIslandLink) mJoints;
	IN_LIST(ContactManifold, islandLink) mManifolds;

public:

	__forceinline unsigned NumRigidBodies(void) const { return mRigidBodies.size(); }
	__forceinline unsigned NumManifolds(void) const { return mManifolds.size(); }
};