/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include "List_Allocator.h"
#include "InList.h"
#include "Mass.h"
#include "PhysicsMaterial.h"
#include "CollisionFilter.h"
#include "Ray3.h"
#include "Proxy.h"
#include "ContactManifold.h"
#include "AABB.h"
struct ColliderType
{
	typedef unsigned char value_type;
	static const value_type SPHERE = 0;
	static const value_type QUAD = 1;
	static const value_type MESH = 4;
	static const value_type BOX = 3;
	static const value_type POLYHEDRON = 2;
	static const value_type DEFAULT = 255;
};

struct ColliderDef
{
	friend class PhysicsWorld;
	friend class RigidBody;
public:

	ColliderDef(ColliderType::value_type type);


	ColliderDef(void)
		: mType(ColliderType::DEFAULT)
		, mUserData(nullptr)
		, mIsSensor(false)
	{ }

	bool mIsSensor;
	ResolutionMaterial mMaterial;
	ColliderFilter filter;
	void *mUserData;
	const ColliderType::value_type mType;

};

class ColliderGeometry
{
public:

	ColliderGeometry(unsigned type)
		: mParent(nullptr)
		, mType(type)
	{ }

	virtual ~ColliderGeometry(void) { }
	virtual void Init(void) { }
	virtual void Dispose(void) { }

	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const = 0;
	virtual void UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid) = 0;
	virtual void UpdateProxy(const RigidBody &body, Proxy &proxy) = 0;
	virtual const Vector3& UpdateSize(const Vector3 &scale) = 0;
	virtual bool RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const = 0;
	virtual void Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const = 0;
	virtual bool Collides(const Math::Vector3 &pos) const = 0;

	Collider *mParent;
	const ColliderType::value_type mType;
};

class Collider

{
	friend class ContactManager;
	friend class ContactSolver;
	friend class RigidBody;
	friend class PhysicsWorld;

public:
	// contact data interface
	typedef IN_LIST(ContactEdge, siblingLink) ContactEdgeList;
	const ContactEdgeList &ContactEdges(void) const;
	bool RayCastCollider(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const;
	bool CollidesCollider(const Math::Vector3 &pos) const;
	const Math::Vector3 &LocalCentroid(void) { return mLocalCentroid; }
	RigidBody &Parent(void) { return *mParent; }
	const Proxy &GetProxy(void) const { return mProxy; }
	const ColliderFilter &Filter(void) const { return mFilter; }
	void SetFilter(const ColliderFilter &filter) { mFilter = filter; }
	bool CanBeCollide(const Collider &rhs) { return mFilter.CanCollide(rhs.mFilter); }
	float GetMass(void) const { return mMassData.mass; }
	void SetMass(float mass);
	const ResolutionMaterial &Material(void) const { return mMaterial; }
	void SetMaterial(const ResolutionMaterial &material) { mMaterial = material; }
	bool IsSensor(void) const { return mIsSensor; }
	void SetSensor(bool isSensor) { mIsSensor = isSensor; }
	static Collider *Create(const ColliderDef &def, FixedSizeAllocator &colliderAllocator, Allocator &geometryAllocator, RigidBody* rigidBody);
	static void Destroy(Collider *collider, FixedSizeAllocator &colliderAllocator, Allocator &geometryAllocator);

	void *mUserData;
	ColliderGeometry *mGeometry;
private:

	Collider(const ColliderDef &def, ColliderGeometry *geom, RigidBody* rigidBody);
	~Collider(void);

	void UpdateProxy(void); // depends on body's transform
	const Vector3 UpdateSize(const Vector3 &scale);
	void Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const;

	// physics data
	ColliderFilter mFilter;
	ResolutionMaterial mMaterial;
	bool mIsSensor;
	RigidBody *mParent;
	MassData mMassData;
	Math::Vector3 mLocalCentroid; // relative to rigid body position in local space
	Proxy mProxy;
	IN_LIST(ContactEdge, siblingLink) mContactEdges;
	IN_LIST_LINK(Collider) mSiblingLink;
};


