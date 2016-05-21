/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
#include "GameObject.h"
#include "CMeshRenderer.h"
#include "CPhysics.h"
#include "CTransform.h"
#include "InputManager.h"
#include "CScript.h"
#include "CCamera.h"
#include "CWidget_Transform.h"
#include "AudioManager.h"
#include "CLight.h"
 

void BlankState::LoadState()
{
	//Implement Any asset loading here
}

void BlankState::UpdateState()
{
	BaseState::UpdateState();
}

void BlankState::InitState()
{
	/*LogLock();
	for(GameObjectIter iter = mManagedObjects.begin(); iter != mManagedObjects.end();++iter)
	{
		CCamera* camera = (*iter)->GetComponent<CCamera>();
		if(camera)
			AddCameraComponent(camera);

		CMeshRenderer* meshRenderer = (*iter)->GetComponent<CMeshRenderer>();
		if(meshRenderer)
			AddMeshRenderer(meshRenderer);
	}

	for(GameObjectIter iter = mUnmanagedObjects.begin(); iter != mUnmanagedObjects.end();++iter)
	{
		CCamera* camera = (*iter)->GetComponent<CCamera>();
		if(camera)
			AddCameraComponent(camera);

		CMeshRenderer* meshRenderer = (*iter)->GetComponent<CMeshRenderer>();
		if(meshRenderer)
			AddMeshRenderer(meshRenderer);
	}

	LogUnlock();*/
}

void BlankState::Draw_State()
{
	BaseState::Draw_State();
	//Draw objects here
}

void BlankState::FreeState()
{
}

void BlankState::UnloadState()
{
	//AudioManager::getInstance()->Pause_All();
	//Unload assets here
}

//void EditorState::UpdateState() { BaseState::UpdateState(); }


BlankState* BlankState::getInstance()
{
	static BlankState* instance = 0;

	if(!instance)
		instance = new BlankState();

	return instance;
}

BlankState::BlankState( BlankState& b ) : BaseState(b)
{
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
//	