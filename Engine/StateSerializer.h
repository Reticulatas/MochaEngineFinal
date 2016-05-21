/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once
#include "IZone.h"

//Transient class responsible for serializing/de-serializing a state to a file
//Visitor - do not manually allocate
class CMapZone;

struct SerializationException
{
	std::string msg;
	enum SERIALIZATIONEXCEPTIONTYPE
	{
		CANNOTFINDMAP,
		FAILEDTOLOAD
	};
	SERIALIZATIONEXCEPTIONTYPE type;

	SerializationException(std::string _msg, SERIALIZATIONEXCEPTIONTYPE _type);
};

//describes a single game object that will be loaded by LoadStatePartialAdditive
struct  StateObjDesc
{
	std::string name;		//name of object
	bool hasParent;			//important - you cannot merge a child object
};

struct BaseState;
class StateSerializer
{
public:
	StateSerializer();
	virtual ~StateSerializer() { }

	bool DoRecoverySave();
	bool SaveCurrentState(std::string filename);
	bool SaveState(BaseState& b, std::string filename);

	void DoOffset(BaseState* new_state, Math::Vector3& offset);

	void DeloadState(std::string filename);
	//loads state from xml file and pushes it to the top
	//removes top state
	void LoadState(std::string filename, bool replaceCurrent = true, Math::Vector3 offset = Math::Vector3::cZero);
	//combine the loaded state with the current
	void LoadStateAdditive(std::string filename, Math::Vector3 offset = Math::Vector3::cZero);
	void LoadStateAdditiveMarkers(std::string filename, std::string markerID);
	//combine parts of the loaded state with the current (0 = merge all)
	void MergeStates(BaseState*, BaseState* , std::vector<bool>* whichObjsToMerge = 0);
	//given a level file, gather information about objects in the file
	std::vector<StateObjDesc> LoadStateInformation(std::string filename, bool remove_state = true, BaseState** ret_newstate = 0);
};

#ifndef MOCHAEDITOR


class Map 
{
public:
	/* Load new map annd merge it with the current one - making sure the id markers match position */
	static void LoadMapAdditiveMergeMarkers(std::string filename, std::string markerID);
	/* Load new map and merge it with the current one */
	static void LoadMapAdditive(std::string filename, Math::Vector3 offset = Math::Vector3::cZero);
	/*! Load new map and remove the current */
	static void LoadMap(std::string filename);
	/*! Load new map but do not remove the current map (for menus, pause screens, etc) */
	static void LoadMapDontUnload(std::string filename);
	/*! Unloads current map, only works if there is more than one state */
	static void UnloadMap(void);
	/*! Removes all maps from play and replaces them with the new single map */
	static void LoadOnlyMap(std::string filename);

	/*! Removes all content from this zone */
	static void DeloadZone(IZone* zoneID);

	static void ReloadMap_Aux(BaseState*);
	static void ReloadMap();
	static void ReloadPreviousMap();
};

struct MapDelegators
{
	void Del_LoadMap(std::string filename);
	void Del_LoadMapAdditive(std::string filename, Math::Vector3 offset);
	void Del_LoadMapAdditiveMarkers(std::string filename, std::string markerID);
	void Del_LoadMapNoUnload(std::string filename);
	void Del_UnloadState(BaseState*);
	void Del_StateClearAndReplace(std::string filename);

	static MapDelegators* getInstance();
};


#endif