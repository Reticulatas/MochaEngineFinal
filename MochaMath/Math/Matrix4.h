///////////////////////////////////////////////////////////////////////////////
///
/// \file Matrix4.hpp
/// Declaration of the Matrix 4 structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MatrixStorage.h"
#include "Reals.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace Math
{
///Forward declaration
struct Quaternion;
typedef const Quaternion& QuatParam;

///Forward declaration
struct Matrix3;
typedef const Matrix3& Mat3Param;
typedef Matrix3* Mat3Ptr;

struct Matrix4;
typedef const Matrix4& Mat4Param;
typedef Matrix4& Mat4Ref;
typedef Matrix4* Mat4Ptr;

///4 dimensional square matrix. Supports operations with other 4 dimensional
///square matrices, 3 dimensional vectors, and 4 dimensional vectors.
struct Matrix4
{
#ifdef ColumnBasis
typedef Vector4   BasisVector;
typedef Vec4Param CrossVector;
#else
typedef Vec4Param BasisVector;
typedef Vector4   CrossVector;
#endif

public:
  Matrix4(void);
  Matrix4(float p00, float p01, float p02, float p03,
          float p10, float p11, float p12, float p13,
          float p20, float p21, float p22, float p23,
          float p30, float p31, float p32, float p33);
  Matrix4(ConstRealPointer data_);
  Matrix4(Vec4Param basisX, Vec4Param basisY,
          Vec4Param basisZ, Vec4Param basisW);

  float* ToFloats();

  //Binary assignment operators (reals)
  void operator*=(float rhs);
  void operator/=(float rhs);

  //Binary operators (reals)
  Matrix4 operator*(float rhs) const;
  Matrix4 operator/(float rhs) const;

  //Binary assignment operators (matrices)
  void operator+=(Mat4Param rhs);
  void operator-=(Mat4Param rhs);

  //Binary operators (matrices)
  Matrix4 operator+(Mat4Param rhs) const;
  Matrix4 operator-(Mat4Param rhs) const;
  Matrix4 operator*(Mat4Param rhs) const;

  Vector4 operator*(Vec4Param rhs) const;

  //Matrix comparisons
  bool operator==(Mat4Param rhs) const;
  bool operator!=(Mat4Param rhs) const;

  float operator()(unsigned r, unsigned c) const;
  float& operator()(unsigned r, unsigned c);

  ///Returns a copy of this matrix with its elements transposed.
  Matrix4 Transposed(void) const;

  //Transposes this matrix in place.
  Mat4Ref Transpose(void);

  ///Returns the inverse of this matrix.
  Matrix4 Inverted(void) const;

  ///Inverts this matrix in place.
  Mat4Ref Invert(void);

  Matrix4 InverseTranspose(void);

  ///Multiplies this matrix with the given matrix on its right-hand side.
  Matrix4 Concat(Mat4Param rhs) const;

  ///Sets this matrix's elements to that of the identity matrix.
  Mat4Ref SetIdentity(void);

  ///Sets all of the elements in this matrix to 0.
  Mat4Ref ZeroOut(void);

  float Determinant(void) const;
  bool Valid(void) const;

  ///Converts this matrix into a pure scaling matrix.
  void Scale(float x, float y, float z);

  ///Converts this matrix into a pure scaling matrix.
  void Scale(Vec3Param axis);

  ///Converts this matrix into a pure rotation matrix, given an axis-angle pair.
  void Rotate(float x, float y, float z, float radians);

  ///Converts this matrix into a pure rotation matrix, given an axis-angle pair.
  void Rotate(Vec3Param axis, float radians);

  ///Converts this matrix into a pure translation matrix for 3-D vectors.
  void Translate(float x, float y, float z);

  ///Converts this matrix into a pure translation matrix for 3-D vectors.
  void Translate(Vec3Param axis);

  ///Converts this matrix into a transformation matrix, incorporating 
  ///translation, rotation, and scale. Meant for 3-D vectors.
  void BuildTransform(Vec3Param translate, QuatParam rotate, Vec3Param scale);

  ///Converts this matrix into a transformation matrix, incorporating 
  ///translation, rotation, and scale. Meant for 3-D vectors.
  void BuildTransform(Vec3Param translate, Mat3Param rotate, Vec3Param scale);

  ///Decomposes this matrix into its scale, rotation, and translational
  ///components.
  void Decompose(Vec3Ptr scale, Mat3Ptr rotate, Vec3Ptr translate) const;

  ///Attempts to decompose this matrix into a scale, rotation, and translational
  ///component while removing shear. The "original" matrix is not necessarily
  ///recovered, though the resulting matrix is orthonormal among its components.
  void Decompose(Vec3Ptr scale, Vec3Ptr shear, Mat3Ptr rotate, 
                 Vec3Ptr translate) const;

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

  ///Accesses the elements in the "w-axis" of the matrix, with the "w-axis" 
  ///defined as the w-axis of a pure rotation matrix.
  BasisVector BasisW(void) const;

  ///Accesses the cross vector at the given index, with the cross vector defined
  ///as the elements in the matrix perpendicular to that of the corresponding
  ///basis vector.
  CrossVector Cross(unsigned index) const;
  void SetBasis(unsigned index, Vec4Param basisVector);
  void SetBasis(unsigned index, Vec3Param basisVector3, float w);
  void SetBasis(unsigned index, float x, Vec3Param basisVector3);
  void SetBasis(unsigned index, float x, float y, float z, float w);
  void SetCross(unsigned index, Vec4Param crossVector);
  void SetCross(unsigned index, Vec3Param crossVector3, float w);
  void SetCross(unsigned index, float x, float y, float z, float w);

  ///Accesses the 3-D basis vector at the given index, ignores the last element.
  Vector3 Basis3(unsigned index) const;

  ///Accesses the first 3 elements in the "x-axis" of the matrix, ignoring the
  ///last element.
  Vector3 Basis3X(void) const;

  ///Accesses the first 3 elements in the "y-axis" of the matrix, ignoring the
  ///last element.
  Vector3 Basis3Y(void) const;
  
  ///Accesses the first 3 elements in the "z-axis" of the matrix, ignoring the
  ///last element.
  Vector3 Basis3Z(void) const;

  ///Accesses the first 3 elements in the "w-axis" of the matrix, ignoring the
  ///last element.
  Vector3 Basis3W(void) const;

  ///Accesses the 3-D cross vector at the given index, ignores the last element.
  Vector3 Cross3(unsigned index) const;

  Vector4& operator[](unsigned index){return ((Vector4*)this)[index];}
  const Vector4& operator[](unsigned index)const{return ((Vector4*)this)[index];}

  union 
  {
    struct
    {
#ifdef ColumnBasis
      float m00, m01, m02, m03, 
           m10, m11, m12, m13, 
           m20, m21, m22, m23, 
           m30, m31, m32, m33;
#else
      float m00, m10, m20, m30, 
           m01, m11, m21, m31, 
           m02, m12, m22, m32, 
           m03, m13, m23, m33;
#endif
    };

    float array[16];
  };

  static const Matrix4 cIdentity;
};

Matrix4 operator*(float lhs, Mat4Param rhs);
Matrix4 Concat(Mat4Param lhs, Mat4Param rhs);

Matrix4 BuildTransform(Vec3Param translate, QuatParam rotate, Vec3Param scale);
Matrix4 BuildTransform(Vec3Param translate, Mat3Param rotate, Vec3Param scale);

Vector4 Transform(Mat4Param mat, Vec4Param vector);
void Transform(Mat4Param mat, Vec4Ptr vector);

///Applies transformation with the translation (p.x, p.y, p.z, 1)
Vector3 TransformPoint(Mat4Param matrix, Vec3Param point);

///Applies transformation without the translation (n.x, n.y, n.z, 0)
Vector3 TransformNormal(Mat4Param matrix, Vec3Param normal);

///Applies transform and projects back to (w = 1)
Vector3 TransformPointProjected(Mat4Param matrix, Vec3Param point);

Vector3 TransformNormalCol(Mat4Param matrix, Vec3Param normal);
Vector3 TransformPointCol(Mat4Param matrix, Vec3Param point);
Vector3 TransformPointProjectedCol(Mat4Param matrix, Vec3Param point);
Vector3 TransformPointProjectedCol(Mat4Param matrix, Vec3Param point, float* w);

float Trace(Mat4Param matrix);

}// namespace Math
