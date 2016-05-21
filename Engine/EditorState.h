/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 

/*
Template for a basic game state
*/
class EditorState: public BaseState
{
	~EditorState() { }

public:
	EditorState() : BaseState() { };
	EditorState(EditorState& b);

	virtual void LoadState();
	virtual void InitState();
	virtual void Draw_State();
	virtual void UnloadState();
	virtual void FreeState();
	void UpdateState();

	virtual void AlwaysUpdateState();

};



/////////////////////////////////////
//Original Author: author
//Modifications:
//	Date	-	Change
//	