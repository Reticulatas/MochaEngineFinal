/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
struct MassData
{
	float mass;
	float inverseMass;
	Math::Matrix3 unitLocalInertiaTensor;
	MassData(void)
		: mass(0.0f)
		, inverseMass(0.0f)
	{
		unitLocalInertiaTensor.ZeroOut();
	}
};