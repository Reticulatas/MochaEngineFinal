/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"

class CPhysics;
class ContactsManager;
class GameObject;


class ColliderGeometry;
class RigidBody;

struct GjkSimplex
{
	enum UpdateResult { NO_COLLISION, COLLISION, CONTINUE };

	Math::Vector3 verts[4]; // support points from MinkowskiDifference(geomA, geomB)
	Math::Vector3 vertsA[4]; // support points from geomA
	Math::Vector3 vertsB[4]; // support points from geomB
	unsigned char numVerts;
	mutable float prevDistSq;
	mutable float currDistSq;


	std::vector<Math::Vector3> vertRecord;
	std::vector<unsigned char> numVertsRecord;
	std::vector<Math::Vector3> dirRecord;
	enum Reduc { B2, C3, CA3, CB3, D4, DA4, DB4, DC4, DBA4, DAC4, DCB4, IMP, NONE };
	std::vector<Reduc> redcRecord;
	float distance; // distance from current simplex to origin


	const ColliderGeometry *geomA;
	const ColliderGeometry *geomB;
	const RigidBody *bodyA;
	const RigidBody *bodyB;

	GjkSimplex(void);
	GjkSimplex(const ColliderGeometry &geomA, const ColliderGeometry &geomB, const RigidBody &bodyA, const RigidBody &bodyB);

	bool CheckOrigin(void) const;
	void ReducePoints(void);
	UpdateResult AddSupportPoint(void);
	UpdateResult Update(void);
	void BlowUpToTetrahedron(void);
	bool TetrahedronContainsOrigin(void) const;
};

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   GJK Class