/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once

//#include <Windows.h>
//#include <xnamath.h>
//#include "Vector.h"
//#include "Matrix.h"
//#include "Point.h"
#include "Rectangle.h"
#include <Math/Math.h>

using namespace Math;

typedef Vector4 Vector;
typedef Vector4 Point4;
typedef Vector4 Point3;
typedef Vector2 MPoint2;
typedef Matrix4 Matrix;
typedef Vector3 Vec3;
typedef Matrix3 Mat3;
// integers
typedef unsigned char       uint8;
typedef   signed char        int8;
typedef unsigned short     uint16;
typedef   signed short      int16;
typedef unsigned int       uint32;
typedef   signed int        int32;
typedef unsigned long long uint64;
typedef   signed long long  int64;
typedef unsigned int         uint;
//typedef Vector4 Vector;
//typedef Point4 Point3;
//typedef Point4 MPoint2;
//typedef Matrix4 Matrix;
//
///* Custom operators */
//static bool operator==(const XMFLOAT3& a, const XMFLOAT3& b)
//{
//	return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
//}
//static XMFLOAT3 operator-(const XMFLOAT3& a, const XMFLOAT3& b)
//{
//	XMFLOAT3 v(a.x - b.x, a.y - b.y, a.z - b.z);
//	return v;
//}
//static XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b)
//{
//	XMFLOAT3 v(a.x + b.x, a.y + b.y, a.z + b.z);
//	return v;
//}
//static XMFLOAT3 operator*(const XMFLOAT3& a, const float& b)
//{
//	XMFLOAT3 v(a.x*b,a.y*b,a.z*b);
//	return v;
//}
//
//static void VecStoreXMFloat3(XMFLOAT3* dest, Vector v)
//{
//  dest->x = v.x;
//  dest->y = v.y;
//  dest->z = v.z;
//}
//
//static void PtStoreXMFloat3(XMFLOAT3* dest, Point3 v)
//{
//  dest->x = v.x;
//  dest->y = v.y;
//  dest->z = v.z;
//}
//static Matrix LoadFloat4X4ToMatrix(XMFLOAT4X4* dest)
//{
//  Matrix ret;
//   for (unsigned i = 0; i < 4; ++i)
//   {
//     for (unsigned j = 0; j < 4; ++j)
//     {
//       ret.m[j][i] = dest->m[i][j];
//     }
//   }
//   return ret;
//}
//static XMMATRIX InverseTranspose(CXMMATRIX matrix)
//{
//  // Inverse-transpose is just applied to normals.  So zero out 
//  // translation row so that it doesn't get into our inverse-transpose
//  // calculation--we don't want the inverse-transpose of the translation.
//  XMMATRIX matrixIT = matrix;
//  matrixIT.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//  XMVECTOR det = XMMatrixDeterminant(matrixIT);
//  return XMMatrixTranspose(XMMatrixInverse(&det, matrixIT));
//}
//
//static XMMATRIX Inverse(CXMMATRIX matrix)
//{
//  XMMATRIX matrixIT = matrix;
//
//  XMVECTOR det = XMMatrixDeterminant(matrixIT);
//  return XMMatrixInverse(&det, matrixIT);
//}


inline float MinFLT(float lhs, float rhs)
{
  return (lhs < rhs) ? (lhs) : (rhs);
}
inline float MaxFLT(float lhs, float rhs)
{
  return (lhs > rhs) ? (lhs) : (rhs);
}
inline int MinIN(int lhs, int rhs)
{
  return (lhs < rhs) ? (lhs) : (rhs);
}
inline int MaxIN(int lhs, int rhs)
{
  return (lhs > rhs) ? (lhs) : (rhs);
}

__forceinline float MixRestitution(float a, float b)
{
  return a > b ? a : b;
}

__forceinline float MixFriction(float a, float b)
{
	//return a > b ? a : b;
  return std::sqrtf(a * b);
}