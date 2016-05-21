#include "stdafx.h"
#include "Skeleton.h"
#include "Renderer.h"

Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{
}

void Skeleton::DrawBindPose()
{
  Color color = COLOR::YELLOW;

  for(unsigned i = 0; i < mBones.size(); ++i)
  {
    if(mBones[i].mParentIndex != -1 && mBones[i].mBindTransform.v != Vector3::cZero)
    {
      VQS parentBone = mBones[mBones[i].mParentIndex].mBindTransform;
      VQS currBone = mBones[i].mBindTransform;

      Vector3 up(0,1,0);
      Vector3 tmp, p1, p2, p3, p4;
      float scale = (parentBone.v - currBone.v).Length();
      scale *= .1f;

      tmp = Cross(up, (currBone.v - parentBone.v));
      if(tmp != Vector3::cZero)
        tmp.Normalize();
      
      p1 = parentBone.v + tmp * scale;
      p2 = parentBone.v - tmp * scale;
      
      Renderer::getInstance()->DrawLine(p1, currBone.v, color);
      Renderer::getInstance()->DrawLine(p2, currBone.v, color);
      
      tmp = Cross(tmp, currBone.v - parentBone.v);
      if(tmp != Vector3::cZero)
        tmp.Normalize();
      
      p3 = parentBone.v + tmp * scale;
      p4 = parentBone.v - tmp * scale;
      
      Renderer::getInstance()->DrawLine(p3, currBone.v, color);
      Renderer::getInstance()->DrawLine(p4, currBone.v, color);
      
      Renderer::getInstance()->DrawLine(p1, p3, color);
      Renderer::getInstance()->DrawLine(p2, p3, color);
      Renderer::getInstance()->DrawLine(p2, p4, color);
      Renderer::getInstance()->DrawLine(p4, p1, color);
    }
  }
}

std::vector<Bone>& Skeleton::GetBones()
{
  return mBones;
}

void Skeleton::DrawBones(std::vector<VQS>& boneTransforms, Matrix4& world)
{
  for (unsigned i = 0; i < boneTransforms.size(); ++i)
  {
    if (mBones[i].mParentIndex != -1 && mBones[i].mBindTransform.v != Vector3::cZero)
    {
      VQS currBone = boneTransforms[i];
      VQS parentBone = boneTransforms[mBones[i].mParentIndex];
      Vector3 v = TransformPoint(world, currBone.v);

      Vector3 up(0, 1, 0);
      Vector3 tmp, p1, p2, p3, p4;
      float scale = (parentBone.v - currBone.v).Length();
      scale *= .1f;

      tmp = Cross(up, (currBone.v - parentBone.v));
      if (tmp != Vector3::cZero)
        tmp.Normalize();

      p1 = parentBone.v + tmp * scale;
      p2 = parentBone.v - tmp * scale;
      p1 = TransformPoint(world, p1);
      p2 = TransformPoint(world, p2);

      Renderer::getInstance()->DrawLinePerma(p1, v, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(p2, v, COLOR::LIME_GREEN);

      tmp = Cross(tmp, currBone.v - parentBone.v);
      if (tmp != Vector3::cZero)
        tmp.Normalize();

      p3 = parentBone.v + tmp * scale;
      p4 = parentBone.v - tmp * scale;
      p3 = TransformPoint(world, p3);
      p4 = TransformPoint(world, p4);

      Renderer::getInstance()->DrawLinePerma(p3, v, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(p4, v, COLOR::LIME_GREEN);

      Renderer::getInstance()->DrawLinePerma(p1, p3, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(p2, p3, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(p2, p4, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(p4, p1, COLOR::LIME_GREEN);
    }
  }
}
