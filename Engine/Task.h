/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once
#include "IBarristaOperation.h"

class Task : public IBarristaOperation
{
public:
	virtual bool Tick() = 0;			/* Override this method to make your own task */
};

/////////////////////////////////////