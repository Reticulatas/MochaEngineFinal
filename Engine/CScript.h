/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include "ret.h"
#include "IRegisterable.h"
#include "Component.h"
#include "MochaString.h"
#include "ISerializeable.h"
#include "EditorSubsystem_Interface.h"
#include "AssetManager.h"
#include "Asset.h"

#ifdef max
#undef max
#endif

class ScriptEvent {
public:
	std::string funcName;
};

//dummy variable to recurse into in wpf editor
struct ScriptProperties {};

class ScriptAsset : public Asset
{
	virtual void OnAssetFileChanged();
};

DEPRECATED class ScriptRegister
{
public:
	std::string name;				//name (w/ ext) of the script this refers to 
	std::string register_id;		//id that persists across save/load used to connect this with the script serialization data

	ScriptRegister() { }

	//copy name, but generate a new UUID
	//boost::serialization uses the copy constructor, but our copy routines use the copy from
	//thus this method ensures there to be a valid shallow copy
	void ShallowCopyFrom(const ScriptRegister& from);

	___SERIALIZE_BEGIN___
		___DEFSER(name, 1);
		___DEFSER(register_id, 1);
		if (register_id == "")
			assert(false);		//something went terribly wrong - this script is invalid and has been assigned the wrong id
	___SERIALIZE_END___
};
___SERIALIZE_CLASS(ScriptRegister, 1);

class CScript : public ::Component
{
public:
	CScript();
	~CScript();
	void OnStart(void);
	void OnInit(void);
	void OnUpdate(void);
	void OnEditorUpdate();
	void OnFree(void);
	void OnCollisionEnter(CollisionData collisionData);
	void OnCollisionExit(CollisionData collisionData);
	virtual void OnMouseClick( CCamera* cam );
	virtual void OnMouseEnter( CCamera* cam );

	virtual void CopyFrom(const ::CScript* c);


	//filename w/o ext of the scripts on this object
	DEPRECATED std::vector<ScriptRegister> scripts;
	AssetHandle scriptAsset;
	AssetHandle prevScriptAsset;
	std::string register_id;
	ScriptProperties dummyProperties;	//see above, dummy wpf property to recurse into

	bool invalid;
	bool isBinded;
	bool isFirstUpdate;

private:
	void generate_registerid();
	void generate_registration();

	void do_init();

	bool isRegistered;
	void _register();
	void _unregister();

	COMPONENTCLONE(CScript);

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSERONLY(scripts, 1)
		___DEFSER(register_id, 2)
		___DEFSER(scriptAsset,2)
		___DEFSER(prevScriptAsset,2)
		___DEFSER(register_id, 2)
		___DEFSER(invalid, 2)
		___SERIALIZE_END___

public:
		metadef(CScript)
		endmetadef;
};

___SERIALIZE_CLASS(CScript, 2);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change