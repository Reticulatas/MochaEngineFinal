///////////////////////////////////////////////////////////////////////////////
///
/// \file Quaternion.hpp
/// Declaration of the Quaternion structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Reals.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"

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

struct Quaternion;
typedef const Quaternion& QuatParam;
typedef Quaternion& QuatRef;
typedef Quaternion* QuatPtr;

//------------------------------------------------------------------- Quaternion
struct Quaternion
{
  static const Quaternion cIdentity;

  Quaternion(void);
  explicit Quaternion(float x, float y, float z, float w);

  void operator=(QuatParam rhs);
  void operator+=(QuatParam rhs);
  void operator-=(QuatParam rhs);
  void operator*=(QuatParam rhs);
  void operator*=(float rhs);
  void operator/=(float rhs);

  Quaternion operator-(void) const;

  Quaternion operator*(QuatParam rhs) const;
  Quaternion operator+(QuatParam rhs) const;
  Quaternion operator-(QuatParam rhs) const;
  Quaternion operator*(float rhs) const;
  Quaternion operator/(float rhs) const;

  bool operator==(QuatParam rhs) const;
  bool operator!=(QuatParam rhs) const;

  float& operator[](unsigned index);
  float operator[](unsigned index) const;

  void Set(float x, float y, float z, float w);

  void Integrate(Vec3Param vector, float scalar);

  float Normalize(void);
  Quaternion Normalized(void) const;
  float Dot(QuatParam rhs) const;
  float Length(void) const;
  float LengthSq(void) const;

  void Conjugate(void);
  Quaternion Conjugated(void) const;
  void Invert(void);
  Quaternion Inverted(void) const;
  Quaternion Exponent(void) const;
  Quaternion Logarithm(void) const;
  //Quaternion Lerp(QuatParam end, float tValue);
  //Quaternion Slerp(QuatParam end, float tValue);
  void RotateVector(Vec3Ptr vector);
  Vector3 RotatedVector(Vec3Param vector) const;
  void ZeroOut(void);

  bool Valid(void) const;

  Vector3& V3();
  Vector4& V4();
  const Vector3& V3() const;
  const Vector4& V4() const;

  union
  {
    struct { float x, y, z, w; };
    float data[4];
  };
};

Quaternion operator*(float lhs, QuatParam rhs);

void Normalize(QuatPtr quaternion);
Quaternion Normalized(QuatParam quaternion);
float Dot(QuatParam lhs, QuatParam rhs);
float Length(QuatParam quaternion);
float LengthSq(QuatParam quaternion);
Quaternion Lerp(QuatParam start, QuatParam end, float tValue);
Quaternion Slerp(QuatParam start, QuatParam end, float tValue);
Quaternion CreateDiagonalizer(Mat3Param matrix);

}// namespace Math
