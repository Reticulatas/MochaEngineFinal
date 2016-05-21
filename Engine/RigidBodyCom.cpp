
#include "stdafx.h"
#include "RigidBodyCom.h"
#include "PhysicsEngine.h"
#include "CTransform.h"
const std::string RigidBodyComponent::mID = "RigidBodyComponent3D";

RigidBodyComponent::RigidBodyComponent(void)
: mBody(nullptr)
, mType(RigidBody::STATIC)
, mLinearVelocity(0.0f, 0.0f, 0.0f)
, mAngularVelocity(0.0f, 0.0f, 0.0f)
, mAllowRotation(true)
, mHasGravity(true)
, mIsAwake(true)
{
	PhysicEngine *system = PhysicEngine::getInstance();
	auto &world = system->mWorld;
	mFirstFrame = true;

	mBody = &world->CreateBody(mType);
	mBody->userData = this;
	mBody->SetLinearVelocity(Math::Vector3(mLinearVelocity.x, mLinearVelocity.y, mLinearVelocity.z));
	mBody->SetAngularVelocity(Math::Vector3(mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z));
	mBody->SetAllowRotation(mAllowRotation);
	mBody->SetHasGravity(mHasGravity);

	system->Register(*this);
}

void RigidBodyComponent::Dispose(void)
{
	PhysicEngine *system = PhysicEngine::getInstance();
	auto &world = system->mWorld;
	world->DestroyBody(*mBody);
	mBody = nullptr;
	cphy = NULL;
	system->Unregister(*this);
}

void RigidBodyComponent::Update(void)
{
	if (cphy->gameObject->HasComponent<CTransform>())
	{
		CTransform* transform = cphy->gameObject->transform;
		if (mFirstFrame)
		{
			mBody->SetPosition(transform->position());
			Math::Matrix3 orientation;
			orientation = transform->GetRotationMatrix3();
			mBody->SetOrientation(orientation);
			mBody->SetScale(transform->scale);
			mBody->SetAwake(mIsAwake);
			mFirstFrame = false;
		}
	
		// Set Position back to Transform
		auto &pos = mBody->Position();
		transform->SetPosition(pos, CTransform::GLOBAL);

		// Set Rotation back to transform
		auto &orientation = mBody->Orientation();
		transform->SetRotation(ToQuaternion(orientation));
		transform->scale = mBody->mScale;
	}

	const auto &contactEdges = mBody->ContactEdges();
	mContactRecords.resize(0);
	mContactRecords.reserve(contactEdges.size());
	for (auto &edge : contactEdges)
	{
		mContactRecords.emplace_back(
			ContactRecord(static_cast<RigidBodyComponent *>(edge.other->userData)));
	}
}

// Setters
RigidBodyComponent &RigidBodyComponent::SetAngularVelocity(const Math::Vector3 &vel)
{
	mAngularVelocity = vel;
	if (mBody)
		mBody->SetAngularVelocity(Math::Vector3(vel.x, vel.y, vel.z));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetAllowRotation(bool allowRotation)
{
	mAllowRotation = allowRotation;
	if (mBody)
		mBody->SetAllowRotation(allowRotation);
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetHasGravity(bool hasGravity)
{
	mHasGravity = hasGravity;
	if (mBody)
		mBody->SetHasGravity(hasGravity);
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetAwake(bool awake)
{
	mIsAwake = awake;
	if (mBody)
		mBody->SetAwake(awake);
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetPosition(const Math::Vector3 &pos)
{
	mBody->SetPosition(pos);
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetType(int type)
{
	mType = type;
	if (mBody)
		mBody->SetType(static_cast<unsigned char>(type));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetLinearVelocity(const Math::Vector3 &vel)
{
	mLinearVelocity = vel;
	if (mBody)
		mBody->SetLinearVelocity(Math::Vector3(vel.x, vel.y, vel.z));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::SetOrientation(const Math::Matrix3 &orientation)
{
	mBody->SetOrientation(orientation);
	return *this;
}
// Getters
const Math::Matrix3 &RigidBodyComponent::GetOrientation(void) const
{
	return mBody->Orientation();
}
bool RigidBodyComponent::GetHasGravity(void) const
{
	return mBody ? mBody->HasGravity() : mHasGravity;
}

const Math::Vector3 RigidBodyComponent::GetAngularVelocity(void) const
{
	if (mBody)
		return mBody->AngularVelocity();
	else
		return mAngularVelocity;
}
bool RigidBodyComponent::GetAllowRotation(void) const
{
	return mBody ? mBody->AllowRotation() : mAllowRotation;
}
int RigidBodyComponent::GetType(void) const
{
	return mBody ? static_cast<int>(mBody->Type()) : mType;
}
bool RigidBodyComponent::GetAwake(void) const
{
	return mBody ? mBody->IsAwake() : mIsAwake;
}
const Math::Vector3 RigidBodyComponent::GetLinearVelocity(void) const
{
	if (mBody)
		return mBody->LinearVelocity();
	else
		return mLinearVelocity;
}
const Math::Vector3 &RigidBodyComponent::GetCentroid(void) const
{
	return mBody->GlobalCentroid();
}

const Math::Vector3 &RigidBodyComponent::GetPosition(void) const
{
	return mBody->Position();
}

RigidBodyComponent &RigidBodyComponent::ApplyForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyForce(Math::Vector3(force.x, force.y, force.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}

RigidBodyComponent &RigidBodyComponent::ApplyLinearForce(const Math::Vector3 &force)
{
	if (mBody)
		mBody->ApplyLinearForce(Math::Vector3(force.x, force.y, force.z));
	return *this;
}

RigidBodyComponent &RigidBodyComponent::ApplyAngularForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyAngularForce(Math::Vector3(force.x, force.y, force.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}

// Helpers

RigidBodyComponent &RigidBodyComponent::ApplyMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyMasslessForce(Math::Vector3(force.x, force.y, force.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::ApplyAngularMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyAngularMasslessForce(Math::Vector3(force.x, force.y, force.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::ApplyImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyImpulse(Math::Vector3(impulse.x, impulse.y, impulse.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::ApplyLinearMasslessForce(const Math::Vector3 &force)
{
	if (mBody)
		mBody->ApplyLinearMasslessForce(Math::Vector3(force.x, force.y, force.z));
	return *this;
}
void RigidBodyComponent::GrabPosFromTransform(void)
{
	float sleepTimer = mBody->mSleepTimer;
	bool a = GetAwake();

	CTransform* transform = cphy->gameObject->transform;
	SetPosition(transform->GetPosition());
	SetOrientation(transform->GetRotationMatrix3());
	mBody->SetScale(transform->scale);

	SetAwake(a);
	mBody->mSleepTimer = sleepTimer;
}

void RigidBodyComponent::SyncCphysics(void)
{
	bool a = GetAwake();
	float sleepTimer = mBody->mSleepTimer;

	SetHasGravity(cphy->mIsGravityOn);
	SetGravityFactor(cphy->mGravityFactor);
	SetAllowRotation(cphy->mIsRotation);
	SetType(cphy->mColliderType);

	SetAwake(a);
	mBody->mSleepTimer = sleepTimer;
}

RigidBodyComponent &RigidBodyComponent::ApplyLinearImpulse(const Math::Vector3 &impulse)
{
	if (mBody)
		mBody->ApplyLinearImpulse(Math::Vector3(impulse.x, impulse.y, impulse.z));
	return *this;
}

Math::Vector3 RigidBodyComponent::Forward(void)
{
	Math::Matrix3 mat = mBody->Orientation();
	return Math::Vector3(-mat.m02, -mat.m12, -mat.m22).Normalized();
}
Math::Vector3 RigidBodyComponent::Right(void)
{
	Math::Matrix3 mat = mBody->Orientation();
	return Math::Vector3(mat.m00, mat.m10, mat.m20).Normalized();
}
Math::Vector3 RigidBodyComponent::Up(void)
{
	Math::Matrix3 mat = mBody->Orientation();
	return Math::Vector3(mat.m01, mat.m11, mat.m21).Normalized();
}
RigidBodyComponent &RigidBodyComponent::ApplyAngularImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyAngularImpulse(Math::Vector3(impulse.x, impulse.y, impulse.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}
RigidBodyComponent &RigidBodyComponent::ApplyImmediateImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at)
{
	if (mBody)
		mBody->ApplyImmediateImpulse(Math::Vector3(impulse.x, impulse.y, impulse.z), Math::Vector3(at.x, at.y, at.z));
	return *this;
}

RigidBodyComponent & RigidBodyComponent::SetGravityFactor(const Vector3& g)
{
	mBody->mGravityFactor = g;
	return *this;
}
