#pragma once
#include <string>
#include <queue>
#include "meta.h"

class EngineMsgDelegator
{
private:
	struct EngineMsg
	{
		meta::metafunction_method* func;
		void* class_inst;
	};

	std::queue<EngineMsg*> msgQueue;

public:
	void ProcessQueue(void);
	/*! Push a new msg into the last spot in the queue */
	void Push(meta::metafunction_method* msg, void* cl);
	/*! Immediately clear the message queue - careful! */
	void ClearQueue(void);

	friend class EditorSubsystem;
	friend void* T_ProcessQueue(void* emd);

	static EngineMsgDelegator& getInstance(void);
};

//Nicholas Fuller
//10/31/13