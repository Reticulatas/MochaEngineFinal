#include "stdafx.h"
#include "Plane.h"

bool Math::Ray3DToPlane(Ray3& ray, Plane& plane, Vector3& result)
{
	float parallelResult = ray.dir.Dot(plane.Normal);

	if (fabs(parallelResult) > 0)
	{
		float d = (plane.Pos - ray.pos).Dot(plane.Normal);
		d /= parallelResult;
		result = ray.pos + d * ray.dir;
		return true;
	}
	return false;
}
