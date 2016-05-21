/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"

namespace Jacobian
{

	struct J1V
	{
		Math::Vector3 v;
	};

	struct J1W
	{
		Math::Vector3 w;
	};

	struct J1V1W
	{
		Math::Vector3 v;
		Math::Vector3 w;
	};

	struct J2V
	{
		Math::Vector3 vA;
		Math::Vector3 vB;
	};

	struct J2W
	{
		Math::Vector3 wA;
		Math::Vector3 wB;
	};

	struct J2V2W
	{
		Math::Vector3 vA;
		Math::Vector3 wA;
		Math::Vector3 vB;
		Math::Vector3 wB;
	};

}