#include "stdafx.h"
#include "BarristaToGo.h"
#include "wrapper_critsection.h"

struct impl_opcritsection
{
	CRITICAL_SECTION* critsection;
};
struct impl_opcritsection_static
{
  CRITICAL_SECTION critsection;
};

void BarristaToGo::BarristaToGoWorkerThread::T_Tick()
{
	while (true) {
		std::unique_lock<std::mutex> locker(mutex);
		while (!occupied)
		{
			cv.wait(locker);
		}

		if (sleepDelay != 0)
		{
			std::chrono::milliseconds dur(sleepDelay);
			std::this_thread::sleep_for(dur);
		}


		//will block is clash op is currently running
		if (clash_op)
			EnterCriticalSection(clash_op_critsec->critsection);
		if (!fnc_to_exec || !class_to_exec)
			throw std::exception("To-Go Thread is attempting to execute malformed data.");
		fnc_to_exec->invoke_with_pack_params(class_to_exec);
		if (clash_op)
			LeaveCriticalSection(clash_op_critsec->critsection);

		if (!repeat)
		{
			occupied = false;
			descriptor.execution_promise.set_value();
		}
	}
}

BarristaToGo::BarristaToGoWorkerThread::BarristaToGoWorkerThread() : occupied(false), fnc_to_exec(0), clash_op(0), sleepDelay(0), class_to_exec(0), repeat(false)
{
	clash_op_critsec = new impl_opcritsection();
	clash_op_critsec->critsection = 0;
 	thr = new std::thread(&BarristaToGo::BarristaToGoWorkerThread::T_Tick, this);
  ThreadUtils::SetThreadName(thr, "BarristaToGoThread");
}

BarristaToGo::BarristaToGoWorkerThread::~BarristaToGoWorkerThread()
{
	delete clash_op_critsec;
	delete thr;
}

void BarristaToGo::stats_PendingJobs(std::list<stats_PendingJob>& r_jobs)
{
	for (auto& t : workers)
	{
		if (t->occupied)
		{
			stats_PendingJob j;
			j.func = t->fnc_to_exec->name;
			j.delay = t->sleepDelay;
			r_jobs.push_back(j);
		}
	}
}

unsigned BarristaToGo::stats_TotalThreads()
{
	return workers.size();
}

unsigned BarristaToGo::stats_ActiveThreads()
{
	unsigned i = 0;
	for (auto& t : workers)
		if (t->occupied)
			++i;
	return i;
}

void BarristaToGo::set_thread_data(BarristaToGoWorkerThread* w, void* cl, meta::metafunction_method* fnc, IBarristaOperation* op /*= 0*/, unsigned delay /*= 0*/, bool repeat)
{
	w->fnc_to_exec = fnc;
	w->sleepDelay = delay;
	w->clash_op = op;
	w->class_to_exec = cl;
	w->repeat = repeat;
	w->descriptor.workerThread = w;
	w->descriptor.execution_promise = std::promise<void>();
	w->descriptor.promise_get = w->descriptor.execution_promise.get_future();
	if (op)
	{
		Barrista::BOpToOpLookupTable::const_iterator i = barrista().bOpToOpLookupTable.find(op);
		if (i == barrista().bOpToOpLookupTable.end())
			throw std::exception("Operation does not exist in the Barrista, thus cannot be a 'clashing' operation for a To Go order");
		w->clash_op_critsec->critsection = &i->second->myCritSection->cs;
	}
	else
		w->clash_op_critsec->critsection = 0;
	w->occupied = true;
	w->cv.notify_one();
}

BarristaToGo::BarristaToGoDescriptor* BarristaToGo::Invoke(void* cl, meta::metafunction_method* fnc, IBarristaOperation* op /*= 0*/, unsigned delay /*= 0*/, bool repeat)
{
	for (auto& w : workers)
	{
		if (w->occupied == false)
		{
			set_thread_data(w, cl, fnc, op, delay,repeat);
			return &w->descriptor;
		} 
	}

	//no worker to give task to, make a new one
	BarristaToGoWorkerThread* w = new BarristaToGoWorkerThread();
	set_thread_data(w, cl, fnc, op, delay,repeat);
	workers.push_back(w);


	return &w->descriptor;
}

void BarristaToGo::BarristaToGoDescriptor::BlockUntilFinished()
{
  if (!this->promise_get.valid())
    throw std::exception("WOOP");
  this->promise_get.wait();
  if (this->workerThread->occupied)
    throw std::exception("WOOP2");
}

void BarristaToGo::BarristaToGoDescriptor::BlockUntilAllSimilarTasksFinished()
{
  if (!this->workerThread)
    throw new MochaException("Invalid descriptor for block");
  if (!this->workerThread->fnc_to_exec)
    throw new MochaException("Invalid descriptor for block");

  for (auto& w : barrista().togo->workers)
  {
    if (*w->fnc_to_exec == *this->workerThread->fnc_to_exec)
      if (w->occupied)
        w->descriptor.BlockUntilFinished();
  }

  for (auto& w : barrista().togo->workers)
  {
    if (w->occupied)
      throw std::exception("nope");
  }
}
