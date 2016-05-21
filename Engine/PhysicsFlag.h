/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
struct PhysicsFlags
{
	typedef unsigned char value_type;
	static const value_type CAN_SLEEP = 1;
	static const value_type AWAKE = 2;
	static const value_type ISLAND = 4;
	static const value_type PICK = 8;
	value_type rawData;

	PhysicsFlags(void)
		: rawData(0)
	{ }

	__forceinline PhysicsFlags &Set(value_type bits)
	{
		rawData |= bits;
		return *this;
	}

	__forceinline PhysicsFlags &Clear(value_type bits)
	{
		rawData &= ~bits;
		return *this;
	}

	__forceinline bool Test(value_type bits) const
	{
		return (rawData & bits) != 0;
	}
};