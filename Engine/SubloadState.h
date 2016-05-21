/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "BaseState.h"

//The state that is shown during loading

class SubloadState: public BaseState
{
	~SubloadState() { }
public:
	SubloadState() : BaseState() { };
	SubloadState(const SubloadState& b);

	virtual void LoadState();
	virtual void InitState();
	virtual void Draw_State();
	virtual void UnloadState();
	virtual void FreeState();
	void UpdateState();

	static SubloadState* getInstance(); // singleton
};



/////////////////////////////////////
//Original Author: author
//Modifications:
//	Date	-	Change
//	