#pragma once
#include <list>
#include <thread>
#include "meta.h"
#include <string>
#include <mutex>
#include <condition_variable>
#include <future>

class IBarristaOperation;
//avoiding including windows.h by wrapping and forward declaring crit sections
struct impl_opcritsection;
struct impl_opcritsection_static;

class BarristaToGo
{
  struct BarristaToGoWorkerThread;

public:

	class BarristaToGoDescriptor
	{
    std::promise<void> execution_promise;
    std::future<void> promise_get;
    BarristaToGoWorkerThread* workerThread;
	public:
    //block until this task has completed
		void BlockUntilFinished();
    //block until all tasks with the same function have completed
    void BlockUntilAllSimilarTasksFinished();

    friend class BarristaToGo;
	};

private:

	struct BarristaToGoWorkerThread
	{
		std::thread* thr;
		bool occupied;
		unsigned sleepDelay;
		bool repeat;
		meta::metafunction_method* fnc_to_exec;
		void* class_to_exec;
		IBarristaOperation* clash_op;
		impl_opcritsection* clash_op_critsec;
		BarristaToGoWorkerThread();
		~BarristaToGoWorkerThread();
		std::condition_variable cv;
		std::mutex mutex;
		BarristaToGoDescriptor descriptor;

		void T_Tick();
	};
	std::list<BarristaToGoWorkerThread*> workers;

    //helper to set thread data for next job
	void set_thread_data(BarristaToGoWorkerThread* w, void* cl, meta::metafunction_method* fnc, IBarristaOperation* op, unsigned delay, bool repeat);

public:
    //op has no function if delay is set to zero, otherwise it blocks that task during that operation's running period
	BarristaToGoDescriptor* Invoke(void* cl, meta::metafunction_method* fnc, IBarristaOperation* op = 0, unsigned delay = 0, bool repeat = false);
	template <typename... Args>
	BarristaToGoDescriptor* Invoke(void* cl, meta::metafunction_method* fnc, IBarristaOperation* op, unsigned delay, bool repeat, Args&&... args)
	{
		fnc->prepack_params(args...);
		return Invoke(cl, fnc, op, delay, repeat);
	}

    ///statistics 
	unsigned stats_ActiveThreads();
	unsigned stats_TotalThreads();
	struct stats_PendingJob { std::string func; unsigned delay; };
	void stats_PendingJobs(std::list<stats_PendingJob>& r_jobs);
};

