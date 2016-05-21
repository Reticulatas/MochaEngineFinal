///////////////////////////////////////////////////////////////////////////////
///
/// \file MathFunctions.cpp
/// Central location for all the math used by the Zero engine.
/// 
/// Authors: Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "MathUtilities.h"
#include "MathFunctions.h"
#include "EulerOrder.h"
#include "stdlib.h"
#include <algorithm>

namespace Math
{
typedef Vector2    Vec2;
typedef Vector3    Vec3;
typedef Vector4    Vec4;
typedef Matrix3    Mat3;
typedef Matrix4    Mat4;

namespace
{
const unsigned I = 0; 
const unsigned J = 1; 
const unsigned H = 2;

void ClampAngle(float* angle)
{
  while(*angle < -Math::cPi)
  {
    *angle += Math::cTwoPi;
  }
  while(*angle > Math::cPi)
  {
    *angle -= Math::cTwoPi;
  }
}

} // namespace

///Creates a skew symmetric matrix from the given 3D vector. Multiplying a 
///vector by this matrix is equivalent to the cross product using the input 
///vector.
Matrix3 SkewSymmetric(Vec3Param vec3)
{
  Matrix3 mtx;
  mtx.m22 = mtx.m11 = mtx.m00 = 0.0f;
  mtx.m01 = -vec3[2];
  mtx.m02 =  vec3[1];
  mtx.m10 =  vec3[2];
  mtx.m12 = -vec3[0];
  mtx.m20 = -vec3[1];
  mtx.m21 =  vec3[0];
  return mtx;
}

Vector3 ProjectOut(Vec3Param vector, Vec3Param normal)
{
  float projCoef = Math::Dot(vector,normal);
  return vector - normal * projCoef;
}

///Converts a quaternion to an axis-angle pair (in radians). Axis is stored in 
///the Vector4's xyz and the angle is stored in the w.
Vector4 ToAxisAngle(QuatParam quaternion)
{
  Vector4 axisAngle;
  ToAxisAngle(quaternion, &axisAngle);
  return axisAngle;
}

void ToAxisAngle(QuatParam quaternion, Vec4Ptr axisAngle)
{
	if (axisAngle == NULL)
  ErrorIf(axisAngle == NULL, "Math - Null pointer passed for axis-angle pair.");
  Quaternion tempQuat(Normalized(quaternion));

  axisAngle->w = 2.0f * Math::ArcCos(tempQuat.w);
  float invSinAngle = Math::Sqrt(1.0f - tempQuat.w * tempQuat.w);

  if(Math::Abs(invSinAngle) < 0.0005f)
  {
    invSinAngle = 1.0f;
  }
  else
  {
    invSinAngle = 1.0f / invSinAngle;
  }
  axisAngle->x = tempQuat.x * invSinAngle;
  axisAngle->y = tempQuat.y * invSinAngle;
  axisAngle->z = tempQuat.z * invSinAngle;
}

///Converts a quaternion to an axis-angle pair (in radians).
void ToAxisAngle(QuatParam quaternion, Vec3Ptr axis, float* radians)
{
	if (axis == NULL)
  ErrorIf(axis == NULL, "Math - Null pointer passed for axis.");
	if (radians == NULL)
  ErrorIf(radians == NULL, "Math - Null pointer passed for radians.");
  Quaternion tempQuat(Normalized(quaternion));

  *radians = 2.0f * Math::ArcCos(tempQuat.w);
  float invSinAngle = Math::Sqrt(1.0f - tempQuat.w * tempQuat.w);

  if(Math::Abs(invSinAngle) < 0.0005f)
  {
    invSinAngle = 1.0f;
  }
  else
  {
    invSinAngle = 1.0f / invSinAngle;
  }
  axis->x = tempQuat.x * invSinAngle;
  axis->y = tempQuat.y * invSinAngle;
  axis->z = tempQuat.z * invSinAngle;
}

///Convert a 3x3 matrix to a set of Euler angles (in radians). The desired order
///of the rotations is expected to be in the given Euler angle structure.
EulerAngles ToEulerAngles(Mat3Param matrix, EulerOrders::Enum order)
{
  EulerAngles eulerAngles(order);
  ToEulerAngles(matrix, &eulerAngles);
  return eulerAngles;
}

void ToEulerAngles(Mat3Param matrix, EulerAnglesPtr eulerAngles)
{
	if (eulerAngles == NULL)
  ErrorIf(eulerAngles == NULL, "Math - Null pointer passed for Euler angles.");
  unsigned i, j, k, h, parity, repeated, frame;
  EulerOrder::GetOrder(eulerAngles->Order, i, j, k, h, parity, repeated, frame);
  if(EulerOrders::Yes == repeated)
  {
    float sy = Math::Sqrt(matrix(i, j) * matrix(i, j) + 
                         matrix(i, k) * matrix(i, k));
    if(sy > 16.0f * FLT_EPSILON)
    {
      (*eulerAngles)[cX] = Math::ArcTan2(matrix(i, j), matrix(i, k));
      (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
      (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), -matrix(k, i));
    }
    else
    {
      (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
      (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
      (*eulerAngles)[cZ] = 0.0f;
    }
  }
  else
  {
    float cy = Math::Sqrt(matrix(i, i) * matrix(i, i) + 
                         matrix(j, i) * matrix(j, i));
    if(cy > 16.0f * FLT_EPSILON)
    {
      (*eulerAngles)[cX] = Math::ArcTan2(matrix(k, j), matrix(k, k));
      (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
      (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), matrix(i, i));
    }
    else
    {
      (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
      (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
      (*eulerAngles)[cZ] = 0.0f;
    }
  }
  if(EulerOrders::Odd == parity)
  {
    (*eulerAngles)[cX] *= -1.0f;
    (*eulerAngles)[cY] *= -1.0f;
    (*eulerAngles)[cZ] *= -1.0f;
  }

  ClampAngle(&((*eulerAngles)[cX]));
  ClampAngle(&((*eulerAngles)[cY]));
  ClampAngle(&((*eulerAngles)[cZ]));

  if(EulerOrders::Rotated == frame)
  {
    Math::Swap((*eulerAngles)[cX], (*eulerAngles)[cZ]);
  }
}

///Convert a 4x4 matrix to a set of Euler angles in radians. The desired order
///of the rotations is expected to be in the given Euler angle structure.
EulerAngles ToEulerAngles(Mat4Param matrix, EulerOrders::Enum order)
{
  EulerAngles eulerAngles(order);
  ToEulerAngles(matrix, &eulerAngles);
  return eulerAngles;
}

void ToEulerAngles(Mat4Param matrix, EulerAnglesPtr eulerAngles)
{
	if(eulerAngles == NULL)
  ErrorIf(eulerAngles == NULL, "Math - Null pointer passed for Euler angles.");

  unsigned i, j, k, h, parity, repeated, frame;
  EulerOrder::GetOrder(eulerAngles->Order, i, j, k, h, parity, repeated, frame);
  if(EulerOrders::Yes == repeated)
  {
    float sy = Math::Sqrt(matrix(i, j) * matrix(i, j) + 
                         matrix(i, k) * matrix(i, k));
    if(sy > 16.0f * FLT_EPSILON)
    {
      (*eulerAngles)[cX] = Math::ArcTan2(matrix(i, j), matrix(i, k));
      (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
      (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), -matrix(k, i));
    }
    else
    {
      (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
      (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
      (*eulerAngles)[cZ] = 0.0f;
    }
  }
  else
  {
    float cy = Math::Sqrt(matrix(i, i) * matrix(i, i) + 
                         matrix(j, i) * matrix(j, i));
    if(cy > 16.0f * FLT_EPSILON)
    {
      (*eulerAngles)[cX] = Math::ArcTan2(matrix(k, j), matrix(k, k));
      (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
      (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), matrix(i, i));
    }
    else
    {
      (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
      (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
      (*eulerAngles)[cZ] = 0.0f;
    }
  }
  if(EulerOrders::Odd == parity)
  {
    (*eulerAngles)[cX] *= -1.0f;
    (*eulerAngles)[cY] *= -1.0f;
    (*eulerAngles)[cZ] *= -1.0f;
  }

  ClampAngle(&((*eulerAngles)[cX]));
  ClampAngle(&((*eulerAngles)[cY]));
  ClampAngle(&((*eulerAngles)[cZ]));

  if(EulerOrders::Rotated == frame)
  {
    Math::Swap((*eulerAngles)[cX], (*eulerAngles)[cZ]);
  }
}

///Convert a quaternion to a set of Euler angles (in radians). The desired order
///of the rotations is expected to be in the given Euler angle structure.
EulerAngles ToEulerAngles(QuatParam quaternion, EulerOrders::Enum order)
{
  EulerAngles eulerAngles(order);
  ToEulerAngles(quaternion, &eulerAngles);
  return eulerAngles;
}

void ToEulerAngles(QuatParam q/*QuatParam quaternion*/, EulerAnglesPtr eulerAngles)
{
	if (eulerAngles == NULL)
  ErrorIf(eulerAngles == NULL, "Math - Null pointer passed for Euler angles.");

  Matrix3 matrix;
  ToMatrix3(q, &matrix);
  ToEulerAngles(matrix, eulerAngles);
}

/// Converts from Vector3 to Vector2, removing the z component of the Vector3.
Vector2 ToVector2(Vec3Param v3)
{
  return Vector2(v3.x, v3.y);
}

/// Converts from Vector2 to Vector3, adding the given z component.
Vector3 ToVector3(Vec2Param v2, float z)
{
  return Vector3(v2.x, v2.y, z);
}

Math::Vector3 ToVector3(Vec4Param v4)
{
  return Vector3(v4.x, v4.y, v4.z);
}

Vector4 ToVector4( Vec3Param v3, float w )
{
  return Vector4(v3.x, v3.y, v3.z, w);
}

/// Turns the 3d transformation matrix into 2d.
Matrix3 ProjectXY(Mat4Param mat4)
{
  Matrix3 mat3;
  mat3.m00 = mat4.m00; 
  mat3.m01 = mat4.m01;
  mat3.m02 = mat4.m03; // Pull over the x translation

  mat3.m10 = mat4.m10;
  mat3.m11 = mat4.m11; 
  mat3.m12 = mat4.m13; // Pull over the y translation

  mat3.m20 = 0;
  mat3.m21 = 0;
  mat3.m22 = 1;

  return mat3;
}

///Converts an axis-angle pair to a 3x3 (in radians). Axis is stored in the
///Vector4's xyz and the angle is stored in the w. Axis is assumed to be 
///normalized.
Matrix3 ToMatrix3(Vec4Param axisAngle)
{
  Matrix3 matrix;
  ToMatrix3(axisAngle, &matrix);
  return matrix;
}

void ToMatrix3(Vec4Param axisAngle, Mat3Ptr matrix)
{
	if (matrix == NULL)
  ErrorIf(matrix == NULL, "Math - Null pointer passed for matrix.");

  float c0 = Math::Cos(axisAngle.w);
  float n1C0 = 1.0f - c0;
  float s0 = Math::Sin(axisAngle.w);

  //| x^2(1-c0)+c0  xy(1-c0)-zs0  xz(1-c0)+ys0 |
  //| xy(1-c0)+zs0  y^2(1-c0)+c0  yz(1-c0)-xs0 |
  //| xz(1-c0)-ys0  yz(1-c0)+xs0  z^2(1-c0)+c0 |
  matrix->m00 = axisAngle.x * axisAngle.x * n1C0 + c0;
  matrix->m01 = axisAngle.x * axisAngle.y * n1C0 - axisAngle.z * s0;
  matrix->m02 = axisAngle.x * axisAngle.z * n1C0 + axisAngle.y * s0;

  matrix->m10 = axisAngle.x * axisAngle.y * n1C0 + axisAngle.z * s0;
  matrix->m11 = axisAngle.y * axisAngle.y * n1C0 + c0;
  matrix->m12 = axisAngle.y * axisAngle.z * n1C0 - axisAngle.x * s0;

  matrix->m20 = axisAngle.x * axisAngle.z * n1C0 - axisAngle.y * s0;
  matrix->m21 = axisAngle.y * axisAngle.z * n1C0 + axisAngle.x * s0;
  matrix->m22 = axisAngle.z * axisAngle.z * n1C0 + c0;
}

///Converts an axis-angle pair to a 3x3 matrix (in radians). Axis is assumed to
///be normalized.
Matrix3 ToMatrix3(Vec3Param axis, float radians)
{
  Matrix3 matrix;
  ToMatrix3(axis, radians, &matrix);
  return matrix;
}

void ToMatrix3(Vec3Param axis, float radians, Mat3Ptr matrix)
{
	if(matrix == NULL)
  ErrorIf(matrix == NULL, "Math - Null pointer passed for matrix.");

  float c0 = Math::Cos(radians);
  float n1C0 = 1.0f - c0;
  float s0 = Math::Sin(radians);

  //| x^2(1-c0)+c0  xy(1-c0)-zs0  xz(1-c0)+ys0 |
  //| xy(1-c0)+zs0  y^2(1-c0)+c0  yz(1-c0)-xs0 |
  //| xz(1-c0)-ys0  yz(1-c0)+xs0  z^2(1-c0)+c0 |
  matrix->m00 = axis.x * axis.x * n1C0 + c0;
  matrix->m01 = axis.x * axis.y * n1C0 - axis.z * s0;
  matrix->m02 = axis.x * axis.z * n1C0 + axis.y * s0;

  matrix->m10 = axis.x * axis.y * n1C0 + axis.z * s0;
  matrix->m11 = axis.y * axis.y * n1C0 + c0;
  matrix->m12 = axis.y * axis.z * n1C0 - axis.x * s0;

  matrix->m20 = axis.x * axis.z * n1C0 - axis.y * s0;
  matrix->m21 = axis.y * axis.z * n1C0 + axis.x * s0;
  matrix->m22 = axis.z * axis.z * n1C0 + c0;
}

///Convert a set of Euler angles to a 3x3 matrix (in radians).
Matrix3 ToMatrix3(EulerAnglesParam eulerAngles)
{
  Matrix3 matrix;
  ToMatrix3(eulerAngles, &matrix);
  return matrix;
}

void ToMatrix3(EulerAnglesParam eulerAngles, Mat3Ptr matrix)
{
	if (matrix == NULL)
  ErrorIf(matrix == NULL, "Math - Null pointer passed for matrix.");

  float angles[3] = { eulerAngles[0], eulerAngles[1], eulerAngles[2] };
  unsigned i, j, k, h, parity, repeated, frame;
  EulerOrder::GetOrder(eulerAngles.Order, i, j, k, h, parity, repeated, frame);
  if(EulerOrders::Rotated == frame)
  {
    Math::Swap(angles[cX], angles[cZ]);
  }
  if(EulerOrders::Odd == parity)
  {
    angles[cX] *= -1.0f;
    angles[cY] *= -1.0f;
    angles[cZ] *= -1.0f;
  }
  
  float t[3], c[3], s[3];
  t[I] = angles[cX];      t[J] = angles[cY];      t[H] = angles[cZ];
  c[I] = Math::Cos(t[I]); c[J] = Math::Cos(t[J]); c[H] = Math::Cos(t[H]);
  s[I] = Math::Sin(t[I]); s[J] = Math::Sin(t[J]); s[H] = Math::Sin(t[H]);

  const float cc = c[I] * c[H]; 
  const float cs = c[I] * s[H]; 
  const float sc = s[I] * c[H]; 
  const float ss = s[I] * s[H];
  if(EulerOrders::Yes == repeated)
  {
    (*matrix)(i, i) =  c[J];        
    (*matrix)(i, j) =  c[J] * s[I];
    (*matrix)(i, k) =  c[J] * c[I];

    (*matrix)(j, i) =  c[J] * s[H];
    (*matrix)(j, j) = -c[J] * ss + cc;
    (*matrix)(j, k) = -c[J] * cs - sc;

    (*matrix)(k, i) = -c[J] * c[H];
    (*matrix)(k, j) =  c[J] * sc + cs;
    (*matrix)(k, k) =  c[J] * cc - ss;
  } 
  else 
  {
    (*matrix)(i, i) =  c[J] * c[H];
    (*matrix)(j, i) =  c[J] * s[H];
    (*matrix)(k, i) = -s[J];

    (*matrix)(i, j) =  s[J] * sc - cs;
    (*matrix)(j, j) =  s[J] * ss + cc;
    (*matrix)(k, j) =  c[J] * s[I];

    (*matrix)(i, k) =  s[J] * cc + ss;
    (*matrix)(j, k) =  s[J] * cs - sc;
    (*matrix)(k, k) =  c[J] * c[I];
  }
}

///Convert a 4x4 matrix to a 3x3 matrix. Simply copies the 4x4 matrix's upper 
///3x3 matrix (rotation & scale) to the 3x3 matrix.
Matrix3 ToMatrix3(Mat4Param matrix)
{
  Matrix3 mat3;
  ToMatrix3(matrix, &mat3);
  return mat3;
}

void ToMatrix3(Mat4Param mat4, Mat3Ptr mat3)
{
	if (mat3 == NULL)
  ErrorIf(mat3 == NULL, "Math - Null pointer passed for matrix.");

  //First "cross" components
  mat3->m00 = mat4.m00; 
  mat3->m01 = mat4.m01;
  mat3->m02 = mat4.m02;

  //Second "cross" components
  mat3->m10 = mat4.m10;
  mat3->m11 = mat4.m11; 
  mat3->m12 = mat4.m12;

  //Third "cross" components
  mat3->m20 = mat4.m20;
  mat3->m21 = mat4.m21;
  mat3->m22 = mat4.m22;
}

///Converts a quaternion to a 3x3 rotation matrix (in radians).
Matrix3 ToMatrix3(QuatParam quaternion)
{
  Matrix3 matrix;
  ToMatrix3(quaternion, &matrix);
  return matrix;
}

void ToMatrix3(QuatParam quaternion, Mat3Ptr matrix)
{
	if (matrix == NULL)
  ErrorIf(matrix == NULL, "Math - Null pointer passed for matrix.");

  //     |       2     2                                |
  //     | 1 - 2Y  - 2Z    2XY - 2ZW      2XZ + 2YW     |
  //     |                                              |
  //     |                       2     2                |
  // M = | 2XY + 2ZW       1 - 2X  - 2Z   2YZ - 2XW     |
  //     |                                              |
  //     |                                      2     2 |
  //     | 2XZ - 2YW       2YZ + 2XW      1 - 2X  - 2Y  |
  //     |                                              |

  float xx = quaternion.x * quaternion.x;
  float xy = quaternion.x * quaternion.y;
  float xz = quaternion.x * quaternion.z;
  float yy = quaternion.y * quaternion.y;
  float yz = quaternion.y * quaternion.z;
  float zz = quaternion.z * quaternion.z;
  float zw = quaternion.z * quaternion.w;
  float yw = quaternion.y * quaternion.w;
  float xw = quaternion.x * quaternion.w;

  matrix->m00 = 1.0f - 2.0f * (yy + zz);
  matrix->m01 = 2.0f * (xy - zw);
  matrix->m02 = 2.0f * (xz + yw);

  matrix->m10 = 2.0f * (xy + zw);
  matrix->m11 = 1.0f - 2.0f * (xx + zz);
  matrix->m12 = 2.0f * (yz - xw);

  matrix->m20 = 2.0f * (xz - yw);
  matrix->m21 = 2.0f * (yz + xw);
  matrix->m22 = 1.0f - 2.0f * (xx + yy);
}

///Convert a set of Euler angles to a 4x4 matrix (in radians).
Matrix4 ToMatrix4(EulerAnglesParam eulerAngles)
{
  Matrix4 matrix;
  ToMatrix4(eulerAngles, &matrix);
  return matrix;
}

void ToMatrix4(EulerAnglesParam eulerAngles, Mat4Ptr matrix)
{
  if(matrix == NULL)
  ErrorIf(matrix == NULL, "Math - Null pointer passed for matrix.");

  float angles[3] = { eulerAngles[0], eulerAngles[1], eulerAngles[2] };
  unsigned i, j, k, h, parity, repeated, frame;
  EulerOrder::GetOrder(eulerAngles.Order, i, j, k, h, parity, repeated, frame);
  if(EulerOrders::Rotated == frame)
  {
    Math::Swap(angles[cX], angles[cZ]);
  }
  if(EulerOrders::Odd == parity)
  {
    angles[cX] *= -1.0f;
    angles[cY] *= -1.0f;
    angles[cZ] *= -1.0f;
  }
  float t[3], c[3], s[3];
  t[I] = angles[cX];       t[J] = angles[cY];       t[H] = angles[cZ];
  c[I] = Math::Cos(t[I]); c[J] = Math::Cos(t[J]); c[H] = Math::Cos(t[H]);
  s[I] = Math::Sin(t[I]); s[J] = Math::Sin(t[J]); s[H] = Math::Sin(t[H]);
  float cc = c[I] * c[H]; 
  float cs = c[I] * s[H]; 
  float sc = s[I] * c[H]; 
  float ss = s[I] * s[H];
  if(EulerOrders::Yes == repeated)
  {
    (*matrix)(i, i) =  c[J];        
    (*matrix)(i, j) =  c[J] * s[I];
    (*matrix)(i, k) =  c[J] * c[I];

    (*matrix)(j, i) =  c[J] * s[H];
    (*matrix)(j, j) = -c[J] * ss + cc;
    (*matrix)(j, k) = -c[J] * cs - sc;

    (*matrix)(k, i) = -c[J] * c[H];
    (*matrix)(k, j) =  c[J] * sc + cs;
    (*matrix)(k, k) =  c[J] * cc - ss;
  } 
  else 
  {
    (*matrix)(i, i) =  c[J] * c[H];
    (*matrix)(j, i) =  c[J] * s[H];
    (*matrix)(k, i) = -s[J];

    (*matrix)(i, j) =  s[J] * sc - cs;
    (*matrix)(j, j) =  s[J] * ss + cc;
    (*matrix)(k, j) =  c[J] * s[I];

    (*matrix)(i, k) =  s[J] * cc + ss;
    (*matrix)(j, k) =  s[J] * cs - sc;
    (*matrix)(k, k) =  c[J] * c[I];
  }
  matrix->m03 = 0.0f;  matrix->m13 = 0.0f;  matrix->m23 = 0.0f;
  matrix->m30 = 0.0f;  matrix->m31 = 0.0f;  matrix->m32 = 0.0f;
  matrix->m33 = 1.0f;
}

///Convert a 3x3 matrix to a 4x4 matrix. Simply copies the 3x3 matrix's values
///into the rotational part of the 4x4 matrix.
Matrix4 ToMatrix4(Mat3Param matrix)
{
  Matrix4 matrix4;
  ToMatrix4(matrix, &matrix4);
  return matrix4;
}

void ToMatrix4(Mat3Param mat3, Mat4Ptr mat4)
{
	if (mat4 == NULL)
  ErrorIf(mat4 == NULL, "Math - Null pointer passed for matrix.");

  //First "cross" components
  mat4->m00 = mat3.m00;
  mat4->m01 = mat3.m01;   
  mat4->m02 = mat3.m02;
  mat4->m03 = 0.0f;
  
  //Second "cross" components
  mat4->m10 = mat3.m10;
  mat4->m11 = mat3.m11;   
  mat4->m12 = mat3.m12;
  mat4->m13 = 0.0f;
  
  //Third "cross" components
  mat4->m20 = mat3.m20;
  mat4->m21 = mat3.m21;   
  mat4->m22 = mat3.m22;
  mat4->m23 = 0.0f;
  
  //Fourth "cross" components
  mat4->m30 = 0.0f;
  mat4->m31 = 0.0f;  
  mat4->m32 = 0.0f;
  mat4->m33 = 1.0f;
}

///Converts a quaternion to a 4x4 rotation matrix (in radians).
Matrix4 ToMatrix4(QuatParam quaternion)
{
  Matrix4 matrix;
  ToMatrix4(quaternion, &matrix);
  return matrix;
}

void ToMatrix4(QuatParam quaternion, Mat4Ptr matrix)
{
	if (matrix == NULL)
  ErrorIf(matrix == NULL, "Math - Null pointer passed for matrix.");

  //     |       2     2                                |
  //     | 1 - 2Y  - 2Z    2XY - 2ZW      2XZ + 2YW     |
  //     |                                              |
  //     |                       2     2                |
  // M = | 2XY + 2ZW       1 - 2X  - 2Z   2YZ - 2XW     |
  //     |                                              |
  //     |                                      2     2 |
  //     | 2XZ - 2YW       2YZ + 2XW      1 - 2X  - 2Y  |
  //     |                                              |

  float xx = quaternion.x * quaternion.x;
  float xy = quaternion.x * quaternion.y;
  float xz = quaternion.x * quaternion.z;
  float yy = quaternion.y * quaternion.y;
  float yz = quaternion.y * quaternion.z;
  float zz = quaternion.z * quaternion.z;
  float zw = quaternion.z * quaternion.w;
  float yw = quaternion.y * quaternion.w;
  float xw = quaternion.x * quaternion.w;

  matrix->m00 = 1.0f - 2.0f * (yy + zz);
  matrix->m01 = 2.0f * (xy - zw);
  matrix->m02 = 2.0f * (xz + yw);
  matrix->m03 = 0.0f;

  matrix->m10 = 2.0f * (xy + zw);
  matrix->m11 = 1.0f - 2.0f * (xx + zz);
  matrix->m12 = 2.0f * (yz - xw);
  matrix->m13 = 0.0f;

  matrix->m20 = 2.0f * (xz - yw);
  matrix->m21 = 2.0f * (yz + xw);
  matrix->m22 = 1.0f - 2.0f * (xx + yy);
  matrix->m23 = 0.0f;

  matrix->m30 = 0.0f;
  matrix->m31 = 0.0f;
  matrix->m32 = 0.0f;
  matrix->m33 = 1.0f;
}

///Converts a 3D vector to a quaternion.
Quaternion ToQuaternion(Vec3Param vector)
{
  Quaternion quaternion;
  ToQuaternion(vector, &quaternion);
  return quaternion;
}

void ToQuaternion(Vec3Param vector, QuatPtr quaternion)
{
	if (quaternion == NULL)
  ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

  quaternion->Set(vector.x, vector.y, vector.z, 0.0f);
}

///Converts an axis-angle pair to a quaternion (in radians). Axis is stored in
///the Vector4's xyz and the angle is stored in the w. Axis is assumed to be 
///normalized.
Quaternion ToQuaternion(Vec4Param axisAngle)
{
  Quaternion quaternion;
  ToQuaternion(axisAngle, &quaternion);
  return quaternion;
}

void ToQuaternion(Vec4Param axisAngle, QuatPtr quaternion)
{
	if (quaternion == NULL)
  ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

  float alpha = axisAngle.w * 0.5f;
  float sinAlpha = Math::Sin(alpha);

  quaternion->x = axisAngle.x * sinAlpha;
  quaternion->y = axisAngle.y * sinAlpha;
  quaternion->z = axisAngle.z * sinAlpha;
  quaternion->w = Math::Cos(alpha);
}

///Converts an axis-angle pair to a quaternion (in radians). Axis is assumed to
///be normalized.
Quaternion ToQuaternion(Vec3Param axis, float radians)
{
  Quaternion quaternion;
  ToQuaternion(axis, radians, &quaternion);
  return quaternion;
}

void ToQuaternion(Vec3Param axis, float radians, QuatPtr quaternion)
{
	if (quaternion == NULL)
  ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

  float alpha = radians * 0.5f;
  float sinAlpha = Math::Sin(alpha);

  quaternion->x = axis.x * sinAlpha;
  quaternion->y = axis.y * sinAlpha;
  quaternion->z = axis.z * sinAlpha;
  quaternion->w = Math::Cos(alpha);
}

///Convert a set of Euler angles to a quaternion (in radians).
Quaternion ToQuaternion(EulerAnglesParam eulerAngles)
{
  Quaternion quaternion;
  ToQuaternion(eulerAngles, &quaternion);
  return quaternion;
}

void ToQuaternion(EulerAnglesParam eulerAngles, QuatPtr quaternion)
{
	if (quaternion == NULL)
  ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

  float angles[3] = { eulerAngles[cX], eulerAngles[cY], eulerAngles[cZ] };
  unsigned i, j, k, h, parity, repeated, frame;
  EulerOrder::GetOrder(eulerAngles.Order, i, j, k, h, parity, repeated, frame);
  if(EulerOrders::Rotated == frame)
  {
    Math::Swap(angles[cX], angles[cZ]);
  }

  if(EulerOrders::Odd == parity)
  {
    angles[cY] *= -1.0f;
  }

  float t[3], c[3], s[3];
  t[I] = angles[cX] * 0.5f; c[I] = Math::Cos(t[I]); s[I] = Math::Sin(t[I]);
  t[J] = angles[cY] * 0.5f; c[J] = Math::Cos(t[J]); s[J] = Math::Sin(t[J]);
  t[H] = angles[cZ] * 0.5f; c[H] = Math::Cos(t[H]); s[H] = Math::Sin(t[H]);
  
  const float cc = c[I] * c[H];
  const float cs = c[I] * s[H];
  const float sc = s[I] * c[H];
  const float ss = s[I] * s[H];
  if(EulerOrders::Yes == repeated)
  {
    angles[i] = c[J] * (cs + sc);
    angles[j] = s[J] * (cc + ss);
    angles[k] = s[J] * (cs - sc);
    quaternion->w = c[J] * (cc - ss);
  }
  else
  {
    angles[i] = c[J] * sc - s[J] * cs;
    angles[j] = c[J] * ss + s[J] * cc;
    angles[k] = c[J] * cs - s[J] * sc;
    quaternion->w = c[J] * cc + s[J] * ss;
  }
  if(EulerOrders::Odd == parity)
  {
    angles[j] *= -1.0f;
  }
  quaternion->x = angles[cX];
  quaternion->y = angles[cY];
  quaternion->z = angles[cZ];
}

///Converts a 3x3 matrix to a quaternion (in radians).
Quaternion ToQuaternion(Mat3Param matrix)
{
  Quaternion quaternion;
  ToQuaternion(matrix, &quaternion);
  return quaternion;
}

void ToQuaternion(Mat3Param matrix, QuatPtr quaternion)
{
	if (quaternion == NULL)
  ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

  if(matrix.m00 + matrix.m11 + matrix.m22 > 0.0f)
  {
    float t = matrix.m00 + matrix.m11 + matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;

    (*quaternion)[3] = s * t;
    (*quaternion)[2] = (matrix.m10 - matrix.m01) * s;
    (*quaternion)[1] = (matrix.m02 - matrix.m20) * s;
    (*quaternion)[0] = (matrix.m21 - matrix.m12) * s;
  }
  else if(matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22)
  {
    float t = matrix.m00 - matrix.m11 - matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;

    (*quaternion)[0] = s * t;
    (*quaternion)[1] = (matrix.m10 + matrix.m01) * s;
    (*quaternion)[2] = (matrix.m02 + matrix.m20) * s;
    (*quaternion)[3] = (matrix.m21 - matrix.m12) * s;
  }
  else if(matrix.m11 > matrix.m22)
  {
    float t = -matrix.m00 + matrix.m11 - matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;
    
    (*quaternion)[1] = s * t;
    (*quaternion)[0] = (matrix.m10 + matrix.m01) * s;
    (*quaternion)[3] = (matrix.m02 - matrix.m20) * s;
    (*quaternion)[2] = (matrix.m21 + matrix.m12) * s;
  }
  else
  {
    float t = -matrix.m00 - matrix.m11 + matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;

    (*quaternion)[2] = s * t;
    (*quaternion)[3] = (matrix.m10 - matrix.m01) * s;
    (*quaternion)[0] = (matrix.m02 + matrix.m20) * s;
    (*quaternion)[1] = (matrix.m21 + matrix.m12) * s;
  }
}

///Converts a 4x4 matrix to a quaternion (in radians).
Quaternion ToQuaternion(Mat4Param matrix)
{
  Quaternion quaternion;
  ToQuaternion(matrix, &quaternion);
  return quaternion;
}

void ToQuaternion(Mat4Param matrix, QuatPtr quaternion)
{
	if (quaternion == NULL)
  ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

  if(matrix.m00 + matrix.m11 + matrix.m22 > 0.0f)
  {
    float t = matrix.m00 + matrix.m11 + matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;

    (*quaternion)[3] = s * t;
    (*quaternion)[2] = (matrix.m10 - matrix.m01) * s;
    (*quaternion)[1] = (matrix.m02 - matrix.m20) * s;
    (*quaternion)[0] = (matrix.m21 - matrix.m12) * s;
  }
  else if(matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22)
  {
    float t = matrix.m00 - matrix.m11 - matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;

    (*quaternion)[0] = s * t;
    (*quaternion)[1] = (matrix.m10 + matrix.m01) * s;
    (*quaternion)[2] = (matrix.m02 + matrix.m20) * s;
    (*quaternion)[3] = (matrix.m21 - matrix.m12) * s;
  }
  else if(matrix.m11 > matrix.m22)
  {
    float t = -matrix.m00 + matrix.m11 - matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;
    
    (*quaternion)[1] = s * t;
    (*quaternion)[0] = (matrix.m10 + matrix.m01) * s;
    (*quaternion)[3] = (matrix.m02 - matrix.m20) * s;
    (*quaternion)[2] = (matrix.m21 + matrix.m12) * s;
  }
  else
  {
    float t = -matrix.m00 - matrix.m11 + matrix.m22 + 1.0f;
    float s = Math::Rsqrt(t) * 0.5f;

    (*quaternion)[2] = s * t;
    (*quaternion)[3] = (matrix.m10 - matrix.m01) * s;
    (*quaternion)[0] = (matrix.m02 + matrix.m20) * s;
    (*quaternion)[1] = (matrix.m21 + matrix.m12) * s;
  }
}

///Generates a set of orthonormal vectors from the given vectors, modifying u 
///and v.
void GenerateOrthonormalBasis(Vec3Param w, Vec3Ptr u, Vec3Ptr v)
{
	if (u == NULL)
  ErrorIf(u == NULL, "Math - Null pointer passed for vector U.");
	if (v == NULL)
  ErrorIf(v == NULL, "Math - Null pointer passed for vector V.");

  if((Math::Abs(w.x) >= Math::Abs(w.y)) && (Math::Abs(w.x) >= Math::Abs(w.z)))
  {
    u->x = -w.y;
    u->y = w.x;
    u->z = 0.0f;
  }
  else
  {
    u->x = 0.0f;
    u->y = w.z;
    u->z = -w.y;
  }
  Normalize(u);
  *v = Cross(w, *u);
  Normalize(v);
}

///Generates a set of orthonormal vectors from the given vectors while using 
///debug checks, modifies u and v
void DebugGenerateOrthonormalBasis(Vec3Param w, Vec3Ptr u, Vec3Ptr v)
{
	if (u == NULL)
  ErrorIf(u == NULL, "Math - Null pointer passed for vector U.");
	if (v == NULL)
  ErrorIf(v == NULL, "Math - Null pointer passed for vector V.");

  if((Math::Abs(w.x) >= Math::Abs(w.y)) && (Math::Abs(w.x) >= Math::Abs(w.z)))
  {
    u->x = -w.y;
    u->y = w.x;
    u->z = 0.0f;
  }
  else
  {
    u->x = 0.0f;
    u->y = w.z;
    u->z = -w.y;
  }
  AttemptNormalize(u);
  *v = Cross(w, *u);
  AttemptNormalize(v);
}

///Perform the dot product with the specified basis vector of the 3x3 matrix and
///the given vector.
float BasisNDot(Mat3Param matrix, unsigned basisIndex, Vec3Param vector)
{
  return Dot(vector, matrix.Basis(basisIndex));
}

///Perform the dot product with the specified basis vector of the 3x3 matrix and
///the given vector.
float BasisNDot(Mat4Param matrix, unsigned basisIndex, Vec4Param vector)
{
  return Dot(vector, matrix.Basis(basisIndex));
}

///Perform the dot product with the basis vector of the 3x3 matrix that 
///represents the x-axis (if the matrix is pure rotation) and the given vector.
float BasisXDot(Mat3Param matrix, Vec3Param vector)
{
  return Dot(vector, matrix.BasisX());
}

///Perform the dot product with the basis vector of the 3x3 matrix that 
///represents the y-axis (if the matrix is pure rotation) and the given vector.
float BasisYDot(Mat3Param matrix, Vec3Param vector)
{
  return Dot(vector, matrix.BasisY());
}

///Perform the dot product with the basis vector of the 3x3 matrix that 
///represents the z-axis (if the matrix is pure rotation) and the given vector.
float BasisZDot(Mat3Param matrix, Vec3Param vector)
{
  return Dot(vector, matrix.BasisZ());
}

///Perform the dot product with the basis vector of the 4x4 matrix that 
///represents the x-axis (if the matrix is pure rotation) and the given vector.
float BasisXDot(Mat4Param matrix, Vec4Param vector)
{
  return Dot(vector, matrix.BasisX());
}

///Perform the dot product with the basis vector of the 4x4 matrix that 
///represents the y-axis (if the matrix is pure rotation) and the given vector.
float BasisYDot(Mat4Param matrix, Vec4Param vector)
{
  return Dot(vector, matrix.BasisY());
}

///Perform the dot product with the basis vector of the 4x4 matrix that 
///represents the z-axis (if the matrix is pure rotation) and the given vector.
float BasisZDot(Mat4Param matrix, Vec4Param vector)
{
  return Dot(vector, matrix.BasisZ());
}

//----------------------------------------------------------- Rotation Functions
float Angle(Vec2Param a, Vec2Param b)
{
  float dot = Dot(a, b);
  return Math::ArcCos(dot);
}

float Angle(Vec3Param a, Vec3Param b)
{
  float dot = Dot(a, b);
  return Math::ArcCos(dot);
}

float Angle(QuatParam a, QuatParam b)
{
  float dot = Dot(a, b);
  if(dot == 0.0f)
    return 0.0f;
  float angle = 2.0f * Math::ArcCos(dot);
  return angle;
}

Vector2 Slerp(Vec2Param start, Vec2Param end, float t)
{
  float dot = Dot(start, end);
  float theta = Math::ArcCos(dot) * t;
  Vector2 relativeVec = end - start * dot;
  relativeVec.Normalize();
  return (start * Math::Cos(theta)) + (relativeVec * Math::Sin(theta));
}

Vector3 Slerp(Vec3Param start, Vec3Param end, float t)
{
  float dot = Dot(start, end);
  float theta = Math::ArcCos(dot) * t;
  Vector3 relativeVec = end - start * dot;
  relativeVec.Normalize();
  return (start * Math::Cos(theta)) + (relativeVec * Math::Sin(theta));
}

template<typename type>
type GenericTowards(const type& a, const type& b, float maxAngle)
{
  const float cAngleEpsilon = 0.0000001f;

  float angle = Angle(a, b);

  if(angle > Math::cPi)
  {
    angle -= Math::cTwoPi;
  }

  angle = Math::Abs(angle);
  if(angle > cAngleEpsilon)
  {
    float t = maxAngle / angle;
    if(t > 1.0f)
    {
      t = 1.0f;
    }
    return Slerp(a, b, t);
  }
  else
  {
    return b;
  }
}

Vector2 RotateTowards(Vec2Param a, Vec2Param b, float maxAngle)
{
  Vec2 an = a.Normalized();
  Vec2 bn = b.Normalized();
  return GenericTowards(an, bn, maxAngle);
}

Vector3 RotateTowards(Vec3Param a, Vec3Param b, float maxAngle)
{
  Vec3 an = a.Normalized();
  Vec3 bn = b.Normalized();
  return GenericTowards(an, bn, maxAngle);
}

Quaternion RotateTowards(QuatParam a, QuatParam b, float maxAngle)
{
  return GenericTowards(a, b, maxAngle);
}

// Get the rotation angle between two vectors (in radians)
float SignedAngle(Vec3Param a, Vec3Param b, Vec3Param up)
{
  // Get the right vector
  Vec3 right = Math::Cross(a, up);
  right.AttemptNormalize();

  // Get the forward and right dot products
  float forwardDot = Math::Clamp(Math::Dot(a, b), -1.0f, 1.0f);
  float rightDot = Math::Clamp(Math::Dot(right, b), -1.0f, 1.0f);

  // Get the actual angle from the forward dot product
  float finalAngle = Math::ArcCos(forwardDot);

  // If we're actually on the left side...
  if(rightDot > 0.0f)
  {
    // Compute the float final angle given the quadrant it's in (kinda like atan2)
    finalAngle = -finalAngle;
  }

  // Return the finally computed angle
  return finalAngle;
}

float RandomFloat(float a, float b)
{
  float max = std::max<float>(a,b);
  float min = std::min<float>(a,b);

  float random = ((float) rand()) / (float) RAND_MAX;
  float range = max - min;  
  return (random*range) + min;
}

// Calculate the view transform matrix for Left Handed Coordinate System
Math::Matrix4 LookAtLH(Vec3Param eye, Vec3Param at, Vec3Param up)
{
  Vector3 z = (at - eye).Normalized();
  Vector3 x = up.Cross(z).Normalized();
  Vector3 y = z.Cross(x).Normalized();

  Matrix4 m(x.x, x.y, x.z, -(x.Dot(eye)),
            y.x, y.y, y.z, -(y.Dot(eye)),
            z.x, z.y, z.z, -(z.Dot(eye)),
            0,   0,   0,    1);
  return m;
}

Math::Matrix4 OrthographicLH(float width, float height, float znear, float zfar)
{
  Matrix4 ortho;

  ortho(0, 0) = 2 / width;
  ortho(1, 1) = 2 / height;
  ortho(2, 2) = 1 / (zfar - znear);
  ortho(2, 3) = znear / (znear - zfar);

  return ortho;
}

Matrix4 OrthographicLH_VR(Mat4Param projection, Vec2Param orthoScale, float orthoDistance, float hmdToEyeViewOffsetX)
{
  float orthoHorizontalOffset = hmdToEyeViewOffsetX / orthoDistance;

  Matrix4 ortho;
  ortho.m00 = projection.m00 * orthoScale.x;
  ortho.m01 = 0.0f;
  ortho.m02 = 0.0f;
  ortho.m03 = -projection.m02 + (orthoHorizontalOffset * projection.m00);

  ortho.m10 = 0.0f;
  ortho.m11 = projection.m11 * orthoScale.y;       // Note sign flip (text rendering uses Y=down).
  ortho.m12 = 0.0f;
  ortho.m13 = -projection.m12;

  // MA: Undo effect of sign
  ortho.m20 = 0.0f;
  ortho.m21 = 0.0f;
  ortho.m22 = projection.m22 * projection.m32 * -1.0f; // reverse right-handedness
  //ortho.M[2][2] = 0.0f;
  ortho.m23 = 0.0f;
  //projection.M[2][3];

  // No perspective correction for ortho.
  ortho.m30 = 0.0f;
  ortho.m31 = 0.0f;
  ortho.m32 = 0.0f;
  ortho.m33 = 1.0f;

  return ortho;
}

Math::Matrix4 PerspectiveLH(float fovY, float aspect, float znear, float zfar)
{
  float yScale = 1 / Math::Tan(fovY/ 2);
  float xScale = yScale / aspect;

  Matrix4 persp;

  persp(0, 0) = xScale;
  persp(1, 1) = yScale;
  persp(2, 2) = zfar / (zfar - znear);
  persp(2, 3) = -znear * zfar / (zfar - znear);
  persp(3, 2) = 1;
  persp(3, 3) = 0;

  return persp;
}

void ClampEulerAngles(EulerAngles& ea, float angle)
{
  ea.Angles.x = Math::FMod(ea.Angles.x, angle);
  ea.Angles.y = Math::FMod(ea.Angles.y, angle);
  ea.Angles.z = Math::FMod(ea.Angles.z, angle);

  if (Equal(ea.Angles.x, 0))
    ea.Angles.x = 0;
  if (Equal(ea.Angles.y, 0))
    ea.Angles.y = 0;
  if (Equal(ea.Angles.z, 0))
    ea.Angles.z = 0;
}

EulerAngles EulerAnglesDegree(const EulerAnglesParam angle)
{
  EulerAngles ea = angle;
  ea.Angles.x = RadToDeg(ea.Angles.x);
  ea.Angles.y = RadToDeg(ea.Angles.y);
  ea.Angles.z = RadToDeg(ea.Angles.z);
  return ea;
}

EulerAngles EulerAnglesRadian(const EulerAnglesParam angle)
{
  EulerAngles ea = angle;
  ea.Angles.x = DegToRad(ea.Angles.x);
  ea.Angles.y = DegToRad(ea.Angles.y);
  ea.Angles.z = DegToRad(ea.Angles.z);
  return ea;
}

}// namespace Math
