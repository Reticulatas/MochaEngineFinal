#include "ThreadQueue.h"

namespace MochaThread
{
	ThreadQueue::ThreadQueue()
	{

	}

	void ThreadQueue::ClearExecutionList(void)
	{
		for (unsigned i = 0; i < mExecutionBlockList.size(); ++i)
		{
			delete mExecutionBlockList[i];
		}

		mExecutionBlockList.clear();
	}

	ThreadQueue::~ThreadQueue()
	{
		ClearExecutionList();
	}

	// Block management
	ExecutionBlock* ThreadQueue::CreateNewExecutionBlock(void)
	{
		ExecutionBlock* newExeBlock = new ExecutionBlock();

		mExecutionBlockList.push_back(newExeBlock);

		return newExeBlock;
	}

	unsigned ThreadQueue::GetExecutionBlockCount(void)
	{
		return mExecutionBlockList.size();
	}

	// Execute the queue
	void ThreadQueue::Execute(void)
	{
		if (mExecutionBlockList.size())
			mExecutionBlockList[0]->Execute();
	}
}

