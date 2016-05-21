///////////////////////////////////////////////////////////////////////////////
///
/// \file Matrix3.hpp
/// Declaration of the Matrix 3 structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MatrixStorage.h"
#include "Reals.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Math
{

///Forward declaration
struct Quaternion;
typedef const Quaternion& QuatParam;

///Forward declaration
struct Matrix4;
typedef const Matrix4& Mat4Param;

struct Matrix3;
typedef const Matrix3& Mat3Param;
typedef Matrix3& Mat3Ref;
typedef Matrix3* Mat3Ptr;

//--------------------------------------------------------------------- Matrix 3
///3 dimensional square matrix. Supports operations with other 3 dimensional
///square matrices, 2 dimensional vectors, and 3 dimensional vectors.
struct Matrix3
{
#ifdef ColumnBasis
typedef Vector3   BasisVector;
typedef Vec3Param CrossVector;
#else
typedef Vec3Param BasisVector;
typedef Vector3   CrossVector;
#endif

public:
  Matrix3(void);
  explicit Matrix3(float p00, float p01, float p02, 
                   float p10, float p11, float p12,
                   float p20, float p21, float p22);
  Matrix3(ConstRealPointer data_);

  float* ToFloats();

  //Binary assignment operators (reals)
  void operator*=(float rhs);
  void operator/=(float rhs);

  //Binary operators (reals)
  Matrix3 operator*(float rhs) const;
  Matrix3 operator/(float rhs) const;

  //Binary assignment operators (matrices)
  void operator+=(Mat3Param rhs);
  void operator-=(Mat3Param rhs);
  
  //Binary operators (matrices)
  Matrix3 operator+(Mat3Param rhs) const;
  Matrix3 operator-(Mat3Param rhs) const;
  Matrix3 operator*(Mat3Param rhs) const;

  Vector3 operator*(Vector3 rhs) const;
  //Matrix comparisons
  bool operator==(Mat3Param rhs) const;
  bool operator!=(Mat3Param rhs) const;

  float operator()(unsigned r, unsigned c) const;
  float& operator()(unsigned r, unsigned c);

  Vector3& operator[](unsigned index);
  const Vector3& operator[](unsigned index) const;

  ///Returns a copy of this matrix with its elements transposed.
  Matrix3 Transposed(void) const;

  ///Transposes this matrix in place.
  Mat3Ref Transpose(void);
  Mat3Ref Prepend(const Matrix3 &lhs);
  ///Returns the inverse of this matrix.
  Matrix3 Inverted(void) const;

  ///Inverts this matrix in place.
  Mat3Ref Invert(void);

  ///Multiplies this matrix with the given matrix on its right-hand side.
  Matrix3 Concat(Mat3Param rhs) const;

  ///Sets this matrix's elements to that of the identity matrix.
  Mat3Ref SetIdentity(void);

  ///Sets all of the elements in this matrix to 0.
  Mat3Ref ZeroOut(void);

  float Determinant(void) const;

  bool Valid(void) const;

  ///Converts this matrix into a pure scaling matrix.
  void Scale(float x, float y, float z);

  ///Converts this matrix into a pure scaling matrix.
  void Scale(Vec3Param rhs);

  ///Converts this matrix into a pure rotation matrix, given an axis-angle pair.
  void Rotate(float x, float y, float z, float radian);

  ///Converts this matrix into a pure rotation matrix, given an axis-angle pair.
  void Rotate(Vec3Param rhs, float radian);

  ///Converts this matrix into a pure translation matrix for 2-D vectors.
  void Translate(float x, float y);

  ///Converts this matrix into a pure translation matrix for 2-D vectors.
  void Translate(Vec2Param rhs);

  ///Builds a matrix that should be used on 2D points/vectors
  void BuildTransform(Vec2Param translate, float radians, Vec2Param scale);

  ///Builds a matrix that should be used on 3D points/vectors
  void BuildTransform(QuatParam rotate, Vec3Param scale);

  ///Forces the current matrix to be orthonormal.
  Mat3Ref Orthonormalize(void);

  ///Accesses the basis vector at the given index, with the basis vector defined
  ///as the basis vector of a pure rotation matrix.
  BasisVector Basis(unsigned index) const;

  ///Accesses the elements in the "x-axis" of the matrix, with the "x-axis" 
  ///defined as the x-axis of a pure rotation matrix.
  BasisVector BasisX(void) const;

  ///Accesses the elements in the "y-axis" of the matrix, with the "y-axis" 
  ///defined as the y-axis of a pure rotation matrix.
  BasisVector BasisY(void) const;

  ///Accesses the elements in the "z-axis" of the matrix, with the "z-axis" 
  ///defined as the z-axis of a pure rotation matrix.
  BasisVector BasisZ(void) const;

  ///Accesses the cross vector at the given index, with the cross vector defined
  ///as the elements in the matrix perpendicular to that of the corresponding
  ///basis vector.
  CrossVector Cross(unsigned index) const;
  void SetBasis(unsigned index, Vec3Param basisVector);
  void SetBasis(unsigned index, float x, float y, float z);
  void SetCross(unsigned index, Vec3Param crossVector);
  void SetCross(unsigned index, float x, float y, float z);

  static const Matrix3 Outer(const Vector3 &v1, const Vector3 &v2);
  Matrix3& One(void);

  union 
  {
    struct
    {
#ifdef ColumnBasis
      float m00, m01, m02,
           m10, m11, m12, 
           m20, m21, m22;
#else
      float m00, m10, m20,
           m01, m11, m21, 
           m02, m12, m22;
#endif 
    };
    float array[9];
  };

  static const Matrix3 cIdentity;
};

Matrix3 operator*(float lhs, Mat3Param rhs);
Matrix3 Concat(Mat3Param lhs, Mat3Param rhs);

///This builds a matrix that should be used on 2D points/vectors
Matrix3 BuildTransform(Vec2Param translate, float radians, Vec2Param scale);

///This builds a matrix that should be used on 3D points/vectors
Matrix3 BuildTransform(QuatParam rotate, Vec3Param scale);

Vector3 Transform(Mat3Param mat, Vec3Param vector);
void Transform(Mat3Param matrix, Vec3Ptr vector);

/// Applies transformation with the translation (p.x, p.y, 1)
Vector2 TransformPoint(Mat3Param matrix, Vec2Param vector);

/// Applies transformation without the translation (n.x, n.y, 0)
Vector2 TransformNormal(Mat3Param matrix, Vec2Param normal);

///Transforms the given vector by the matrix as if the matrix was transposed.
Vector3 TransposedTransform(Mat3Param mat, Vec3Param vector);

///Transforms the given vector by the matrix as if the matrix was transposed.
void TransposedTransform(Mat3Param matrix, Vec3Ptr vector);

float Trace(Mat3Param matrix);

float Cofactor(Mat3Param matrix, unsigned row, unsigned column);

///Takes a symmetric matrix and diagonalizes it.
void Diagonalize(Mat3Ptr matrix);
Matrix3 Diagonalized(Mat3Param matrix);

void Invert(Mat3Ptr matrix);
Matrix3 Inverted(Mat3Param matrix);

}// namespace Math
