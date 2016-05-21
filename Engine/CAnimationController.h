#pragma once

#include "Component.h"
#include "Math/VQS.h"
#include "SkinnedMesh.h"

class CAnimationController : public Component
{
public:

  CAnimationController();
  virtual ~CAnimationController();

  void OnStart();
  void OnFree();
  void OnInit();
  void OnUpdate();

  void SetAnimation(const std::string& animName);
  void AddAnimation(AssetHandle& anim);
  void Bind();
  Animation* GetActiveAnimation();
  std::vector<VQS>& GetFinalTransforms();

  virtual void CopyFrom(const ::CAnimationController* c)
  {
    mIsPlaying = c->mIsPlaying;
    mTimer = c->mTimer;
    mAnimSpeed = c->mAnimSpeed;
    ActiveAnimationIndex = c->ActiveAnimationIndex;
    mAnimations = c->mAnimations;
    mFinalTransforms = c->mFinalTransforms;
    mSingleAnim = c->mSingleAnim;
    Component::CopyFrom(c);
  }
  COMPONENTCLONE(CAnimationController);

  bool mIsPlaying;
  float mAnimSpeed;
  AssetHandle mSingleAnim;
  std::vector<AssetHandle> mAnimations;

  friend class AnimationSubSystem;

private:

  float mTimer;    
  unsigned ActiveAnimationIndex;
  std::vector<VQS> mFinalTransforms;

  void Interpolate(Animation* anim, float t, std::vector<VQS>& boneTransforms);
  void Animate(float dt);

  ___SERIALIZE_BEGIN___
	  ___DEFBASE(Component)
	  ___DEFSER(mAnimations, 1);
    ___DEFSER(ActiveAnimationIndex, 1);
    ___DEFSER(mAnimSpeed, 2);
    ___DEFSER(mIsPlaying, 2);
    ___DEFSER(mAnimations, 2);
    ___DEFSER(mSingleAnim, 2);
  ___SERIALIZE_END___
	
public:
    metadef(CAnimationController)
    endmetadef;
};
___SERIALIZE_CLASS(CAnimationController, 2);