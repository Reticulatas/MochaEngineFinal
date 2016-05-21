#include "stdafx.h"
#include "ColliderCom.h"
#include "PhysicsEngine.h"
#include "EditorSubsystem.h"
#include "EditorSubsystem_Interface.h"
ColliderComponent::ColliderComponent(void)
: mCphy(nullptr)
, mCollider(nullptr)
, mValidCollider(false)
, mIsSensor(false)
, mMass(-1.0f)
, mFriction(1.0f)
, mRestitution(0.0f)
{}

void ColliderComponent::OnInit(void)
{}

void ColliderComponent::OnStart(void)
{
	if (EditorSS::IsTesting() || !Engine::getInstance()->GetIsEditorMode())
	{
		PhysicEngine* system = PhysicEngine::getInstance();
		system->Register(*this);
	}
}
void ColliderComponent::OnUpdate(void)
{}

void ColliderComponent::OnFree(void)
{}

ColliderComponent::~ColliderComponent(void)
{
	if (gameObject->HasComponent<CPhysics>())
	{
		if (mCphy->rigidCom)
		{
			auto body = mCphy->rigidCom->mBody;
			body->DestroyCollider(*mCollider);
		}
		mCphy->collidarCom = NULL;
	}

	PhysicEngine* system = PhysicEngine::getInstance();
	system->Unregister(*this);
}

void ColliderComponent::Update(void)
{
	mContactRecords.resize(0);
	mMaterial.mFriction = mCphy->mFriction;
	mMaterial.mRestitution = mCphy->mRestitution;
	SetMaterial(mMaterial);
	//mMass = mCphy->mMass;
	//if (mMass > 0.0f)
	//	mCollider->SetMass(mMass);
	if (!mValidCollider)
	{
		auto &body = mCphy->rigidCom->mBody;

		if (mCollider)
			body->DestroyCollider(*mCollider);

		mCollider = CreateCollider(*body);
		mCollider->SetMaterial(mMaterial);
		mCollider->mUserData = this;
		mCollider->SetSensor(mIsSensor);
		mCollider->SetFilter(mFilter);
		mMass = mCollider->GetMass();
		mValidCollider = true;
	}

	if (!mCollider)
		return;

	mCollider->SetMaterial(mMaterial);
	const auto &contactEdges = mCollider->ContactEdges();
	mContactRecords.reserve(contactEdges.size());
	for (auto &edge : contactEdges)
	{
		const ContactManifold &manifold = *edge.manifold;
		ContactRecord record;
		record.wasColliding = manifold.wasColliding;
		record.isColliding = manifold.isColliding;

		record.m_other = static_cast<ColliderComponent *>(edge.other->mUserData);
		record.numContacts = manifold.numContacts;
		for (unsigned char i = 0; i < manifold.numContacts; ++i)
		{
			ContactRecord::Contact &c = record.contacts[i];
			const Contact &contact = *manifold.contacts[i];
			c.position = (mCollider == manifold.colliderA) ? contact.globalPositionB : contact.globalPositionA;
			c.normal = (mCollider == manifold.colliderA) ? -contact.normal : contact.normal;
			c.penetration = contact.depth;
			c.normalRelativeVelocity = contact.normalRelativeVelocity;
			c.tangentRelativeVelocity = contact.tangentRelativeVelocity;
			c.normalImpulse = contact.normalImpulse;
			c.tangentImpulse = contact.tangentImpulse;
		}
		mContactRecords.push_back(record);
	}
}

// Getters
const ResolutionMaterial &ColliderComponent::GetMaterial(void) const
{
	return mCollider ? mCollider->Material() : mMaterial;
}

bool ColliderComponent::IsSensor(void) const
{
	return mCollider ? mCollider->IsSensor() : mIsSensor;
}

// Setters
void ColliderComponent::InvalidateCollider(void)
{
	mValidCollider = false;
}

ColliderComponent &ColliderComponent::SetSensor(bool isSensor)
{
	mIsSensor = isSensor;
	if (mCollider)
		mCollider->SetSensor(isSensor);
	return *this;
}

ColliderComponent &ColliderComponent::SetMaterial(const ResolutionMaterial &material)
{
	mMaterial = material;
	if (mCollider)
		mCollider->SetMaterial(material);
	return *this;
}
meta_define(ColliderComponent);
