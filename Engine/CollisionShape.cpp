/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CollisionShape.h"
#include "CPhysics.h"
#include "CTransform.h"
#include "Renderer.h"

/* constructors */
CollisionShape::CollisionShape()
{}

CollisionShape::CollisionShape(ShapeType sid): id(sid)
{
	IsActivate = true;
	IsUpdated  = false;
}

OBB::OBB() : OBB(NULL)
{}

GenericShape::GenericShape() : GenericShape(NULL)
{}

Circle::Circle() : Circle(NULL)
{}

Box::Box() : Box(NULL)
{}

OBB::OBB(CPhysics* cphy) :CollisionShape(IdOBB)
{
  cphysics = cphy;
  mPoints = new Point3[4];
  mHalfextend[0].ZeroOut();
  mHalfextend[1].ZeroOut();
  	for (int i = 0; i < 4 ;++i)
	{
		mPoints[i].ZeroOut();
	}
}

GenericShape::GenericShape(CPhysics* cphy) :CollisionShape(IdGeneric)
{
	cphysics = cphy;
	vector<Vertex> mashesFromGraphics = cphysics->gameObject->GetComponent<CMeshRenderer>()->GetMesh()->GetVertices();
	// for 2D only, hard coded stuff
	Vertex temp = mashesFromGraphics[2];
	mashesFromGraphics[2] = mashesFromGraphics[3];
	mashesFromGraphics[3] = temp;
	// end
	vector<Vertex>::iterator  ViterBegin = mashesFromGraphics.begin();
	vector<Vertex>::iterator  ViterEnd   = mashesFromGraphics.end();
	for (; ViterBegin != ViterEnd; ++ViterBegin)
		mashes.push_back(ToVector4(ViterBegin->mPosition, 1));
}

Circle::Circle(CPhysics* cphy):CollisionShape(IdCircle)
{
	cphysics = cphy;
}

Box::Box(CPhysics* cphy):CollisionShape(IdBox)
{
	cphysics = cphy;
	for (int i = 0; i < 4 ;++i)
	{
		mPoints[i].ZeroOut();
	}
}
/* destructors */
CollisionShape::~CollisionShape()
{}

Circle::~Circle() 
{}

Box::~Box()
{}

OBB::~OBB()
{
	delete [] mPoints;
}

GenericShape::~GenericShape()
{}

/* CollisionShape Functions */
void CollisionShape::SetIsUpdated(bool swi)
{
	IsUpdated = swi;
}

/* Virtual Functions */
void GenericShape::SetUpValue()
{
	// update center
	mCenter = cphysics->GetPosition();

}

void Box::SetUpValue()
{
  SetUpPoints();
}

void Circle::SetUpValue()
{
	Vector3 scale = cphysics->gameObject->transform->scale;
	mRadius = abs(scale.x) / 2;
	Matrix Tran = cphysics->gameObject->transform->mTranslation;
	Matrix InvTran = Tran.Inverted();
	mWorld = Tran;
	mInvWorld = InvTran;
}

void OBB::SetUpValue()
{
  /*   V1      V4  */
  /*   --------    */
  /*   |   .  |    */
  /*   --------    */
  /*   V2      V3  */

  // update mHalfextend
  static Matrix Tran;
  static Matrix InvRot;
  static Matrix InvTran;
  static Vector3 scale;
  mRotation = cphysics->gameObject->transform->GetGlobalRotationMatrix();
  Tran = cphysics->gameObject->transform->mTranslation;
  Tran.m03 += cphysics->GetOffSetX(); 
  Tran.m13 += cphysics->GetOffSetY();
  InvRot = mRotation.Inverted();
  InvTran = Tran.Inverted();
  mWorld = Tran * mRotation;
  mInvWorld = InvRot * InvTran;
  scale = cphysics->gameObject->transform->scale;
  float scalex = cphysics->mWidthScale;
  float scaley = cphysics->mHeightScale;
  mHalfextend[0].x = abs(scale.x) * 0.5f * scalex;
  mHalfextend[1].y = abs(scale.y) * 0.5f * scaley;
  
  // update center
	mCenter	   = cphysics->GetPosition();
  // update axis
  Vector AxisX(1.0f,0,0,0);
  Vector AxisY(0,1.0f,0,0);
  static Vector newAxisX; 
  static Vector newAxisY;
  newAxisX = mRotation * AxisX;
  newAxisY = mRotation * AxisY;
  newAxisX.Normalize();
  newAxisY.Normalize();
  mAxis[0] = newAxisX;
  mAxis[1] = newAxisY;


   // update four points
  float halfX = GetHalfExtendX().x;
  float halfY = GetHalfExtendY().y;

  mPoints[0] = mCenter - mAxis[0] * halfX + mAxis[1] * halfY;
  mPoints[1] = mCenter - mAxis[0] * halfX - mAxis[1] * halfY;
  mPoints[2] = mCenter + mAxis[0] * halfX - mAxis[1] * halfY;
  mPoints[3] = mCenter + mAxis[0] * halfX + mAxis[1] * halfY;
}
Point3 GenericShape::getFarthestPointInDirection(const Vector& dir)
{
	
	float max = -FLT_MAX;
	int index = 0;
	int i = 0;
	int size = mashes.size();
	float dot;
	for (; i < size; ++i)
	{
		dot = mashes[i].Dot(dir);
		if (dot > max)
		{
			max = dot;
			index = i;
		}
	}
	Point3 pt(mashes[index].x,mashes[index].y,mashes[index].z);
	return pt;
}

const Math::Vector3 GenericShape::Support(const Math::Vector3 &dir) const
{
	Math::Vector3 ret;
	return ret;
}

Point3 Circle::getFarthestPointInDirection(const Vector& dir)
{
	Point3 a;
	return a;
}

Point3 Box::getFarthestPointInDirection(const Vector& dir)
{
	Point3 a;
	return a;
}

Point3 OBB::getFarthestPointInDirection(const Vector& dir)
{
	Point3 a;
	return a;
}

//Setters
void Circle::SetRadius(const float radius)
{
	mRadius = radius;
}
void Box::SetLength(const Vector length)
{
	mhalfLength = length;
}
void Box::SetUpPoints()
{
  Point3 position;
  Vector3 scale = cphysics->gameObject->transform->scale;
  mhalfLength.x = abs(scale.x) * 0.5f;
  mhalfLength.y = abs(scale.y) * 0.5f;
  position = cphysics->GetPosition();

  max[0] = position.x + mhalfLength.x;
  max[1] = position.y + mhalfLength.y;
  min[0] = position.x - mhalfLength.x;
  min[1] = position.y - mhalfLength.y;

  mPoints[0].x = position.x - 0.5f * scale.x;
  mPoints[0].y = position.y + 0.5f * scale.y;
  mPoints[1].x = position.x - 0.5f * scale.x;
  mPoints[1].y = position.y - 0.5f * scale.y;
  mPoints[2].x = position.x + 0.5f * scale.x;
  mPoints[2].y = position.y - 0.5f * scale.y;
  mPoints[3].x = position.x + 0.5f * scale.x;
  mPoints[3].y = position.y + 0.5f * scale.y;
}

void OBB::SetCenter(Point3 center)
{
	mCenter = center;
}
void OBB::SetAxisU(Vector axis)
{
	mAxis[0] = axis;
}
void OBB::SetAxisV(Vector axis)
{
	mAxis[1] = axis;
}

//Getters
Point3* OBB::GetVertices() const 
{
	return mPoints;
}
const float Circle::GetRadius() const
{
	return mRadius;
}

const Math::Vector3 Circle::Support(const Math::Vector3 &dir) const
{
	Math::Vector3 ret;
	return ret;
}

const Vector& Box::GetLength() const
{
	return mhalfLength;
}
const float* Box::GetMax()const
{
	return max;
}
const float* Box::GetMin()const
{
	return min;
}

const Math::Vector3 Box::Support(const Math::Vector3 &dir) const
{
	Math::Vector3 ret;
	return ret;
}

const Point3& OBB::GetCenter() const
{
	return mCenter;
}
const Vector& OBB::GetHalfExtendX() const
{
	return mHalfextend[0];
}
const Vector& OBB::GetHalfExtendY() const
{
  return mHalfextend[1];
}
const Vector& OBB::GetAxisU() const
{
	return mAxis[0];
}

const Vector& OBB::GetAxisV() const
{
	return mAxis[1];
}

const Math::Vector3 OBB::Support(const Math::Vector3 &dir) const
{
	Math::Vector3 ret;
	return ret;
}

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Shape Class