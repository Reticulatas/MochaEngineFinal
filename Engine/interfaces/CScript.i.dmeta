%{
#include "CScript.h"
	%}

struct ScriptProperties {};

%csattributes CScript::isBinded "[MochaDoNotShow]"
%csattributes CScript::register_id "[MochaDoNotShow]"
%rename(p) CScript::dummyProperties;
class CScript : public Component
{
	CScript();
	~CScript();
public:
	bool isBinded;
	AssetHandle scriptAsset;
	std::string register_id;
	ScriptProperties dummyProperties;
};