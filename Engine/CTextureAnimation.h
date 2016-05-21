/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include "AssetHandle.h"
#include "SpriteAnimationSet.h"

class Material;
class SpriteAnimation;

 
class CTextureAnimation : public Component
{
public:
	CTextureAnimation(void);
	~CTextureAnimation();

	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);

	___BEGIN_REGISTERED___
	float mAnimationTimeScale;
	___END_REGISTERED___

	void ChangeAnimationSet(std::string animationSetName);

	 void ChangeCurrentAnimation(std::string animationSetName);
	 void ChangeCurrentAnimation(unsigned int index);

	SpriteAnimationSet* GetAnimationSet();
	 void PlayAnimation(std::string AnimationName);
	 void PlayAnimation(unsigned int animIndex);
	 void PauseCurrentAnimation();
	 void StopCurrentAnimation();
	 void FlipHorizontal(bool flip);
	 void FlipVertical(bool flip);

	virtual void CopyFrom(const ::CTextureAnimation* c)
	{
		mIsPlaying = c->mIsPlaying;
		mMaterial = c->mMaterial;
		mAnimationTimeScale = c->mAnimationTimeScale;
		mCurrentAnimation = 0;
		mAnimationSet = 0;
		mCurrentFrame = c->mCurrentFrame;
		Component::CopyFrom(c);
	}
	COMPONENTCLONE(CTextureAnimation);

private:
	void ChangeCurrentAnimation(SpriteAnimation* newAnim);

  Vector2 mTextureOffset;
	Material* mMaterial;
	SpriteAnimationSet* mAnimationSet;
	SpriteAnimation* mCurrentAnimation;

	//* Control variables *//
	bool mIsPlaying;
	unsigned int mCurrentFrame;
	float mElapsedTime;

	bool mFlipHorizontal;
	bool mFlipVertical;
	bool mForward;			// Records if it is playing forward or backwards

	void UpdateTextureTransform();
	void ProcessEndOfAnimation();

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(mIsPlaying, 1)
		___DEFSER(mMaterial, 1)
		___DEFSER(mAnimationTimeScale, 1)
		___DEFSER(mCurrentFrame, 1)
		___DEFSER(mAnimationSet, 2)
		___DEFSERONLY(mCurrentAnimation, 2)
	___SERIALIZE_END___

public:
	metadef(CTextureAnimation)
		m_add(float, mAnimationTimeScale)
		endmetadef;
};
 
___SERIALIZE_CLASS(CTextureAnimation, 3);

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change