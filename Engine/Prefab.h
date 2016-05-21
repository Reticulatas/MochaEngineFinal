/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

 
#include <list>
#include <sstream>
#include "IRegisterable.h"

/* Data struct containing relevant component info per instance */
/* CONTAINS NO VOLATILE OR ALLOCATED DATA - ONLY CREATION INFO */
struct ComponentCreationInfo
{
	std::string name;
	std::vector<std::pair<std::string, std::string>> varDefaults;
	typedef std::vector<std::pair<std::string, std::string>>::iterator varDefsIter;
	___SERIALIZE_BEGIN___
		___DEFSER(name, 1)
		___DEFSER(varDefaults, 1)
	___SERIALIZE_END___
};
___SERIALIZE_CLASS(ComponentCreationInfo, 1);

class GameObject;
/* Data struct containing relevant prefab info per instance */
/* CONTAINS NO VOLATILE DATA - ONLY CREATION INFORMATION	*/
/* Rescinded, to a degree, now contains a dormant copy of   */
/* the object that is represents. See CopyFrom in the comps */
struct Prefab

{
	static Prefab* MakePrefabFrom(GameObject*);

	//Make prefab from existing gameObject
	Prefab(GameObject*);
	Prefab();
	virtual ~Prefab();
	std::string name;
	std::string filename;
	bool isTransient;
	//transient string buffer
	std::string stringBuf;
	//dormant gameObject template
	GameObject* dormantObject;
	bool operator==(const Prefab &other);
	//called from ObjectFactory, use those methods
	void CreateInstance(GameObject* gObj) const;

	void Reset();
	void ReplaceInternals(GameObject* newGobj);
	void Rename(std::string& newName);

	___SERIALIZE_SAVE_BEGIN___
		___DEFSER(name, 1)
		___DEFSER(filename, 1)
	___SERIALIZE_END___
	___SERIALIZE_LOAD_BEGIN___
		___DEFSER(name, 1)
		___DEFSER(filename, 1)
		Init();
	___SERIALIZE_END___
	___SERIALIZE_SAVE_LOAD___
protected:
	//initialization from an existing gameObject, best case scenario is the prefab has a template in which to use already
	//this function will write a file for the prefab if the base prefab type is not TransientPrefab
	void Init(const GameObject* const);
	//initialization from string buffer (e.g. file) - this is the old, slow version but is still used for initial save/load
	//assumes filename and name are both intact
	void Init();
	
	friend class PrefabContainer;
	friend struct Task_LoadPrefab;
};
___SERIALIZE_CLASS(Prefab, 1);

/*  TransientPrefab is a decorator for Prefab that does not write a file version of the object specified and is	
	used primarily to copy an object without doing any disk i/o													*/
struct TransientPrefab : public Prefab
{
	TransientPrefab(const GameObject*);
};

/* Singleton Container class manages lists of prefabs */
class PrefabContainer
{
private:
	std::vector<Prefab*> container;
	typedef std::vector<Prefab*>::iterator prefabIter;
public:
	static PrefabContainer* getInstance(void);
	/* Get prefab at position in container */
	Prefab* operator[](unsigned i);
	/* Get prefab by name in container */
	Prefab* operator[](const std::string& name);

	/* Add new prefab to be managed */
	bool AddPrefab(Prefab* p);
	/* Removes prefab from management by ptr    */
	void RemovePrefab(const Prefab* p);
	/* Removes prefab from management by pos	*/
	void RemovePrefab(const int i);
	/* Removes prefab from management by name	*/
	void RemovePrefab(const std::string& name);

	/* Fill list of prefab name */
	void FillListNames(std::list<std::string>& list);
	
	/* Serialization functions for persistence - true on success */
	bool Restore();

	friend class PrefabTree;

	___SERIALIZE_BEGIN___
		___DEFSER(container, 1)
	___SERIALIZE_END___
};
___SERIALIZE_CLASS(PrefabContainer, 1);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change