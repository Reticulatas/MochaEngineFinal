/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
struct ResolutionMaterial
{
	float mDensity;
	float mRestitution;
	float mFriction;

	ResolutionMaterial(float density = 1.0f, float restitution = 0.0f, float friction = 1.0f)
		: mDensity(density), mRestitution(restitution), mFriction(friction)
	{ }

	___SERIALIZE_BEGIN___
		___DEFSER(mDensity,1)
		___DEFSER(mRestitution,1)
		___DEFSER(mFriction,1)
		___SERIALIZE_END___
};
___SERIALIZE_CLASS(ResolutionMaterial, 1);
