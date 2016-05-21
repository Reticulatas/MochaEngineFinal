%{
#include "Component.h"
%}

%csattributes Component::gameObject "[MochaDoNotShow]"
%csattributes Component::myType "[MochaDoNotShow]"
class Component : public IMeta {
private:
	Component();
	~Component();
public:
	bool isEnabled();
	void setEnabled(bool v);
	bool globalEnabled(void);

	GameObject* gameObject;

	std::string myType;

	virtual void CopyFrom(const ::Component* c);

	void Destroy();
};
