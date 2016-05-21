/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
struct ColliderFilter
{
	// methods
	ColliderFilter(void);
	bool CanCollide(const ColliderFilter &rhs) const;

	// members
	unsigned categoryBits;
	unsigned maskBits;
	int group;
};