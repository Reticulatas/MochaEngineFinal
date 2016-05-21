#include "stdafx.h"
#include "EditorMsgDelegators.h"
#include "ThreadUtils.h"

/////---------------------------------Implementation------------------------------

EngineMsgDelegator& EngineMsgDelegator::getInstance( void )
{
	static EngineMsgDelegator* instance = 0;
	if (!instance)
		instance = new EngineMsgDelegator();
	return *instance;
}

void* T_ProcessQueue(void* emd)
{
	EngineMsgDelegator* inst = (EngineMsgDelegator*)emd;

	while (!inst->msgQueue.empty())
	{
		//get it
		EngineMsgDelegator::EngineMsg* m = inst->msgQueue.front();

		//pop it off
		inst->msgQueue.pop();

		m->func->invoke_with_pack_params(m->class_inst);

		//delete the msg
		delete m;
	}
	return 0;
}

void EngineMsgDelegator::ProcessQueue( void )
{
	if (!msgQueue.empty())
	{
		ThreadUtils::ThreadTryLockVisitor v(3);			//something might be trying to modify the engine on a message thread
		if (!v.GetDidLock())							//if this is the case, then we have no choice but to skip this frame
			return;										//of message processing

		T_ProcessQueue(this);
	}
}

void EngineMsgDelegator::Push( meta::metafunction_method* func, void* cl )
{
	EngineMsg* msg = new EngineMsg();
	msg->func = func;
	msg->class_inst = cl;

	msgQueue.push(msg);
}

void EngineMsgDelegator::ClearQueue( void )
{
	while (msgQueue.size() != 0)
	{
		delete msgQueue.front();
		msgQueue.pop();
	}
}

//Nicholas Fuller
//10/31/13