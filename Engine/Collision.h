/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */


#pragma once
#include "CollisionShape.h"
#include "SAT.h"
#include "Collidar.h"
class CPhysics;
class ContactsManager;
class GameObject;

struct CollisionData : public IMeta
{
	/*! \brief The object we're colliding with */
	 GameObject* collidedObj;
	 CPhysics* selfCPhy;
	 CPhysics* collidedObjCPhy;
	 std::vector<Vector3> normals;
	 int numContacts;
	 bool isA;
	// more stuff

	 metadef(CollisionData)
		 m_addbase(IMeta)
		 endmetadef;
};
 
typedef bool (*CollisionMethod)(CollisionShape* a, Point3 aPosition, CollisionShape* b, Point3 bPosition, ContactsManager* c);

class Collision
{
public:
	Collision();
	Vector AABBFindNormal();    //aabb

	bool Detection(CollisionShape* objectA,CollisionShape* objectB, ContactsManager *c);
	void RegisterCollisionMethods(CollisionShape::ShapeType a, CollisionShape::ShapeType b,CollisionMethod method);
	CollisionMethod methods[CollisionShape::NumberOfShpaes][CollisionShape::NumberOfShpaes];
};


/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Collision Class