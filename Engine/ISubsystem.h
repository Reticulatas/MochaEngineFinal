/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "IRegisterable.h"
#include "IBarristaOperation.h"

class ISubsystem : public IRegisterable, public IBarristaOperation
{
public:
	virtual ~ISubsystem(void) { }
	virtual bool Tick() = 0;
};

