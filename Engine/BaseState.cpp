/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CTransform.h"
#include <exception>
#include "EditorSubsystem_Interface.h"

#include "EditorState.h"
#include "ObjectFactory.h"
#include "EditorSubsystem.h"

void BaseState::AddGameObject(GameObject * g, bool isManaged)
{
  //queue these object for addition next frame
  if (isManaged)
  {
    //assumed to be in the mmanagedobject slottable already
    if (std::find(mManagedObjectsAddedThisFrame.begin(), mManagedObjectsAddedThisFrame.end(), g) != mManagedObjectsAddedThisFrame.end())
      assert(false);
    mManagedObjectsAddedThisFrame.push_back(g);
  }
  else
    this->mUnmanagedObjectsToAdd.push_back(g);

  wasAdditionsLastFrame = true;
}

void BaseState::UpdateState(void)
{
  wasDeletesLastFrame = false;
  wasAdditionsLastFrame = false;

  //generate properties for new managed objects
  /*
  if (mManagedObjectsAddedThisFrame.size())
  {
    for (GameObject*const& g : mManagedObjectsAddedThisFrame)
    {
      for (std::pair<std::string, ::Component*>const& c_p : g->mComponentLookupTable)
      {
        try {
        //componentNamePropertyTable.Insert(c_p.first, c_p.second);
        }catch(std::exception& e)
        {
          std::cout << e.what() << std::endl;
        }
      }
    }*/
  {
    mManagedObjectsAddedThisFrame.clear();
  }

  //add new unmanaged objects
  mUnmanagedObjects.insert(mUnmanagedObjects.end(), mUnmanagedObjectsToAdd.begin(), mUnmanagedObjectsToAdd.end());
  mUnmanagedObjectsToAdd.clear();
  numOfUnManagedObjects = mUnmanagedObjects.size();

  DoUpdates();
  DoDeletes();
  //DoPropertyRefresh();
}

void BaseState::AlwaysUpdateState()
{
    //Update all game objects
    GameObjectIter iter;
    for (GameObject*const& obj : *mManagedObjects)
      obj->OnAlwaysUpdate();
    for (iter = mUnmanagedObjects.begin(); iter != mUnmanagedObjects.end(); ++iter)
      (*iter)->OnAlwaysUpdate();
}

void BaseState::DoPropertyRefresh()
{
  componentNamePropertyTable.Clear();
  for (GameObject*const& g : *mManagedObjects)
    for (std::pair<std::string, ::Component*>const& c_p : g->mComponentLookupTable)
      AddComponentToPropertyTable(c_p.second, c_p.first);
}

void BaseState::DoDeletes(void)
{
  std::list<GameObject*> objectsToRemove;

  //Delete objects marked for deletion
  GameObjectIter iter;
  for (GameObject*const& o : *mManagedObjects)
  {
    if (o->GetMarkedForDeletion())
    {
      //game object on free removes all components and puts them in the garbage
      o->OnFree();
      objectsToRemove.push_back(o);
    }
  }

  //run one final frame of deletion - just to catch stragglers
  DoComponentDeletes();

  for (GameObject*& o : objectsToRemove)
  {
    //Log("Freed: " + o->GetName());
    ret::SlotTable<GameObject>::SlotTableError er = mManagedObjects.Delete(o);
    if (er != ret::SlotTable<GameObject>::SlotTableError::ST_NOERROR)
    {
      Log("ERROR: failed to delete object queued for deletion", LogEntry::Error);
      continue;
    }
    wasDeletesLastFrame = true;
  }

}

void BaseState::DoComponentDeletes()
{
  //Delete components that have been unlinked from their gameObject (see: Destroy())
  for (ComponentsIter iter = mComponentsToDelete.begin(); iter != mComponentsToDelete.end(); iter++)
  {
    unsigned removed = componentNamePropertyTable.RemoveAll(*iter);
    if (removed == 0)
    {
      Log("WARNING: Component: [" + ((::Component*)*iter)->myType + ", " + (*iter)->gameObject->GetName() + "] not found in property table");
#ifdef SAFETABLE
      assert(removed != 0);
#endif
      //DoPropertyRefresh();
    }

    (*iter)->OnFree();
	if (Engine::getInstance()->GetIsEditorMode())
		(*iter)->OnEditorFree();
    delete (*iter);
    wasDeletesLastFrame = true;
  }
  mComponentsToDelete.clear();
}

void BaseState::DoUpdates(void)
{
  if (!GSM::getInstance()->IsFrozen() && isActive)	//updates do not happen if state is frozen
  {
    //Update all game objects
    GameObjectIter iter;
    for (GameObject*const& obj : *mManagedObjects)
      obj->OnUpdate();
    for (iter = mUnmanagedObjects.begin(); iter != mUnmanagedObjects.end(); ++iter)
      (*iter)->OnUpdate();
  }
  if (!EditorSS::IsTesting() && Engine::getInstance()->GetIsEditorMode())
  {
	  for (GameObject*const& obj : *mManagedObjects)
		  obj->OnEditorUpdate();
	  GameObjectIter iter;
	  for (iter = mUnmanagedObjects.begin(); iter != mUnmanagedObjects.end(); ++iter)
		  (*iter)->OnEditorUpdate();
  }
}

void BaseState::RemoveGameObject( GameObject * g)
{
  if (g->IsManaged())
  {
    Log("ERROR: No explicit removal of managed objects!", LogEntry::Error);
    return;
  }

  mUnmanagedObjects.erase(std::remove(mUnmanagedObjects.begin(),mUnmanagedObjects.end(),g),mUnmanagedObjects.end());
}

//Copy constructor
BaseState::BaseState( BaseState& other )
{
  isActive = true;
  wasDeletesLastFrame = false;
  wasAdditionsLastFrame = false;
  numOfUnManagedObjects = numOfManagedObjects = 0;
  filepath = other.filepath;
  physicsCollisionTable = other.physicsCollisionTable;

  //copy managed
  {
    for ( const GameObject*const& obj : *other.mManagedObjects )
    {
		if (obj->transform->GetParent() == 0)

			GameObject* copyG = ObjectFactory::getInstance()->MakeNewGameObject(obj, false, this);
    }
  }
  //copy unmanaged
  {
    ConstGameObjectIter iter = other.mUnmanagedObjects.begin();
    for (; iter != other.mUnmanagedObjects.end(); ++iter )
    {
		if ((*iter)->transform->GetParent() == 0)
			GameObject* copyG = ObjectFactory::getInstance()->MakeNewGameObject(*iter, false, this);
    }
  }


  wasAdditionsLastFrame = true;
}

BaseState::BaseState(void) : wasDeletesLastFrame(false), numOfManagedObjects(0), numOfUnManagedObjects(0), filepath("")
{
}

void BaseState::AddComponentToPropertyTable_Rec(::Component* ele, const meta::metaclass* _metaobj, const std::string& prop)
{
  try {
    componentNamePropertyTable.Insert(prop, ele);
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }

  //recursively add the base class component types
  if (_metaobj->_base_info != 0)
  {
    AddComponentToPropertyTable_Rec(ele, _metaobj->_base_info, _metaobj->_base_info->type_name);
  }
}

void BaseState::AddComponentToPropertyTable( Component* ele, const std::string& prop)
{
  AddComponentToPropertyTable_Rec(ele, &ele->mymeta(), prop);
}

void BaseState::Draw_State()
{
  for (GameObject* const& obj : *mManagedObjects)
    obj->OnDraw();
}

bool BaseState::IsActive() const
{
	return isActive;
}

void BaseState::IsActive(bool val)
{
	isActive = val;
}

void Destroy(BaseState* s)
{
  assert(s != 0);

  s->FreeState();
  s->UnloadState();

  //in case something was freed this frame, do deletes
  s->DoDeletes();

  //set all objects as to be freed
  BaseState::GameObjectIter iter;
  for (GameObject*const& obj : *s->mManagedObjects)
	  if (obj->transform->GetParent() == 0)
		  Destroy(obj);
  for (iter = s->mUnmanagedObjects.begin(); iter != s->mUnmanagedObjects.end();iter++)
    Destroy(*iter, false);

  //do final deletes after all the freeing
  s->DoDeletes();

  delete(s);
}

//----------



GameObject* GameObjectIterator::Next()
{
  if (_iter != viewState->GetManagedObjectsSlotTable().GetAllocatedObjects()->end())
  {
    GameObject* g = *_iter;
    ++_iter;
    return g;
  }
  return 0;
}

GameObjectIterator::GameObjectIterator(BaseState* statetoview) : viewState(statetoview)
{
  _iter = statetoview->GetManagedObjectsSlotTable().GetAllocatedObjects()->begin();
}

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change

