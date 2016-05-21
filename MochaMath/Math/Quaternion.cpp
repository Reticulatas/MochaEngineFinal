///////////////////////////////////////////////////////////////////////////////
///
/// \file Quaternion.cpp
/// Implementation of the Quaternion structure.
///
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "MathUtilities.h"
#include "Quaternion.h"
#include "MathFunctions.h"

namespace Math
{
typedef Quaternion Quaternion;

const Quaternion Quaternion::cIdentity = Quaternion(0.0f, 0.0f, 
                                                    0.0f, 1.0f);

Quaternion::Quaternion(float xx, float yy, float zz, float ww)
{
  x = xx;
  y = yy;
  z = zz;
  w = ww;
}

Quaternion::Quaternion(void)
{
  x = y = z = 0;
  w = 1;
}

void Quaternion::operator=(QuatParam rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
}

void Quaternion::operator+=(QuatParam rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
}

void Quaternion::operator-=(QuatParam rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
}

void Quaternion::operator*=(QuatParam rhs)
{
  Quaternion lhs(*this);
  x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
  y = lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z;
  z = lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x;
  w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
}

void Quaternion::operator*=(float rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
}

void Quaternion::operator/=(float rhs)
{
  if (Math::IsZero(rhs))
  ErrorIf(Math::IsZero(rhs), "Quaternion - Division by zero.");
  float reciprocal = 1.0f / rhs;
  x *= reciprocal;
  y *= reciprocal;
  z *= reciprocal;
  w *= reciprocal;
}

Quaternion Quaternion::operator-(void) const
{
  return Quaternion(-x, -y, -z, -w);
}

Quaternion Quaternion::operator*(QuatParam quat) const
{
  return Quaternion(w * quat.x + x * quat.w + y * quat.z - z * quat.y,
              w * quat.y + y * quat.w + z * quat.x - x * quat.z,
              w * quat.z + z * quat.w + x * quat.y - y * quat.x,
              w * quat.w - x * quat.x - y * quat.y - z * quat.z);
}

Quaternion Quaternion::operator+(QuatParam rhs) const
{
  return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Quaternion Quaternion::operator-(QuatParam rhs) const
{
  return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Quaternion Quaternion::operator*(float rhs) const
{
  return Quaternion(x * rhs, y * rhs, z * rhs, w * rhs);
}

Quaternion Quaternion::operator/(float rhs) const
{
  ErrorIf(Math::IsZero(rhs), "Quaternion - Division by zero.");
  float reciprocal = 1.0f / rhs;
  return Quaternion(x * reciprocal, y * reciprocal, z * reciprocal, w * reciprocal);
}

bool Quaternion::operator==(QuatParam rhs) const
{
  return Equal(x, rhs.x) &&
         Equal(y, rhs.y) &&
         Equal(z, rhs.z) &&
         Equal(w, rhs.w);
}

bool Quaternion::operator!=(QuatParam rhs) const
{
  return !(*this == rhs);
}

float& Quaternion::operator[](unsigned index)
{
  if (index > 3)
  ErrorIf(index > 3, "Quaternion - Subscript out of range.");
  return V4()[index];
}

float Quaternion::operator[](unsigned index) const
{
  ErrorIf(index > 3, "Quaternion - Subscript out of range.");
  return V4()[index];
}

void Quaternion::Set(float xx, float yy, float zz, float ww)
{
  x = xx;
  y = yy;
  z = zz;
  w = ww;
}

void Quaternion::Integrate(Vec3Param vector, float scalar)
{
  Quaternion q(vector.x * scalar, vector.y * scalar, vector.z * scalar, 0.0f);

  q *= *this;

  x += 0.5f * q.x;
  y += 0.5f * q.y;
  z += 0.5f * q.z;
  w += 0.5f * q.w;
}

float Quaternion::Normalize(void)
{
  float length = LengthSq();

  if(Math::Equal(length, 0.0f))
  {
    x = 0.0f;
    y = x;
    z = x;
    w = 1.0f;
    return 0.0f;
  }
  else
  {
    length = Math::Rsqrt(length);
    *this *= length;
    return length;
  }
}

Quaternion Quaternion::Normalized(void) const
{
  float length = LengthSq();

  if(Math::Equal(length, 0.0f))
  {
    return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
  }
  else
  {
    length = Math::Rsqrt(length);
    return (*this) * length;
  }
}

float Quaternion::Dot(QuatParam rhs) const
{
  return V4().Dot(rhs.V4());
}

float Quaternion::Length(void) const
{
  return Math::Sqrt(LengthSq());
}

float Quaternion::LengthSq(void) const
{
  return V4().LengthSq();
}

void Quaternion::Conjugate(void)
{
  x *= -1.0f;
  y *= -1.0f;
  z *= -1.0f;
}

Quaternion Quaternion::Conjugated(void) const
{
  return Quaternion(-x, -y, -z, w);
}

void Quaternion::Invert(void)
{
  Conjugate();
  float lengthSq = LengthSq();

  if (Math::IsZero(lengthSq))
    ErrorIf(Math::IsZero(lengthSq), "Quaternion - Division by zero.");
  *this /= lengthSq;
}

Quaternion Quaternion::Inverted(void) const
{
  float lengthSq = LengthSq();
  if (Math::IsZero(lengthSq))
    ErrorIf(Math::IsZero(lengthSq), "Quaternion - Division by zero.");
  lengthSq = 1.0f / lengthSq;
  return Quaternion(-x * lengthSq, -y * lengthSq, -z * lengthSq, w * lengthSq);
}

Quaternion Quaternion::Exponent(void) const
{
  float angle = V3().Length();
  Quaternion quaternion(x, y, z, 0.0f);

  if(Math::Abs(angle) > Math::Epsilon())
  {
    quaternion.w = Math::Cos(angle);
    angle = Math::Sin(angle) / angle;
    quaternion.x *= angle;
    quaternion.y *= angle;
    quaternion.z *= angle;
  }
  return quaternion;
}

Quaternion Quaternion::Logarithm(void) const
{
  Quaternion quaternion(x, y, z, 0.0f);
  float theta = Math::ArcCos(w);
  float sinTheta = Math::Sin(theta);

  if(Math::Abs(sinTheta) > Math::Epsilon())
  {
    theta = theta / sinTheta;
    quaternion.x *= theta;
    quaternion.y *= theta;
    quaternion.z *= theta;
  }
  return quaternion;
}

void Quaternion::RotateVector(Vec3Ptr vector)
{
  Quaternion tempVec = Math::ToQuaternion(*vector);
  Quaternion conjugate(-x, -y, -z, w);
  Quaternion result(*this);
  result *= tempVec;
  result *= conjugate;
  vector->Set(result.x, result.y, result.z);
}

Vector3 Quaternion::RotatedVector(Vec3Param vector) const
{
  Quaternion tempVec = Math::ToQuaternion(vector);
  Quaternion conjugate(-x, -y, -z, w);
  Quaternion result(*this);
  result *= tempVec;
  result *= conjugate;
  return Vector3(result.x, result.y, result.z);
}

void Quaternion::ZeroOut(void)
{
  V3().ZeroOut();
  w = 1.0f;
}

bool Quaternion::Valid(void) const
{
  return Math::IsValid(x) && Math::IsValid(y) && 
         Math::IsValid(z) && Math::IsValid(w);
}

Vector3& Quaternion::V3()
{
  return *(Vector3*)this;
}

Vector4& Quaternion::V4()
{
  return *(Vector4*)this;
}

const Vector3& Quaternion::V3() const
{
  return *(Vector3*)this;
}

const Vector4& Quaternion::V4() const
{
  return *(Vector4*)this;
}

Quaternion operator*(float lhs, QuatParam rhs)
{
  return rhs * lhs;
}

void Normalize(QuatPtr quaternion)
{
  quaternion->Normalize();
}

Quaternion Normalized(QuatParam quaternion)
{
  return quaternion.Normalized();
}

float Dot(QuatParam lhs, QuatParam rhs)
{
  return lhs.Dot(rhs);
}

float Length(QuatParam quaternion)
{
  return quaternion.Length();
}

float LengthSq(QuatParam quaternion)
{
  return quaternion.LengthSq();
}

Quaternion Lerp(QuatParam start, QuatParam end, float tValue)
{
  if (!Math::InRange(tValue, 0.0f, 1.0f))
  WarnIf(!Math::InRange(tValue, 0.0f, 1.0f), 
         "Quaternion - Interpolation value is not in the range of [0, 1]");
  float alpha = tValue;
  float oneMinusAlpha = 1.0f - alpha;
  Quaternion quaternion(start.x * oneMinusAlpha + end.x * alpha,
                  start.y * oneMinusAlpha + end.y * alpha,
                  start.z * oneMinusAlpha + end.z * alpha,
                  start.w * oneMinusAlpha + end.w * alpha);
  Normalize(&quaternion);
  return quaternion;
}

Quaternion Slerp(QuatParam start, QuatParam end, float tValue)
{
  if (!Math::InRange(tValue, 0.0f, 1.0f))
  WarnIf(!Math::InRange(tValue, 0.0f, 1.0f), 
         "Quaternion - Interpolation value is not in the range of [0, 1]");

  //
  // Quaternion Interpolation With Extra Spins, pp. 96f, 461f
  // Jack Morrison, Graphics Gems III, AP Professional
  //

  const float cSlerpEpsilon = 0.00001f;

  bool flip;

  float cosTheta = Dot(start, end); 

  //Check to ensure that the shortest path is taken (cosine of the angle between 
  //the two quaternions is positive).
  if((flip = (cosTheta < 0.0f)))
  {
    cosTheta = -cosTheta;
  }

  float startVal, endVal;
  if((1.0f - cosTheta) > cSlerpEpsilon)
  {
    float theta = Math::ArcCos(cosTheta);
    float sinTheta = Math::Sin(theta);
    startVal = Math::Sin((1.0f - tValue) * theta) / sinTheta;
    endVal = Math::Sin(tValue * theta) / sinTheta;
  }
  else
  {
    startVal = 1.0f - tValue;
    endVal = tValue;
  }

  if(flip)
  {
    endVal = -endVal;
  }

  return Quaternion(startVal * start.x + endVal * end.x,
                    startVal * start.y + endVal * end.y,
                    startVal * start.z + endVal * end.z,
                    startVal * start.w + endVal * end.w);
}

Quaternion CreateDiagonalizer(Mat3Param matrix)
{
  const unsigned cMaxSteps = 50;
  const float cThetaLimit = 1.0e6f;

  Quaternion quat(0.0f, 0.0f, 0.0f, 1.0f);
  Matrix3 quatMatrix;
  Matrix3 diagMatrix;
  for(unsigned i = 0; i < cMaxSteps; ++i)
  {
    ToMatrix3(quat, &quatMatrix);
    diagMatrix = Concat(Concat(quatMatrix, matrix), quatMatrix.Transposed());

    //Elements not on the diagonal
    Vector3 offDiag(diagMatrix(1, 2), diagMatrix(0, 2), diagMatrix(0, 1));

    //Magnitude of the off-diagonal elements
    Vector3 magDiag = Abs(offDiag);

    //Index of the largest element 
    unsigned k = ((magDiag.x > magDiag.y) && (magDiag.x > magDiag.z)) ? 0 :
             ((magDiag.y > magDiag.z) ? 1 : 2);
    unsigned k1 = (k + 1) % 3;
    unsigned k2 = (k + 2) % 3;

    //Diagonal already
    if(offDiag[k] == 0.0f)
    {
      break;
    }

    float theta = (diagMatrix(k2, k2) - diagMatrix(k1, k1)) / 
                 (2.0f * offDiag[k]);
    float sign = Math::GetSign(theta);
    
    //Make theta positive
    theta *= sign;

    //Large term in T
    float thetaTerm = theta < 1e6f ? Math::Sqrt(Math::Sq(theta) + 1.0f)
                                       : theta;

    //Sign(T) / (|T| + sqrt(T^2 + 1))
    float t = sign / (theta + thetaTerm);

    //c = 1 / (t^2 + 1)      t = s / c
    float c = 1.0f / Math::Sqrt(Math::Sq(t) + 1.0f);

    //No room for improvement - reached machine precision.
    if(c == 1.0f)
    {
      break;
    }

    //Jacobi rotation for this iteration
    Quaternion jacobi(0.0f, 0.0f, 0.0f, 0.0f);

    //Using 1/2 angle identity sin(a/2) = sqrt((1-cos(a))/2)
    jacobi[k] = sign * Math::Sqrt((1.0f - c) / 2.0f);

    //Since our quat-to-matrix convention was for v*M instead of M*v
    jacobi.w = Math::Sqrt(1.0f - Math::Sq(jacobi[k]));

    //Reached limits of floating point precision
    if(jacobi.w == 1.0f)
    {
      break;
    }

    quat *= jacobi;
    Normalize(&quat);
  }

  return quat;
}

}// namespace Math
