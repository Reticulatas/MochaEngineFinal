/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "MathIncludes.h"

class Frustum
{
public:

  void ComputeFrustumPlane(const Matrix4& viewProj);
  bool PointInsidePlane(const Vector4& point, const Vector4& plane);
  bool PointInsideFrustum(const Vector4& point);
  bool CircleInsideFrustum(const Vector3& point, float radius);
  bool TriangleInsideFrustum(const Vector4& p0, const Vector4& p1, const Vector4& p2);
  bool IntersectionBtw2Planes(const Vector4& p0, const Vector4& p1);
  bool RectInsideFrustum(const std::vector<Vector3>& box);
  Vector4 BuildPlaneFrom3Points(const Vector4& p1, const Vector4& p2, const Vector4& p3);

  Vector3 GetPos() const { return mPos; }
  void SetCenterPos(const Vector3& val) { mPos = val; }
  void SetPoints(const CCamera* cam);

  void DrawFrustum();

private:

  Vector3 mPos;
  Vector4 mPoints[8];
  Vector4 mPlane[6];
};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change