#include "stdafx.h"
#include "RigidBody.h"
#include "../MochaMath/Math/Quat.h"
#include "PhysicsScene.h"
#include "InertialTensor.h"
#include "MeshShape.h"

BodyContactEdge::BodyContactEdge(void)
: other(nullptr)
, twin(nullptr)
, counter(0)
{ }

const RigidBody::ContactEdgeList &RigidBody::ContactEdges(void) const
{
	return mContactEdges;
}

RigidBody::RigidBody(const RigidBodyDef &def)
: userData(def.userData)
, mType(def.type)
, mSleepTimer(0.0f)
, mAllowRotation(def.allowRotation)
, mHasGravity(def.hasGravity)
, mPosition(def.position)
, mLinearVelocity(def.linearVelocity)
, mOrientation(def.orientation)
, mInverseOrientation(mOrientation.Transposed())
, mAngularVelocity(def.angularVelocity)
, mForceAccumulator(0.0f, 0.0f, 0.0f)
, mTorqueAccumulator(0.0f, 0.0f, 0.0f)
, mLinearImpulseAccumulator(0.0f, 0.0f, 0.0f)
, mAngularImpulseAccumulator(0.0f, 0.0f, 0.0f)
, mColliderDirty(false)
, mGravityFactor(1.0f)
{
	SetAwake(true);
}

RigidBody::~RigidBody(void)
{
	// destroy colliders
	while (!mColliders.empty())
		DestroyCollider(*mColliders.begin());
}

Collider &RigidBody::CreateCollider(const ColliderDef &def)
{
	Collider *collider =
		Collider::Create(def, mWorld->mColliderAllocator, mWorld->mColliderGeometryAllocator, this);

	collider->mParent = this;
	mColliders.push_back(collider);
	collider->mGeometry->Init();
	collider->mGeometry->UpdateProxy(*this, collider->mProxy);

	InvalidateColliders();

	if (collider->mGeometry->mType != 4)
		mWorld->mBroadphase->Add(collider->mProxy);
	//if (collider->mGeometry->mType == 4)
	//{
	//	MeshColliderGeometry3D* m = static_cast<MeshColliderGeometry3D*>(collider->mGeometry);
	//	collider->mMassData = m->m_colliders[0]->mMassData;
	//}
		
	return *collider;
}

void RigidBody::DestroyCollider(Collider &collider)
{
	GV_ASSERT_MESSAGE(collider.mParent == this, "Invalid collider parent.");

	if (collider.mGeometry->mType != 4)
		mWorld->mBroadphase->Remove(collider.mProxy);
	mColliders.erase(&collider);

	Collider::Destroy(&collider, mWorld->mColliderAllocator, mWorld->mColliderGeometryAllocator);
}

void RigidBody::InvalidateColliders(void) { mColliderDirty = true; }
void RigidBody::ValidateColliders(void)
{
	UpdateGlobalCentroidFromPosition();
	//SetAwake(true); // wake body up due to dirty colliders (do we need this?)
	mColliderDirty = false;
}
bool RigidBody::CollidersValid(void) const { return !mColliderDirty; }

void RigidBody::SetAwake(bool awake)
{
	if (awake)
	{
		mFlags.Set(PhysicsFlags::AWAKE);
		mSleepTimer = 0.0f;
	}
	else
		mFlags.Clear(PhysicsFlags::AWAKE);
}

void RigidBody::SetPosition(const Math::Vector3 &position)
{
	mPosition = position;
	UpdateGlobalCentroidFromPosition();
	SetAwake(true);
}

void RigidBody::SetScale(const Math::Vector3 &scale)
{
	mScale = scale;
	if (mScale != Vector3::cZero)
	{
		mInvseScale.x = 1 / scale.x;
		mInvseScale.y = 1 / scale.y;
		mInvseScale.z = 1 / scale.z;
	}
	else
		mInvseScale = Vector3::cZero;
	SetAwake(true);
}

void RigidBody::SetOrientation(Math::Matrix3 orientation)
{
	mOrientation = orientation;
	UpdateOrientation();
	UpdateGlobalCentroidFromPosition();
	SetAwake(true);
}

void RigidBody::SetLinearVelocity(const Math::Vector3 &velocity)
{
	mLinearVelocity = velocity;
	if (mLinearVelocity.x == 0 &&
		mLinearVelocity.y == 0 && 
		mLinearVelocity.z == 0)
		SetAwake(false);
	else
		SetAwake(true);
}

void RigidBody::SetAngularVelocity(const Math::Vector3 &velocity)
{
	mAngularVelocity = velocity;
	if (mAngularVelocity.x == 0 &&
		mAngularVelocity.y == 0 &&
		mAngularVelocity.z == 0)
		SetAwake(false);
	else
		SetAwake(true);
}

bool RigidBody::AllowRotation(void) const
{
	return mAllowRotation;
}

void RigidBody::SetAllowRotation(bool allowRotation)
{
	mAllowRotation = allowRotation;
	SetAwake(true);
}

bool RigidBody::HasGravity(void) const
{
	return mHasGravity;
}

void RigidBody::SetHasGravity(bool hasGravity)
{
	mHasGravity = hasGravity;
	if (hasGravity)
		SetAwake(true);
}

void RigidBody::ApplyForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	mForceAccumulator += force;
	mTorqueAccumulator += (at - mGlobalCentroid).Cross(force);
	SetAwake(true);
}

void RigidBody::ApplyLinearForce(const Math::Vector3 &force)
{
	mForceAccumulator += force;
	SetAwake(true);
}

void RigidBody::ApplyAngularForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	mTorqueAccumulator += (at - mGlobalCentroid).Cross(force);
	SetAwake(true);
}

void RigidBody::ApplyMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	ApplyForce(force * mMassData.mass, at);
}

void RigidBody::ApplyLinearMasslessForce(const Math::Vector3 &force)
{
	ApplyLinearForce(force * mMassData.mass);
}

void RigidBody::ApplyAngularMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	ApplyAngularForce(force * mMassData.mass, at);
}

void RigidBody::ApplyImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at)
{
	mLinearImpulseAccumulator += impulse;
	mAngularImpulseAccumulator += (at - mGlobalCentroid).Cross(impulse);
	SetAwake(true);
}

void RigidBody::ApplyLinearImpulse(const Math::Vector3 &impulse)
{
	mLinearImpulseAccumulator += impulse;
	SetAwake(true);
}

void RigidBody::ApplyAngularImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at)
{
	mAngularImpulseAccumulator += (at - mGlobalCentroid).Cross(impulse);
	SetAwake(true);
}

void RigidBody::ApplyImmediateImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at)
{
	mLinearVelocity += mMassData.inverseMass * impulse;
	mAngularVelocity += mGlobalInverseInertiaTensor * (at - mGlobalCentroid).Cross(impulse);
}

void RigidBody::IntegratePosition(const TimeStep &timeStep)
{
	switch (mType)
	{
	case DYNAMIC:
	{
					mGlobalCentroid += mLinearVelocity * timeStep.dt;

					if (mAllowRotation)
					{

						float lenSQ = mAngularVelocity.LengthSq();
						float len = mAngularVelocity.Length();
						// TODO: directly build orientation change matrix from arbitrary axis rotation
						if (lenSQ > EPSILON)
						{
							Math::Matrix3 orientationChange;
							bool ResetLength = false;
							if (mParent->cphy->mLockOnX)
							{
								mAngularVelocity.x = 0;
								ResetLength = true;
							}
							if (mParent->cphy->mLockOnY)
							{
								mAngularVelocity.y = 0;
								ResetLength = true;
							}
							if (mParent->cphy->mLockOnZ)
							{
								mAngularVelocity.z = 0;
								ResetLength = true;
							}
							if (ResetLength)
								mAngularVelocity = mAngularVelocity.Normalized() * len;
							Quat(len * timeStep.dt, mAngularVelocity.Normalized()).ToMatrix(orientationChange);
							mOrientation.Prepend(orientationChange);
						}

						mOrientation.Orthonormalize();
					}
					else
						mAngularVelocity.ZeroOut();
	
					UpdatePositionFromGlobalCentroid();
					UpdateOrientation();
	
					break;
	}
	case STATIC:
	{
				   mLinearVelocity.ZeroOut();
				   mAngularVelocity.ZeroOut();
				   break;
	}
	}
}

void RigidBody::UpdateOrientation(void)
{
	mOrientation.Orthonormalize();
	mInverseOrientation = mOrientation.Transposed();
}

void RigidBody::UpdateProxies(void)
{
	for (auto &collider : mColliders)
	{
		// TODOKai
		//Vector3 size = collider.UpdateSize(mParent->cphy->gameObject->transform->scale);
		collider.UpdateProxy();

		//mParent->cphy->collidarCom->UpdateSize(size);
	}
		
}

void RigidBody::UpdateGlobalCentroidFromPosition(void)
{

	mGlobalCentroid = mOrientation * (mScale * mLocalCentroid) + mPosition;
	//mGlobalCentroid = mOrientation *  mLocalCentroid+ mPosition;
}

void RigidBody::UpdatePositionFromGlobalCentroid(void)
{
	//mPosition = mOrientation * (-mLocalCentroid) + mGlobalCentroid;
	mPosition = mOrientation * (mScale * -mLocalCentroid) + mGlobalCentroid;
}

void RigidBody::Draw(PhysicsDebugDraw &debugDraw) const
{
	// determine debug draw colors
	unsigned lineColor;
	unsigned fillColor = 0x000000; // TODO
	float fillAlpha = 0.0f;      // TODO
	if (IsAwake())
	{
		switch (mType)
		{
		case RigidBody::DYNAMIC:
			lineColor = PhysicsDebugDraw::k_dynamicRigidBodyLineColor;
			break;
		case RigidBody::KINEMATIC:
			lineColor = PhysicsDebugDraw::k_kinematicRigidBodyLineColor;
			break;
		case RigidBody::STATIC:
			lineColor = PhysicsDebugDraw::k_staticRigidBodyLineColor;
			break;
		}
	}
	else
		lineColor = PhysicsDebugDraw::k_sleepingRigidBodyLineColor;
	
	// draw colliders
	for (const auto &collider : mColliders)
	{
		unsigned finalLineColor = lineColor;
	
		// override with sensor line color
		if (IsAwake() && collider.mIsSensor)
			finalLineColor = PhysicsDebugDraw::k_sensorLineColor;
	
		collider.Draw(debugDraw, finalLineColor, fillColor, fillAlpha);
	}
	
	// draw centroid
	debugDraw.DrawSphere(mGlobalCentroid, PhysicsDebugDraw::k_centroidSize, mOrientation, PhysicsDebugDraw::k_centroidLineColor);
	
	// draw X & Y & Z axes
	const Math::Vector3 xAxis(PhysicsDebugDraw::k_axisSize, 0.0f, 0.0f);
	const Math::Vector3 yAxis(0.0f, PhysicsDebugDraw::k_axisSize, 0.0f);
	const Math::Vector3 zAxis(0.0f, 0.0f, PhysicsDebugDraw::k_axisSize);
	debugDraw.DrawLine(mPosition, mPosition + mOrientation * xAxis, PhysicsDebugDraw::k_xAxisLineColor);
	debugDraw.DrawLine(mPosition, mPosition + mOrientation * yAxis, PhysicsDebugDraw::k_yAxisLineColor);
	debugDraw.DrawLine(mPosition, mPosition + mOrientation * zAxis, PhysicsDebugDraw::k_zAxisLineColor);
}

void RigidBody::UpdateMassAndLocalCentroid(void)
{
	// compute local centroid & mass
	mLocalCentroid.ZeroOut();
	mMassData.mass = 0.0f;
	for (auto &collider : mColliders)
	{
		// skip sensors
		if (collider.mIsSensor)
			continue;
	
		// accumulate mass
		if (mParent->cphy->mMass == 0)
		{
			mMassData.mass += collider.mMassData.mass;

			// accumulate weighted local centroid
			mLocalCentroid += collider.mMassData.mass * collider.mLocalCentroid;
		}
		else
		{
			mMassData.mass = collider.mMassData.mass;

			// accumulate weighted local centroid
			mLocalCentroid = collider.mMassData.mass * collider.mLocalCentroid;
		}

	}
	
	// compute inverse mass
	if (mMassData.mass)
		mMassData.inverseMass = 1.0f / mMassData.mass;
	else // pure sensor or no colliders, not affected by impulses
		mMassData.inverseMass = 0.0f;
	
	// average out local centroid
	mLocalCentroid *= mMassData.inverseMass;
	
	// compute local inertia tensor
	switch (mType)
	{
	case DYNAMIC:
		mMassData.unitLocalInertiaTensor.ZeroOut();
		for (auto &collider : mColliders)
		{
			// accumulate local inertia tensor
			//   - Parallel Axis Theorem: 
			//     http://en.wikipedia.org/wiki/Parallel_axis_theorem
			// TODO: take collider orientation into account?
			mMassData.unitLocalInertiaTensor +=
				ShiftInertiaTensorReferencePoint(collider.mMassData.unitLocalInertiaTensor * collider.mMassData.mass,
				mLocalCentroid - collider.mLocalCentroid,
				collider.mMassData.mass);
		}
		// scale back to unit inertia tensor
		mMassData.unitLocalInertiaTensor *= mMassData.inverseMass;
		break;
	case STATIC:
	case KINEMATIC:
		mMassData.mass = std::numeric_limits<float>::infinity();
		mMassData.inverseMass = 0.0f;
		mMassData.unitLocalInertiaTensor.One() *= std::numeric_limits<float>::infinity();
		break;
	}
	
	// pure sensor or no colliders, not affected by impulses
	if (!mMassData.mass)
		mMassData.unitLocalInertiaTensor.One() *= std::numeric_limits<float>::infinity();
}

void RigidBody::UpdateGlobalInverseInertiaTensor(void)
{
	// pure sensor or no colliders, not affected by impulses
	if (!mMassData.mass)
	{
		mGlobalInverseInertiaTensor.ZeroOut();
		return;
	}
	
	switch (mType)
	{
	case DYNAMIC:
		mGlobalInverseInertiaTensor = mOrientation * mMassData.unitLocalInertiaTensor * mInverseOrientation;
		mGlobalInverseInertiaTensor *= mMassData.mass; // scale to regular inertia tensor
		mGlobalInverseInertiaTensor.Invert();
		break;
	case STATIC:
	case KINEMATIC:
		mGlobalInverseInertiaTensor.ZeroOut();
		break;
	}
}

const Math::Vector3 RigidBody::LocalToGlobal(const Math::Vector3 &vec) const
{
	 return mPosition + mOrientation * (vec * mScale); 
}

