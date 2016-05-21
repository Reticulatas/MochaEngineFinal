/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include "InList.h"
#include <algorithm>
#include <cstring>
#include "Jacobian.h"
#include "Memory.h"
#include "ContactLisener.h"
#include "PhysicsFlag.h"

class Collider;
class PhysicsDebugDraw;
class PhysicsWorld;
struct BodyContactEdge;
struct Contact;
struct ContactManifold;
class ContactSolver;
class ContactManager;
class Island;

typedef unsigned char ContactIndex3D;

// between colliders
struct ContactEdge

{
	ContactManifold *manifold;
	Collider *other;

	// links to other contact edges of the same collider
	IN_LIST_LINK(ContactEdge) siblingLink;

	ContactEdge(void)
		: manifold(nullptr)
		, other(nullptr)
	{ }
};

struct Contact
{
	friend class PhysicsWorld;
	friend class ContactManager;
	friend class ContactSolver;
	friend struct ContactManifold;
public:

	// contact point data
	bool persistent; // for warm starting
	Math::Vector3 globalPositionA; // in global space
	Math::Vector3 globalPositionB;
	Math::Vector3 localPositionA;
	Math::Vector3 localPositionB;
	Math::Vector3 normal; // always points from colliderA to colliderB
	Math::Vector3 tangent1, tangent2;
	float depth;

	// before resolution
	Math::Vector3 normalRelativeVelocity;
	Math::Vector3 tangentRelativeVelocity;

	// resolution result
	Math::Vector3 normalImpulse;
	Math::Vector3 tangentImpulse;

	Contact(void)
		: persistent(false)
		, normalImpulseSum(0.0f)
		, normalVelocityBias(0.0f)
		, normalImpulseCache(0.0f)
		, tangentImpulseSum1(0.0f)
		, tangentImpulseSum2(0.0f)
	{ }

private:

	float effectiveMassN;
	float effectiveMassT1;
	float effectiveMassT2;

	// normal
	Jacobian::J2V2W jn;
	float normalImpulseSum;
	float normalVelocityBias;
	float normalImpulseCache; // for warm starting

	// tangent
	Jacobian::J2V2W jt1, jt2;
	float tangentImpulseSum1, tangentImpulseSum2;

	__forceinline void FlipNormalAndPosition(void)
	{
		normal.Negate();
		std::swap(globalPositionA, globalPositionB);
		std::swap(localPositionA, localPositionB);
	}
};

struct ContactManifold
{
	friend class PhysicsWorld;
	friend struct BodyContactEdge;
	friend class ContactManager;
	friend class ContactSolver;
	friend class Island;
public:
	static const unsigned char k_maxContacts = 4;

	float restitution;
	float friction;

	ContactEdge edgeA;   // set by ContactManager3D
	ContactEdge edgeB;   // set by ContactManager3D
	Collider *colliderA; // set by PhysicsWorld3D
	Collider *colliderB; // set by PhysicsWorld3D

	bool invalidateExistingContacts;
	Contact *contacts[k_maxContacts]; // set by collision detector
	unsigned char numContacts;                  // set by collision detector
	bool isColliding;
	bool wasColliding;

	ContactManifold(void);

	__forceinline bool SameColliders(const ContactManifold &rhs) const
	{
		// collider1 is always less than collider2 (memory address), so no need to check the reverse
		return colliderA == rhs.colliderA && colliderB == rhs.colliderB;
	}

	__forceinline void FlipNormalsAndPosition(void)
	{
		for (unsigned char i = 0; i < numContacts; ++i)
			contacts[i]->FlipNormalAndPosition();
	}

private:

	// links to other contact manifolds in the same contact manager
	IN_LIST_LINK(ContactManifold) managerLink;

	// links to other contact manifolds in the same island
	IN_LIST_LINK(ContactManifold) islandLink;

	PhysicsFlags flags;
};

class ContactManager
{
	friend class PhysicsWorld;
private:

	ContactManager(FixedSizeAllocator &manifoldAllocator, FixedSizeAllocator &contactAllocator, FixedSizeAllocator &bodyContactEdgeAllocator)
		: mManifoldAllocator(manifoldAllocator)
		, mContactAllocator(contactAllocator)
		, mBodyContactEdgeAllocator(bodyContactEdgeAllocator)
		, mListener(nullptr)
	{ }

	bool Add(ContactManifold &manifold, float persistenceThresholdSQ);
	void Remove(ContactManifold &manifold);
	void Merge(ContactManifold &src, ContactManifold &dest, float persistenceThresholdSQ);
	void PreNarrowphase(void);
	void PostNarrowphase(void);
	void Draw();
	void DrawAABB(const Vector3& maxPoint, const Vector3& minPoint);
	FixedSizeAllocator &mManifoldAllocator;
	FixedSizeAllocator &mContactAllocator;
	FixedSizeAllocator &mBodyContactEdgeAllocator;
	unique_ptr_phy<ContactListener> mListener;
	IN_LIST(ContactManifold, managerLink) mManifolds;
};
