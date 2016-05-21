%{
#include "ScriptSubsystem.h"
%}

class ScriptSubsystem
{
	ScriptSubsystem();
	~ScriptSubsystem();
public:
void Register_Callbacks(MochaDotNetCallback _tickcallback, MochaDotNetCScriptCopyCallback _copyFromCallback,
		MochaDotNetCScriptEventCallback _eventQueueCallback, MochaDotNetCallback _reloadCallback, MochaDotNetCallback _serialize,
		MochaDotNetCallback _deserialize, MochaDotNetCScriptCallback _registerCallback, MochaDotNetCScriptCallback _unregCallback,
        MochaDotNetCallback _FocusCallback,MochaDotNetCallback _UnFocusCallback);


	static ScriptSubsystem* getInstance();
};
