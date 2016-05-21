/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Collision.h" 
#include "CPhysics.h"
#include "Resolution.h"
#include "Renderer.h"

#include "GJK.h"
#define EPSLON 0.01

std::pair<float, Vector> AABBFindContact(Box* a, Point3 aPosition, Box* b ,Point3 bPosition)
{
  Vector relPos = bPosition - aPosition;
  Vector normal;
  float minX = (a->max[0] - b->min[0] > b->max[0] - a->min[0]) 
    ? b->max[0] - a->min[0] : a->max[0] - b->min[0];

  float minY = (a->max[1] - b->min[1] > b->max[1] - a->min[1]) 
    ? b->max[1] - a->min[1] : a->max[1] - b->min[1];

  std::pair <float, Vector> ret;
  if(minX < minY || minY == 0.0f)
  {
    ret.first = minX;
    normal.Set(1.0f,0,0);
    float result = relPos.Dot(normal);
    if(  result >= 0) 
      ret.second.Set(1.0f,0,0);
    else
      ret.second.Set(-1.0f,0,0);
  }
  else
  {
    ret.first = minY;
    normal.Set(0,1.0f,0);
    float result = relPos.Dot(normal);
    if(  result > 0) 
      ret.second.Set(0,1.0f,0);
    else
      ret.second.Set(0,-1.0f,0);
  }
  return ret;
}
bool AABoxToAABox(CollisionShape* a, Point3 aPosition, CollisionShape* b, Point3 bPosition, ContactsManager *c)
{
	Box* Ba = (Box*) a;
	Box* Bb = (Box*) b;
	if(Ba->max[0] >= Bb->min[0] && Ba->min[0] <= Bb->max[0] &&
		Ba->max[1] >= Bb->min[1] && Ba->min[1] <= Bb->max[1]) 
	{
		std::pair<float, Vector> pen = AABBFindContact(Ba,aPosition,Bb,bPosition);
    c->ContactList[c->NumContacts].penetration = pen.first;
    c->ContactList[c->NumContacts].normal = pen.second;
    c->ContactList[c->NumContacts].contacts[0] = a->cphysics;
    c->ContactList[c->NumContacts].contacts[1] = b->cphysics;
    c->ContactList[c->NumContacts].changePosA.Set(0,0,0);
    c->ContactList[c->NumContacts].changePosB.Set(0,0,0);
    c->ContactList[c->NumContacts].type = 0;
    ++(c->NumContacts);
		return true;
	}
	return false;
}

bool AABoxToCircle(CollisionShape* aBox, Point3 aPosition, CollisionShape* bCircle, Point3 bPosition, ContactsManager *cm)
{
	CPhysics * cphyBox = aBox->cphysics;
	CPhysics * cphyCircle = bCircle->cphysics;
	// box is index B circle is index a, which is cphyA = contact[1], cphyB = contact[2], 
	OBB* box = static_cast<OBB*> (aBox);
	Circle* circle = static_cast<Circle*>(bCircle);
	float width, height, biggestLengthSqr, dist2CenterSqr;
	float radius = circle->GetRadius();
	width = box->GetHalfExtendX().x;
	height = box->GetHalfExtendY().y;
	biggestLengthSqr = sqrtf(width * width + height * height) + radius; 
	dist2CenterSqr = (aPosition - bPosition).Length();
	if(dist2CenterSqr <= biggestLengthSqr)
	{
	   int next;
	   Vector line;
	   Vector centerToStart;
	   for (int i = 0; i < 4; ++i)
	   {
		   if(i == 3)
			   next = 0;
		   else 
			   next = i + 1;
		   line = box->mPoints[next] - box->mPoints[i]; //d
		   centerToStart = box->mPoints[i] - bPosition; //f
		   float a = line.Dot(line) ;
		   float b = 2*centerToStart.Dot(line) ;
		   float c = centerToStart.Dot(centerToStart) - radius * radius;
		   float discriminant = b*b-4*a*c;
		   if( discriminant < 0 )
			   continue;
		   else
		   {
			   Vector normal = box->mPoints[next] - box->mPoints[i];
			   SAT::FlipNormal(&normal);
			   normal.Normalize();
			   discriminant = sqrtf( discriminant );
			   float t1 = (-b - discriminant)/(2*a);
			   float t2 = (-b + discriminant)/(2*a);
			   if( t1 >= 0 && t1 <= 1 )
			   {
				   Point3 contactPt;
				   
				   contactPt = box->mPoints[i] + line * t1;
				   cm->ContactList[cm->NumContacts].contactPoint = contactPt;
				   cm->ContactList[cm->NumContacts].contactPoint2= contactPt;
				   cm->ContactList[cm->NumContacts].cp1mA        = circle->mInvWorld * contactPt;
				   cm->ContactList[cm->NumContacts].cp1mB        = box->mInvWorld * contactPt;
				   cm->ContactList[cm->NumContacts].cp2mA        = cm->ContactList[cm->NumContacts].cp1mA;
				   cm->ContactList[cm->NumContacts].cp2mB        = cm->ContactList[cm->NumContacts].cp1mB;
				   cm->ContactList[cm->NumContacts].normal = -normal;
				   cm->ContactList[cm->NumContacts].contacts[0] = cphyCircle;
				   cm->ContactList[cm->NumContacts].contacts[1] = cphyBox;
				   cm->ContactList[cm->NumContacts].indexA = -1;
				   cm->ContactList[cm->NumContacts].indexB = i;
				   cm->ContactList[cm->NumContacts].penetration = 0;
				   cm->ContactList[cm->NumContacts].collsionPair = BoxToCircle;
				   ++(cm->NumContacts);	
				   return true;
			   }
			   if( t2 >= 0 && t2 <= 1 )
			   {
				   Point3 contactPt;
				   contactPt = box->mPoints[i] + line * t2;
				   cm->ContactList[cm->NumContacts].contactPoint = contactPt;
				   cm->ContactList[cm->NumContacts].contactPoint2= contactPt;
				   cm->ContactList[cm->NumContacts].cp1mA        = circle->mInvWorld * contactPt;
				   cm->ContactList[cm->NumContacts].cp1mB        = box->mInvWorld * contactPt;
				   cm->ContactList[cm->NumContacts].cp2mA        = cm->ContactList[cm->NumContacts].cp1mA;
				   cm->ContactList[cm->NumContacts].cp2mB        = cm->ContactList[cm->NumContacts].cp1mB;
				   cm->ContactList[cm->NumContacts].normal = -normal;
				   cm->ContactList[cm->NumContacts].contacts[0] = cphyCircle;
				   cm->ContactList[cm->NumContacts].contacts[1] = cphyBox;
				   cm->ContactList[cm->NumContacts].indexA = -1;
				   cm->ContactList[cm->NumContacts].indexB = i;
				   cm->ContactList[cm->NumContacts].penetration = 0;
				   cm->ContactList[cm->NumContacts].collsionPair = BoxToCircle;
				   ++(cm->NumContacts);
				   return true ;
			   }
			   
		   }
		   
	   }
	}
	else 
		return false;
	return false;
}
bool CircleToAABox(CollisionShape* a, Point3 aPosition, CollisionShape* b, Point3 bPosition, ContactsManager *c)
{
	return AABoxToCircle(b,bPosition,  a, aPosition, c);
}
bool SphereToSohere(CollisionShape* a, Point3 aPosition, CollisionShape* b, Point3 bPosition, ContactsManager *c)
{
	return true;
}

bool Collision::Detection(CollisionShape* objectA,CollisionShape* objectB, ContactsManager *c)
{
  if(!objectA || !objectB)
    return false;
  return (*methods[objectA->id][objectB->id]) (objectA, objectA->cphysics->GetPosition(), objectB, objectB->cphysics->GetPosition(), c);

}

void Collision::RegisterCollisionMethods(CollisionShape::ShapeType a, CollisionShape::ShapeType b,CollisionMethod method)
{
	methods[a][b] = method;
}
Collision::Collision()
{
	RegisterCollisionMethods(CollisionShape::IdOBB, CollisionShape::IdOBB,SAT::CollisionDetection);// 
	RegisterCollisionMethods(CollisionShape::IdBox, CollisionShape::IdBox, AABoxToAABox);
	RegisterCollisionMethods(CollisionShape::IdOBB, CollisionShape::IdCircle, AABoxToCircle);
	RegisterCollisionMethods(CollisionShape::IdCircle, CollisionShape::IdOBB, CircleToAABox);
	RegisterCollisionMethods(CollisionShape::IdCircle, CollisionShape::IdCircle, SphereToSohere);
	//RegisterCollisionMethods(CollisionShape::IdGeneric, CollisionShape::IdGeneric,GJK::CollisionDetection3D);
}

/*
bool OBBToOBB(CollisionShape* a, Vector aPosition, CollisionShape* b, Vector bPosition, ContactsManager *c)
{

OBB* BoxA;  // smaller X
OBB* BoxB ; // smaller Y
std::pair<float,float> BoxAXYtoCenter,BoxBXYtoCenter;
float centerYdist, centerXdist;
if(aPosition.x < bPosition.x)
{
BoxA = (OBB*) a;
BoxB = (OBB*) b;
}
else
{
BoxA = (OBB*) b;
BoxB = (OBB*) a;
Vector temp= aPosition;
aPosition = bPosition;
bPosition = temp;
}

if(aPosition.y > bPosition.y)
{
BoxAXYtoCenter =  OBBFindMaxMinVertexs(BoxA,Max, Min);
BoxBXYtoCenter =  OBBFindMaxMinVertexs(BoxB,Min, Max);
}
else 
{
BoxAXYtoCenter =  OBBFindMaxMinVertexs(BoxA,Max, Max);
BoxBXYtoCenter =  OBBFindMaxMinVertexs(BoxB,Min, Min);
}
centerXdist = bPosition.x - aPosition.x;
if(BoxAXYtoCenter.first + BoxBXYtoCenter.first < centerXdist)
return false;

if(bPosition.y - aPosition.y > 0.0f)
centerYdist = bPosition.y - aPosition.y;
else
centerYdist = aPosition.y - bPosition.y;

if(BoxAXYtoCenter.second + BoxBXYtoCenter.second < centerYdist)
return false;

return true;
}
*/

meta_define(CollisionData);


/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Collision Class