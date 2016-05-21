/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include <vector>
#include "ISubsystem.h"

struct BaseState;
class GameObject;

/* 
	Game State Manager Class
*/
class GSM : public ISubsystem
{
	std::vector<BaseState*> mStates; // List of all active states
	typedef std::vector<BaseState*>::iterator StateIter;
	bool mIsFrozen;

	GSM(void);

public:

  typedef std::vector<BaseState*>::const_iterator ConstStateIter;

  void PushStateToBottom(BaseState* state, bool doInit = true);
	void PushState(BaseState* state, bool doInit = true); /* Push a new state to the top of the stack. It becomes the active state*/
	void PopState();
	void RemoveState(BaseState* state);
	bool Tick();
	void Draw();
	void Freeze(); /* Stops updating current state, but continues with creation/deletion */
	void UnFreeze();
	bool IsFrozen();

	BaseState* getStateAtIndex(unsigned i);
	BaseState* getActiveState(); /* Returns a pointer to the active state*/
	unsigned GetNumberOfStates(void); /* Returns the number of inactive and active states */
	BaseState* getStateUnderActive();
  ConstStateIter EnumerateStates(); //Returns iterator for all states
  ConstStateIter EnumeratedLastState(); //returns iterator to one-over-the-end of the state list

	static GSM* getInstance(); // singleton 

	friend class FullGameObjectIterator;

	metadef(GSM)
		m_add(bool, mIsFrozen);
	endmetadef;
};

/* All object iterator class */

/*
class FullGameObjectIterator
{
	std::vector<GameObject*>::iterator objIter;
	std::vector<GameObject*>* curList;
	BaseState* curState;
	unsigned curStateNum; 
	bool iterateUnmanaged;
	bool iteratingUnmanaged;
	bool hasIteratedEditorState;
	bool isGood;
public:
	//if you want to iterate unmanaged as well, set the first flag
	FullGameObjectIterator(bool iterateUnmanaged);

	//go to next game object
	const FullGameObjectIterator& operator++(void);

	//check for iterator validity
	bool good();

	//return the current object
	GameObject* getObject();
};
*/

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change