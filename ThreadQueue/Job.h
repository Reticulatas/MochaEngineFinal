#pragma once
#include "meta.h"

namespace MochaThread
{
	class ExecutionBlock; // Forward Declaration

	enum JobType
	{
		JOB_NORMAL,
		JOB_WAIT,
		JOB_BEGIN_RACE,
		JOB_END_RACE
	};

	class Job
	{
		meta::metafunction_method* mFunction;	//the function to execute on this Job
		void* mFunctionClass;					//the class to execute the function on

		JobType mJobType;

		ExecutionBlock* mCurrentExecutionBlock;

		/////// All methods must be private ///////////
		Job(JobType jobType, meta::metafunction_method* FunctionToRun, void* FunctionClass, ExecutionBlock* execBlock);
		virtual void Run();

		friend class ExecutionBlock;
	};
}

