/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "ISubsystem.h"
/* Factory to produce GameObjects based on input parameters */
/* Current:
		Prefab by Name - use prefab data from editor
		*/
struct ComponentCreationInfo;
class Component;
class GameObject;
struct Prefab;
struct BaseState;

class ObjectFactory : public ISubsystem
{
public:
	/*! \brief Given the specified prefab name, returns a handle to the new GameObject based on said prefab */
	GameObject* Instantiate(std::string prefabName);
	GameObject* operator()(std::string prefabName);				//make from stored prefab
	void Make(std::string prefabName, GameObject* gToWrite);	//write direct
	void Make(Prefab* p, GameObject* gToWrite);					//make from custom prefab

	bool Tick() { return true; }

	static ObjectFactory* getInstance(void);
	::Component* CreateComponent(std::string compName, bool setDefaults = false, ComponentCreationInfo* cci = 0);

	GameObject* MakeNewGameObject(const std::string& name, BaseState* state = 0, bool isManaged = true);
	GameObject* MakeNewGameObject(const GameObject* obj, bool addToState, BaseState* state, bool asChild = false);
	friend class GameObject;
private:

	metadef(ObjectFactory)
		endmetadef;
};

