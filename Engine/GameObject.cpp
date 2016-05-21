/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include <assert.h>
#include "GameObject.h"
#include "CTransform.h"
#include "ObjectFactory.h"
#include "Prefab.h"
#include "EditorSubsystem.h"
#include "CWidget_Transform.h"
#include <set>
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"

#define DELETIONSAFEMODE 

static unsigned LASTASSIGNEDID = 0;
#ifdef DELETIONSAFEMODE
std::set<unsigned> destroyedIDs;		//save mode list
#endif

unsigned GetNextID() { return LASTASSIGNEDID++; }

GameObject::GameObject(void) : myState(0), mMarkedForDeletion(false), transform(0), isManaged(true), mID((std::numeric_limits<unsigned>::max)()), mIsSelected(false), isDormant(false), linkedPrefab(0)
{
	mName = "UNSETEMPTYGAMEOBJECT";
	boost::uuids::uuid u = boost::uuids::random_generator()();
	mGUID = boost::uuids::to_string(u);
}

GameObject::GameObject(const std::string& _name, bool _isManaged) : mName(_name), isManaged(_isManaged), mIsSelected(false), isDormant(false), linkedPrefab(0)
{
	assert(GSM::getInstance()->getActiveState() != 0);	//there's no active state!
	Initialize(GSM::getInstance()->getActiveState());
	boost::uuids::uuid u = boost::uuids::random_generator()();
	mGUID = boost::uuids::to_string(u);
}

GameObject* GameObject::Duplicate()
{
	return ObjectFactory::getInstance()->MakeNewGameObject(this, true, myState, true);
}

GameObject::GameObject(const GameObject* g, bool addToState, BaseState* stateToAddTo) : GameObject()
{	
	myState = stateToAddTo;
	mMarkedForDeletion = false;
	transform = AddComponent<CTransform>();
	boost::uuids::uuid u = boost::uuids::random_generator()();
	mGUID = boost::uuids::to_string(u);

	assert(GSM::getInstance()->getActiveState() != 0);

	//set the name to define this as a copy of the original object
	mName = g->GetName();

	isManaged = g->IsManaged();
	mID = LASTASSIGNEDID++;

	//make temporary prefab from the other object, then use it to instantiate the copy
	TransientPrefab p(g);
	ObjectFactory::getInstance()->Make(&p, this);

	myState->AddGameObject(this, isManaged);

	//this is a reconfirmation post-start and post-init
	//it's a stupid insurance check to make goddamn sure this object is in
	//at least a state in which it can be removed from the gameObject without
	//deadlocking the entire fucking engine
	___GOBJ_SENDMESSAGE_BEGIN(this)
		c->gameObject = this;
	___GOBJ_SENDMESSAGE_END(this);
}

GameObject::GameObject( const std::string& _name, BaseState* stateToAddTo, bool _isManaged /*= true*/ )
	: mName(_name), isManaged(_isManaged), mID((std::numeric_limits<unsigned>::max)()), isDormant(false), linkedPrefab(0)
{
	boost::uuids::uuid u = boost::uuids::random_generator()();
	mGUID = boost::uuids::to_string(u);

	Initialize(stateToAddTo);
}

void GameObject::Initialize( BaseState* stateToAddTo )
{
	myState = stateToAddTo;

	transform = AddComponent<CTransform>();

	mMarkedForDeletion = false;

	if (!isManaged)
		stateToAddTo->AddGameObject(this, isManaged);

	if (mID == (std::numeric_limits<unsigned>::max)())
		mID = LASTASSIGNEDID++;

	transform->SetParent(0);
}

GameObject::~GameObject(void)
{
	for (GameObject* gobj : children)
	{
		gobj->transform->SetParent(0);
	}
}

void GameObject::OnForceStart(bool recursivelyChildren)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		c->gameObject = reinterpret_cast<GameObject*>((void*)&(*this));
	c->OnStart();
	___GOBJ_SENDMESSAGE_END(this);

	if (Engine::getInstance()->GetIsEditorMode())
		OnEditorStart();

	if (recursivelyChildren)
	{
		for (CTransform* child : transform->children)
			child->gameObject->OnForceStart(true);
	}
}

void GameObject::OnUpdate(void)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		if (c->globalEnabled() && c->myType != "CTransform")
			c->OnUpdate();
	___GOBJ_SENDMESSAGE_END(this);

	//ctransform is updated in Draw States
}

void GameObject::OnDraw(void)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		//if (c->globalEnabled())
    //if (Engine::getInstance()->GetIsEditorMode() && !EditorSubsystem::getInstance()->IsTesting())
			c->OnDraw();
	___GOBJ_SENDMESSAGE_END(this);
}

void GameObject::OnEditorUpdate(void)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		//if you broke here, this object is not valid in relation to it's components
		assert(c->gameObject == this);
		c->OnEditorUpdate();
	___GOBJ_SENDMESSAGE_END(this);
}

void GameObject::OnEditorStart(void)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		c->OnEditorStart();
	___GOBJ_SENDMESSAGE_END(this);
}

void GameObject::OnEditorFree(void)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		c->OnEditorFree();
	___GOBJ_SENDMESSAGE_END(this);
}

void GameObject::OnAlwaysUpdate( void )
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		//if you broke here, this object is not valid in relation to it's components
		assert(c->gameObject == this);
		c->OnAlwaysUpdate();
	___GOBJ_SENDMESSAGE_END(this);
}

void GameObject::OnFree(void)
{
	while (mComponents.size() != 0)
	{
		Component* c = mComponents.at(0);
		::Destroy(c);
	}
}

void GameObject::OnMouseClick( CCamera* cam)
{
	std::vector<Component*>::iterator iter;													
	for (iter = mComponents.begin(); iter != mComponents.end(); ++iter)	
	{																						
		Component * c = (*iter);
		c->OnMouseClick(cam);
	}
}

void GameObject::OnMouseEnter( CCamera* cam)
{
	___GOBJ_SENDMESSAGE_BEGIN(this)
		c->OnMouseEnter(cam);
	___GOBJ_SENDMESSAGE_END(this);
}

/*::Component* GameObject::RequireDependency(std::string name)
{
	LogLock();
	::Component* c = GetComponentByName(name);
	LogUnlock();
	if (c)
		return c;
	
}*/
void GameObject::RemoveComponent( ::Component* c )
{
	if (mComponents.size() == 0)
		return;
	CompIter iter;
	for (iter = mComponents.begin();iter != mComponents.end(); ++iter)
	{
		if (c == (*iter))
		{
			mComponents.erase(iter);
			break;
		}
	}
	mCompLookupIter citer;
	for (citer = mComponentLookupTable.begin(); citer != mComponentLookupTable.end(); ++citer)
	{
		if (c == (citer->second))
		{
			mComponentLookupTable.erase(citer);
			break;
		}
	}

	//remove component from memory
	//delete c;
	//NOTE: DESTROY TAKES CARE OF THIS
}

::Component* GameObject::AddComponentByName( std::string name )
{
	if (HasComponentByName(name))
	{
		Log("GameObject rejects new component " + name + " - No Duplicate Components Allowed");
		return 0;
	}
	::Component* c = ObjectFactory::getInstance()->CreateComponent(name, false);
	return AddComponent(c, name, true);
}
Component* GameObject::RequireComponentByName( std::string name )
{
	if (HasComponentByName(name))
		return GetComponentByName(name, false);

	Component* c = ObjectFactory::getInstance()->CreateComponent(name, false);
	return AddComponent(c, name, true);
}

Component* GameObject::AddComponent( Component* newC, std::string typeID_name, bool start)
{
	if (HasComponentByName(typeID_name))
	{
		Log("GameObject rejects new component " +typeID_name + " - No Duplicate Components Allowed");
		return 0;
	}

	AddComponentToLists(newC, typeID_name);
	if (start)
	{
		newC->OnInit();
		newC->OnStart();
		if (Engine::getInstance()->GetIsEditorMode())
			newC->OnEditorStart();
	}
	//tell the state the list of components for this object has changed
	if (myState)
		myState->wasAdditionsLastFrame = true;
	return newC;
}

::Component* GameObject::AddComponentToLists( ::Component* newC, std::string typeID_name)
{
	newC->gameObject = this;
	newC->myType = typeID_name;
	newC->myType = newC->mymeta().gettoplevelname();
	mComponents.push_back(newC);
	if (newC->GetManagedComponent())
		mComponentLookupTable.insert(std::pair<std::string, Component*>(typeID_name, newC));
	if (myState)
		myState->AddComponentToPropertyTable(newC, typeID_name);
	return newC;
}

bool GameObject::HasComponentByName(std::string name)
{
	if (mComponentLookupTable.find(name) == mComponentLookupTable.end())
		return false;
	return true;
}
Component* GameObject::GetComponentByName(std::string name)
{
	return GetComponentByName(name, true);
}

Component* GameObject::GetComponentByName( std::string name, bool logerror )
{
	mCompLookupIter iter = mComponentLookupTable.find(name);
	if (iter == mComponentLookupTable.end())
	{
		if (logerror)
			Log("Could not find component " + name + " to get on object " + this->GetName(), LogEntry::Error);
		return 0;
	}
	return iter->second;
}

//-----------

GameObject* GameObject::Create( std::string prefab )
{
	return (*ObjectFactory::getInstance())(prefab);
}

void GameObject::SetFlag(std::string f)
{
	flags.insert(f);
}

void GameObject::ClearFlag(std::string f)
{
	flags.erase(f);
}

bool GameObject::GetFlag(std::string f)
{
	if (flags.find(f) != flags.end())
		return true;
	return false;
}

void GameObject::CopyFrom( const GameObject* other, bool startCopy)
{
	mName = other->mName;
	isManaged = other->isManaged;
	flags = other->flags;
	mLayerMask = other->mLayerMask;
	
	ConstCompIter iter = other->mComponents.begin();
	for (; iter != other->mComponents.end(); ++iter)
	{
		//TYPE NOT SET
		if ((*iter)->myType.empty())
			Log("Component on " + mName + " does not have it's typename set! New object will not be valid.", LogEntry::Error);
		//special case for CTransform, as always
		if (transform && (*iter)->myType == "CTransform")
		{
			transform->CopyFrom(reinterpret_cast<CTransform*>(*iter));
			transform->gameObject = this;
		}
		else
			::Component* newC = AddComponent((*iter)->CloneTo(this), (*iter)->myType, startCopy);
	}



#ifdef _DEBUG
	//check for errors
	//mismatch
	iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
	{
		assert (this == (*iter)->gameObject);
	}
#endif // DEBUG

	//relink ctransform
	iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
	{
		if ((*iter)->myType == "CTransform")
		{
			transform = reinterpret_cast<CTransform*>(*iter);
			return;
		}
	}

	transform->SetParent(other->transform->GetParent());

	//reset transform to maintain location
	transform->CopyFrom(other->transform);

	//if we hit here, the ctransform was not relinked
	throw MochaException("CTransform not found for new object: ");
}

void GameObject::Destroy()
{
	::Destroy(this);
}

void GameObject::LookAt(const Vector3& point)
{
  transform->LookAt(&point);
}

void GameObject::LookAt(const GameObject& point)
{
  transform->LookAt(&point.transform->position());
}

void Destroy(GameObject * g)
{
	::Destroy(g, true);
}

void Destroy(GameObject * g, bool removeFromState)
{
	//null object
	if (!g)
	{
		Log("Game object to destroy not found", LogEntry::Error);
		return;
	}
	if (!g->GetIsValidObject())
	{
		Log("ERROR: Freed or Invalid Game Object attempted to be deleted", LogEntry::Error);
		return;
	}

	//if this is a unmanaged object, kill it immediately
	if (!g->IsManaged())
	{
		g->OnFree();
		if (removeFromState)
			g->GetState()->RemoveGameObject(g);
		delete g;
		return;
	}
	g->mMarkedForDeletion = true;
	for (unsigned int i = 0; i < g->transform->children.size(); ) // Dont even need to increment index
	{
		CTransform* child = g->transform->children[i];
		::Destroy(child->gameObject);
	}

	//remove from my parent
	if (g->transform->GetParent())
		g->transform->SetParent(0);
}

//Returns first GameObject found with specified name
GameObject* GameObject::GetGameObjectByName(std::string name_, bool errorIfNotFound /* = true */)
{
	BaseState* curState = GSM::getInstance()->getActiveState();
	for (GameObject*const& obj : *curState->mManagedObjects)
	{
		if (obj->GetName() == name_)
			return obj;
	}
	if (errorIfNotFound)
		Log("Object not found: " + name_, LogEntry::Error);
	return 0;
}

std::vector<GameObject*> GameObject::GetGameObjectsWithTag(std::string tag)
{
	std::vector<GameObject*> result;
	for (auto& gobj : *GSM::getInstance()->getActiveState()->GetManagedObjectsSlotTable().GetAllocatedObjects())
	{
		if (gobj->GetFlag(tag))
			result.push_back(gobj);
	}
	return result;
}

GameObject* GameObject::GetGameObjectWithTag(std::string tag)
{
	for (auto& gobj : *GSM::getInstance()->getActiveState()->GetManagedObjectsSlotTable().GetAllocatedObjects())
	{
		if (gobj->GetFlag(tag))
			return gobj;
	}
	return 0;
}

GameObject* GameObject::RevertToPrefab()
{
	if (!linkedPrefab)
	{
		Log("Attempt to RevertToPrefab an unlinked game object", LogEntry::Error);
		return 0;
	}
	std::string name = mName;
	
	GameObject* newGObj = ObjectFactory::getInstance()->Instantiate(linkedPrefab->name);
	newGObj->transform->CopyFrom(this->transform);

	this->Destroy();

	return newGObj;
}

void GameObject::WriteToPrefab()
{
	if (!linkedPrefab)
	{
		Log("Attempt to WriteToPrefab an unlinked game object", LogEntry::Error);
		return;
	}

	linkedPrefab->ReplaceInternals(this);

	//update other linked prefabs
	if (myState != 0) {
		for (GameObject* g : *myState->GetManagedObjectsSlotTable().GetAllocatedObjects())
		{
			if (g != this)
				if (g->linkedPrefab == linkedPrefab)
					g->RevertToPrefab();
		}
	}
}

void GameObject::DetachFromPrefab()
{
	if (!linkedPrefab)
	{
		Log("Attempt to DetachFromPrefab an unlinked game object", LogEntry::Error);
		return;
	}

	linkedPrefab = 0;
}

Component* GameObject::GetComponentInChildren(std::string name)
{
	for (const CTransform* ct : transform->children)
	{
		Component* comp = ct->gameObject->GetComponentByName(name, false);
		if (comp)
			return comp;
	}
	return 0;
}

meta_define(GameObject);


/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change