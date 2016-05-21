%{
#include "ret.h"
%}

%template(SetGameObjectPtr) std::set<GameObject*>;


namespace ret {
	template <typename T>
	class SlotTable
	{
	private:
		SlotTable();
		~SlotTable();
	public:
		std::set<T*>* GetAllocatedObjects();
		unsigned GetNumberOfObjects();
	};

	%template(gameObjectSlotTable)SlotTable<GameObject>;
};