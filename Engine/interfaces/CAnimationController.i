%{
#include "CAnimationController.h"
%}

namespace std {
%template(AnimContainer)vector<AssetHandle>;
}

class CAnimationController : public Component
{
  CAnimationController();
  virtual ~CAnimationController();

public:

	void SetAnimation(const std::string& animName);
	void AddAnimation(AssetHandle& anim);
  Animation* GetActiveAnimation();

  bool mIsPlaying;
  float mAnimSpeed;
  AssetHandle mSingleAnim;
  std::vector<AssetHandle> mAnimations;
};