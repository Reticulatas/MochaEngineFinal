#include "VQS.h"

VQS::VQS() : s(1.0f)
{
}

VQS VQS::operator*(const VQS& rhs)
{
  VQS result;

  result.v = q.RotatedVector(rhs.v * s) + v;
  result.q = q * rhs.q;
  result.s = s * rhs.s;

  return result;
}

VQS VQS::Inverse()
{
  VQS inv;

  inv.s = 1.0f / s;
  inv.q = q.Inverted();
  inv.v = -inv.q.RotatedVector(v);
  //inv.v = inv.q.RotatedVector(-v) * inv.s;

  return inv;
}

Math::Matrix4 VQS::toMatrix()
{
  Matrix4 result , trans, rot, scale;

  trans.Translate(v);
  rot = ToMatrix4(q);
  scale.Scale(Vector3(s));

  result = trans * rot * scale;
  return result;
}
