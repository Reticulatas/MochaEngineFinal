#include "stdafx.h"
#include "ContactManifold.h"
#include "RigidBody.h"
#include "MathIncludes.h"
#include "SupportFunction.h"
#include <algorithm>
#include <limits>
ContactManifold::ContactManifold(void)
: colliderA(nullptr)
, colliderB(nullptr)
, invalidateExistingContacts(false)
, numContacts(0)
, isColliding(false)
, wasColliding(false)
{
	for (unsigned char i = 0; i < k_maxContacts; ++i)
		contacts[i] = nullptr;
}

bool ContactManager::Add(ContactManifold &manifold, float persistenceThresholdSQ)
{
	GV_ASSERT(manifold.isColliding);

	Collider &colliderA = *manifold.colliderA;
	Collider &colliderB = *manifold.colliderB;
	RigidBody &bodyA = *colliderA.mParent;
	RigidBody &bodyB = *colliderB.mParent;

	// check if the contact manifold already exists
	{
		const auto &shorterEdgeList =
			colliderA.mContactEdges.size() < colliderB.mContactEdges.size() ? colliderA.mContactEdges : colliderB.mContactEdges;

		for (const auto &edge : shorterEdgeList)
		{
			ContactManifold &existingManifold = *edge.manifold;

			// check existing manifold
			if (manifold.SameColliders(existingManifold))
			{
				existingManifold.isColliding = true;
				Merge(manifold, existingManifold, persistenceThresholdSQ);
				return false;
			}
		}
	}

	// check if body contact edge already exists
	{
		const bool aIsShorter = bodyA.mContactEdges.size() < bodyB.mContactEdges.size();
		auto &shorterEdgeList = aIsShorter ? bodyA.mContactEdges : bodyB.mContactEdges;
		auto &body1 = aIsShorter ? bodyA : bodyB;
		auto &body2 = aIsShorter ? bodyB : bodyA;

		bool found = false;
		for (auto &edge : shorterEdgeList)
		{
			if (edge.other == &body2)
			{
				++edge.counter;
				++edge.twin->counter;
				GV_ASSERT_MESSAGE(edge.counter == edge.twin->counter, "Something's wrong with body contact edge counter.");
				found = true;
				break;
			}
		}

		// body contact not found, make new pair
		// this is a new contact between two bodies
		if (!found)
		{
			BodyContactEdge *newEdge12 = new (mBodyContactEdgeAllocator.Allocate()) BodyContactEdge();
			BodyContactEdge *newEdge21 = new (mBodyContactEdgeAllocator.Allocate()) BodyContactEdge();

			newEdge12->counter = 1;
			newEdge12->other = &body2;
			newEdge12->twin = newEdge21;

			newEdge21->counter = 1;
			newEdge21->other = &body1;
			newEdge21->twin = newEdge12;

			body1.mContactEdges.push_back(newEdge12);
			body2.mContactEdges.push_back(newEdge21);

			if (mListener)
				mListener->BeginContact(bodyA, bodyB);
		}
	}

	// create edges
	manifold.edgeA.manifold = manifold.edgeB.manifold = &manifold;
	manifold.edgeA.other = &colliderB;
	manifold.edgeB.other = &colliderA;
	colliderA.mContactEdges.push_back(&manifold.edgeA);
	colliderB.mContactEdges.push_back(&manifold.edgeB);

	// link new manifold
	mManifolds.push_back(&manifold);

	// cache mixed material properties
	// if we want to support real-time material change, then we need to move this into contact solver
	//manifold.restitution = MixRestitution(colliderA.mMaterial.mRestitution, colliderB.mMaterial.mRestitution);
	// delete this!!!!!!
	//manifold.restitution = 0;
	//manifold.friction = MixFriction(colliderA.mMaterial.mFriction, colliderB.mMaterial.mFriction);

	// dispatch begin contact event
	if (mListener)
	{
		mListener->BeginContact(colliderA, colliderB, manifold);
		for (unsigned char i = 0; i < manifold.numContacts; ++i)
			mListener->BeginContact(colliderA, colliderB, *manifold.contacts[i]);
	}

	return true;
}

void ContactManager::Remove(ContactManifold &manifold)
{
	Collider &colliderA = *manifold.colliderA;
	Collider &colliderB = *manifold.colliderB;
	RigidBody &bodyA = *colliderA.mParent;
	RigidBody &bodyB = *colliderB.mParent;

	// find possible body contact edges to be removed
	bool endBodyContact = false;
	const bool aIsShorter = bodyA.mContactEdges.size() < bodyB.mContactEdges.size();
	auto &shorterEdgeList = aIsShorter ? bodyA.mContactEdges : bodyB.mContactEdges;
	auto &body1 = aIsShorter ? bodyA : bodyB;
	auto &body2 = aIsShorter ? bodyB : bodyA;
	for (auto &edge : shorterEdgeList)
	{
		if (edge.other == &body2)
		{
			--edge.counter;
			--edge.twin->counter;
			GV_ASSERT_MESSAGE(edge.counter == edge.twin->counter, "Something's wrong with body contact edge counter.");

			// last contact manifold removed
			// two bodies have lost last contact
			if (!edge.counter)
			{
				edge.twin->~BodyContactEdge();
				edge.~BodyContactEdge();
				mBodyContactEdgeAllocator.Free(edge.twin);
				mBodyContactEdgeAllocator.Free(&edge);

				// this will make listener dispatch end contact event for these two bodies
				endBodyContact = true;
			}
			break;
		}
	}

	// dispatch end contact event
	if (mListener)
	{
		// per manifold
		for (unsigned char i = 0; i < manifold.numContacts; ++i)
			mListener->EndContact(colliderA, colliderB, *manifold.contacts[i]);

		// per collider pair
		mListener->EndContact(colliderA, colliderB, manifold);

		// per body pair
		if (endBodyContact)
			mListener->EndContact(bodyA, bodyB);
	}

	// in-lists should automatically handle the link removal
	manifold.~ContactManifold();
	mManifoldAllocator.Free(&manifold);
}

void ContactManager::Merge(ContactManifold &src, ContactManifold &dest, float persistentThresholdSQ)
{
	GV_ASSERT(src.SameColliders(dest));

	auto &colliderA = *dest.colliderA;
	auto &colliderB = *dest.colliderB;
	auto &bodyA = colliderA.Parent();
	auto &bodyB = colliderB.Parent();

	// list of valid contacts
	Contact *validContacts[ContactManifold::k_maxContacts * 2] = { 0 };
	unsigned char numValidContacts = 0;

	// check persistent contacts
	{
		for (unsigned char i = 0; i < dest.numContacts; ++i)
		{
			Contact &contact = *dest.contacts[i];
			const Math::Vector3 localToGlobalA = bodyA.LocalToGlobal(contact.localPositionA);
			const Math::Vector3 localToGlobalB = bodyB.LocalToGlobal(contact.localPositionB);
			const Math::Vector3 rA = contact.globalPositionA - localToGlobalA;
			const Math::Vector3 rB = contact.globalPositionB - localToGlobalB;
			if (!src.invalidateExistingContacts
				&& contact.normal.Dot(localToGlobalB - localToGlobalA) <= 0.0f
				&& rA.LengthSq() < persistentThresholdSQ
				&& rB.LengthSq() < persistentThresholdSQ)
			{
				// contact persistent, keep
				contact.persistent = true;
				validContacts[numValidContacts++] = &contact;
			}
			else
			{
				// dispatch end contact event
				if (mListener)
					mListener->EndContact(colliderA, colliderB, contact);

				// contact not persistent, destroy
				contact.~Contact();
				mContactAllocator.Free(&contact);
			}
		}
	} // end of check persistent contacts

	// process new contacts
	for (unsigned char i = 0; i < src.numContacts; ++i)
	{
		auto newContact = src.contacts[i];

		// discard new contact if it's too close to cached contacts
		bool discard = false;
		for (unsigned char j = 0; j < numValidContacts; ++j)
		{
			if ((newContact->globalPositionA - validContacts[j]->globalPositionA).LengthSq() < persistentThresholdSQ)
			{
				discard = true;
				break;
			}
		}

		if (discard)
		{
			// contact too close to cached contacts, destroy
			newContact->~Contact();
			mContactAllocator.Free(newContact);
		}
		else
		{
			// add new contact to valid list
			validContacts[numValidContacts++] = newContact;
		}
	}

	// repopulate new manifold
	if (numValidContacts <= ContactManifold::k_maxContacts)
	{
		// enough contact slots, merge all
		for (unsigned char i = 0; i < numValidContacts; ++i)
			dest.contacts[i] = validContacts[i];
		for (unsigned char i = numValidContacts; i < dest.numContacts; ++i)
			dest.contacts[i] = nullptr;
		dest.numContacts = numValidContacts;
	}
	else // not enough contact slots, pick ones furthest apart
	{
		dest.numContacts = 0;

		// 1) find deepest contact first
		Contact *deepest = validContacts[0];
		unsigned char deepestIndex = 0;
		for (unsigned char i = 1; i < numValidContacts; ++i)
		{
			if (validContacts[i]->depth > deepest->depth)
			{
				deepest = validContacts[i];
				deepestIndex = i;
			}
		}
		// move deepest contact to end of valid list
		std::swap(validContacts[deepestIndex], validContacts[--numValidContacts]);
		// add deepest contact
		dest.contacts[dest.numContacts++] = deepest;

		// 2) find furthest contact to form 1D simplex (a line)
		if (dest.numContacts < ContactManifold::k_maxContacts)
		{
			float distSQ = std::numeric_limits<float>::lowest();
			Contact *furthest = nullptr;
			unsigned char furthestIndex = 0;
			for (unsigned char i = 0; i < numValidContacts; ++i)
			{
				const float currDistSQ = (validContacts[i]->globalPositionA - deepest->globalPositionA).LengthSq();
				if (currDistSQ > distSQ)
				{
					furthest = validContacts[i];
					distSQ = currDistSQ;
					furthestIndex = i;
				}
			}
			// move furthest contact to end of valid list
			std::swap(validContacts[furthestIndex], validContacts[--numValidContacts]);
			// add furthest contact
			dest.contacts[dest.numContacts++] = furthest;
		}

		// 3) expand line to a triangle
		if (dest.numContacts == 2
			&& dest.numContacts < ContactManifold::k_maxContacts
			&& dest.numContacts < ContactManifold::k_maxContacts)
		{
			const Math::Vector3 &linePos = dest.contacts[0]->globalPositionA;
			const Math::Vector3  lineDir = (dest.contacts[1]->globalPositionA - dest.contacts[0]->globalPositionA).Normalized();
			Contact *furthest = nullptr;
			unsigned char furthestIndex = 0;
			float distSQ = std::numeric_limits<float>::lowest();
			for (unsigned char i = 0; i < numValidContacts; ++i)
			{
				// calculate distance from 1D simplex
				const Math::Vector3 posDiff = validContacts[i]->globalPositionA - linePos;
				const float currDistSQ = (posDiff - posDiff.Project(lineDir)).LengthSq();
				if (currDistSQ > distSQ)
				{
					furthest = validContacts[i];
					distSQ = currDistSQ;
					furthestIndex = i;
				}
			}
			// move furthest contact to end of valid list
			std::swap(validContacts[furthestIndex], validContacts[--numValidContacts]);
			// add furthest contact
			dest.contacts[dest.numContacts++] = furthest;
		}

		// 4) blow up manifold using furthest contact from 2D simplex (triangle)
		if (dest.numContacts == 3
			&& dest.numContacts < ContactManifold::k_maxContacts
			&& dest.numContacts < ContactManifold::k_maxContacts)
		{
			const Math::Vector3 &a = dest.contacts[0]->globalPositionA;
			const Math::Vector3 &b = dest.contacts[1]->globalPositionA;
			const Math::Vector3 &c = dest.contacts[2]->globalPositionA;
			Contact *furthest = nullptr;
			unsigned char furthestIndex = 0;
			float distSQ = std::numeric_limits<float>::lowest();
			for (unsigned char i = 0; i < numValidContacts; ++i)
			{
				// calculate distance from 1D simplex
				const Math::Vector3 &p = validContacts[i]->globalPositionA;
				const float currDistSQ = (ClosestPointTriangle(p, a, b, c) - p).LengthSq();
				if (currDistSQ > distSQ)
				{
					furthest = validContacts[i];
					distSQ = currDistSQ;
					furthestIndex = i;
				}
			}
			// move furthest contact to end of valid list
			std::swap(validContacts[furthestIndex], validContacts[--numValidContacts]);
			// add furthest contact
			dest.contacts[dest.numContacts++] = furthest;
		}

		// 5) dispose all remaining valid contacts
		for (unsigned char i = 0; i < numValidContacts; ++i)
		{
			// dispatch end contact event for persistent contacts
			if (mListener && validContacts[i]->persistent)
				mListener->EndContact(colliderA, colliderB, *validContacts[i]);

			validContacts[i]->~Contact();
			mContactAllocator.Free(validContacts[i]);
		}
	}

	// dispatch begin begin contact event for new contacts
	if (mListener)
	for (unsigned char i = 0; i < dest.numContacts; ++i)
	if (!dest.contacts[i]->persistent)
		mListener->BeginContact(colliderA, colliderB, *dest.contacts[i]);

	// clean up source manifold
	for (unsigned char i = 0; i < ContactManifold::k_maxContacts; ++i)
		src.contacts[i] = nullptr;
	src.numContacts = 0;
}

void ContactManager::PreNarrowphase(void)
{
	for (auto &manifold : mManifolds)
	{
		manifold.wasColliding = manifold.isColliding;
		manifold.isColliding = false;
	}
}

void ContactManager::PostNarrowphase(void)
{
	// clean up non-colliding manifolds
	auto manifoldIter = mManifolds.begin();
	while (manifoldIter != mManifolds.end())
	{
		ContactManifold &manifold = *manifoldIter;
		const Collider &colliderA = *manifold.colliderA;
		const Collider &colliderB = *manifold.colliderB;
		const RigidBody &bodyA = *colliderA.mParent;
		const RigidBody &bodyB = *colliderB.mParent;

		if (!manifold.isColliding && bodyA.IsAwake() && bodyB.IsAwake())
		{
			// unlink manifold
			manifoldIter = mManifolds.erase(manifoldIter);

			// proxies not colliding, remove manifold
			Remove(manifold);

			continue;
		}

		++manifoldIter;
	}
}

void ContactManager::Draw()
{
	// delete this !
	return;
	for (const auto &manifold : mManifolds)
	{
		if (manifold.isColliding)
		{
			for (unsigned i = 0; i < manifold.numContacts; ++i)
			{
				const Contact &contact = *manifold.contacts[i];

				const Math::Vector3 minPointA = contact.globalPositionA + Math::Vector3(-0.5f, -0.5f, -0.5f) * PhysicsDebugDraw::k_contactSize;
				const Math::Vector3 maxPointA = contact.globalPositionA + Math::Vector3(0.5f, 0.5f, 0.5f) * PhysicsDebugDraw::k_contactSize;
				DrawAABB(minPointA, maxPointA);


				const Math::Vector3 minPointB = contact.globalPositionB + Math::Vector3(-0.5f, -0.5f, -0.5f) * PhysicsDebugDraw::k_contactSize;
				const Math::Vector3 maxPointB = contact.globalPositionB + Math::Vector3(0.5f, 0.5f, 0.5f) * PhysicsDebugDraw::k_contactSize;
				DrawAABB(minPointB, maxPointB);
			} // end of looping through contacts
		}
	} // end of looping through manifolds
}

void ContactManager::DrawAABB(const Vector3& maxPoint,const Vector3& minPoint)
{
	Vector3 p1(minPoint.x, minPoint.y, minPoint.z);
	Vector3 p2(maxPoint.x, minPoint.y, minPoint.z);
	Vector3 p3(minPoint.x, minPoint.y, maxPoint.z);
	Vector3 p4(maxPoint.x, minPoint.y, maxPoint.z);

	Vector3 p5(minPoint.x, maxPoint.y, minPoint.z);
	Vector3 p6(maxPoint.x, maxPoint.y, minPoint.z);
	Vector3 p7(minPoint.x, maxPoint.y, maxPoint.z);
	Vector3 p8(maxPoint.x, maxPoint.y, maxPoint.z);

	Renderer::getInstance()->DrawLine(p1, p2, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p1, p3, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p4, p2, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p4, p3, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p5, p6, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p5, p7, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p8, p6, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p8, p7, Color(1, 0, 0));

	Renderer::getInstance()->DrawLine(p1, p5, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p2, p6, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p3, p7, Color(1, 0, 0));
	Renderer::getInstance()->DrawLine(p4, p8, Color(1, 0, 0));
}


