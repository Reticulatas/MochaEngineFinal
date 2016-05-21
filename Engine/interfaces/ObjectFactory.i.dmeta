%{
#include "ObjectFactory.h"
%}

class ObjectFactory
{
public:
	GameObject* MakeNewGameObject(const std::string& name, BaseState* state = 0, bool isManaged = true);
	GameObject* MakeNewGameObject(const GameObject* obj, bool addToState, BaseState* state);

	GameObject* Instantiate(std::string prefabName);

	static ObjectFactory* getInstance(void);
};