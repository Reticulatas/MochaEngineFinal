#include "stdafx.h"
#include "JointPhysics.h"
#include "MJoint.h"
Joint *Joint::Create(const JointDef &def, Allocator &allocator)
{
	Joint *joint = nullptr;
	switch (def.type)
	{
	case JointType::MOUSE:
	{
							   joint =
								   new (allocator.Allocate(sizeof(MouseJoint)))
								   MouseJoint(static_cast<const MouseJointDef &>(def));
							   break;
	}
	default:
		GV_ASSERT_MESSAGE(false, "Invalid joint type.");
	}
	joint->type = def.type;

	return joint;
}

void Joint::Destroy(Joint *joint, Allocator &allocator)
{
	unsigned jointSize = 0;
	switch (joint->type)
	{
	case JointType::MOUSE: jointSize = sizeof(MouseJoint); break;
	default: GV_ASSERT_MESSAGE(false, "Invalid joint type.");
	}
	joint->~Joint();
	allocator.Free(joint, jointSize);
}

Joint::~Joint(void) { }