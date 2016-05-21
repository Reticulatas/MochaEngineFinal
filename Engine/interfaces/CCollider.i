
%{
#include "ColliderCom.h"
#include "BoxColliderComponent.h"
#include "MeshCollider.h"
%}


class ColliderComponent : public Component
{
	ColliderComponent(void);
	~ColliderComponent(void);
public:

	// component end
	//const ResolutionMaterial &GetMaterial(void) const;
	//ColliderComponent &SetMaterial(const ResolutionMaterial &material);

	const Collider &GetCollider(void) const;

	struct ContactRecord
	{
	public:
		unsigned char numContacts;
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

		
		ColliderComponent &Other(void) const;
	private:
		ContactRecord(void);
		~ContactRecord(void);
	};


	void SetMass(float mass);

public:
	
	Collider *mCollider;

	// param copies


};

class CBoxColliderComponent
	: public ColliderComponent
{
public:

	const Math::Vector3 &GetCenter(void) const;
	CBoxColliderComponent &SetCenter(const Math::Vector3 &c);

	const Math::Vector3 &GetHalfExtents(void) const;
	CBoxColliderComponent &SetHalfExtents(const Math::Vector3 &halfExtents);


private:
	CBoxColliderComponent(void);
	~CBoxColliderComponent(void);

};

class CMeshColliderComponent
	: public ColliderComponent
{
private:
	CMeshColliderComponent(void);
	~CMeshColliderComponent(void);

};

class CSphereColliderComponent
	: public ColliderComponent
{
	float radius;
};