%{
#include "Prefab.h"
%}

struct Prefab
{
private:
	Prefab();
	virtual ~Prefab();
	Prefab(GameObject*);
public:
	static Prefab* MakePrefabFrom(GameObject*);

	std::string name;
	std::string filename;
	bool isTransient;
	std::string stringBuf;
	GameObject* dormantObject;
	void CreateInstance(GameObject* gObj) const;

	void Rename(std::string& newName);
};