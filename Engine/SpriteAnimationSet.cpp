/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "SpriteAnimationSet.h"

void SpriteAnimationSet::RemoveAnimation( const SpriteAnimation& anim )
{

}

void SpriteAnimationSet::AddAnimation( std::string animationName,const SpriteAnimation& anim )
{
	mAnimations.insert(std::pair<std::string,SpriteAnimation>(animationName,anim));
	mAnimationsByIndex.push_back(animationName);
}

void SpriteAnimationSet::AddAnimation( std::string animationName, unsigned int frameCount, float frameWidth, float animationSpeed /*= 1.0f/24.0f*/, LOOP_TYPES ltype /*= LOOP_NONE*/,unsigned int nextAnimation)
{
	SpriteAnimation anim;

	anim.SetName(animationName);
	anim.SetAnimationSpeed(animationSpeed);
	anim.SetFrameCount(frameCount);
	anim.SetFrameWidth(frameWidth);
	anim.SetIndex(GetAnimationCount());
	anim.SetLoopType(ltype);
	anim.SetNextAnimationIndex(nextAnimation);

	AddAnimation(animationName,anim);
}

/************************************************************************/
/*					SPRITE ANIMATION SET IMPLEMENTATION					*/
/************************************************************************/
SpriteAnimation* SpriteAnimationSet::GetAnimation(std::string animationName)
{
	std::map<std::string,SpriteAnimation>::iterator it;
	it = mAnimations.find(animationName);
	if(it == mAnimations.end())
		return 0;
	else
		return &(it->second);
}

SpriteAnimation* SpriteAnimationSet::GetAnimation(unsigned int animationIndex)
{
	return GetAnimation(mAnimationsByIndex[animationIndex]);
}

unsigned int SpriteAnimationSet::GetAnimationCount()
{
	return mAnimationsByIndex.size();
}

void SpriteAnimationSet::RebuildAnimationIndices()
{
	unsigned index = 0;
	for (std::string& s : mAnimationsByIndex)
	{
		mAnimations[s].SetIndex(index);
		++index;
	}
}


/************************************************************************/
/*					SPRITE ANIMATION CLASS IMPLEMENTATION				*/
/************************************************************************/

unsigned int SpriteAnimation::GetFrameCount()
{
	return frameCount;
}

float SpriteAnimation::GetFrameWidth()
{
	return frameWidth;
}

unsigned int SpriteAnimation::GetIndex()
{
	return index;
}

float SpriteAnimation::GetAnimationSpeed()
{
	return animationSpeed;
}

LOOP_TYPES SpriteAnimation::GetLoopType()
{
	return loopType;
}

unsigned int SpriteAnimation::GetNextAnimationIndex()
{
	return nextAnimationIndex;
}

void SpriteAnimation::SetName( const std::string Name )
{
	name = Name;
}

void SpriteAnimation::SetIndex( unsigned int index )
{
	this->index = index;
}

void SpriteAnimation::SetFrameCount( unsigned int FrameCount )
{
	this->frameCount = FrameCount;
}

void SpriteAnimation::SetFrameWidth( float frameWidth )
{
	this->frameWidth = frameWidth;
}

void SpriteAnimation::SetAnimationSpeed( float speed)
{
	this->animationSpeed = speed;
}

void SpriteAnimation::SetLoopType( LOOP_TYPES lt )
{
	this->loopType = lt;
}

void SpriteAnimation::SetNextAnimationIndex( unsigned int nextAnimIndex )
{
	this->nextAnimationIndex = nextAnimIndex;
}
