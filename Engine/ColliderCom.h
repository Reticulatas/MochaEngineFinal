/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "PhysicsMaterial.h"
#include "Collidar.h"
#include "RigidBodyCom.h"
#include "CPhysics.h"
class ColliderComponent : public Component
{
	friend class PhysicsSystem3D;
protected:
	virtual Collider *CreateCollider(RigidBody &body) { return nullptr; }
	void InvalidateCollider(void);
public:
	// ContactInfo Struct
	struct ContactRecord
	{
		friend class ColliderComponent;
	public:
		ContactRecord(void) : m_other(nullptr) { }
		ColliderComponent &Other(void) const { return *m_other; }

		struct Contact
		{
			Math::Vector3 position;
			Math::Vector3 normal;
			float penetration;
			Math::Vector3 normalRelativeVelocity;
			Math::Vector3 tangentRelativeVelocity;
			Math::Vector3 normalImpulse;
			Math::Vector3 tangentImpulse;
		} contacts[ContactManifold::k_maxContacts];

		bool wasColliding;
		bool isColliding;
		unsigned char numContacts;

	private:
		ColliderComponent *m_other;
	};
	typedef std::vector<ContactRecord> ContactRecordList;

	// methods
	virtual void OnStart(void);
	virtual void OnUpdate(void);
	virtual void OnFree(void);
	virtual void OnInit(void);

	void Update(void);
	virtual const Vector3& UpdateSize(const Vector3 &size) = 0;
	ColliderComponent(void);
	~ColliderComponent(void);

	// Getters
	bool IsSensor(void) const;
	const ResolutionMaterial &GetMaterial(void) const;
	const Collider &GetCollider(void) const { return *mCollider; }
	const ContactRecordList &ContactData(void) const { return mContactRecords; }

	// Setters
	ColliderComponent &SetSensor(bool isSensor);
	ColliderComponent &SetMaterial(const ResolutionMaterial &material);
	void SetMass(float mass) { mMass = mass; if (mCollider) mCollider->SetMass(mass); }
	void SetFilter(const ColliderFilter &filter) { mFilter = filter; if (mCollider) mCollider->SetFilter(filter); }
	

public:
	___BEGIN_REGISTERED___

	___END_REGISTERED___

	IN_LIST_LINK(ColliderComponent) mSystemLink;
	float mRestitution;
	float mFriction;
	float mMass;
	CPhysics* mCphy;
	Collider *mCollider;
	bool mValidCollider;
	ResolutionMaterial mMaterial;
	ColliderFilter mFilter;
	ContactRecordList mContactRecords;
	bool mIsSensor;
	// for editor

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(mMass, 1)
		___DEFSER(mRestitution, 2)
		___DEFSER(mFriction, 2)
		___DEFSER(mMaterial, 3)
	___SERIALIZE_END___
	virtual void CopyFrom(const ::ColliderComponent* c)
	{
		mMass = c->mMass;
		mRestitution = c->mRestitution;
		mFriction = c->mFriction;
		mMaterial = c->mMaterial;
		mMass = c->mMass;
	}
	metabasedef(ColliderComponent)
		endmetabasedef;
};
___SERIALIZE_CLASS(ColliderComponent, 3);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ColliderComponent);
