/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"

struct AABB
{
	Math::Vector3 minPoint, maxPoint;
	AABB(void) { }
	AABB(const Math::Vector3 &minPoint, const Math::Vector3 &maxPoint)
		: minPoint(minPoint)
		, maxPoint(maxPoint){}

	bool Collides(const AABB &rhs) const
	{
		return (minPoint.x <= rhs.maxPoint.x)
			&& (maxPoint.x >= rhs.minPoint.x)
			&& (minPoint.y <= rhs.maxPoint.y)
			&& (maxPoint.y >= rhs.minPoint.y)
			&& (minPoint.z <= rhs.maxPoint.z)
			&& (maxPoint.z >= rhs.minPoint.z);
	}

	float Volume(void) const
	{
		return (maxPoint.x - minPoint.x) * (maxPoint.y - minPoint.y) * (maxPoint.z - minPoint.z);
	}

	const AABB Union(const AABB &rhs) const
	{
		AABB result;
		result.minPoint.x = MinFLT(minPoint.x, rhs.minPoint.x);
		result.minPoint.y = MinFLT(minPoint.y, rhs.minPoint.y);
		result.minPoint.z = MinFLT(minPoint.z, rhs.minPoint.z);
		result.maxPoint.x = MaxFLT(maxPoint.x, rhs.maxPoint.x);
		result.maxPoint.y = MaxFLT(maxPoint.y, rhs.maxPoint.y);
		result.maxPoint.z = MaxFLT(maxPoint.z, rhs.maxPoint.z);
		return result;
	}

	bool Contains(const AABB &rhs) const
	{
		return (minPoint.x <= rhs.minPoint.x)
			&& (maxPoint.x >= rhs.maxPoint.x)
			&& (maxPoint.y >= rhs.maxPoint.y)
			&& (minPoint.y <= rhs.minPoint.y)
			&& (maxPoint.z >= rhs.maxPoint.z)
			&& (minPoint.z <= rhs.minPoint.z);
	}

	bool Contains(const Math::Vector3 &pos) const
	{
		return (minPoint.x <= pos.x)
			&& (minPoint.y <= pos.y)
			&& (minPoint.z <= pos.z)
			&& (maxPoint.x >= pos.x)
			&& (maxPoint.y >= pos.y)
			&& (maxPoint.z >= pos.z);
	}
};
