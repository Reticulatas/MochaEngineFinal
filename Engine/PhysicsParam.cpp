#include "stdafx.h"
#include "PhysicsParam.h"
PhysicsParams::PhysicsParams(void)

// world
: linearDamping(0.025f)
, angularDamping(0.025f)

// warm start
, warmStart(true)
, warmStartFraction(0.95f)
, contactPersistenceThreshold(0.05f)

// sleep
, allowSleep(true)
, sleepLinearThreshold(0.25f)
, sleepAngularThreshold(0.25f)
, sleepTimeThreshold(2.0f)

// collision resolution
, velocityBaumgarteSlop(0.04f)
, velocityBaumgarte(0.2f)
, restitutionSlop(0.15f)
, frictionSlop(0.01f)
, positionBaumgarteSlop(0.045f)
, positionBaumgarte(0.05f)
, minPositionCorrection(0.00025f)
, maxPositionCorrection(0.5f)
{ }