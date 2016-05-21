///////////////////////////////////////////////////////////////////////////////
///
///  \file EulerAngles.hpp
///  Declaration of the EulerAngle structure, interface referenced from
///  Insomniac Games, implementation referenced from Graphics Gems IV.
///  
///  Authors: Benjamin Strukus
///  Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EulerOrder.h"
#include "Vector3.h"

namespace Math
{

///Forward declaration
struct Matrix3;
typedef const Matrix3& Mat3Param;
typedef Matrix3& Mat3Ref;

///Forward declaration
struct Matrix4;
typedef const Matrix4& Mat4Param;
typedef Matrix4& Mat4Ref;

///Forward declaration
struct Quaternion;
typedef const Quaternion& QuatParam;
typedef Quaternion& QuatRef;

struct EulerAngles;
typedef const EulerAngles&  EulerAnglesParam;
typedef EulerAngles&        EulerAnglesRef;
typedef EulerAngles*        EulerAnglesPtr;

//----------------------------------------------------------------- Euler Angles
///Structure to provide a consistent way to deal with rotations stored as Euler
///angles.
struct EulerAngles
{
  EulerAngles(EulerOrderParam order = EulerOrders::Enum::XYZr);
  EulerAngles(Vec3Param xyzRotation, EulerOrderParam order = EulerOrders::Enum::XYZr);
  EulerAngles(float xRotation, float yRotation, float zRotation, 
              EulerOrderParam order);
  EulerAngles(Mat3Param matrix, EulerOrderParam order);
  EulerAngles(Mat4Param matrix, EulerOrderParam order);
  EulerAngles(QuatParam quaternion, EulerOrderParam order);

  ///Index operator to access the Angles data directly.
  float operator [] (unsigned index) const;

  ///Index operator to access the Angles data directly.
  float& operator [] (unsigned index);


  float I(void) const;
  float J(void) const;
  float K(void) const;
  float H(void) const;

  void I(float i);
  void J(float j);
  void K(float k);
  void H(float h);

  void Reorder(EulerOrderParam newOrder);

  Vector3     Angles;
  EulerOrder  Order;
};

}// namespace Math
