#include "stdafx.h"
#include "ScriptSubsystem.h"

bool ScriptSubsystem::Tick()
{
	//call tick - and subsequently all the queued events
	if (mTickCallback)
		mTickCallback();
	//delete lingering data
	for (auto* ptr : lingeringData)
		delete ptr;
	return true;
}

void ScriptSubsystem::Register_Callbacks(MochaDotNetCallback _tickcallback, MochaDotNetCScriptCopyCallback _copyFromCallback,
	MochaDotNetCScriptEventCallback _eventQueueCallback, MochaDotNetCallback _reloadCallback, MochaDotNetCallback _serialize,
	MochaDotNetCallback _deserialize, MochaDotNetCScriptCallback _registerCallback, MochaDotNetCScriptCallback _unregCallback,
    MochaDotNetCallback _FocusCallback, MochaDotNetCallback _UnFocusCallback)
{
	mTickCallback = _tickcallback;
	mCopyFromCallback = _copyFromCallback;
	mQueueEventCallback = _eventQueueCallback;
	mReloadCallback = _reloadCallback;
	mSerializeCallback = _serialize;
	mDeSerializeCallback = _deserialize;
	mRegisterCallback = _registerCallback;
	mUnregisterCallback = _unregCallback;
    mFocusCallback = _FocusCallback;
    mUnFocusCallback = _UnFocusCallback;
}

ScriptSubsystem::ScriptSubsystem() : mTickCallback(0)
{
}

ScriptSubsystem* ScriptSubsystem::getInstance(bool create)
{
	static ScriptSubsystem* inst = 0;
	if (inst == 0 && create)
		inst = new ScriptSubsystem();
	return inst;
}

void ScriptSubsystem::CallCScriptCopyFrom(const CScript* from, const CScript* to)
{
	mCopyFromCallback(from, to);
}

void ScriptSubsystem::QueueEvent(const GameObject* from, char* methodname)
{
	for (Component* comp : from->mComponents)
	{
		if (comp->mymeta().getisobjecttype<CScript>())
			mQueueEventCallback(dynamic_cast<CScript*>(comp), methodname, 0, 0);
	}
}

void ScriptSubsystem::Reload()
{
	mReloadCallback();
}

void ScriptSubsystem::CallFocus()
{
	if (Engine::getInstance()->GetIsEditorMode())
		mFocusCallback();
}

void ScriptSubsystem::CallUnFocus()
{
	if (Engine::getInstance()->GetIsEditorMode())
		mUnFocusCallback();
}

void ScriptSubsystem::CallCScriptSerialize()
{
	mSerializeCallback();
}

void ScriptSubsystem::CallCScriptDeSerialize()
{
	if (mDeSerializeCallback)
		mDeSerializeCallback();
	else
		Log("Skipped DeSerializing Scripts since the call happened before the ScriptSubystem ran the initial load.");
}

void ScriptSubsystem::CallCScriptRegister(CScript* a)
{
	mRegisterCallback(a);
}

void ScriptSubsystem::CallCScriptUnregister(CScript* a)
{
	mUnregisterCallback(a);
}

meta_define(ScriptSubsystem);
