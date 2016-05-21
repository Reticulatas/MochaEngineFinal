/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */


#pragma once

#include "MathIncludes.h"
#include "IRegisterable.h"

struct Vertex;
class CPhysics;
using namespace std;
 
class CollisionShape 
{
    ___SERIALIZE_BEGIN___
    ___DEFSER(id, 1)
    ___DEFSER(cphysics, 1)
    ___DEFSER(IsActivate, 1)
    ___SERIALIZE_END___
public:
     
	enum ShapeType
	{
		IdNone,
		IdOBB,
		IdCircle,	
		IdBox,
		IdGeneric,
		NumberOfShpaes
	};
     
    Matrix mWorld;
    Matrix mInvWorld;
    Matrix mRotation;
	ShapeType id;
	CollisionShape();
	CollisionShape(ShapeType sid);
	~CollisionShape();
	CPhysics* cphysics;
	virtual void SetUpValue() = 0;
	virtual Point3 getFarthestPointInDirection(const Vector& dir) = 0;
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const = 0;
	void SetIsUpdated(bool swi);
	CPhysics& GetCPhysics(void) { return *cphysics; };
	bool IsActivate;
	bool IsUpdated;
	Point3 mCenter;
};
 
  ___SERIALIZE_CLASS(CollisionShape, 1);

 
class Circle : public CollisionShape
{
public:
	Circle();
	Circle(CPhysics* cphy);
	~Circle();

	// Setters
	void SetRadius(const float radius);

	// Getters
	const float GetRadius()const;

	// Virtual functions
	virtual void SetUpValue();
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	virtual Point3 getFarthestPointInDirection(const Vector& dir);

private:
	float mRadius;
    ___SERIALIZE_BEGIN___
    ___DEFBASE(CollisionShape)
    ___DEFSER(mRadius, 1)
	SetUpValue();
    ___SERIALIZE_END___
};
 
  ___SERIALIZE_CLASS(Circle, 1);

 
class Box : public CollisionShape
{
public:
	Box();
	Box(CPhysics* cphy);
	~Box();
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	// Setters
	void SetLength(const Vector length);
    void SetUpPoints();

	// Getters
	const Vector & GetLength()const;
	const float* GetMax()const;
	const float* GetMin()const;

	// Virtual functions
	virtual void SetUpValue();
	virtual Point3 getFarthestPointInDirection(const Vector& dir);

	// Public members
	float max[3], min[3];
    Point3 mPoints[4];

private:
	Vector mhalfLength;
    ___SERIALIZE_BEGIN___
    ___DEFBASE(CollisionShape)
    ___DEFSER(max, 1)
    ___DEFSER(min, 1)
	SetUpPoints();
    ___SERIALIZE_END___

};
 
  ___SERIALIZE_CLASS(Box, 1);

 
class OBB : public CollisionShape
{
public:
	OBB();
	OBB(CPhysics* cphy);
	~OBB();

	// Virtual functions
	virtual void SetUpValue();
	virtual Point3 getFarthestPointInDirection(const Vector& dir);
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	//Getters
	const Point3& GetCenter() const;
	const Vector& GetHalfExtendX() const;
    const Vector& GetHalfExtendY() const;
	const Vector& GetAxisU() const;
	const Vector& GetAxisV() const;
	Point3* GetVertices() const;
		
	//Setters
	void SetCenter(Point3 center);
	void SetAxisU(Vector axis);
	void SetAxisV(Vector axis);

	// Public members
    Point3* mPoints;

private:
	Vector mAxis[2];
	Vector mHalfextend[2];
    ___SERIALIZE_BEGIN___
    ___DEFBASE(CollisionShape)
	SetUpValue();
    ___SERIALIZE_END___
    
};
 
	 ___SERIALIZE_CLASS(OBB, 1);
 
class GenericShape : public CollisionShape
{
public:
	GenericShape();
	GenericShape(CPhysics* cphy);
	~GenericShape();

	// Virtual functions
	virtual void SetUpValue();
	virtual Point3 getFarthestPointInDirection(const Vector& dir);
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	// Public members
	vector<Vector> mashes;
	typedef vector<Vector>::iterator VerIter;

	___SERIALIZE_BEGIN___
	___DEFBASE(CollisionShape)
	SetUpValue();
	___SERIALIZE_END___
};
 
  ___SERIALIZE_CLASS(GenericShape, 1);
/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Shape Class