/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#pragma once
#include "Log.h"
#include <exception>
#include <map>
#include "IRegisterable.h"
#include "ret.h"
#include "meta.h"
#include "CTransform.h"
#include "LayerMask.h"
#include "Prefab.h"

class Component;
struct CollisionData;
class CCamera;
struct BaseState;

unsigned GetNextID();


class GameObject : public IMeta, public ret::ISlottable
{
	//Removes only references, not the actual component
	void RemoveComponent(::Component* c);
	___SERIALIZE_SAVE_BEGIN___
		___DEFSER(mName, 1);
	___DEFSER(transform, 1);
	___DEFSER(mComponents, 1);
	___DEFSER(mComponentLookupTable, 1);
	DEPRECATED GameObject* mParent;
	mParent = 0;
	___DEFSER(mParent, 2);
	___DEFSER(isManaged, 2);
	___DEFSER(flags, 5);
	___DEFSER(children, 6);
	___DEFSER(mLayerMask, 7);
	___DEFSER(mGUID, 8)
	___SERIALIZE_END___
		___SERIALIZE_LOAD_BEGIN___
		___DEFSER(mName, 1);
	___DEFSER(transform, 1);
	___DEFSER(mComponents, 1);
	___DEFSER(mComponentLookupTable, 1);
	DEPRECATED GameObject* mParent;
	___DEFSER(mParent, 2);
	___DEFSER(isManaged, 2);
	___DEFSER(flags, 5);
	___DEFSER(children, 6);
	___DEFSER(mLayerMask, 7);
	___DEFSER(mGUID,8);
	mID = GetNextID();
	___SERIALIZE_END___
		___SERIALIZE_SAVE_LOAD___;

	bool		isManaged;			//managed objects are managed by the state, unmanaged are freed immediately, allowing external forces to determine lifetime
	BaseState*	myState;			//state in which this object resides, assigned by state on link
	unsigned	mID;				//internal ID based on the global ID pool ( 0 == invalid )
	unsigned	mVersion;			//internal version based on the local ID instances
	bool		isDormant;			//is a dormant prefab holder
	DEPRECATED std::vector<GameObject*> children;

	void Initialize(BaseState* stateToAddTo);

public:
	GameObject(void);
	~GameObject(void);

	std::set<std::string> flags;

	/*! \brief Duplicate this object */
	GameObject* Duplicate();
	void Destroy();

	/*! \brief Set a user-defined flag on this object */
	void SetFlag(std::string f);
	/*! \brief Removes a user-defined flag for this object */
	void ClearFlag(std::string f);
	/*! \brief Returns true if this object contains the flag f */
	bool GetFlag(std::string f);

	unsigned GetID()
	{
		return mID;
	}

	std::string mGUID;
	std::string mName;

	bool mMarkedForDeletion;										//will this object be deleted next frame
	std::map<std::string, ::Component*> mComponentLookupTable;		//name -> component table for easy component lookup
	std::vector<::Component*>			mComponents;				//list of all components on this object
	typedef std::map<std::string, ::Component*>::iterator		mCompLookupIter;
	typedef std::map<std::string, ::Component*>::const_iterator ConstCompLookupIter;
	typedef std::vector<::Component*>::iterator					CompIter;
	typedef std::vector<::Component*>::const_iterator			ConstCompIter;

	/*! \brief The transform attached to this object that holds information such a Position and Rotation */
	CTransform* transform;
	bool mIsSelected;
	LayerMask mLayerMask;
	Prefab* linkedPrefab;	//the prefab in which I was spawned from, used for relinking and saving

	GameObject* RevertToPrefab();		//revert to prefab instance - creates a new object and destroys this one, keeping the name and returning a ptr to it
	void WriteToPrefab();				//write this to the prefab
	void DetachFromPrefab();			//this object will no longer receive change requests from linked objects
	
	bool GetSelected() const { return mIsSelected; }
	BaseState*	GetState() const { return myState; }

	GameObject(const std::string& _name, bool isManaged = true);
	GameObject(const std::string& _name, BaseState* stateToAddTo, bool isManaged = true);
	GameObject(const GameObject* g, bool addToState = true, BaseState* stateToAddTo = 0);
	//helper for copy constructor, also a means on access from the prefabs, you can stop onstart with start
	void CopyFrom(const GameObject* other, bool start = true);

	//call to force all Components to call their respective OnStart() methods
	//used in state reloads from file (see: StateSerializer.cpp)
	void OnForceStart(bool recursively = false);
	void OnUpdate(void);
	void OnAlwaysUpdate(void);		//called every frame regardless of state/active/gsm
	void OnDraw(void);
	void OnEditorUpdate(void);
	void OnEditorStart(void);
	void OnEditorFree(void);
	void OnFree(void);

	void LookAt(const Vector3& point);
	void LookAt(const GameObject& point);
	void OnMouseClick(CCamera*);
	void OnMouseEnter(CCamera*);
	/* Add specifed component type to this object - use like gObj->AddComponent<T>() */
	template<typename T>
	T* AddComponent();

	/* Add generic component by name - string is component typename */
	Component* RequireComponentByName(std::string name);
	/* Add generic component by name - string is component typename */
	Component* AddComponentByName(std::string name);
	/* Add generic component by pointer - string is component typename */
	::Component* AddComponent(::Component*, std::string, bool start = true);
	/* Helper, called by AddComponent and by Prefabs when they went to inject a component without initialization logic */
	::Component* AddComponentToLists(::Component*, std::string);

	/* Check to see if object has component of type T */
	template<typename T>
	bool HasComponent();

	/*! \brief Returns true if this object has a component of type "name". Ex: gameObject.HasComponentByName("CPhysics") */
	bool HasComponentByName(std::string name);
	/*! \brief Returns the components of an object of type "name". Ex" CPhysics@ c = gameObject.HasComponentByName("CPhysics") */
	Component* GetComponentByName(std::string name);
	Component* GetComponentByName(std::string name, bool logerror);
	template<typename T>
	T* GetComponent();
	template <typename T>
	void GetComponentsFromBase(T** arr, unsigned max_size, unsigned* found);

	Component* GetComponentInChildren(std::string name);

	bool IsManaged() const { return isManaged; }
	void IsManaged(bool val) { isManaged = val; }

	/* Public Accessors - if this object has been set to be deleted this frame */
	bool GetMarkedForDeletion(void) { return mMarkedForDeletion; }
	bool GetIsDormantObject(void)	{ return isDormant; }
	/*! \brief Gets this object's name */
	std::string GetName(void) const
	{
		return mName;
	}
	std::map<std::string, ::Component*>* GetComponentLookupTable(void) { return &mComponentLookupTable; }
	/*! \brief Sets this object's name */
	std::string SetName(std::string s)
	{
		return (mName = s);
	}

	friend void Destroy(::Component * c);

	/* Access to Object Factory */
	friend class ObjectFactory;
	friend struct Prefab;
	/* Access for Editor */
	friend class HierarchyTree;
	friend class ComponentsComponent;
	friend class ComponentNode;
	friend class StateSerializer;
	static GameObject* Create(std::string prefab);
	static GameObject* GetGameObjectByName(std::string name_, bool errorIfNotFound = true);

	//not runtime operations
	static std::vector<GameObject*> GetGameObjectsWithTag(std::string tag);
	static GameObject* GetGameObjectWithTag(std::string tag);

	metadef(GameObject)
		m_add(std::string, mName)
		m_tag(MINTBOARDTAG)
		m_add(unsigned, mID)
		m_tag(MINTBOARDTAG)
		m_addfunc(Destroy)
		endmetadef;
};

template <typename T>
void GameObject::GetComponentsFromBase(T** arr, unsigned max_size, unsigned* found)
{
	unsigned cur = 0;
	for (Component* comp : mComponents)
	{
		if (comp->mymeta().getisderivedfrom<T>())
		{
			(arr)[cur] = dynamic_cast<T*>(comp);
			cur++;
			if (cur == max_size)
				throw std::exception("Buffer size too small");
		}
	}
	*found = cur;
}


/*! Destroys the passed Game Object */
void Destroy(GameObject * c);
void Destroy(GameObject * c, bool removeFromState);

template <typename T>
T* GameObject::AddComponent()
{
	if (HasComponent<T>())
		Log("Warning: Adding duplicate component to game object\nUnintended behavior may occur");
	T* newC = new T();
	return reinterpret_cast<T*>(AddComponent(newC, T::getmeta().gettoplevelname()));
}


template<typename T>
bool GameObject::HasComponent()
{
	return HasComponentByName(T::getmeta().gettoplevelname());
}

template <typename T>
T* GameObject::GetComponent()
{
	if (!HasComponent<T>())
	{
		//Log("GameObject has no component of type " + T::getmeta().gettoplevelname());
		return 0;
	}
	return static_cast<T*>(mComponentLookupTable.find(T::getmeta().gettoplevelname())->second);
}

//refactor as solutions come in

#define ___GOBJ_SENDMESSAGE_BEGIN(gObjPtr)													\
	std::vector<Component*>::iterator iter;													\
	unsigned numberOfComps = gObjPtr->mComponents.size();									\
	for (iter = gObjPtr->mComponents.begin(); iter != gObjPtr->mComponents.end(); iter++)	\
	{																						\
	Component * c = (*iter);

#define ___GOBJ_SENDMESSAGE_END(gObjPtr)													\
	if (gObjPtr->mComponents.size() != numberOfComps)										\
	break;																				\
	}																						

//external registrations
___SERIALIZE_CLASS(GameObject, 8);

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change