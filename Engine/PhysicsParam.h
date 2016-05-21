/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
struct PhysicsParams
{
	float linearDamping;
	float angularDamping;
	bool  warmStart;
	float warmStartFraction;
	float contactPersistenceThreshold;
	bool  allowSleep;
	float sleepLinearThreshold;
	float sleepAngularThreshold;
	float sleepTimeThreshold;

	float velocityBaumgarteSlop;
	float velocityBaumgarte;
	float restitutionSlop;
	float frictionSlop;
	float positionBaumgarteSlop;
	float positionBaumgarte;
	float minPositionCorrection;
	float maxPositionCorrection;

	PhysicsParams(void);
};