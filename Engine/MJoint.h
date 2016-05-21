/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "JointPhysics.h"
#include "MathIncludes.h"
#include "PhysicsDebugDraw.h"

struct MouseJointDef
	: public JointDef
{

	RigidBody *body;
	Math::Vector3 anchor; // in rigid body's local space
	Math::Vector3 target; // mouse position in global space
	float maxForce;
	float dampingRatio;
	float frequencyHz;

	MouseJointDef(RigidBody *body, float maxForce = 1000.0, float dampingRatio = 0.75f, float frequencyHz = 2.0f)
		: JointDef(JointType::MOUSE)
		, body(body)
		, anchor(0.0f, 0.0f, 0.0f)
		, target(0.0f, 0.0f, 0.0f)
		, maxForce(maxForce)
		, dampingRatio(dampingRatio)
		, frequencyHz(frequencyHz)
	{ }
};

class MouseJoint
	: public Joint
{
public:

	MouseJoint(const MouseJointDef &def)
		: mAnchor(def.anchor)
		, mTarget(def.target)
		, mMaxForce(def.maxForce)
		, mDampingRatio(def.dampingRatio)
		, mFrequencyHz(def.frequencyHz)
		, mImpulse(0.0f, 0.0f, 0.0f) // must be initialized to zero so the initial warm starting is correct
	{
		// just use body 1
		mBodyA = def.body;

		// don't allow sleep
		mAllowSleep = false;

		GV_ASSERT_MESSAGE(mBodyA, "Invalid rigid body.");
	}

	MouseJoint &SetAnchor(const Math::Vector3 &anchor)
	{
		mAnchor = anchor;
		return *this;
	}
	const Math::Vector3 Anchor(void) const { return mAnchor; }

	MouseJoint &SetTarget(const Math::Vector3 &target)
	{
		mTarget = target;
		return *this;
	}
	const Math::Vector3 Target(void) const { return mTarget; }

	virtual void InitializeVelocityConstraints(const TimeStep &timeStep);
	virtual void SolveVelocityConstraints(const TimeStep &timeStep);

protected:

	virtual void Draw(PhysicsDebugDraw &debugDraw) const;

private:

	// parameters
	Math::Vector3 mAnchor;
	Math::Vector3 mTarget;
	float mMaxForce;
	float mDampingRatio;
	float mFrequencyHz;

	// solution
	float mBeta;
	float mGamma;
	Math::Vector3 mGlobalAnchor;
	Math::Vector3 mImpulse;
	Math::Matrix3 mEffectiveMass;
	Math::Vector3 mC; // position constraint feedback
	Math::Vector3 mR; // global centroid to global anchor
};