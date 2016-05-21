/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Config.h"
#include "StringUtils.h"
#include "AvailableAssets.h"
#include "Prefab.h"
#include "GameObject.h"
#include "SerializationIncludes.h"
#include "Serializations_Persistent.h"
#include "IRegisterable.h"
#include "GUIFactory_Variables.h"
#include "MaterialImpl.h"
#include <algorithm>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include "TypeRegs_ConversionHelper.h"
#include <mmsystem.h>
#include "ObjectFactory.h"

//#define TIMEPOPDROP

bool Prefab::operator==( const Prefab &other )
{
	return (!name.compare(other.name));
}

Prefab::Prefab( GameObject* gObj )
{
	isTransient = false;
	dormantObject = 0;
	Init(gObj);
}

Prefab::Prefab()
{
	dormantObject = 0;
}

Prefab::~Prefab()
{
	if (dormantObject)
		delete dormantObject;
}

void Prefab::CreateInstance(GameObject* gObj) const
{
#ifndef NOSERIALIZE
	try
	{
		//if you broke here, this prefab has not been initialized, check your pipeline or call ::Init()
		assert(dormantObject != 0);
		//if you broke here, this object must have been given a state before being sent here
		assert(gObj->GetState() != 0);

		//reset the name from the prefab's name
		gObj->SetName(name);
		
		gObj->CopyFrom(dormantObject, false);

		//reset the name from the prefab's name
		gObj->SetName(name);

		//link this prefab
		if (!this->isTransient)
			gObj->linkedPrefab = const_cast<Prefab*>(this);

		//add serialized children to this gameObject and state artificially
		for (CTransform* dormant_child : dormantObject->transform->children)
		{
			//make
			GameObject* active_child = ObjectFactory::getInstance()->MakeNewGameObject(dormant_child->gameObject, false, gObj->GetState());
			
			//relink
			active_child->transform->mParent = gObj->transform;
			gObj->transform->children.push_back(active_child->transform);
		}

		//call onStarts for all components
		gObj->OnForceStart();

		//call onStarts for all children
		//for (CTransform*& child : gObj->transform->children)
			//child->gameObject->OnForceStart();

	}
	catch (std::exception const& e)
	{
		Log("ERROR occurred during prefab instantiation: " + std::string(e.what()), LogEntry::Error);
	}
#endif
}

void Prefab::Init(const GameObject* const gObj)
{
	name = gObj->GetName();

#ifndef NOSERIALIZE
	try
	{
#ifdef TIMEPOPDROP
		DWORD startTime = timeGetTime();
#endif
		//if you broke here, this prefab was initialized twice somehow
		assert(dormantObject == 0);

#ifdef _DEBUG

		//recheck the validity of the original object
		GameObject::ConstCompIter iter = gObj->mComponents.begin();
		for (; iter != gObj->mComponents.end(); ++iter)
			assert((*iter)->gameObject == gObj);
#endif	

		//start this object as a dormant, don't call on start (we only care about non-transient data)
		dormantObject = new GameObject();
		dormantObject->isDormant = true;
		dormantObject->CopyFrom(gObj, false);
		dormantObject->SetName("Dormant Prefab Object");

#ifdef _DEBUG
		{
		//recheck the validity of the original object
		GameObject::ConstCompIter iter = gObj->mComponents.begin();
		for (; iter != gObj->mComponents.end(); ++iter)
			assert((*iter)->gameObject == gObj);
		}
#endif	

		std::function<void(const GameObject*, GameObject*)> lamba_recursedormantchild
			= [&](const GameObject* active_gobj, GameObject* dormant_obj) {
			//force link dummy children objects
			for (CTransform* active_child : active_gobj->transform->children)
			{
				//create and copy
				GameObject* dormant_child = new GameObject();
				dormant_child->isDormant = true;
				dormant_child->CopyFrom(active_child->gameObject, false);

				//link transforms
				dormant_obj->transform->children.push_back(dormant_child->transform);
				dormant_child->transform->mParent = dormant_obj->transform;

				lamba_recursedormantchild(active_child->gameObject, dormant_child);
			}
		};

		lamba_recursedormantchild(gObj, dormantObject);


		if (!isTransient)
		{
			filename = "Assets\\Prefabs\\" + name + ".prefab";
			std::stringbuf fb;
			std::ostream ofs(&fb);
			boost::archive::xml_oarchive oa(ofs);
			const GameObject& g = *gObj;
			oa << BOOST_SERIALIZATION_NVP(g);
			stringBuf = fb.str();
			std::ofstream ffs(filename);
			ffs << stringBuf;
			ffs.close();
			LogVerbose("Wrote gameObject prefab to " + filename, LogEntry::Command);
		}
#ifdef TIMEPOPDROP
		std::cout << "Save Time: " << timeGetTime() - startTime << std::endl;
#endif
	}
	catch (std::exception const& e)
	{
		Log("ERROR occurred during prefab creation: " + std::string(e.what()), LogEntry::Error);
	}
#endif
}

//filename and name must both be intact for this function to work
//initialization from string buffer (e.g. file) - this is the old, slow version but is still used for initial save/load
void Prefab::Init()
{
	//if you broke here, check where this prefab came from, the old version requires it to have (most likely) come from available assets pool
	assert(!filename.empty());

	std::ifstream ia(filename);
	if (ia.good())
	{
		boost::archive::xml_iarchive arch(ia);

		//if you broke here, this prefab is already inited somehow
		assert(dormantObject == 0);

		dormantObject = new GameObject();
		dormantObject->SetName("Dormant Prefab Object");
		dormantObject->isDormant = true;

		arch >> BOOST_SERIALIZATION_NVP(*dormantObject);

		//security modifications - make sure this dormant object stays dormant
		dormantObject->myState = 0;
	}
	else
	{
		std::string ss = "Could not open prefab file: " + filename;
		throw std::exception(ss.c_str());
	}
}

void Prefab::Rename( std::string& newName )
{
	assert(!isTransient);		//cannot rename transient prefabs
	assert(!filename.empty());	//must have filename
	std::string newFileName = "Assets\\Prefabs\\" + newName + ".prefab";
	
	try
	{
		boost::filesystem::path p( filename );
		boost::filesystem::path np( newFileName );
		if (boost::filesystem::exists(p))
		{
			boost::filesystem::copy_file(p, np);
			boost::filesystem::remove(p);
		}
	}
	catch (boost::filesystem::filesystem_error& e)
	{
		Log("Error renaming prefab: " + std::string(e.what()), LogEntry::Error);
		return;
	}

	name = newName;
	filename = newFileName;
}

Prefab* Prefab::MakePrefabFrom(GameObject* g)
{
	Prefab* p = new Prefab(g);
	PrefabContainer::getInstance()->AddPrefab(p);
	return p;
}

void Prefab::Reset()
{
	if (dormantObject)
	{
		delete dormantObject;
		dormantObject = 0;
	}
}

void Prefab::ReplaceInternals(GameObject* newGobj)
{
	Reset();
	Init(newGobj);
}

TransientPrefab::TransientPrefab( const GameObject* gObj)
{
	isTransient = true;
	Init(gObj);
}

//-----------------------------

PrefabContainer* PrefabContainer::getInstance()
{
	static PrefabContainer* instance = 0;
	if (instance == 0)
	{
		instance = new PrefabContainer();
		if (Config::loadPrefabsOnStart)
			PrefabContainer::getInstance()->Restore();	//restore saved prefabs
	}
	return instance;
}

Prefab* PrefabContainer::operator[]( unsigned i )
{
	return *(container.begin() + i);
}

Prefab* PrefabContainer::operator[]( const std::string& name )
{
	prefabIter iter = container.begin();
	for (; iter != container.end(); ++iter)
	{
		if (!name.compare((*iter)->name))
			return (*iter);
	}
	return 0;
}

bool PrefabContainer::AddPrefab( Prefab* p )
{
	Prefab* check = (*this)[p->name];
	if (check != 0)
	{
		container.erase(std::remove(container.begin(), container.end(), check), container.end());
		Log("Overwrote Previous Prefab");
	}
	container.push_back(p);
	return true;	
}

void PrefabContainer::RemovePrefab( const Prefab* p )
{
	//delete corresponding file
	try
	{
		boost::filesystem::remove(boost::filesystem::absolute(p->filename));
	}
	catch (std::exception const& e)
	{
		Log("ERROR deleting prefab from filesystem: " + std::string(e.what()), LogEntry::Error);
		return;
	}
	container.erase(std::remove(container.begin(), container.end(), p), container.end());
}

void PrefabContainer::RemovePrefab( const int i )
{
	Prefab* p = (*this)[i];
	RemovePrefab(p);
}

void PrefabContainer::RemovePrefab( const std::string& name )
{
	RemovePrefab((*this)[name]); 
}

#include <fstream>
#include "SerializationIncludes.h"

struct Task_LoadPrefab : public Task
{
	std::string m_name;
	std::string m_filename;
	virtual bool Tick()
	{
		Prefab* p = new Prefab();
		p->name = m_name;
		p->isTransient = false;		//do not put this in the constructor, it gets forwarded
		p->filename = m_filename;
		p->Init();
		PrefabContainer::getInstance()->AddPrefab(p);
		return false;
	}

	metadef(Task_LoadPrefab)
		m_add(std::string, m_name)
		m_add(std::string, m_filename)
		endmetadef;
};

bool PrefabContainer::Restore()
{
	if (container.size() != 0)
		return false;
	this->container.clear();

	//repopulate the prefab container based on the files in the Prefabs/ directory
	std::vector<AvailableAsset*> prefabAssets = AvailableAsset::GetAvailableAssetsFromType("Prefabs");
	AvailableAsset::AvailableAsset_iter iter = prefabAssets.begin();
	for (; iter != prefabAssets.end(); ++iter)
	{
		Task_LoadPrefab* t = new Task_LoadPrefab();
		t->m_name = boost::filesystem::path((*iter)->Filename()).stem().string();
		try
		{
			t->m_filename = (*iter)->FileID().Filename();
		}
		catch (FileIDException& e)
		{
			Log(e.what(), LogEntry::Error);
			continue;
		}
		t->Tick();
		//TaskManager::getInstance()->ScheduleTask(t);
		delete t;
	}

	//TaskManager::getInstance()->WaitForAllTasks();
	return true;
}

void PrefabContainer::FillListNames( std::list<std::string>& list )
{
	prefabIter iter = container.begin();
	for (; iter != container.end(); ++iter)
	{
		list.push_back((*iter)->name);
	}
}

meta_define(Task_LoadPrefab);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change

