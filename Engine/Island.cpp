#include "stdafx.h"
#include "Island.h"
#include "ConstraintSolver.h"
Island::Island(const Math::Vector3 &gravity, float linearDampingFactor, float angularDampingFactor)
: mGravity(gravity)
, mLinearDamp(linearDampingFactor)
, mAngularDamp(angularDampingFactor)
{ }

void Island::Add(RigidBody &body)
{
	mRigidBodies.push_back(&body);
}

void Island::Remove(RigidBody &body)
{
	mRigidBodies.erase(&body);
}

void Island::Add(Joint &joint)
{
	mJoints.push_back(&joint);
}

void Island::Remove(Joint &joint)
{
	mJoints.erase(&joint);
}

void Island::Add(ContactManifold &manifold)
{
	mManifolds.push_back(&manifold);
}

void Island::Remove(ContactManifold &manifold)
{
	mManifolds.erase(&manifold);
}

void Island::Clear(void)
{
	mRigidBodies.clear();
	mManifolds.clear();
	mJoints.clear();
}

void Island::Sleep(const TimeStep &timeStep)
{
	for (auto &body : mRigidBodies)
		body.SetAwake(false);
}

void Island::Step(const TimeStep &timeStep)
{
	ContactSolver contactSolver(*this);

	// update velocity
	for (auto &body : mRigidBodies)
	{
		switch (body.mType)
		{
		case RigidBody::DYNAMIC:
			float dtscale = body.mParent->cphy->dtScale;

			// apply gravity
			if (body.mHasGravity)
			{
				Vector3 newG( mGravity + body.mGravityFactor);
				body.mLinearVelocity += newG * timeStep.dt;
			}
				
			// integrate velocity
			body.mLinearVelocity +=
				body.mMassData.inverseMass * (body.mLinearImpulseAccumulator + body.mForceAccumulator * timeStep.dt  * dtscale);
			body.mAngularVelocity +=
				body.mGlobalInverseInertiaTensor * (body.mAngularImpulseAccumulator + body.mTorqueAccumulator * timeStep.dt  * dtscale);
			body.mForceAccumulator.ZeroOut();
			body.mTorqueAccumulator.ZeroOut();
			body.mLinearImpulseAccumulator.ZeroOut();
			body.mAngularImpulseAccumulator.ZeroOut();
			body.mLinearVelocity *= mLinearDamp;
			body.mAngularVelocity *= mAngularDamp;
			break;
		}
	}

	// warm start
	if (timeStep.params.warmStart)
		contactSolver.WarmStartVelocityConstraints(timeStep);

	// initialize constraints
	for (auto &joint : mJoints)
		joint.InitializeVelocityConstraints(timeStep);
	contactSolver.InitializeVelocityConstraints(timeStep);

	// solve constraints
	for (unsigned char i = 0; i < timeStep.velocityIterations; ++i)
	{
		for (auto &joint : mJoints)
			joint.SolveVelocityConstraints(timeStep);
		contactSolver.SolveVelocityConstraints(timeStep);
	}

	// cache velocity constraint results
	if (timeStep.params.warmStart)
		contactSolver.CacheVelocityConstraintResults(timeStep);

	// integrate position
	for (auto &body : mRigidBodies)
	{
		// integrate
		body.IntegratePosition(timeStep);

		// update
		body.UpdateOrientation();
		body.UpdatePositionFromGlobalCentroid();
	}

	// solve position constraints, NOPE
	for (unsigned char i = 0; i < timeStep.positionIterations; ++i)
	{
		for (auto &joint : mJoints)
			joint.SolvePositionConstraints(timeStep);
		contactSolver.SolvePositionConstraints(timeStep);
	}

	// update proxies
	for (auto &body : mRigidBodies)
		body.UpdateProxies();
}