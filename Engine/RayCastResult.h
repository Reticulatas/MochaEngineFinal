/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "Ray3.h"
class Collider;

struct RayCastResult
{
	bool hit;
	Ray3 ray;
	float t;
	Math::Vector3 intersection;
	Math::Vector3 normal;
	Collider *collider;
	GameObject* go;
	RayCastResult(void) : hit(false) { }
};