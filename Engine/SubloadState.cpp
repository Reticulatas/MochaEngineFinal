/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "SubloadState.h"
#include "ObjectFactory.h"
#include "CFontRenderer.h"
#include "CScript.h"

void SubloadState::LoadState()
{
	//Implement Any asset loading here
}

void SubloadState::UpdateState()
{
	BaseState::UpdateState();
}

void SubloadState::InitState()
{
	GameObject* text = ObjectFactory::getInstance()->MakeNewGameObject("LoadingObj");
	CFontRenderer* f = text->AddComponent<CFontRenderer>();
	f->text = "Loading...";
	CScript* s = text->AddComponent<CScript>();
	s->AddScript("TextTypewriterEffect");

	GameObject* cam = ObjectFactory::getInstance()->MakeNewGameObject("Blue Cam");
	cam->AddComponent<CCamera>();
	CScript* cs = cam->AddComponent<CScript>();
	cs->AddScript("FadeScreen");

}

void SubloadState::Draw_State()
{
	BaseState::Draw_State();
}

void SubloadState::FreeState()
{
}

void SubloadState::UnloadState()
{
}

SubloadState* SubloadState::getInstance()
{
	static SubloadState* instance = 0;

	if(!instance)
		instance = new SubloadState();

	return instance;
}

SubloadState::SubloadState( const SubloadState& b ) : BaseState(b)
{
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
//	