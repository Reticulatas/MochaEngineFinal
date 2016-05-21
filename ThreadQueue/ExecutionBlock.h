#pragma once

#include <vector>
#include "Job.h"
#include "IRegisterable.h"
#include "meta.h"

#define MAKEJOB(cl, func) MAKEMETAMETHODVARIABLE(cl, func)

namespace MochaThread
{
	class ExecutionBlock //: public IMeta
	{
		std::vector<Job> mJobs;
		std::vector<ExecutionBlock*> mChildrenLinks;
		std::vector<ExecutionBlock*> mParentLinks;

	private:
		// Execution blocks must be created from the Thread Queue
		ExecutionBlock();

	public:
		// Execute all jobs in sequence
		void Execute();

		// Add a job to be executed in this block
		//AddJob(MAKEJOB(Bob,MethodFoo), &bob) // bob is a cool guy
		template <typename Args...>
		void AddJob(meta::metafunction_method*, void* cl, Args&& args...);

		// Wait and flow control Jobs
		// Wait for all parent jobs to complete before progressing
		void AddWaitJob(void);

		// The First parent job to complete will execute
		// and block the others
		// If you begin a race, you must always call end
		void AddBeginRaceJob(void);
		void AddEndRaceJob(void);


		
		// Adds a Child Link and sets this Execution Block as a parent
		void AddLinkAsChild(ExecutionBlock* childBlock);

		// Removes all the jobs from the current execution list
		void ClearJobsList(void);

		// Checks for Begin Races without ends
		void ValidateJobsList(void);

		friend class ThreadQueue;

		//metadef(ExecutionBlock)
		//	endmetadef;
	};

	template <typename Args...>
	void ExecutionBlock::AddJob(meta::metafunction_method* fn, void* cl, Args&& args...)
	{
		Job jb(JOB_NORMAL,fn, cl, this);
		fn->prepack_params(args);

		// Add the new job to our job vector
		mJobs.push_back(jb);
	}
}
