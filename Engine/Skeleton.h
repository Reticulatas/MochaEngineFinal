#pragma once

#include <vector>
#include "Math/VQS.h"

#define MAX_SKELBONES 80

struct Bone
{
  std::string mName;
  int mParentIndex;
  VQS mBindTransform;
  VQS mBoneSpaceTransform;

  Bone()
  {
    mParentIndex = -1;
  }
};

class Skeleton
{
public:

  Skeleton();
  ~Skeleton();
  void DrawBindPose();
  void DrawBones(std::vector<VQS>& boneTransforms, Matrix4& world);
  std::vector<Bone>& GetBones();

private:

  std::vector<Bone> mBones;

  friend class ModelLoader;
};