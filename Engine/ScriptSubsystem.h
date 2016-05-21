#pragma once
#include "meta.h"
#include "ISubsystem.h"
#include "MochaDotNetCallbacks.h"

class ScriptSubsystem : public ISubsystem
{
	MochaDotNetCallback mTickCallback;
	MochaDotNetCallback mReloadCallback;
	MochaDotNetCScriptCopyCallback mCopyFromCallback;
	MochaDotNetCScriptEventCallback mQueueEventCallback;
	MochaDotNetCallback mSerializeCallback;
	MochaDotNetCallback mDeSerializeCallback;
	MochaDotNetCScriptCallback mRegisterCallback;
	MochaDotNetCScriptCallback mUnregisterCallback;
    MochaDotNetCallback mFocusCallback;
    MochaDotNetCallback mUnFocusCallback;
public:
	ScriptSubsystem();

	void Register_Callbacks(MochaDotNetCallback _tickcallback, MochaDotNetCScriptCopyCallback _copyFromCallback,
		MochaDotNetCScriptEventCallback _eventQueueCallback, MochaDotNetCallback _reloadCallback, MochaDotNetCallback _serialize,
		MochaDotNetCallback _deserialize, MochaDotNetCScriptCallback _registerCallback, MochaDotNetCScriptCallback _unregCallback,
        MochaDotNetCallback _FocusCallback,MochaDotNetCallback _UnFocusCallback);

	bool Tick();

	//force reload all scripts
	void Reload();

	void CallCScriptCopyFrom(const CScript* from, const CScript* to);
	void CallCScriptSerialize();
	void CallCScriptDeSerialize();
	void CallCScriptRegister(CScript* a);
	void CallCScriptUnregister(CScript* a);

    void CallFocus();
    void CallUnFocus();

	//stores the data objects until they are to be deleted when passed as params
	std::vector<const void*> lingeringData;

	template <typename T>
	void QueueEvent(const GameObject* from, char* methodname, const T* data)
	{
		static_assert(std::is_base_of<IMeta, T>::value, "Data given to CSharp must be extended from IMeta");
		std::string data_type = dynamic_cast<const IMeta*>(data)->mymeta().gettoplevelname();
		for (Component* comp : from->mComponents)
		{
			if (comp->mymeta().getisobjecttype<CScript>())
				if (dynamic_cast<CScript*>(comp)->scriptAsset.Valid())
					mQueueEventCallback(dynamic_cast<CScript*>(comp), methodname, data, const_cast<char*>(data_type.c_str()));
		}
		lingeringData.push_back(data);
	}
	void QueueEvent(const GameObject* from, char* methodname);
	static ScriptSubsystem* getInstance(bool create = true);

	metadef(ScriptSubsystem)
		endmetadef;
};

