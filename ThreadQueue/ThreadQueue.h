#pragma once

#include <vector>
#include "ExecutionBlock.h"

namespace MochaThread
{
	class ThreadQueue
	{
		std::vector<ExecutionBlock*> mExecutionBlockList;

	public:
		// ctors & dtor
		ThreadQueue();
		~ThreadQueue();

		// Block management
		ExecutionBlock* CreateNewExecutionBlock(void);
		unsigned GetExecutionBlockCount(void);
		void ClearExecutionList(void);

		// Execute the queue
		void Execute(void);
	};
}

