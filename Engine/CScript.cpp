/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CScript.h"
#include "ScriptSubsystem.h"
#include "Log.h"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"


CScript::CScript() : isBinded(false), Component(), invalid(true), isRegistered(false)
{
	//do not allow this component to be indexed - there will be multiple
	managedComponent = false;
}

CScript::~CScript()
{
}


void CScript::OnFree( void )
{
	if (!invalid)
		_unregister();
}

void CScript::OnUpdate( void )
{
}

void CScript::OnInit( void )
{
}

void CScript::OnStart( void )
{
	if (register_id.empty()) {
		generate_registerid();
	}
	if (!isRegistered)
		generate_registration();

	prevScriptAsset = scriptAsset;
}

void CScript::OnEditorUpdate(void)
{
	//if we have a valid script asset that has not been registered
	if ((scriptAsset.Valid() && invalid) || (scriptAsset != prevScriptAsset))
	{
		invalid = false;
		//if the previous script asset was also valid
		if (prevScriptAsset.Valid())
			_unregister();
		_register();
	}

	prevScriptAsset = scriptAsset;
}

void CScript::OnCollisionExit( CollisionData collisionData )
{
 }

void CScript::OnCollisionEnter( CollisionData collisionData )
{
}

void CScript::OnMouseClick( CCamera* cam )
{
	//PerformBroadcastFunctionCall("OnMouseClick", 0);
}

void CScript::OnMouseEnter( CCamera* cam )
{
	//PerformBroadcastFunctionCall("OnMouseEnter", 0);
}

void CScript::CopyFrom(const ::CScript* c)
{
	scriptAsset = c->scriptAsset;
	prevScriptAsset = c->prevScriptAsset;
	invalid = c->invalid;
	isBinded = false;

	generate_registerid();
	generate_registration();

	if (!invalid)
		ScriptSubsystem::getInstance()->CallCScriptCopyFrom(c, this);
	Component::CopyFrom(c);
}

void CScript::generate_registerid()
{
	boost::uuids::uuid u = boost::uuids::random_generator()();
	register_id = boost::uuids::to_string(u);
}

void CScript::generate_registration()
{
	if (scriptAsset.Valid())
	{
		_register();
		invalid = false;
	}
	else
		invalid = true;
}

void CScript::_unregister()
{
	ScriptSubsystem::getInstance()->CallCScriptUnregister(this);
	isRegistered = false;
}

void CScript::_register()
{
	ScriptSubsystem::getInstance()->CallCScriptRegister(this);
	isRegistered = true;
}


void ScriptAsset::OnAssetFileChanged()
{
	Log("Reloading " + this->GetName() + " ...");
	ScriptSubsystem::getInstance()->Reload();
}

___COMP_REGISTER(CScript);
meta_define(CScript);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
