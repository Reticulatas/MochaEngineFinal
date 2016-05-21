/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "EditorState.h"
 

void EditorState::LoadState()
{
}

void EditorState::UpdateState()
{
	//will not be called
	//BaseState::UpdateState();
}

void EditorState::InitState()
{
}

void EditorState::Draw_State()
{
	BaseState::Draw_State();
}

void EditorState::FreeState()
{
}

void EditorState::UnloadState()
{
}

EditorState::EditorState( EditorState& b ) : BaseState(b)
{
}

void EditorState::AlwaysUpdateState()
{
	BaseState::UpdateState();
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
//	