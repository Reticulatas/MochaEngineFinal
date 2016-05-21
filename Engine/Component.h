/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "IRegisterable.h"
#include "Collision.h"
#include "meta.h"

struct BaseState;
class CCamera;

//helper to make the clone method for every component
#define COMPONENTCLONE(CompName)\
	virtual ::Component* Clone(GameObject* newgobj) const {\
	CompName* c = new CompName();\
	c->gameObject = newgobj;\
	c->OnInit();\
	c->CopyFrom(this);\
	return reinterpret_cast<Component*>(c); }

class GameObject;
 
class Component : public IRegisterable
{
public:
	virtual ~Component(void) { };
	Component(void) : enabled(true), managedComponent(true) { };
	virtual void OnStart(void) { assert(gameObject != 0); };
	virtual void OnInit(void) = 0;
	virtual void OnUpdate(void) = 0;
	virtual void OnAlwaysUpdate(void);
	virtual void OnFree(void) = 0;
	virtual void OnDraw(void) { }
	virtual void OnMouseClick(CCamera* cam) { }
	virtual void OnMouseEnter(CCamera* cam) { }
	virtual void OnEditorUpdate(void) { }
	virtual void OnEditorStart(void) {}
	virtual void OnEditorFree(void) {}

	/* call to check whether another component has been added to our parent game object
	 * if it does not exist, the component will be added
	 * should only be used in ONSTART */
	template <typename T>
	T* RequireDependency(void)
	{
		LogLock();
		T* c = gameObject->GetComponent<T>();
		LogUnlock();
		if (!c)
			c = gameObject->AddComponent<T>();
		return c;
	}

	/*! \brief If false, this component will never receive update calls. If it's a CScript, no OnUpdate()'s will be called */

	 bool isEnabled();
	 void setEnabled(bool v);

	bool globalEnabled(void);

	bool GetManagedComponent();
	/*! \brief The game object this component is attached to */
	 GameObject* gameObject;

	BaseState* GetState();

	std::string myType;

	virtual void CopyFrom(const ::Component* c)
	{
		enabled = c->enabled;
		myType = c->myType;
		//gameObject must be pre-linked
		//assert(gameObject != 0);
	}

	//check the top of this file for the helper macro that must be in every component
	::Component* CloneTo(GameObject* newGObj) const
	{
		return Clone(newGObj);
	}

	void Destroy();
private:
	virtual ::Component* Clone(GameObject* newgobj) const = 0;
	bool enabled;
protected:
	bool managedComponent;	//is this component capable of being indexed? (turned off for cscripts, mostly)

	___SERIALIZE_SAVE_BEGIN___
		___DEFBASE(IRegisterable)
		___DEFSER(enabled, 1)
		___DEFSER(gameObject, 1)
		___DEFSER(myType, 3)
		//do not ser myState
	___SERIALIZE_END___
	___SERIALIZE_LOAD_BEGIN___
		___DEFBASE(IRegisterable)
		___DEFSER(enabled, 1)
		___DEFSER(gameObject, 1)
		___DEFSER(myType, 3)
		OnInit();
		//do not ser myState
	___SERIALIZE_END___
		___SERIALIZE_SAVE_LOAD___

public:
		metabasedef(Component)
		m_add(std::string, myType)
		m_add(bool, enabled)
		m_addfunc(Destroy)
		endmetabasedef;

};
 
___SERIALIZE_CLASS(Component, 3);
//BOOST_SERIALIZATION_ASSUME_ABSTRACT(Component);

void Destroy(::Component * c);

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change