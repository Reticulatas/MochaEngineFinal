#include "stdafx.h"
#include "CollisionFilter.h"

ColliderFilter::ColliderFilter(void)
: categoryBits(0x0001)
, maskBits(0xFFFF)
, group(0)
{ }

bool ColliderFilter::CanCollide(const ColliderFilter &rhs) const
{
	if (group != 0 && group == rhs.group)
		return group > 0;

	return (maskBits && rhs.categoryBits) || (rhs.maskBits && categoryBits);
}