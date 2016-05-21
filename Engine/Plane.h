#pragma once

#include "Ray3.h"

namespace Math {

	struct Plane
	{
		Vector3	Pos;
		Vector3	Normal;
	};

	bool Ray3DToPlane(Ray3& ray, Plane& plane, Vector3& result);
}
