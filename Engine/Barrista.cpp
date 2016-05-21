#include "stdafx.h"
#include "Barrista.h"
#include "IBarristaOperation.h"
#include <chrono>
#include "def.h"
#include "BarristaToGo.h"
#include "wrapper_critsection.h"

Barrista& barrista()
{
	if (Barrista::instance == 0)
		Barrista::instance = new Barrista();
	return *Barrista::instance;
}

//////////////////////////////////////////////////////////////////////////

//debug struct
struct OutputBarristaDebugOperation : IBarristaOperation
{
	bool waspressedlastframe;
	OutputBarristaDebugOperation() : waspressedlastframe(false){}

	bool Tick()
	{
        //dump stats when CTRL-b is pressed
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('B'))
		{
			if (!waspressedlastframe)
			{
				barrista().OutputSchedule();
				waspressedlastframe = true;
			}
		}
		else
			waspressedlastframe = false;
		return true;
	}

	metadef(OutputBarristaDebugOperation)
		m_add(bool, waspressedlastframe)
	endmetadef
};
meta_define(OutputBarristaDebugOperation);

//////////////////////////////////////////////////////////////////////////

bool Barrista::workerline::T_Tick()
{
	while (true)
	{
		std::unique_lock<std::mutex> locker(mutex);
		while (g_wait)
		{
			cv.wait(locker);
		}

		while (!opqueue.empty())
		{
			//process tasks
			operation* op = opqueue.front();

            //timing
			if (op->sleepMS == 0)
			{
				EnterCriticalSection(&op->myCritSection->cs);
				std::chrono::system_clock::time_point start;
				if (barrista().TimeOperations())
					start = std::chrono::system_clock::now();
				if (!op->op->Tick())
					ops.erase(std::remove(ops.begin(), ops.end(), op), ops.end());
				if (barrista().TimeOperations())
				{
					auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
					op->samples_sum += duration.count();
					++op->samples_num;
				}
				LeaveCriticalSection(&op->myCritSection->cs);
			}
			else
			{
				auto curtime = std::chrono::system_clock::now();
				auto timepassed = std::chrono::duration_cast<std::chrono::milliseconds>(curtime - op->timeAtLastTick);
				op->sleepMS -= timepassed.count();
				if (op->sleepMS < 0)
					op->sleepMS = 0;
			}

			op->timeAtLastTick = std::chrono::system_clock::now();

			opqueue.pop();
		}

		g_wait = true;
	}

	return true;
}

bool Barrista::primaryline::T_Tick()
{

    //initials
	barrista().signal_operationended(0);

    //process entire line, signaling as we go 
	operationlist::iterator i = ops.begin();
	for (; i != ops.end(); ++i)
	{
		operation* op = *i;
		if (op->sleepMS == 0)
		{
			EnterCriticalSection(&op->myCritSection->cs);
			std::chrono::system_clock::time_point start;
			if (barrista().TimeOperations())
				start = std::chrono::system_clock::now();
			if (!op->op->Tick())
				i = ops.erase(i);
			barrista().signal_operationended(op);
			if (barrista().TimeOperations())
			{
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
				op->samples_sum += duration.count();
				++op->samples_num;
			}
			LeaveCriticalSection(&op->myCritSection->cs);
			if (i == ops.end())
				break;
		}
		else
		{
			auto curtime = std::chrono::system_clock::now();
			auto timepassed = std::chrono::duration_cast<std::chrono::milliseconds>(curtime - op->timeAtLastTick);
			op->sleepMS -= timepassed.count();
			if (op->sleepMS < 0)
				op->sleepMS = 0;
		}

		op->timeAtLastTick = std::chrono::system_clock::now();
	}


	return true;
}


Barrista::workerline::workerline(void)
{
	g_wait = true;
	thr = new std::thread(&Barrista::line::T_Tick, this);
}

Barrista::workerline::workerline(const workerline& o)
{
	thr = new std::thread(&Barrista::line::T_Tick, this);
	ops = o.ops;
	g_wait = true;
}

//////////////////////////////////////////////////////////////////////////

void Barrista::RegisterToPrimary(IBarristaOperation* o)
{
	operation* op = addoperation(primaryLine, o, 0);
	primaryLine->ops.push_back(op);
}

void Barrista::RegisterToPrimary_After(IBarristaOperation* toReg, IBarristaOperation* o)
{
	BOpToOpLookupTable::iterator i = bOpToOpLookupTable.find(o);
	if (i == bOpToOpLookupTable.end())
		throw std::exception("Existing operation does not exist");

	line::operationlist::const_iterator iter = std::find(primaryLine->ops.begin(), primaryLine->ops.end(), i->second);

  operation* op = addoperation(primaryLine, toReg, bOpToOpLookupTable.find(o)->second);
  if (iter == primaryLine->ops.end())
  {
    //was in primary line
    ++iter;
    primaryLine->ops.insert(iter, op);
  }
  else
  {
    //was in secondary line
    primaryLine->ops.push_back(op);
  }
}

void Barrista::RegisterToPrimary_AfterLast(IBarristaOperation* toReg)
{
	operation* prev = 0;
	if (!primaryLine->ops.empty())
		prev = primaryLine->ops.back();
	operation* op = addoperation(primaryLine, toReg, prev);
	primaryLine->ops.push_back(op);
}

void Barrista::RegisterToPrimary_Before(IBarristaOperation* toReg, IBarristaOperation* o)
{
	BOpToOpLookupTable::iterator i = bOpToOpLookupTable.find(o);
	if (i == bOpToOpLookupTable.end())
		throw std::exception("Existing operation does not exist");

	line::operationlist::const_iterator iter = std::find(primaryLine->ops.begin(), primaryLine->ops.end(), i->second);
	--iter;
	operation* prevop = 0;
	if (iter != primaryLine->ops.end())
		prevop = *iter;
	++iter;
	operation* op = addoperation(primaryLine, toReg, prevop);
	primaryLine->ops.insert(iter, op);
}

void Barrista::RunAfter(IBarristaOperation* toRun, IBarristaOperation* existing)
{
	BOpToOpLookupTable::iterator i = bOpToOpLookupTable.find(existing);
	if (i == bOpToOpLookupTable.end())
		throw std::exception("Existing operation does not exist");

	line::operationlist::const_iterator iter = std::find(i->second->myline->ops.begin(), i->second->myline->ops.end(), i->second);
	operation* op = addoperation((*iter)->myline, toRun, i->second);
	++iter;
	op->myline->ops.insert(iter, op);
}

void Barrista::RunBefore(IBarristaOperation* toRun, IBarristaOperation* existing)
{
	BOpToOpLookupTable::iterator i = bOpToOpLookupTable.find(existing);
	if (i == bOpToOpLookupTable.end())
		throw std::exception("Existing operation does not exist");

	line::operationlist::const_iterator iter = std::find(i->second->myline->ops.begin(), i->second->myline->ops.end(), i->second);
	--iter;
	operation* prevop = 0;
	if (iter != i->second->myline->ops.end())
		prevop = *iter;
	++iter;
	operation* op = addoperation((*iter)->myline, toRun, prevop);
	op->myline->ops.insert(iter, op);
	return;
}

void Barrista::RunParallelTo(IBarristaOperation* toRun, IBarristaOperation* existing)
{
	//query down lines for preexisting dependencies on [existing], if all lines busy, make new one
	//naive search
	BOpToOpLookupTable::iterator i = bOpToOpLookupTable.find(existing);
	if (i == bOpToOpLookupTable.end())
		throw std::exception("Existing operation does not exist");

	//full search
	for (line* l : lines)
	{
		bool found = false;
		//not the existing line
		for (BOpToOpLookupTable::const_iterator iter = i; iter != bOpToOpLookupTable.end(); ++iter)
		{
			if (iter->second->myline == l)
			{
				found = true;
				break;
			}
		}

		if (found)
			continue;

		//add to this line
		operation* prevop = 0;
		if (!l->ops.empty())
			prevop = l->ops.back();
		operation* op = addoperation(l, toRun, prevop);
		l->ops.push_back(op);
		return;
	}

	//no appropriate line found
	line* l = addline();
	operation* op = addoperation(l, toRun,i->second);
	l->ops.push_back(op);
}

void Barrista::OutputSchedule()
{
	std::map<operation*, int> ids;
    //assign ids
	int i = 0;
	for (operation* op : primaryLine->ops)
		ids[op] = ++i;
	for (line* l : lines)
		for (operation* op : l->ops)
			ids[op] = ++i;
    //draw
	std::cout << "----BARRISTA STATISTICS----" << std::endl;
	std::cout << "PRIMARYLINE < ";
	for (operation* op : primaryLine->ops)
	{
		std::cout << "[" << op->op->mymeta().type_name << "#" << ids[op];
		if (op->prev_async)
			std::cout << "(sync" << ids[op->prev_async] << ")";
		else
			std::cout << "(N)";
		if (op->samples_num != 0)
			std::cout << "(" << op->samples_sum / op->samples_num << "Ms)";
		std::cout << "] ";
	}
	std::cout << ">" << std::endl;
	int workercounter = 1;
	for (line* l : lines)
	{
		std::cout << "WORKERLINE #" << workercounter++ << " <";
		for (operation* op : l->ops)
		{
			std::cout << "[" << op->op->mymeta().type_name << "#" << ids[op];
			if (op->prev_async)
				std::cout << "(sync " << ids[op->prev_async] << ")";
			else
				std::cout << "(N)";
			if (op->samples_num != 0)
				std::cout << "(" << op->samples_sum / op->samples_num << "Ms)";
			std::cout << "] ";
		}
		std::cout << ">" << std::endl;
	}
	std::cout << "TO GO ORDERS < " << std::endl;
	std::cout << "Sub-threads: Total=" << togo->stats_TotalThreads() << " Active=" << togo->stats_ActiveThreads() << std::endl;
	std::cout << "Pending Jobs:" << std::endl;
	std::list<BarristaToGo::stats_PendingJob> j;
	togo->stats_PendingJobs(j);
	for (BarristaToGo::stats_PendingJob& p : j)
	{
		std::cout << p.func << " " << p.delay << " ms delay" << std::endl;
	}
	std::cout << ">" << std::endl;
	std::cout << "---------------------------" << std::endl;
}

void Barrista::signal_operationended(operation* endedOp)
{
	PrevToOperationLookupTable::iterator i = prevToOperationLookupTable.find(endedOp);
	for (; i != prevToOperationLookupTable.end(); ++i)
	{
		if (i->second->myline != primaryLine)
		{
			std::unique_lock<std::mutex> locker(((workerline*)i->second->myline)->mutex);
			((workerline*)i->second->myline)->opqueue.push(i->second);
			((workerline*)i->second->myline)->g_wait = false;
			((workerline*)i->second->myline)->cv.notify_one();
		}
	}
}

Barrista::Barrista()
{
	togo = new BarristaToGo();
	primaryLine = new primaryline();
	timeOperations = Config::allowBarristaDebugStats;

	std::cout << "The Coffee Shop is open! Barrista is now serving drinks." << std::endl;
}

void Barrista::RegisterDebugOutputToParallel(IBarristaOperation* duringThis)
{
	if (timeOperations)
	{
		workerline* l = addline();
		RunParallelTo(new OutputBarristaDebugOperation(), duringThis);
		std::cout << "\t[Barrista Debug Enabled] Barrista reports stats when CTRL-b is pressed" << std::endl;
	}
}

Barrista::workerline* Barrista::addline()
{
	workerline* l = new workerline();
	lines.push_back(l);
	return l;
}

Barrista::operation* Barrista::addoperation(line* myline, IBarristaOperation* op, operation* prev)
{
	operation* o;
	if (operationPool.empty())
	{
		o = new operation(myline, op, prev);
	}
	else
	{
		operation* prev = operationPool.front();
		operationPool.pop();
		o = new(prev) operation(myline, op, prev);
	}

	return o;
}

bool Barrista::Tick()
{
	primaryLine->T_Tick();
	return true;
}

void Barrista::SleepOperation(IBarristaOperation* toSleep, long long timeMS)
{
	BOpToOpLookupTable::iterator i = bOpToOpLookupTable.find(toSleep);
	if (i == bOpToOpLookupTable.end())
		throw std::exception(std::string("Operation: " + std::string(toSleep->mymeta().type_name) + " does not exist").c_str());
	//i->second->sleepMS = timeMS;
	throw std::exception("NOT YET IMPLEMENTED");
}

std::vector<Barrista::operation_info> Barrista::Schedule_Generate()
{
	std::vector<Barrista::operation_info> sch;
	for (const operation* o : primaryLine->ops)
	{
		operation_info i;
		i.line = 0;
		i.samples_num = o->samples_num;
		i.samples_sum = o->samples_sum;
		i.name = o->GetName();
		sch.push_back(i);
	}
	int line_count = 1;
	for (const line* l : lines)
	{
		for (const operation* o : l->ops)
		{
			operation_info i;
			i.line = line_count;
			i.samples_num = o->samples_num;
			i.name = o->GetName();
			i.samples_sum = o->samples_sum;
			sch.push_back(i);
		}
		++line_count;
	}

	return sch;
}

Barrista* Barrista::getInstance()
{
	return &barrista();
}

Barrista* Barrista::instance = 0;

void Barrista::operation::deregister()
{
	{
		PrevToOperationLookupTable::iterator i = barrista().prevToOperationLookupTable.find(prev_async);
		for (; i != barrista().prevToOperationLookupTable.end(); ++i)
		{
			if (i->second == this)
			{
				barrista().prevToOperationLookupTable.erase(i);
				return;
			}
		}
	}

	{
		BOpToOpLookupTable::iterator i = barrista().bOpToOpLookupTable.find(op);
		for (; i != barrista().bOpToOpLookupTable.end(); ++i)
		{
			if (i->second == this)
			{
				barrista().bOpToOpLookupTable.erase(i);
				return;
			}
		}
	}
}

Barrista::operation::operation(line* myline, IBarristaOperation* op, operation* prev) : myline(myline), op(op), samples_num(0), samples_sum(0), sleepMS(0)
{
	myCritSection = new wrapper_critsection();
	InitializeCriticalSection(&myCritSection->cs);
	prev_async = prev;
	barrista().prevToOperationLookupTable.insert(std::make_pair(prev, this));
	barrista().bOpToOpLookupTable.insert(std::make_pair(op,this));
}

Barrista::operation::~operation()
{
	DeleteCriticalSection(&myCritSection->cs);
	delete myCritSection;
}

std::string Barrista::operation::GetName() const
{
	return op->mytoplevelname();
}

meta_define(Barrista);