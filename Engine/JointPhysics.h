/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "InList.h"
#include "TimeStep.h"
#include "PhysicsFlag.h"
#include "List_Allocator.h"
#include "PhysicsDebugDraw.h"
class RigidBody;
class PhysicsWorld;
class Island;
class Joint;

struct JointEdge
{
	Joint *joint;
	RigidBody *other;

	// links to other contact edges of the same rigid body
	IN_LIST_LINK(JointEdge) siblingLink;

	JointEdge(void)
		: joint(nullptr)
		, other(nullptr)
	{ }
};

struct JointType
{
	typedef unsigned char value_type;
	static const value_type MOUSE = 0;
};

struct JointDef
{
	const JointType::value_type type;

	JointDef(unsigned char type)
		:type(type)
	{ }
};

class Joint
{
	friend class PhysicsWorld;
	friend class RigidBody;
	friend class Island;
public:

	// Getters
	RigidBody *BodyA(void) { return mBodyA; }
	RigidBody *BodyB(void) { return mBodyB; }

	// Helpers
	virtual void SolvePositionConstraints(const TimeStep &timeStep) { }
	static Joint *Create(const JointDef &def, Allocator &allocator);
	virtual void InitializeVelocityConstraints(const TimeStep &timeStep) { }
	virtual void SolveVelocityConstraints(const TimeStep &timeStep) { }
	static void Destroy(Joint *joint, Allocator &allocator);

protected:

	Joint(void)
		: mBodyA(nullptr)
		, mBodyB(nullptr)
		, mAllowSleep(true)
	{
		mEdgeA.joint = mEdgeB.joint = this;
	}
	virtual ~Joint(void) = 0;

	RigidBody *mBodyA;
	RigidBody *mBodyB;
	bool mAllowSleep;

	virtual void Draw(PhysicsDebugDraw &debugDraw) const { }

private:

	JointType::value_type type;
	PhysicsFlags mFlags;

	IN_LIST_LINK(Joint) mWorldLink;
	IN_LIST_LINK(Joint) mIslandLink;

	JointEdge mEdgeA;
	JointEdge mEdgeB;
};
