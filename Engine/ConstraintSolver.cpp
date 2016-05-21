#include "stdafx.h"
#include "ConstraintSolver.h"
#include "Mass.h"
#include "RigidBody.h"
#include "Island.h"
#include "../MochaMath/Math/Quat.h"
ContactSolver::ContactSolver(Island &island)
: mIsland(island)
{ }

__forceinline float ContactSolver::ComputeEffectiveMass(const RigidBody &bodyA, const RigidBody &bodyB, const Jacobian::J2V2W &j) const
{
	float bot = (bodyA.mMassData.inverseMass
		+ j.wA.Dot(bodyA.mGlobalInverseInertiaTensor * j.wA)
		+ j.wB.Dot(bodyB.mGlobalInverseInertiaTensor * j.wB))
		+ bodyB.mMassData.inverseMass;


	return 1.0f / bot;
}

__forceinline float ContactSolver::JDotV(const RigidBody &bodyA, const RigidBody &bodyB, const Jacobian::J2V2W &j) const
{
	return
		j.vA.Dot(bodyA.mLinearVelocity)
		+ j.wA.Dot(bodyA.mAngularVelocity)
		+ j.vB.Dot(bodyB.mLinearVelocity)
		+ j.wB.Dot(bodyB.mAngularVelocity);
}

__forceinline void ContactSolver::ComputeJacobian(const Math::Vector3 &v, const Math::Vector3 &rA, const Math::Vector3 &rB, Jacobian::J2V2W &j) const
{
  j.vA = -v;
  j.wA = -rA.Cross(v);
  j.vB = v;
  j.wB = rB.Cross(v);
}

void ContactSolver::WarmStartVelocityConstraints(const TimeStep &timeStep)
{
  for (ContactManifold &manifold : mIsland.mManifolds)
  {
    RigidBody &bodyA = *manifold.colliderA->mParent;
    RigidBody &bodyB = *manifold.colliderB->mParent;
	const Collider &colliderA = *manifold.colliderA;
	const Collider &colliderB = *manifold.colliderB;
    const MassData &mA = bodyA.mMassData;
    const MassData &mB = bodyB.mMassData;
    Math::Vector3 &vA = bodyA.mLinearVelocity;
    Math::Vector3 &vB = bodyB.mLinearVelocity;
    Math::Vector3 &wA = bodyA.mAngularVelocity;
    Math::Vector3 &wB = bodyB.mAngularVelocity;

    for (unsigned char i = 0; i < manifold.numContacts; ++i)
    {
      Contact &contact = *manifold.contacts[i];
      if (!contact.persistent)
        continue;

      const Math::Vector3 &n = contact.normal;
      const Math::Vector3 &t1 = contact.tangent1;
      const Math::Vector3 &t2 = contact.tangent2;
      const Math::Vector3 rA = bodyA.LocalToGlobal(contact.localPositionA) - bodyA.GlobalCentroid();
      const Math::Vector3 rB = bodyB.LocalToGlobal(contact.localPositionB) - bodyB.GlobalCentroid();
      auto &jn = contact.jn;

      // apply normal impulse
      ComputeJacobian(n, rA, rB, jn);
      const float normalImpulse = contact.normalImpulseCache * timeStep.params.warmStartFraction;
      vA += mA.inverseMass                     * jn.vA * normalImpulse;
      wA += bodyA.mGlobalInverseInertiaTensor * jn.wA * normalImpulse;
      vB += mB.inverseMass                     * jn.vB * normalImpulse;
      wB += bodyB.mGlobalInverseInertiaTensor * jn.wB * normalImpulse;
      contact.normalImpulseSum = normalImpulse;

      // derive warm start friction from cached normal impulse
      Math::Vector3 vBA = vB + wB.Cross(rB) - vA - wA.Cross(rA);
      if (vBA.LengthSq() > EPSILON * EPSILON)
      {
        const Math::Vector3 t = (vBA - vBA.ProjectedUnit(n)).Normalized();
        Jacobian::J2V2W jt;
        ComputeJacobian(t, rA, rB, jt);

        // apply tangent impulse
        const float lambda = -JDotV(bodyA, bodyB, jt) * ComputeEffectiveMass(bodyA, bodyB, jt);
		const float maxFriction = MixFriction(bodyA.mParent->cphy->mFriction, bodyB.mParent->cphy->mFriction) * normalImpulse;
        const float tangentImpulse =
          Clamp(lambda * timeStep.params.warmStartFraction, -maxFriction, maxFriction);
        vA += mA.inverseMass                     * jt.vA * tangentImpulse;
        wA += bodyA.mGlobalInverseInertiaTensor * jt.wA * tangentImpulse;
        vB += mB.inverseMass                     * jt.vB * tangentImpulse;
        wB += bodyB.mGlobalInverseInertiaTensor * jt.wB * tangentImpulse;
        contact.tangentImpulseSum1 = tangentImpulse * t.Dot(contact.tangent1);
        contact.tangentImpulseSum2 = tangentImpulse * t.Dot(contact.tangent2);
      }
    } // end of for each contact
  } // end of for each manifold
}

void ContactSolver::SolveVelocityConstraints(const TimeStep &timeStep)
{
	for (ContactManifold &manifold : mIsland.mManifolds)
	{
		RigidBody &bodyA = *manifold.colliderA->mParent;
		RigidBody &bodyB = *manifold.colliderB->mParent;
		const Collider &colliderA = *manifold.colliderA;
		const Collider &colliderB = *manifold.colliderB;
		const MassData &mA = bodyA.mMassData;
		const MassData &mB = bodyB.mMassData;
		Vec3 &vA = bodyA.mLinearVelocity;
		Vec3 &vB = bodyB.mLinearVelocity;
		Vec3 &wA = bodyA.mAngularVelocity;
		Vec3 &wB = bodyB.mAngularVelocity;

		for (uint8 i = 0; i < manifold.numContacts; ++i)
		{
			Contact &contact = *manifold.contacts[i];

			const auto &jn = contact.jn;
			const auto &jt1 = contact.jt1;
			const auto &jt2 = contact.jt2;

			// lambdas (technically called Lagrange Multipliers)
			float normalImpulse;
			float tangentImpulse1, tangentImpulse2;

			// compute normal impulse
			normalImpulse = -(JDotV(bodyA, bodyB, jn) + contact.normalVelocityBias) * contact.effectiveMassN;
			const float clampedNormalImpulseSum = Max(normalImpulse + contact.normalImpulseSum, 0.0f);
			normalImpulse = clampedNormalImpulseSum - contact.normalImpulseSum;
			contact.normalImpulseSum = clampedNormalImpulseSum;

			// apply true normal impulse (bounce)
			vA += mA.inverseMass                     * jn.vA * normalImpulse;
			wA += bodyA.mGlobalInverseInertiaTensor * jn.wA * normalImpulse;
			vB += mB.inverseMass                     * jn.vB * normalImpulse;
			wB += bodyB.mGlobalInverseInertiaTensor * jn.wB * normalImpulse;

			// compute tangent impulse (don't care that we can actually get a max friction of sqrt(2) * real max friction)
			tangentImpulse1 = -JDotV(bodyA, bodyB, jt1) * contact.effectiveMassT1;
			tangentImpulse2 = -JDotV(bodyA, bodyB, jt2) * contact.effectiveMassT2;
			const float maxFriction = MixFriction(bodyA.mParent->cphy->mFriction, bodyB.mParent->cphy->mFriction) * Max(0.0f, contact.normalImpulseSum - timeStep.params.frictionSlop);
			const float clampedTangentImpulseSum1 = Clamp(tangentImpulse1 + contact.tangentImpulseSum1, -maxFriction, maxFriction);
			const float clampedTangentImpulseSum2 = Clamp(tangentImpulse2 + contact.tangentImpulseSum2, -maxFriction, maxFriction);
			tangentImpulse1 = clampedTangentImpulseSum1 - contact.tangentImpulseSum1;
			tangentImpulse2 = clampedTangentImpulseSum2 - contact.tangentImpulseSum2;
			contact.tangentImpulseSum1 = clampedTangentImpulseSum1;
			contact.tangentImpulseSum2 = clampedTangentImpulseSum2;

			// apply tangent impulses (friction)
			vA += mA.inverseMass                     * (jt1.vA * tangentImpulse1 + jt2.vA * tangentImpulse2);
			wA += bodyA.mGlobalInverseInertiaTensor * (jt1.wA * tangentImpulse1 + jt2.wA * tangentImpulse2);
			vB += mB.inverseMass                     * (jt1.vB * tangentImpulse1 + jt2.vB * tangentImpulse2);
			wB += bodyB.mGlobalInverseInertiaTensor * (jt1.wB * tangentImpulse1 + jt2.wB * tangentImpulse2);


		} // end of looping through contactJoints
	} // end of looping through manifolds
}

void ContactSolver::InitializeVelocityConstraints(const TimeStep &timeStep)
{
  const float baumgarte = timeStep.params.velocityBaumgarte / timeStep.dt;
  for (ContactManifold &manifold : mIsland.mManifolds)
  {
    const RigidBody &bodyA = *manifold.colliderA->mParent;
    const RigidBody &bodyB = *manifold.colliderB->mParent;
	const Collider &colliderA = *manifold.colliderA;
	const Collider &colliderB = *manifold.colliderB;
    const MassData &mA = bodyA.mMassData;
    const MassData &mB = bodyB.mMassData;
    const Math::Vector3 &vA = bodyA.mLinearVelocity;
    const Math::Vector3 &vB = bodyB.mLinearVelocity;
    const Math::Vector3 &wA = bodyA.mAngularVelocity;
    const Math::Vector3 &wB = bodyB.mAngularVelocity;

    for (unsigned char i = 0; i < manifold.numContacts; ++i)
    {
      Contact &contactInfo = *manifold.contacts[i];
      const Math::Vector3 &normal = contactInfo.normal;
      const Math::Vector3 globalPosA = bodyA.LocalToGlobal(contactInfo.localPositionA);
      const Math::Vector3 globalPosB = bodyB.LocalToGlobal(contactInfo.localPositionB);
	  const Math::Vector3 rA = globalPosA - bodyA.GlobalCentroid();
	  const Math::Vector3 rB = globalPosB - bodyB.GlobalCentroid();
	  const Math::Vector3 &t1 = contactInfo.tangent1;
	  const Math::Vector3 &t2 = contactInfo.tangent2;
      auto &jn = contactInfo.jn;
      auto &jt1 = contactInfo.jt1;
      auto &jt2 = contactInfo.jt2;

      ComputeJacobian(normal, rA, rB, jn);
      ComputeJacobian(t1, rA, rB, jt1);
      ComputeJacobian(t2, rA, rB, jt2);
      contactInfo.effectiveMassN = ComputeEffectiveMass(bodyA, bodyB, jn);
      contactInfo.effectiveMassT1 = ComputeEffectiveMass(bodyA, bodyB, jt1);
      contactInfo.effectiveMassT2 = ComputeEffectiveMass(bodyA, bodyB, jt2);

      // split relative velocity into normal and tangent components
      contactInfo.tangentRelativeVelocity = vB + wB.Cross(rB) - vA - wA.Cross(rA);
      contactInfo.normalRelativeVelocity = normal * normal.Dot(contactInfo.tangentRelativeVelocity);
      contactInfo.tangentRelativeVelocity -= contactInfo.normalRelativeVelocity;

      // compute normal velocity bias
      contactInfo.depth = (globalPosA - globalPosB).Dot(normal);
      const float baumgarteBias = -baumgarte * Max(0.0f, contactInfo.depth - timeStep.params.velocityBaumgarteSlop);
	  const float restitutionBias = Min(0.0f, MixRestitution(bodyA.mParent->cphy->mRestitution, bodyB.mParent->cphy->mRestitution) * normal.Dot(vB + wB.Cross(rB) - vA - wA.Cross(rA)) + timeStep.params.restitutionSlop);
	  float normalDerivativeBias;
	  //if ((vB - vA) == Vector3().cZero)
	  //  normalDerivativeBias = 0;
	  //else
          normalDerivativeBias = (vB - vA).Normalized().Dot(globalPosA - globalPosB);
      contactInfo.normalVelocityBias = baumgarteBias + restitutionBias + normalDerivativeBias;
    }
  }
}

void ContactSolver::CacheVelocityConstraintResults(const TimeStep &timeStep)
{
  for (auto &manifold : mIsland.mManifolds)
  {
    for (unsigned i = 0; i < manifold.numContacts; ++i)
    {
      // cache & clear impulse
      Contact &contact = *manifold.contacts[i];

      // compute contact information for external query
      contact.normalImpulse = contact.normal * contact.normalImpulseSum;
      contact.tangentImpulse = contact.tangentImpulseSum1 * contact.tangent1 + contact.tangentImpulseSum2 * contact.tangent2;

      // cache normal impulse sum
      contact.normalImpulseCache = contact.normalImpulseSum;
      contact.normalImpulseSum = 0.0f;

      // don't need a cache for warm start
      // can be derived from normal impulse cache
      contact.tangentImpulseSum1 = 0.0f;
      contact.tangentImpulseSum2 = 0.0f;
    } // end for each contact
  } // end for each manifold
}

void ContactSolver::SolvePositionConstraints(const TimeStep &timeStep)
{
  const float maxPositionCorrection = timeStep.params.maxPositionCorrection * timeStep.dt / timeStep.positionIterations;
  for (ContactManifold &manifold : mIsland.mManifolds)
  {
    RigidBody &bodyA = *manifold.colliderA->mParent;
    RigidBody &bodyB = *manifold.colliderB->mParent;
    const MassData &m1 = bodyA.mMassData;
    const MassData &m2 = bodyB.mMassData;

    for (unsigned char i = 0; i < manifold.numContacts; ++i)
    {
      Contact &contact = *manifold.contacts[i];

      const Math::Vector3 localToGlobalPositionA = bodyA.LocalToGlobal(contact.localPositionA);
      const Math::Vector3 localToGlobalPositionB = bodyB.LocalToGlobal(contact.localPositionB);
      const Math::Vector3 &n = contact.normal;
      const Math::Vector3 rA = localToGlobalPositionA - bodyA.GlobalCentroid();
      const Math::Vector3 rB = localToGlobalPositionB - bodyB.GlobalCentroid();

      // compute error to correct
      const float penetration = n.Dot(localToGlobalPositionA - localToGlobalPositionB);
      const float error =
        Clamp(timeStep.params.positionBaumgarte * (penetration - timeStep.params.positionBaumgarteSlop),
        0.0f, timeStep.params.maxPositionCorrection);

      // skip if error low enough
      if (error < timeStep.params.minPositionCorrection)
        continue;

      // compute correction
      const Math::Vector3 rn1 = n.Cross(rA);
      const Math::Vector3 rn2 = n.Cross(rB);
      Jacobian::J2V2W j;
      ComputeJacobian(n, rA, rB, j);
      const float impulse = error * ComputeEffectiveMass(bodyA, bodyB, j);
      const Math::Vector3 &correction = impulse * n;

      // apply correction
      if (bodyA.mType == RigidBody::DYNAMIC)
      {
        Math::Matrix3 orientationChange;
        bodyA.mGlobalCentroid -= correction * m1.inverseMass;
		if (bodyA.AllowRotation())
			bodyA.mOrientation.Prepend(Quat(bodyA.mGlobalInverseInertiaTensor * rA.Cross(correction)).ToMatrix(orientationChange));
		//else
			//bodyA.mOrientation.SetIdentity();
        bodyA.UpdateOrientation();
        bodyA.UpdatePositionFromGlobalCentroid();
        bodyA.UpdateGlobalInverseInertiaTensor();
      }
      if (bodyB.mType == RigidBody::DYNAMIC)
      {
        Math::Matrix3 orientationChange;
        bodyB.mGlobalCentroid += correction * m2.inverseMass;
		if (bodyB.AllowRotation())
			bodyB.mOrientation.Prepend(Quat(bodyB.mGlobalInverseInertiaTensor * rB.Cross(correction)).ToMatrix(orientationChange));
		//else
			//bodyB.mOrientation.SetIdentity();
        bodyB.UpdateOrientation();
        bodyB.UpdatePositionFromGlobalCentroid();
        bodyB.UpdateGlobalInverseInertiaTensor();
      } // end applying correction
    } // end for each contact
  } // end for each manifold
}