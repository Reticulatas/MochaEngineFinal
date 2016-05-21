/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Frustum.h"

void Frustum::ComputeFrustumPlane(const Matrix4& viewProj )
{
  //mPlane[0] = BuildPlaneFrom3Points(mPoints[4], mPoints[5], mPoints[1]);  // Left
  //mPlane[1] = BuildPlaneFrom3Points(mPoints[3], mPoints[2], mPoints[6]);  // Right
  //mPlane[2] = BuildPlaneFrom3Points(mPoints[2], mPoints[1], mPoints[5]);  // Bot
  //mPlane[3] = BuildPlaneFrom3Points(mPoints[0], mPoints[3], mPoints[4]);  // Top
  //mPlane[5] = BuildPlaneFrom3Points(mPoints[6], mPoints[5], mPoints[4]);  // Far
  //mPlane[4] = BuildPlaneFrom3Points(mPoints[0], mPoints[1], mPoints[2]);  // Near
  
  //// Left
  mPlane[0].x = viewProj(0, 3) + viewProj(0, 0);
  mPlane[0].y = viewProj(1, 3) + viewProj(1, 0);
  mPlane[0].z = viewProj(2, 3) + viewProj(2, 0);
  mPlane[0].w = viewProj(3, 3) + viewProj(3, 0);

  // Right
  mPlane[1].x = viewProj(0, 3) - viewProj(0, 0);
  mPlane[1].y = viewProj(1, 3) - viewProj(1, 0);
  mPlane[1].z = viewProj(2, 3) - viewProj(2, 0);
  mPlane[1].w = viewProj(3, 3) - viewProj(3, 0);

  // Bottom Frustum Plane
  mPlane[2].x = viewProj(0, 3) + viewProj(0, 1);
  mPlane[2].y = viewProj(1, 3) + viewProj(1, 1);
  mPlane[2].z = viewProj(2, 3) + viewProj(2, 1);
  mPlane[2].w = viewProj(3, 3) + viewProj(3, 1);

  // Top Frustum Plane
  mPlane[3].x = viewProj(0, 3) - viewProj(0, 1);
  mPlane[3].y = viewProj(1, 3) - viewProj(1, 1);
  mPlane[3].z = viewProj(2, 3) - viewProj(2, 1);
  mPlane[3].w = viewProj(3, 3) - viewProj(3, 1);

  // Near Frustum Plane
  mPlane[4].x = viewProj(0, 2);
  mPlane[4].y = viewProj(1, 2);
  mPlane[4].z = viewProj(2, 2);
  mPlane[4].w = viewProj(3, 2);

  // Far Frustum Plane
  mPlane[5].x = viewProj(0, 3) - viewProj(0, 2);
  mPlane[5].y = viewProj(1, 3) - viewProj(1, 2);
  mPlane[5].z = viewProj(2, 3) - viewProj(2, 2);
  mPlane[5].w = viewProj(3, 3) - viewProj(3, 2);

  for (int i = 0; i < 6; ++i)
  {
    float length = ToVector3(mPlane[i]).Length();
    if (!IsZero(length))
      mPlane[i] /= length;
  }
}

bool Frustum::PointInsidePlane(const Vector4& point, const Vector4& plane)
{
  return point.Dot(plane) < 0;
}

bool Frustum::PointInsideFrustum(const Vector4& point)
{
  for(int i = 0; i < 6; ++i)
  {
    // If point is not inside 1 of the plane, it is not in the frustum
    if(!PointInsidePlane(point, mPlane[i]) )
      return false;
  }
  
  return true;
}

Vector4 Frustum::BuildPlaneFrom3Points(const Vector4& p1, const Vector4& p2, const Vector4& p3)
{
  Vector3 v1, v2, normal, point;
  float d;
  
  // Get 2 vectors
  point = ToVector3(p1);
  v1 = ToVector3(p2 - p1);
  v2 = ToVector3(p3 - p1);

  normal = Cross(v2, v1);
  normal.Normalize();

  d = normal.Dot(point);

  return ToVector4(normal, -d);
}

bool Frustum::CircleInsideFrustum(const Vector3& point, float radius)
{
  for (int i = 0; i < 6; i++)
  {    
    float ret = mPlane[i].Dot(ToVector4(point, 1));
    if (ret < -radius)
      return false;
  }

  return true;

  //Vector3 farPoint, vec;
  //float farDist, distCheck;
  //
  //farPoint.x = mPoints[0].x;
  //farPoint.y = mPoints[0].y;
  //farPoint.z = 0;
  //
  //vec = farPoint - centerPos;
  //
  //farDist = vec.Length() + radius;
  //
  //vec = point - centerPos;
  //distCheck = vec.Length();
  //
  //if (distCheck < farDist)
  //  return true;
  //else
  //  return false;
}

bool Frustum::TriangleInsideFrustum(const Vector4& p0, const Vector4& p1, const Vector4& p2)
{
  if(PointInsideFrustum(p0) || PointInsideFrustum(p1) || PointInsideFrustum(p2))
    return true;

  else if (IntersectionBtw2Planes(p0, p1) ||
           IntersectionBtw2Planes(p1, p2) ||
           IntersectionBtw2Planes(p0, p2) )
    return true;

  else
    return false;
}

bool Frustum::IntersectionBtw2Planes(const Vector4& p0, const Vector4& p1)
{
  unsigned intersection = 0;

  for(int i = 0; i < 6; ++i)
  {
    if(PointInsidePlane(p0, mPlane[i]) != PointInsidePlane(p1, mPlane[i]))
      ++intersection;

    if(intersection >= 2)
      return true;
  }

  return false;
}

void Frustum::DrawFrustum()
{
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[0]), ToVector3(mPoints[1]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[1]), ToVector3(mPoints[2]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[2]), ToVector3(mPoints[3]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[3]), ToVector3(mPoints[0]), COLOR::LIME_GREEN);

  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[4]), ToVector3(mPoints[5]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[5]), ToVector3(mPoints[6]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[6]), ToVector3(mPoints[7]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[7]), ToVector3(mPoints[4]), COLOR::LIME_GREEN);

  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[0]), ToVector3(mPoints[4]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[3]), ToVector3(mPoints[7]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[1]), ToVector3(mPoints[5]), COLOR::LIME_GREEN);
  Renderer::getInstance()->DrawLinePerma(ToVector3(mPoints[2]), ToVector3(mPoints[6]), COLOR::LIME_GREEN);
}

bool Frustum::RectInsideFrustum(const std::vector<Vector3>& box)
{
  for (int i = 0; i < 6; ++i)
  {
    if (!PointInsidePlane(ToVector4(box[0], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[1], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[2], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[3], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[4], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[5], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[6], 1), mPlane[i]))
      continue;

    if (!PointInsidePlane(ToVector4(box[7], 1), mPlane[i]))
      continue;

    return false;
  }

  return true;
}

void Frustum::SetPoints(const CCamera* cam)
{
  Vector3 camNearPos, camFarPos, vec;

  Vector3 Right = cam->gameObject->transform->GetRightVector();
  Vector3 Up = cam->gameObject->transform->GetUpVector();
  Vector3 Look = cam->gameObject->transform->GetForwardVector();

  mPos = cam->gameObject->transform->GetGlobalPosition();
  camNearPos = mPos + Look * cam->GetNearZ();
  camFarPos = mPos + Look * cam->GetFarZ();

  vec = camNearPos - Right * cam->GetNearWindowWidth() * .5f + Up * cam->GetNearWindowHeight() * .5f;
  mPoints[0].x = vec.x;
  mPoints[0].y = vec.y;
  mPoints[0].z = vec.z;
  mPoints[0].w = 1;

  vec = camNearPos - Right * cam->GetNearWindowWidth() * .5f - Up * cam->GetNearWindowHeight() * .5f;
  mPoints[1].x = vec.x;
  mPoints[1].y = vec.y;
  mPoints[1].z = vec.z;
  mPoints[1].w = 1;

  vec = camNearPos + Right * cam->GetNearWindowWidth() * .5f - Up * cam->GetNearWindowHeight() * .5f;
  mPoints[2].x = vec.x;
  mPoints[2].y = vec.y;
  mPoints[2].z = vec.z;
  mPoints[2].w = 1;

  vec = camNearPos + Right * cam->GetNearWindowWidth() * .5f + Up * cam->GetNearWindowHeight() * .5f;
  mPoints[3].x = vec.x;
  mPoints[3].y = vec.y;
  mPoints[3].z = vec.z;
  mPoints[3].w = 1;

  vec = camFarPos - Right * cam->GetFarWindowWidth() * .5f + Up * cam->GetFarWindowHeight() * .5f;
  mPoints[4].x = vec.x;
  mPoints[4].y = vec.y;
  mPoints[4].z = vec.z;
  mPoints[4].w = 1;

  vec = camFarPos - Right * cam->GetFarWindowWidth() * .5f - Up * cam->GetFarWindowHeight() * .5f;
  mPoints[5].x = vec.x;
  mPoints[5].y = vec.y;
  mPoints[5].z = vec.z;
  mPoints[5].w = 1;

  vec = camFarPos + Right * cam->GetFarWindowWidth() * .5f - Up * cam->GetFarWindowHeight() * .5f;
  mPoints[6].x = vec.x;
  mPoints[6].y = vec.y;
  mPoints[6].z = vec.z;
  mPoints[6].w = 1;

  vec = camFarPos + Right * cam->GetFarWindowWidth() * .5f + Up * cam->GetFarWindowHeight() * .5f;
  mPoints[7].x = vec.x;
  mPoints[7].y = vec.y;
  mPoints[7].z = vec.z;
  mPoints[7].w = 1;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change