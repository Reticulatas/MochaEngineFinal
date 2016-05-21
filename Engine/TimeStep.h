/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma  once
#include "PhysicsParam.h"
#include "PhysicsState.h"
struct TimeStep
{
	float dt;
	unsigned velocityIterations;
	unsigned positionIterations;
	const PhysicsParams &params;
	mutable PhysicsStats &stats;

	TimeStep
		(
		float dt,
		unsigned velocityIterations,
		unsigned positionIterations,
		const PhysicsParams &params,
		PhysicsStats &stats
		)
		: dt(dt)
		, velocityIterations(velocityIterations)
		, positionIterations(positionIterations)
		, params(params)
		, stats(stats)
	{ }
};