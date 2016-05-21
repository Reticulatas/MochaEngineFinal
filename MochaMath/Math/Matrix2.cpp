///////////////////////////////////////////////////////////////////////////////
///
///	\file Matrix2.hpp
///	Implementation of the Matrix 2 structure.
///	
///	Authors: Joshua Davis
///	Copyright 2010, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "MathUtilities.h"
#include "Matrix2.h"

#include <cmath>


namespace Math
{

  Matrix2::Matrix2(ConstRealPointer data_)
  {
    vect[0].Set(data_[0], data_[1]);
    vect[1].Set(data_[2], data_[3]);
  }

  float* Matrix2::ToFloats()
  {
    return (float*)this;
  }

  Vec2Ref Matrix2::operator[](unsigned index)
  {
    return vect[index];
  }

  Vec2Param Matrix2::operator[](unsigned index) const
  {
    return vect[index];
  }

  ConstRealPointer Matrix2::Begin() const
  {
    return array;
  }

  ConstRealPointer Matrix2::End() const
  {
    return array + 4;
  }

  RealPointer Matrix2::Begin()
  {
    return array;
  }

  RealPointer Matrix2::End()
  {
    return array + 4;
  }

  //////////////////////////////////////////////////////////////////////////
  //BINARY ASSIGNMENT OPERATORS (realS)
  //////////////////////////////////////////////////////////////////////////

  void Matrix2::operator*=(float rhs)
  {
    vect[0] *= rhs;
    vect[1] *= rhs;
  }

  void Matrix2::operator/=(float rhs)
  {
    vect[0] /= rhs;
    vect[1] /= rhs;
  }

  //////////////////////////////////////////////////////////////////////////
  //BINARY OPERATORS (realS)
  //////////////////////////////////////////////////////////////////////////

  Matrix2 Matrix2::operator*(float rhs) const
  {
    Matrix2 ret = *this;
    ret *= rhs;
    return ret;
  }

  Matrix2 Matrix2::operator/(float rhs) const
  {
    Matrix2 ret = *this;
    ret /= rhs;
    return ret;
  }

  //////////////////////////////////////////////////////////////////////////
  //BINARY ASSIGNMENT OPERATOR (MATRICES)
  //////////////////////////////////////////////////////////////////////////

  void Matrix2::operator+=(Mat2Param rhs)
  {
    vect[0] += rhs.vect[0];
    vect[1] += rhs.vect[1];
  }

  void Matrix2::operator-=(Mat2Param rhs)
  {
    vect[0] -= rhs.vect[0];
    vect[1] -= rhs.vect[1];
  }

  //////////////////////////////////////////////////////////////////////////
  //BINARY OPERATORS (MATRICES)
  //////////////////////////////////////////////////////////////////////////

  Matrix2 Matrix2::operator+(Mat2Param rhs) const
  {
    Matrix2 ret = *this;
    ret += rhs;
    return ret;
  }

  Matrix2 Matrix2::operator-(Mat2Param rhs) const
  {
    Matrix2 ret = *this;
    ret -= rhs;
    return ret;
  }

  Matrix2 Matrix2::operator*(Mat2Param rhs) const
  {
    return Concat(rhs);
  }

  //////////////////////////////////////////////////////////////////////////
  //BINARY COMPARISONS
  //////////////////////////////////////////////////////////////////////////

  bool Matrix2::operator==(Mat2Param rhs) const
  {
    return vect[0] == rhs.vect[0] &&
           vect[1] == rhs.vect[1];
  }

  bool Matrix2::operator!=(Mat2Param rhs) const
  {
    return !(*this == rhs);
  }



  Matrix2 Matrix2::Transpose() const
  {
    Matrix2 ret;

    ret.m00 = m00;
    ret.m01 = m10;
    ret.m10 = m01;
    ret.m11 = m11;

    return ret;
  }

  Matrix2 Matrix2::Inverse()
  {
    Matrix2 inverse;
    inverse.m00 = m11;
    inverse.m01 = -m01;
    inverse.m10 = -m10;
    inverse.m11 = m00;
    inverse *= 1.0f / Determinate();
    
    return inverse;
  }

  Matrix2 Matrix2::Concat(Mat2Param rhs) const
  {
    Matrix2 ret;

    ret.m00 = Dot(vect[0], Vector2(rhs.m00, rhs.m10));
    ret.m01 = Dot(vect[0], Vector2(rhs.m01, rhs.m11));

    ret.m10 = Dot(vect[1], Vector2(rhs.m00, rhs.m10));
    ret.m11 = Dot(vect[1], Vector2(rhs.m01, rhs.m11));

    return ret;
  }

  Mat2Ref Matrix2::Identity()
  {
    vect[0].Set(1.0f, 0.0f);
    vect[1].Set(0.0f, 1.0f);
    return *this;
  }

  Mat2Ref Matrix2::Zero()
  {
    vect[0].ZeroOut();
    vect[1].ZeroOut();
    return *this;
  }

  float Matrix2::Determinate() const
  {
    return m00 * m11 - m01 * m10;
  }

  bool Matrix2::Valid() const
  {
    return vect[0].Valid() && vect[1].Valid();
  }

  void Matrix2::Scale(float x, float y)
  {
    vect[0].Set(      x, 0.0f);
    vect[1].Set(0.0f,         y);
  }

  void Matrix2::Scale(Vec2Param rhs)
  {
    Scale(rhs[0], rhs[1]);
  }

  Vector2 Matrix2::Transform(Vec2Param vector) const
  {
    float x = Dot(vect[0], vector);
    float y = Dot(vect[1], vector);
    return Vector2(x, y);
  }

  void Matrix2::TransformVector(Vec2Ref vector) const
  {
    float x = Dot(vect[0], vector);
    float y = Dot(vect[1], vector);
    vector.Set(x, y);
  }

  Vector2 Matrix2::BasisVector(unsigned index) const
  {
    //Could be optimized but I don't want to rely on the order of the elements in 
    //the matrix
    switch(index)
    {
    case 0:
      return Vector2(m00, m10);
      break;

    case 1:
      return Vector2(m01, m11);
      break;

    default:
      //ErrorIf(index > 2, "Invalid index given, matrix dimension is too low.");
      return Vector2(0.0f, 0.0f);
      break;
    }
  }

  Matrix2 operator*(float lhs, Mat2Param rhs)
  {
    return rhs * lhs;
  }

  Matrix2 Concat(Mat2Param lhs, Mat2Param rhs)
  {
    return lhs.Concat(rhs);
  }

  Vector2 Transform(Mat2Param mat, Vec2Param vector)
  {
    return mat.Transform(vector);
  }

}// namespace Math
