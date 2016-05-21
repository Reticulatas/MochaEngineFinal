///////////////////////////////////////////////////////////////////////////////
///
/// \file Vector3.hpp
/// Declaration of the Vector 3 structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Reals.h"
#include "Vector2.h"

namespace Math
{

struct Vector3;
typedef const Vector3& Vec3Param;
typedef Vector3& Vec3Ref;
typedef Vector3* Vec3Ptr;

//--------------------------------------------------------------------- Vector 3
///3 dimensional vector.
struct Vector3
{
  Vector3(void);
  explicit Vector3(float x, float y, float z);
  //Splat all elements
  explicit Vector3(float xyz);
  explicit Vector3(Vec2Param vec2, float z = 0.0f);
  explicit Vector3(ConstRealPointer data);
  Vector3(Vec3Param vec3);

  float* ToFloats();

  float& operator[](unsigned index);
  float operator[](unsigned index) const;

  //Unary Operators
  Vector3 operator-(void) const;

  //Binary Assignment Operators (reals)
  void operator*=(float rhs);
  void operator/=(float rhs);

  //Binary Operators (Reals)
  Vector3 operator*(float rhs) const;
  Vector3 operator/(float rhs) const;

  //Binary Assignment Operators (vectors)
  void operator+=(Vec3Param rhs);
  void operator-=(Vec3Param rhs);

  //Binary Operators (vectors)
  Vector3 operator+(Vec3Param rhs) const;
  Vector3 operator-(Vec3Param rhs) const;  

  //Binary Vector Comparisons
  bool operator==(Vec3Param rhs) const;
  bool operator!=(Vec3Param rhs) const;

  //Vector component wise multiply and divide
  Vector3 operator*(Vec3Param rhs) const;
  Vector3 operator/(Vec3Param rhs) const;

  ///Component-wise assignment multiplication
  void operator*=(Vec3Param rhs);
  void operator/=(Vec3Param rhs);

  ///Set all of the values of this vector at once.
  void Set(float x, float y, float z);

  ///Set all of the values of the vector to the passed in value.
  void Splat(float xyz);

  ///Do a component-wise scaling of this vector with the given vector.
  void ScaleByVector(Vec3Param rhs);

  ///Set all of this vector's elements to 0.
  void ZeroOut(void);

  ///Calculate and return this vector reflected about the given vector.
  Vector3 Reflect(Vec3Param reflectionAxis) const;

  ///Projects the vector onto the axis
  Vector3 Project(Vec3Param axis);

  Vec3Param ProjectUnit(const Vec3Param ontoUnit);

  const Vector3 ProjectedUnit(const Vec3Param ontoUnit);

  const Vector3 Vector3::Project(const Vec3Param axis) const;

  ///Add a vector multiplied by a scalar to this vector. A commonly done 
  ///operation and this reduces temporaries.
  void AddScaledVector(Vec3Param vector, float scalar);

  ///Compute the dot product of this vector with the given vector.
  float Dot(Vec3Param rhs) const;

  ///Get the length of this vector.
  float Length(void) const;

  ///Get the squared length of this vector.
  float LengthSq(void) const;

  ///Calculate and return a unit-length copy of this vector.
  Vector3 Normalized(void) const;

  ///Make this vector have a length of 1, returns the original length.
  float Normalize(void);

  ///Ceil each component of the vector.
  void Ceil(void);

  ///Floor each component of the vector.
  void Floor(void);

  ///Truncate each component of the vector.
  void Truncate(void);

  ///If positive, it always rounds up, if negative, it always rounds down
  void RoundToExtremes();

  ///Round each component of the vector.
  void Round(void);

  ///Attempt to give this vector a length of 1, but checks if it's possible.
  ///Instead of crashing, will return 0 if the vector was not able to be 
  ///normalized.
  float AttemptNormalize(void);

  ///Flip this vector so it's pointing in the opposite direction.
  Vec3Ref Negate(void);

  ///Checks to see if the values of this vector's elements are usable.
  bool Valid(void) const;

  ///Compute the cross product of this vector with the given vector.
  Vector3 Cross(const Vector3* rhs) const;
  Vector3 Cross(Vec3Param rhs) const;
  void InvertComponents(void);

  //global -> local functions////

  ///Compute the distance between two given vectors.
  float Distance(const Vector3* rhs);

  ///Compute the cross product of the two given vectors.
  Vector3 Cross(const Vector3* rhs);

  ///Compute the cross product of the two given vectors for 2d.
  ///The result is only the z axis of the cross product.
  Vector3 Cross2d(const Vector3* rhs);

  ///Returns the component-wise minimum vector of the two vectors.
  Vector3 Min(const Vector3* rhs);

  ///Returns the component-wise maximum vector of the two vectors.
  Vector3 Max(const Vector3* rhs);

  ///Linearly interpolate between the two vectors, the t-value is restricted to 
  ///the range [0, 1].
  Vector3 Lerp(const Vector3* end, float tValue);

  ///Calculates and returns the given vector with its values clamped to the range
  ///[min, max].
  void Clamp(float min, float max);

  ///Returns if all values in lhs are less than all values in rhs
  bool AllLess(const Vector3* rhs);

  ///Returns if any value in lhs is less than any value in rhs
  bool AnyLess(const Vector3* rhs);

  ///Returns if all values in lhs are greater than all values in rhs
  bool AllGreater(const Vector3* rhs);

  ///Returns if any value in lhs is greater than any value in rhs
  bool AnyGreater(const Vector3* rhs);

  union
  {
    struct  
    {
      float x, y, z;
    };
    float array[3];
  };

  static const Vector3 cZero;
  static const Vector3 cXAxis;
  static const Vector3 cYAxis;
  static const Vector3 cZAxis;

};

Vector3 operator*(float lhs, Vec3Param rhs);

///Compute the distance between two given vectors.
float Distance(Vec3Param lhs, Vec3Param rhs);

///Compute the dot product of the two given vectors.
float Dot(Vec3Param lhs, Vec3Param rhs);

//Vector component-wise multiply
Vector3 ScaledByVector(Vec3Param lhs, Vec3Param rhs);

//Vector component-wise divide
Vector3 DividedByVector(Vec3Param lhs, Vec3Param rhs);

///Get the length of the given vector.
float Length(Vec3Param vec);

///Get the squared length of the given vector.
float LengthSq(Vec3Param vec);

///Calculate and return a unit-length copy of the given vector.
Vector3 Normalized(Vec3Param vec);

///Make the given vector have a length of 1, returns the original length.
float Normalize(Vec3Ptr vec);

///Attempt to give the given vector a length of 1, but checks if it's possible.
///Instead of crashing, this will return 0 if the vector was not able to be 
///normalized.
float AttemptNormalize(Vec3Ptr vec);

///Compute the cross product of the two given vectors.
Vector3 Cross(Vec3Param lhs, Vec3Param rhs);

///Compute the cross product of the two given vectors for 2d.
///The result is only the z axis of the cross product.
Vector3 Cross2d(Vec3Param lhs, Vec3Param rhs);

///Set all of the given vector's elements to 0.
void ZeroOut(Vec3Ptr vec);

///Flips the given vector so it's pointing in the opposite direction.
void Negate(Vec3Ptr vec);

///Returns a vector pointing in the opposite direction of the given vector.
Vector3 Negated(Vec3Param vec);

///Returns a vector with absolute valued elements of the given vector.
Vector3 Abs(Vec3Param vec);

///Returns the component-wise minimum vector of the two vectors.
Vector3 Min(Vec3Param lhs, Vec3Param rhs);

///Returns the component-wise maximum vector of the two vectors.
Vector3 Max(Vec3Param lhs, Vec3Param rhs);

///Linearly interpolate between the two vectors, the t-value is restricted to 
///the range [0, 1].
Vector3 Lerp(Vec3Param start, Vec3Param end, float tValue);

///Clamps the values of the elements of the given vector to the range [min, max]
void Clamp(Vec3Ptr vec, float min, float max);

///Calculates and returns the given vector with its values clamped to the range
///[min, max].
Vector3 Clamped(Vec3Param vec, float min, float max);

/// Same as clamp, however it fills out whether or not anything was clamped.
/// Useful when an assert message should be shown if anything was clamped.
Vector3 DebugClamp(Vec3Param vec, float min, float max, bool& wasClamped);

///Returns if all values in lhs are less than all values in rhs
bool AllLess(Vec3Param lhs, Vec3Param rhs);

///Returns if any value in lhs is less than any value in rhs
bool AnyLess(Vec3Param lhs, Vec3Param rhs);

///Returns if all values in lhs are greater than all values in rhs
bool AllGreater(Vec3Param lhs, Vec3Param rhs);

///Returns if any value in lhs is greater than any value in rhs
bool AnyGreater(Vec3Param lhs, Vec3Param rhs);

}// namespace Math
