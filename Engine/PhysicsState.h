/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */
#pragma once

struct PhysicsStats
{
	unsigned rigidBodies;
	unsigned colliders;
	unsigned manifolds;
	unsigned broadphasePairs;
	unsigned collisions;
	unsigned contacts;
	unsigned joints;
	unsigned rayCasts;

	float integration;
	float broadphase;
	float narrowphase;
	float contactResolution;
	float jointResolution;

	unsigned islands;
	unsigned minIslandSize;
	unsigned maxIslandSize;
	float avgIslandSize;

	float energy;

	void Clear(void)
	{
		rigidBodies = 0;
		colliders = 0;
		manifolds = 0;
		broadphasePairs = 0;
		collisions = 0;
		contacts = 0;
		joints = 0;
		rayCasts = 0;

		integration = 0.0f;
		broadphase = 0.0f;
		narrowphase = 0.0f;
		jointResolution = 0.0f;
		contactResolution = 0.0f;

		islands = 0;
		minIslandSize = 0xffffffff;
		maxIslandSize = 0;
		avgIslandSize = 0;

		energy = 0.0f;
	}

	PhysicsStats(void) { Clear(); }
};