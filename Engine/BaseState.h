/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#pragma once
#include "GameObject.h"
#include "Prefab.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "GenericUtils.h"
#include "PropertyTable.h"
#include "ret.h"
#include "CollisionTable.h"
#include "CScript.h"

class CMeshRenderer;
class CCamera;
struct BaseState;

struct GameObjectIterator
{
	GameObject* Next();
	std::set<GameObject*>::const_iterator _iter;
	GameObjectIterator(BaseState* stateToView);
	BaseState* viewState;
};

/*
Interface class for every game state
All the methods below MUST be implemented
*/
struct BaseState
{

	void DoComponentDeletes();
	std::string filepath;

protected:
	std::vector<GameObject*>	mUnmanagedObjectsToAdd;
	std::list<GameObject*>		mManagedObjectsAddedThisFrame;
	ret::SlotTable<GameObject>	mManagedObjects;
	std::vector<GameObject*>	mUnmanagedObjects;
	std::vector<::Component*>	mComponentsToDelete;

	//Component Name -> List of all gameobjects that have it
	PropertyTable<std::string, ::Component*> componentNamePropertyTable;

  void AddComponentToPropertyTable_Rec(::Component*, const meta::metaclass*, const std::string& prop);


	//Collision table referenced by physics engine - remove later
	CollisionTable physicsCollisionTable;
	unsigned numOfManagedObjects;
	unsigned numOfUnManagedObjects;
	typedef std::vector<GameObject*>::const_iterator	ConstGameObjectIter;
	typedef std::vector<::Component*>::const_iterator	ConstComponentsIter;
	typedef std::vector<GameObject*>::iterator			GameObjectIter;
	typedef std::vector<::Component*>::iterator			ComponentsIter;
	friend void Destroy(::Component*);
	friend void Destroy(BaseState*);

	bool isActive;
	___SERIALIZE_SAVE_BEGIN___
		////save meta data////
		//save loaded prefabs
		std::list<std::string> loadedPrefabs;
	PrefabContainer::getInstance()->FillListNames(loadedPrefabs);
	//save collision data
	bool collisionTable[CollisionTable::IDnum][CollisionTable::IDnum];
	for (unsigned i = 0; i < physicsCollisionTable.GetWidth(); ++i)
		for (unsigned j = 0; j < physicsCollisionTable.GetWidth(); ++j)
			collisionTable[i][j] = physicsCollisionTable.GetDoesIDsCollide(i,j);
	___DEFSER(collisionTable, 3);
	___DEFSER(loadedPrefabs,2);
	___DEFSER(mManagedObjects,0)
		___DEFSERONLY(mUnmanagedObjects, 0);
	___SERIALIZE_END___
		___SERIALIZE_LOAD_BEGIN___
		////Load Meta Data////
		//collision table data
	bool collisionTable[CollisionTable::IDnum][CollisionTable::IDnum];
	___DEFSER(collisionTable, 3);
	if (ver >= 3) {
		for (unsigned i = 0; i < physicsCollisionTable.GetWidth(); ++i)
			for (unsigned j = 0; j < physicsCollisionTable.GetWidth(); ++j)
				physicsCollisionTable.SetDoesIDsCollide(i,j, collisionTable[i][j]);
	}
	//(actually saved prefabs)
	std::list<std::string> loadedPrefabs;
	___DEFSER(loadedPrefabs,2);
	std::list<std::string> myLoadedPrefabs;
	PrefabContainer::getInstance()->FillListNames(myLoadedPrefabs);
	std::list<std::string> missingPrefabs;
	ListFindDifferences<std::string>(loadedPrefabs,myLoadedPrefabs, missingPrefabs);
	if (missingPrefabs.size() != 0)
	{
		Log("\nMissing Prefabs: \n");
		ListLogList(missingPrefabs);
		Log("\n\n");
	}

	if (ver < 4){
		std::vector<GameObject*> mManagedObjects;
		___DEFSER(mManagedObjects,0);
		for (GameObject*const& o : mManagedObjects)
			BaseState::mManagedObjects.NewManaged(o);
	}
	else
		___DEFSER(mManagedObjects,0);
	___DEFSERONLY(mUnmanagedObjects, 0);
	wasAdditionsLastFrame = true;
	___SERIALIZE_END___
		___SERIALIZE_SAVE_LOAD___

public:
	//Copy constructor ensures a valid state immediately, no need to call OnInits() or other methods
	BaseState(BaseState& other);
	BaseState(void);
	virtual ~BaseState() { };

	virtual void LoadState() = 0;	// Load Assets here
	virtual void InitState() = 0;	// Initialize objects here
	virtual void UpdateState();		// Called automatically every update
	virtual void AlwaysUpdateState();		// Called automatically every update regardless of active/stack
	virtual void Draw_State();		// Called automatically every draw
	virtual void FreeState() = 0;	// Free objects here
	virtual void UnloadState() = 0; // Unload Assets here

	void DoUpdates(void);
	void DoDeletes(void);
	//recreate or refresh the property table based on changes this frame
	void DoPropertyRefresh(void);
	void RemoveGameObject(GameObject * g );
	void AddGameObject(GameObject * g, bool isManaged = true);

	unsigned NumOfManagedObjects() const { return numOfManagedObjects; }
	unsigned NumOfUnManagedObjects() const { return numOfUnManagedObjects; }
	bool IsActive() const;
	void IsActive(bool val);

	ret::SlotTable<GameObject>&	GetManagedObjectsSlotTable() {
		return mManagedObjects;
	}

	//returns a list of the component of type T
	template <typename T>
	const std::list<T*>* GetListOfComponentsOfType();

	CollisionTable* GetCollisionTable() { return &physicsCollisionTable; }

	//only used by runtime instantiating objects
	void AddComponentToPropertyTable(::Component*, const std::string&);

	//if true, some objects were deleted last frame
	bool wasDeletesLastFrame;
	bool wasAdditionsLastFrame;

	friend class GameObject;
	friend class HierarchyTree;
	friend class StateSerializer;
	friend class ComponentsComponent;
	friend class FullGameObjectIterator;
	friend class PhysicEngine;
	friend class EditorSubsystem;
	friend class ObjectFactory;
};
___SERIALIZE_CLASS(BaseState, 4);

	//use remove state for safety
	void Destroy(BaseState*);

template <typename T>
const std::list<T*>* BaseState::GetListOfComponentsOfType( )
{
	static_assert(!std::is_same<T, CScript>::value, "Cannot get list of CScript due to underlying container");
	const std::list<T*>* out_ptr;
	const std::list<::Component*>* out_ptr_not;
	if (componentNamePropertyTable.GetList(T::getmeta().gettoplevelname(), out_ptr_not))
	{
		out_ptr = reinterpret_cast<const std::list<T*>*>(out_ptr_not);
		return out_ptr;
	}
	return 0;
}


/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change