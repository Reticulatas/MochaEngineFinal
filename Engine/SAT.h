/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */   


#pragma once
#include "MathIncludes.h"
class CPhysics;
class ContactsManager;
class GameObject;

typedef enum 
{
	None,
	AtoB,
	BtoA
}Direction;

struct ContactInfo //save 8 contacts info to find the penetration
{
	Vector normal;
	Point4 contactPoint;
	float penetration;
	int indexA, indexB;
	Direction direction;
	bool IsInBox;
};

typedef enum 
{
	BothOutside,
	BothInside,
	OneInOneOut
}PointsUnderNormal;

typedef enum 
{
	BoxToBox,
	BoxToCircle,
	CircleToCircle
}CollisionPair;

typedef enum Value
{
	MinNum,
	MaxNum
}Option;

typedef enum
{
	X,
	Y
}Axis;

struct Contacts
{
	/*!	\two physics objects pointers which are collided */
	CPhysics* contacts[2];
	/*!	\penetration of collision */
	float penetration;
	/*!	\normal of collision */
	Vector normal;
	/*!	\position of obj A and obj B */
	Point3 PosA, PosB;
	/*!	\index of vertex of obj A and B */
	int indexA, indexB;
	/*!	\lamda of obj A and B (used in resolution)*/
	float L1,L2;
	/*!	\contact points of collision of a and b (used in resolution)*/
	Point3 contactPoint, contactPoint2;
	/*!	\contact pair in model space*/
	Point3 cp1mA, cp1mB, cp2mA, cp2mB;
	/*!	\change position of A, B(used in resolution)*/
	Vector changePosA, changePosB;
	/*!	\collision position*/
	PointsUnderNormal side;
	/*!	\nums of collision contacts*/
	int numberOfcontacts;
	/*!	\not used*/
	int type;
	/*!	\type of collision*/
	CollisionPair collsionPair;
};

class SAT
{
public:
	static void FlipNormal(Vector* normal);
	static bool IsIndexInA(OBB* OBBa, Point4 pt);
	static void FindPreNextIndex(int index, int* prev, int* next);
	static float OBBFindX(Vector* v1, Vector* v2, Vector* v3, Vector* v4, Option X);
	static float OBBFindY(Vector* v1, Vector* v2, Vector* v3, Vector* v4, Option Y);
	static int FindSmall(float* proj, float* xfactor, bool*IsInBox);
	static int FindSmallIndex(Point3 * aPoint1,Point3 * aPoint2,Point3 * aPoint3,Point3 * aPoint4,
		float halfX, float halfY, Axis axis, Vector* normal,OBB *b);
	static bool ProjectAToBLeastPenetration(Point3* aPoint1,Point3* aPoint2,Point3* aPoint3,Point3* aPoint4,
		Vector* normalAxis, OBB* b,  Axis* axis, 
		Vector* normal,Point3* contactPoint,float* penetration, 
		int* indexA, int* indexB, int refB, Direction* dir, Direction* result, ContactInfo* info,
		int listCounter);
	static Point3 FindIntersectPointReturnGarbage(Point3*p1,Point3*p2,Point3*p3,Point3*p4 );
	static Point3 FindIntersectPoint(Point3*p1,Point3*p2,Point3*p3,Point3*p4 );
	static float PosOfPointsCompVec(Point3* mpoint,Point3* mpointonline, Vector* mvector);
	static int NumberOfPointsIntersect(ContactsManager *cm, OBB* OBBa, OBB* OBBb);
	static void TwoUnderNormalFindPoints(int indexA,int indexB,  OBB* OBBa,OBB* OBBb,Vector*normal,
		Point3*ptA1, Point3*ptA2,Point3*ptA3,Point3*ptA4);
	static void FlipXY(Vector*mnormal);
	static PointsUnderNormal TwoUnderNormalCheck(int indexA,int indexB,  OBB* OBBa,OBB* OBBb,Vector*mnormal);
	static void TwoUnderNormalFindPointsFlip(int indexA,int indexB,  OBB* OBBa,OBB* OBBb,Vector*normal,
		Point3*ptA1, Point3*ptA2,Point3*ptA3,Point3*ptA4,
		Point3* normalA, Point3* normalB);
	static Point3 FindPointAlongNormalByPenetration(Point3 oriCP,Vector normal, float penetration);
	static Point3 FindSecondPair(Point3 oriCP, Point3 cp1,Point3 cp2);
	static void UpdateContacPoints(ContactsManager *cm, OBB* OBBa, OBB* OBBb);
	static ContactInfo FindFinalContactInfo(ContactInfo* info);
	static bool CollisionDetection(CollisionShape* a, Point3 aPosition, CollisionShape* b, Point3 bPosition, ContactsManager *c);
};

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   SAT Class