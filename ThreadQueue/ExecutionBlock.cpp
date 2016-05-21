#include "ExecutionBlock.h"
#include <Asset.h>


namespace MochaThread
{
	ExecutionBlock::ExecutionBlock()
	{
		// Empty
	}

	//////////////// JOB LIST ///////////////////////////

	void ExecutionBlock::AddLinkAsChild(ExecutionBlock* childBlock)
	{
		// Check for self referencing and null pointer
		if (!childBlock || childBlock == this)
			return;

		// Add this as a parent link
		childBlock->mParentLinks.push_back(this);

		// Add the child Block to the children vector
		mChildrenLinks.push_back(childBlock);
	}

	void ExecutionBlock::Execute()
	{
		for (unsigned i = 0; i < mJobs.size(); ++i)
		{
			mJobs[i].Run();
		}
	}

	// Removes all the jobs from the current execution list
	void ExecutionBlock::ClearJobsList(void)
	{
		mJobs.clear();
	}


	////////////////  JOB MANAGEMENT ////////////////////

	void ExecutionBlock::AddWaitJob(void)
	{
		Job jb(JOB_WAIT,0, 0,this);

		// Add the new job to our job vector
		mJobs.push_back(jb);
	}

	void ExecutionBlock::AddBeginRaceJob(void)
	{
		Job jb(JOB_BEGIN_RACE, 0, 0, this);

		// Add the new job to our job vector
		mJobs.push_back(jb);
	}

	void ExecutionBlock::AddEndRaceJob(void)
	{
		Job jb(JOB_END_RACE, 0, 0, this);

		// Add the new job to our job vector
		mJobs.push_back(jb);
	}


	/////////////// DEBUG //////////////////////

	// Checks for Begin Races without ends
	void ExecutionBlock::ValidateJobsList(void)
	{
		// Cehck to see if this job execution list is valid
		// Tests for Begins without ends

		int outstandingRaceCount = 0;

		for (unsigned i = 0; i < mJobs.size(); ++i)
		{
			switch (mJobs[i].mJobType)
			{
				case JOB_NORMAL:
					break;
				case JOB_BEGIN_RACE:
					outstandingRaceCount++;
					break;
				case JOB_END_RACE:
					outstandingRaceCount--;
					break;
			}
		}

		// We have an unmatching pair of Begins and Ends
		// Panic!
		assert(outstandingRaceCount != 0);

	}

}

//meta_define(ExecutionBlock);