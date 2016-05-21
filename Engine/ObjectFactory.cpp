/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "ObjectFactory.h"
#include "Prefab.h"
#include "IRegisterable.h"
#include "GUIFactory_Variables.h"
#include "GameObject.h"
 
#include <assert.h>

#define ___NAME(var) #var

GameObject* ObjectFactory::operator()( std::string prefabName )
{
	return Instantiate(prefabName);
}

void ObjectFactory::Make( std::string prefabName, GameObject* gObj )
{
	Prefab * p = (*PrefabContainer::getInstance())[prefabName];
	if (p == 0)	{
		Log("Could not find Prefab to Instantiate: " + prefabName, LogEntry::Error);
		return;	
	}
	Make(p, gObj);
}

void ObjectFactory::Make( Prefab* p, GameObject* gObj)
{
	if (p == 0)	{
		Log("Could not find Prefab to Instantiate", LogEntry::Error);
		return;	}

	p->CreateInstance(gObj);
}

GameObject* ObjectFactory::Instantiate( const std::string prefabName )
{
	GameObject * gObj = ObjectFactory::getInstance()->MakeNewGameObject(prefabName);
	Make(prefabName, gObj);
	return gObj;
}

ObjectFactory* ObjectFactory::getInstance( void )
{
	static ObjectFactory* instance = 0;
	if (instance == 0)
		instance = new ObjectFactory();
	return instance;
}

Component* ObjectFactory::CreateComponent( std::string compName, bool setDefaults, ComponentCreationInfo* cci)
{
	Component* c = Serializer::MakeInstanceOfTFromString<Component>(compName);
	
	return c;
}

GameObject* ObjectFactory::MakeNewGameObject( const std::string& name, BaseState* state /*= 0*/, bool isManaged /*= true*/ )
{
	if (!state)
		state = GSM::getInstance()->getActiveState();
	if (!state)
		throw std::exception("No active state");

	if (isManaged)
	{
		state->wasAdditionsLastFrame = true;
		GameObject* g = state->mManagedObjects.New(name, state, isManaged);
		state->AddGameObject(g);
		return g;
	}
	else
		return new GameObject(name, state, isManaged);
}

GameObject* ObjectFactory::MakeNewGameObject( const GameObject* obj, bool addToState, BaseState* state, bool asChild )
{
	if (addToState)
	{
		if (!state)
			state = GSM::getInstance()->getActiveState();
		if (!state)
			throw std::exception("No active state");
	}

	GameObject* rootNewObj = 0;
	if (obj->IsManaged())
	{
		state->wasAdditionsLastFrame = true;
		rootNewObj = state->mManagedObjects.New(obj, addToState, state);
	}
	else
	{
		state->wasAdditionsLastFrame = true;
		rootNewObj = new GameObject(obj, addToState, state);
	}

	//reconnect as child when duplicating
	if (obj->transform->GetParent() && asChild)
		rootNewObj->transform->SetParent(obj->transform->GetParent());

	return rootNewObj;
}

meta_define(ObjectFactory);

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change