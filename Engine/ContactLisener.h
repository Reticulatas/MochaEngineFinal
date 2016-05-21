/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */
#pragma once
class Collider;
class RigidBody;
struct Contact;
struct ContactManifold;

class ContactListener
{
public:
	// per contact
	virtual void BeginContact(const Collider &colliderA, const Collider &colliderB, const Contact &contact) { }
	virtual void EndContact(const Collider &colliderA, const Collider &colliderB, const Contact &contact) { }

	// per manifold pair
	virtual void BeginContact(const Collider &colliderA, const Collider &colliderB, const ContactManifold &manifoild) { }
	virtual void EndContact(const Collider &colliderA, const Collider &colliderB, const ContactManifold &manifold) { }

	// per body pair
	virtual void BeginContact(const RigidBody &bodyA, const RigidBody &bodyB) { }
	virtual void EndContact(const RigidBody &bodyA, const RigidBody &bodyB) { }
};