%{
#include "BaseState.h"
%}

struct BaseState
{
private:
	BaseState();
	~BaseState();
public:
	unsigned NumOfManagedObjects() const;
	unsigned NumOfUnManagedObjects() const;
	bool IsActive() const;
	void IsActive(bool val);
	
	template <typename T>
	const std::list<T*>* GetListOfComponentsOfType();

	ret::SlotTable<GameObject>&	GetManagedObjectsSlotTable();
};

%extend BaseState{
	//%template(GetListOfCScripts)GetListOfComponentsOfType<CScript>;
}


struct GameObjectIterator
{
	GameObject* Next();
	GameObjectIterator(BaseState* stateToView);
};
