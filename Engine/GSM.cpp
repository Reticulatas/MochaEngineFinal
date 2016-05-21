/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "EditorSubsystem.h"
#include "EditorSubsystem_Interface.h"
#include "EditorState.h"

  // Base State Class
const bool ONLYDRAWTOP = true;

bool GSM::Tick()
{
	mStates.back()->UpdateState();

	//Always update, regardless of active or stack
	//except for editor state, to prevent sound issues
	std::vector<BaseState*>::iterator iter;
	for(iter = mStates.begin(); iter != mStates.end(); iter++)
	{
		(*iter)->AlwaysUpdateState();
	}

  // Note GAME LOOP:   GSM -> RENDERER -> FRAMECONTROLLER -> PHYSICS -> SS 
  // Do all transform updates right before drawing
	for (auto state : mStates)
	{
		const std::list<CTransform*>* listTrans = state->GetListOfComponentsOfType<CTransform>();
		if (listTrans)
		{
			for (auto& transformIter : *listTrans)
				transformIter->OnUpdate();
		}
	}
// const std::list<CTransform*>* listTrans = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CTransform>();
// for (auto& transformIter : *listTrans)
//	  transformIter->OnUpdate();
  //new location GSM draw
	GSM::getInstance()->Draw();

	return true;
}

void GSM::Draw()
{
	if (ONLYDRAWTOP)
	{
		mStates.back()->Draw_State();
	}
	else
	{
		std::vector<BaseState*>::iterator iter;
		for(iter = mStates.begin(); iter != mStates.end(); iter++)
		{
			(*iter)->Draw_State();
		}
	}
}

/* Push a new state to the list. This will be the active state*/
void GSM::PushState(BaseState* state, bool doInit /* = true */)
{
	//add state to list first to make it active before initiating
		this->mStates.push_back(state);	

	state->LoadState();
	if (doInit)
		state->InitState();
}

/* Remove the active state and call the Free and Unload Methods*/
void GSM::PopState()
{
	if(this->mStates.size())
	{
		BaseState* state = this->getActiveState();
		RemoveState(state);
	}
}

void GSM::RemoveState( BaseState* state )
{
	assert(state != 0);
	if (std::find(mStates.begin(), mStates.end(), state) == mStates.end())
		throw std::exception("State not found to remove");

  Destroy(state);
  mStates.erase(std::remove(mStates.begin(), mStates.end(), state),mStates.end());
}

/* Singleton */
GSM* GSM::getInstance()
{
	static GSM* instance  = 0;

	if(!instance)
		instance = new GSM();

	return instance;
}

/*Returns the current state*/
BaseState* GSM::getActiveState()
{
	BaseState* state = 0;

	if(this->mStates.size() > 0)
		state = this->mStates[this->mStates.size()-1];

	return state ;
}

void GSM::Freeze()
{
	mIsFrozen = true;
}

void GSM::UnFreeze()
{
	mIsFrozen = false; 
}

bool GSM::IsFrozen()
{
	return mIsFrozen;
}

GSM::GSM( void )
{
	mIsFrozen = false;
}

unsigned GSM::GetNumberOfStates( void )
{
	return mStates.size();
}

BaseState* GSM::getStateUnderActive()
{
	//not enough states to enumerate
	if (getActiveState() == 0 || GetNumberOfStates() < 2)
		return 0;

	//active state is first state
	if (getActiveState() == mStates.front())
		return 0;

	return *(--std::find(mStates.begin(), mStates.end(), getActiveState()));
}

GSM::ConstStateIter GSM::EnumerateStates()
{
  return mStates.begin();
}

GSM::ConstStateIter GSM::EnumeratedLastState()
{
  return mStates.end();
}

void GSM::PushStateToBottom(BaseState* state, bool doInit /*= true*/)
{
	//add state to list first to make it active before initiating
	this->mStates.insert(mStates.begin(),state);

	state->LoadState();
	if (doInit)
		state->InitState();
}

BaseState* GSM::getStateAtIndex(unsigned i)
{
	if (i >= GetNumberOfStates())
		return 0;
	return mStates[i];
}

//iterator implementations
/*

FullGameObjectIterator::FullGameObjectIterator( bool _iterateUnmanaged )
{
	iteratingUnmanaged = false;
	iterateUnmanaged = _iterateUnmanaged;
	hasIteratedEditorState = false;
	curStateNum = 0;
	isGood = true;
	//no states
	if (GSM::getInstance()->mStates.size() == 0)
  {
		isGood = false;
    return;
  }
	curState = GSM::getInstance()->mStates.at(curStateNum);
	objIter = (*curState->mManagedObjects).begin();
	curList = &curState->mManagedObjects;
}

const FullGameObjectIterator& FullGameObjectIterator::operator++( void )
{
	if (!isGood)
		return *this;

	//if we aren't already at the end of the list, keep incrementing
	if ((*curList).size() != 0)
		objIter++;
	else
		objIter = (*curList).end();

	if (objIter == (*curList).end())
	{
		//increment to unmanaged objects, if necessary
		if (!iteratingUnmanaged && iterateUnmanaged && curState->mUnmanagedObjects.size() != 0)
		{
			iteratingUnmanaged = true;
			objIter = curState->mUnmanagedObjects.begin();
			curList = &curState->mUnmanagedObjects;
		}
		//increment to next state, if possible
		else if (++curStateNum < GSM::getInstance()->mStates.size())
		{
			iteratingUnmanaged = false;
			curState = GSM::getInstance()->mStates.at(curStateNum);
			objIter = curState->mManagedObjects.begin();
			curList = &curState->mManagedObjects;
		}
		//we're at the end of the road
		else
		{
			isGood = false;
			return *this;
		}
	}
	return *this;
}

bool FullGameObjectIterator::good()
{
	return isGood;
}

GameObject* FullGameObjectIterator::getObject()
{
  if (!isGood)
    return 0;
	if (curList->size() != 0)
		return *objIter;
	return 0;
}*/

meta_define(GSM);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
// 10/25/13 - added iterator (nicholas)

