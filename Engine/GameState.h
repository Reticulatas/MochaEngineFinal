/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once
 

/*
  Template for a basic game state
*/
class GameState: public BaseState
{
  ~GameState() { }
public:
  virtual void LoadState();
  virtual void InitState();
  virtual void Draw_State();
  virtual void UnloadState();
  virtual void FreeState();
  void UpdateState();

  static GameState* getInstance(); // singleton
};



/////////////////////////////////////
//Original Author: author
//Modifications:
//	Date	-	Change