/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include "Asset.h"
#include <map>
 

enum LOOP_TYPES {LT_NONE,LT_LOOP,LT_PINGPONG,LT_CHANGE_ANIMATION};

class SpriteAnimation
{
	std::string name;
	unsigned int index;
	unsigned int frameCount;
	float frameWidth;
	float animationSpeed;
	LOOP_TYPES loopType;
	unsigned int nextAnimationIndex;
public:

	/* By animation name */
	unsigned int GetFrameCount();
	float GetFrameWidth();
	unsigned int GetIndex();
	float GetAnimationSpeed();
	LOOP_TYPES GetLoopType();
	unsigned int GetNextAnimationIndex();			/* If animation flag = LT_CHANGE_ANIMATION */

	void SetName(const std::string Name);
	void SetIndex(unsigned int index);
	void SetFrameCount(unsigned int FrameCount);
	void SetFrameWidth(float frameWidth);
	void SetAnimationSpeed(float speed);
	void SetLoopType(LOOP_TYPES lt);
	void SetNextAnimationIndex(unsigned int nextAnimIndex);

	___SERIALIZE_BEGIN___
		___DEFSER(name, 1)
		___DEFSER(frameCount, 1)
		___DEFSER(frameWidth, 1)
			index = 0;
		___DEFSER(loopType, 1)
		___DEFSER(animationSpeed, 1)
		___DEFSER(nextAnimationIndex, 1)
	___SERIALIZE_END___
};
___SERIALIZE_CLASS(SpriteAnimation, 1);

class SpriteAnimationSet
{

	/************************************************************************/
	/*						 Member variables                               */
	/************************************************************************/

	std::map<std::string,SpriteAnimation> mAnimations;				// Map of all animations
	std::vector<std::string> mAnimationsByIndex;

public:

	void RemoveAnimation(const SpriteAnimation& anim);
	void AddAnimation(std::string animationName,const SpriteAnimation& anim);
	void AddAnimation(std::string animationName,
						unsigned int frameCount,
						float frameWidth,
						float animationSpeed = 1.0f/24.0f,
						LOOP_TYPES ltype = LT_NONE,unsigned int nextAnimation = 0);

	SpriteAnimation* GetAnimation(std::string animationName);
	SpriteAnimation* GetAnimation(unsigned int animationIndex);

	unsigned int GetAnimationCount();
	void RebuildAnimationIndices();

	___SERIALIZE_LOAD_BEGIN___
		___DEFSER(mAnimations, 1)
		___DEFSER(mAnimationsByIndex, 1)
		RebuildAnimationIndices();
	___SERIALIZE_END___
	___SERIALIZE_SAVE_BEGIN___
		___DEFSER(mAnimations, 1)
		___DEFSER(mAnimationsByIndex, 1)
	___SERIALIZE_END___
	___SERIALIZE_SAVE_LOAD___

	friend class CTextureAnimation;
};
___SERIALIZE_CLASS(SpriteAnimationSet, 1);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
