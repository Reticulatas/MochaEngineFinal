/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "EditorSubsystem.h"
#include "EditorState.h"

void Destroy(Component * c)
{
	//get the gameObject this pointer is attached to
	//validity at lower levels than this are due to data mismatch between component and game object
	c->gameObject->RemoveComponent(c);
	c->gameObject->GetState()->mComponentsToDelete.push_back(c);

}

bool Component::isEnabled() { return enabled; }
void Component::setEnabled(bool v) { enabled = v; }

bool Component::globalEnabled()
{ 
	if (GetState() && !GetState()->IsActive()) {
		if (Engine::getInstance()->GetIsEditorMode() && GetState() == EditorSubsystem::getInstance()->GetEditorState())
			return enabled;
		return false;
	}
	return enabled;
}

void Component::OnAlwaysUpdate( void )
{
	//override to have a call that always happens regardless of GSM/active/stacks
}

void Component::Destroy()
{
	::Destroy(this);
}

BaseState* Component::GetState()
{
	assert(gameObject != 0); return gameObject->GetState();
}

bool Component::GetManagedComponent()
{
	return managedComponent;
}

meta_define(Component);

////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change