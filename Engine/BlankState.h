/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 

/*
Template for a basic game state
*/
class BlankState: public BaseState
{
	~BlankState() { }

public:
	BlankState() : BaseState() { };
	BlankState(BlankState& b);

	virtual void LoadState();
	virtual void InitState();
	virtual void Draw_State();
	virtual void UnloadState();
	virtual void FreeState();
	void UpdateState();

	static BlankState* getInstance(); // singleton
};



/////////////////////////////////////
//Original Author: author
//Modifications:
//	Date	-	Change
//	