/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include "InList.h"
#include "AABB.h"
class Collider;
struct Proxy
{
	IN_LIST_LINK(Proxy) broadphaseLink; // links all proxies in broadphase

	// points to the fixture the proxy represents
	Collider *parent;
	void *userData;
	AABB aabb;
	Math::Vector3 &minPoint, &maxPoint;

	Proxy(void)
		: parent(nullptr)
		, userData(nullptr)
		, minPoint(aabb.minPoint)
		, maxPoint(aabb.maxPoint)
	{ }

	Proxy(const Math::Vector3 &minPoint, const Math::Vector3 &maxPoint)
		: parent(nullptr)
		, userData(nullptr)
		, aabb(minPoint, maxPoint)
		, minPoint(aabb.minPoint)
		, maxPoint(aabb.maxPoint)
	{ }

	bool Contains(const Math::Vector3 &pos) const { return aabb.Contains(pos); }
	bool Contains(const Proxy &rhs) const { return aabb.Contains(rhs.aabb); }
	bool Collides(const Proxy &rhs) const { return aabb.Collides(rhs.aabb); }
};