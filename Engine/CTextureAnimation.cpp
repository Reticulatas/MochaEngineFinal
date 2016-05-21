/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CTextureAnimation.h"
#include "CMeshRenderer.h"
#include "SpriteAnimationSet.h"
 
 
#include "CTransform.h"
#include "AssetManager.h"
#include "boost/filesystem.hpp"
#include "IRegisterable.h"
#include "SerializationIncludes.h"

void CTextureAnimation::FlipHorizontal(bool flip)
{
	mFlipHorizontal = flip;
}
void CTextureAnimation::FlipVertical(bool flip)
{
	mFlipVertical = flip;
}

CTextureAnimation::CTextureAnimation( void )
{
  mTextureOffset = Vector2(0, 0);
	mMaterial = 0;
	mFlipHorizontal = false;
	mFlipVertical = false;
}

void CTextureAnimation::OnInit( void )
{
	mIsPlaying = false;
	mCurrentFrame = 0;
	mElapsedTime = 0.0f;
	mAnimationTimeScale = 1.0f;
	mAnimationSet = 0;
	mCurrentAnimation = 0;
	mForward = true;
}

void CTextureAnimation::OnFree( void )
{
	//mMaterial->SetTextureScale(XMFLOAT2(1.0f,1.0f), 0);
 	//mMaterial->SetTextureOffset(XMFLOAT2(0.0f,0.0f), 0);

	//if(mAnimationSet)
//		delete mAnimationSet;
}

void CTextureAnimation::OnStart()
{
	RequireDependency<CMeshRenderer>();

	if (!mAnimationSet)
		mAnimationSet = new SpriteAnimationSet();
	
	mMaterial = gameObject->GetComponent<CMeshRenderer>()->GetCopyMaterial();

	boost::filesystem::path texPath (mMaterial->GetTexture()->FileID().Filename());
	boost::filesystem::path animPath (texPath.remove_filename().string() + "/" + texPath.stem().string() + ".anim");

	if (boost::filesystem::exists(animPath))
	{
		try
		{
			std::ifstream ifs(animPath.string());
			if (!ifs.good())
			{
				Log("ERROR: Problem loading " + animPath.string() + "\n", LogEntry::Error);
				return;
			}
			boost::archive::xml_iarchive ia(ifs);

			std::map<std::string, SpriteAnimation> mAnimations;
			ia >> BOOST_SERIALIZATION_NVP(mAnimations);
			std::vector<std::string> mAnimationsByIndex;
			ia >> BOOST_SERIALIZATION_NVP(mAnimationsByIndex);

			mAnimationSet->mAnimations = mAnimations;
			mAnimationSet->mAnimationsByIndex = mAnimationsByIndex;
			PlayAnimation(0);
			mAnimationSet->RebuildAnimationIndices();

		}
		catch (std::exception const& e)
		{
			Log("ERROR occurred during loading: " + std::string(e.what()), LogEntry::Error);
			return ;
		}
	}
}

void CTextureAnimation::OnUpdate( void )
{
	if(!mIsPlaying)
		return;

	if(mMaterial && mCurrentAnimation)
	{
		mElapsedTime += FrameController::getInstance()->GetDeltaTime() * mAnimationTimeScale;

		if(mElapsedTime >= 1.0f/mCurrentAnimation->GetAnimationSpeed())
		{
			mElapsedTime = 0.0f;
			if(mForward)
			{
				mCurrentFrame++;
			}
			else
			{
				mCurrentFrame--;

				if(mCurrentFrame == -1)
				{
					mForward = true;
					mCurrentFrame = 1;
				}
			}

			if(mCurrentFrame >= mCurrentAnimation->GetFrameCount())
				ProcessEndOfAnimation();	

		}

		UpdateTextureTransform();
	}
}

void CTextureAnimation::ChangeCurrentAnimation( std::string animationSetName )
{
	SpriteAnimation* newAnim = mAnimationSet->GetAnimation(animationSetName);
	ChangeCurrentAnimation(newAnim);
}

void CTextureAnimation::ChangeCurrentAnimation( unsigned int index )
{
	SpriteAnimation* newAnim = mAnimationSet->GetAnimation(index);
	ChangeCurrentAnimation(newAnim);
}

void CTextureAnimation::ChangeCurrentAnimation( SpriteAnimation* newAnim )
{
	//invalid anim case
	if (newAnim == 0)
	{
		Log("Animation doesn't exist", LogEntry::Error);
		return;
	}

	if(mAnimationSet)
	{
		if(newAnim && newAnim != mCurrentAnimation)
		{
			mCurrentAnimation = newAnim;
			mCurrentFrame = 0;
			this->mIsPlaying = true;
		}

	}
}

void CTextureAnimation::PauseCurrentAnimation()
{
	this->mIsPlaying = false;
}

void CTextureAnimation::StopCurrentAnimation()
{
	mCurrentFrame = 0;
	PauseCurrentAnimation();
	UpdateTextureTransform();
}

void CTextureAnimation::UpdateTextureTransform()
{
	float frameWidth = float(mCurrentAnimation->GetFrameWidth()) / float(mMaterial->GetTextureWidth(0)-1);
	float frameHeight = 1.0f / float(mAnimationSet->GetAnimationCount());

	mTextureOffset.x = (frameWidth * mCurrentFrame) + 1;
	mTextureOffset.y = (mCurrentAnimation->GetIndex() * frameHeight) + 1;

	// Flip Texture 
	float FlipWidth = 1.0f;
	if(mFlipHorizontal)
	{
		FlipWidth = -1.0f;
		mTextureOffset.x = frameWidth * (mCurrentFrame + 1);
	}

	float FlipHeight = 1.0f;
	if(mFlipVertical)
	{
		FlipHeight = -1.0f;
	}

	

	mMaterial->SetTextureScale(Vector2(FlipWidth * frameWidth, FlipHeight * frameHeight), 0);
	mMaterial->SetTextureOffset(mTextureOffset, 0);

	this->gameObject->transform->scale.y = this->gameObject->transform->scale.x *(float(frameHeight * (mMaterial->GetTextureHeight(0)-1)) / float(frameWidth * (mMaterial->GetTextureWidth(0)-1)));
}

void CTextureAnimation::ProcessEndOfAnimation()
{
	if(mCurrentAnimation->GetLoopType() == LOOP_TYPES::LT_LOOP)
	{
		mCurrentFrame = 0;
	}
	else if(mCurrentAnimation->GetLoopType() == LOOP_TYPES::LT_CHANGE_ANIMATION)
	{
		mCurrentFrame = 0;
		ChangeCurrentAnimation(mCurrentAnimation->GetNextAnimationIndex());
	}
	else if(mCurrentAnimation->GetLoopType() == LOOP_TYPES::LT_NONE)
	{
		PauseCurrentAnimation();
	}
	else if(mCurrentAnimation->GetLoopType() == LOOP_TYPES::LT_PINGPONG)
	{
		//StopCurrentAnimation(); Implement ping pong
		//PauseCurrentAnimation();
		mForward = !mForward;
		mCurrentFrame = mCurrentAnimation->GetFrameCount()-2;
	}
	else
	{
		PauseCurrentAnimation();
	}
}

void CTextureAnimation::PlayAnimation( std::string AnimationName )
{
	ChangeCurrentAnimation(AnimationName);
	mIsPlaying = true;
}

void CTextureAnimation::PlayAnimation( unsigned int animIndex )
{
	ChangeCurrentAnimation(animIndex);
	mIsPlaying = true;
}

CTextureAnimation::~CTextureAnimation()
{

}

___COMP_REGISTER(CTextureAnimation);
meta_define(CTextureAnimation);

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change