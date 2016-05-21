#include "Job.h"

namespace MochaThread
{
	Job::Job(JobType jobType, meta::metafunction_method* FunctionToRun, void* FunctionClass, ExecutionBlock* execBlock) :
		mJobType(jobType),
		mFunction(FunctionToRun), 
		mFunctionClass(FunctionClass),
		mCurrentExecutionBlock(execBlock)
	{
		// Empty
	}

	void Job::Run()
	{
		// Run the current function
		mFunction->invoke_with_pack_params(mFunctionClass);
	}
}

