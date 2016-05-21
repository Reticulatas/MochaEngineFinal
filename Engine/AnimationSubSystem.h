#pragma once

#include "ISubsystem.h"
#include "CAnimationController.h"
#include "CSkinMeshRenderer.h"

class AnimationSubSystem : public ISubsystem
{
public:

  AnimationSubSystem();
  ~AnimationSubSystem();

  bool Tick();
  void Animate(CAnimationController* animController);
  void InterpolateKeyFrames(Animation* anim, float t, std::vector<VQS>& boneTransforms);

  static AnimationSubSystem* getInstance();

  metadef(AnimationSubSystem)
    endmetadef;
};