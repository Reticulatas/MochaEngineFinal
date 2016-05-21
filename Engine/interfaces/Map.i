%{
#include "StateSerializer.h"
%}

%template(BoolVector)std::vector < bool > ;
%template(StateObjDescVector) std::vector<StateObjDesc>;

struct StateObjDesc
{
	std::string name;
	bool hasParent;	
};

class StateSerializer
{
public:
	StateSerializer();
	virtual ~StateSerializer();

	bool DoRecoverySave();
	bool SaveCurrentState(std::string filename);
	bool SaveState(BaseState& b, std::string filename);
	void LoadState(std::string filename, bool replaceCurrent = true);

	void MergeStates(BaseState* src, BaseState* dest, std::vector<bool>* whichObjsToMerge = 0);
	std::vector<StateObjDesc> LoadStateInformation(std::string filename, bool remove_state = true, BaseState** ret_newstate = 0);

};

class Map
{
public:
	static void LoadMapAdditive(std::string filename);
	static void LoadMap(std::string filename);
	static void LoadMapDontUnload(std::string filename);
	static void UnloadMap(void);
	static void LoadOnlyMap(std::string filename);

	static void DeloadZone(IZone* zoneID);

	static void ReloadMap();
	static void ReloadPreviousMap();
};