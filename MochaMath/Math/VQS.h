#ifndef VQS_H_
#define VQS_H_

#include "Math.h"

using namespace Math;

struct VQS
{
  Vector3 v;
  Quaternion q;
  float s;

  VQS();
  VQS operator*(const VQS& rhs);
  VQS Inverse();
  Matrix4 toMatrix();
};

#endif