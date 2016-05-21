%{
#include "GSM.h"
%}

class GSM
{
private:
	GSM();
	~GSM();
public:
	BaseState* getActiveState();
	unsigned GetNumberOfStates(void);
	BaseState* getStateUnderActive();
	BaseState* getStateAtIndex(unsigned i);

	void PushStateToBottom(BaseState* state, bool doInit = true);
	void PushState(BaseState* state, bool doInit = true);
	void PopState();
	void RemoveState(BaseState* state);

	void Freeze();
	void UnFreeze();
	bool IsFrozen();

	static GSM* getInstance();
};