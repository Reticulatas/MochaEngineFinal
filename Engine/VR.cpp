#include "stdafx.h"
#include "VR.h"

VR::VR()
{
  mCurrentEyeIndex = 0;
  mVsyncEnabled = true;
}

VR::~VR()
{
  Release();
}

void VR::Release()
{
  mVR_RT.ReleaseTarget();
}

void VR::Resize()
{

}

unsigned VR::GetRecommendedWidth()
{
  return mRecommendedWidth;
}

unsigned VR::GetRecommendedHeight()
{
  return mRecommendedHeight;
}

Math::Vector3 VR::GetEyePos(unsigned index)
{
  return mEyePos[index];
}

Math::Vector3 VR::GetEyePos()
{
  return mEyePos[mCurrentEyeIndex];
}

Math::Matrix4 VR::GetProjMatrix(unsigned index)
{
  return mProj[index];
}

Math::Matrix4 VR::GetViewMatrix(unsigned index)
{
  return mView[index];
}
