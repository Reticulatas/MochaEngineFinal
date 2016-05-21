/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
class Island;
class RigidBody;
struct TimeStep;
struct Contact;

namespace Jacobian
{
	struct J2V2W;
}

class ContactSolver 
{
public:

	ContactSolver(Island &island);
	void WarmStartVelocityConstraints(const TimeStep &timeStep);
	void InitializeVelocityConstraints(const TimeStep &timeStep);
	void SolveVelocityConstraints(const TimeStep &timeStep);
	void CacheVelocityConstraintResults(const TimeStep &timeStep);
	void SolvePositionConstraints(const TimeStep &timeStep);

private:

	void ComputeJacobian(const Math::Vector3 &v, const Math::Vector3 &rA, const Math::Vector3 &rB, Jacobian::J2V2W &j) const;
	float ComputeEffectiveMass(const RigidBody &bodyA, const RigidBody &bodyB, const Jacobian::J2V2W &j) const;
	float JDotV(const RigidBody &bodyA, const RigidBody &bodyB, const Jacobian::J2V2W &j) const;

	Island &mIsland;
};