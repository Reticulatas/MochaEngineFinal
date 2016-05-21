#pragma once
#include <thread>
#include <list>
#include <vector>
#include <mutex>
#include <queue>
#include <map>
#include <condition_variable>
#include <chrono>
#include <ctime>
#include "ISubsystem.h"
#include "BarristaToGo.h"

class IBarristaOperation;
struct wrapper_critsection;

class Barrista : public ISubsystem
{
	struct line;
	struct operation
	{
		line* myline;                   //my encapsulated thread
		IBarristaOperation* op;         //underlying task/subsystem
		operation* prev_async;          //the operation on ANY line before this one

		wrapper_critsection* myCritSection;

		//statistics
		long long samples_sum;              //summation of lifetime runtime samples of this operation
		long long samples_num;              //number of lifetime runtime samples of this operation

        //timing
		long long sleepMS;
		std::chrono::system_clock::time_point timeAtLastTick;

		void deregister();
		std::string GetName() const;
		operation(line* myline, IBarristaOperation* op, operation* prev);
		~operation();
	};

    //provide prev_async -> operation for the signal functions
	typedef std::multimap<operation*, operation*> PrevToOperationLookupTable;
	PrevToOperationLookupTable prevToOperationLookupTable;
	//list of dependencies of a IBarristaOperation
	typedef std::multimap<IBarristaOperation*, operation*> BOpToOpLookupTable;
	BOpToOpLookupTable bOpToOpLookupTable;

	struct line
	{
		virtual bool T_Tick() = 0;                      //process currentOp and check for continuation condition
		typedef std::list<operation*> operationlist;
		operationlist ops;          //all ops on this line
	};

	struct workerline : public line
	{
		bool T_Tick();
		std::thread* thr;
		std::mutex mutex;
		std::condition_variable cv;
		bool g_wait;
		workerline(void);
		std::queue<operation*> opqueue;     //lines to be processed currently (or lastly, in a blocking state) 
		workerline(const workerline& o);
	};

	struct primaryline : public line
	{
		bool T_Tick();
	};

	primaryline* primaryLine;
	std::vector<workerline*> lines;
	std::queue<operation*> operationPool;   //collection resource to avoid manually memory allocation

	void signal_operationended(operation* endedOp);

	workerline* addline();
	operation* addoperation(line* myline, IBarristaOperation* op, operation* prev);

    //settings
	bool timeOperations;        //should we collect statistics on operation runtimes?
	Barrista();
public:

    /* static base list registration on primary thread (main thread) */

    //Adds task as LAST operation in primary thread
	void RegisterToPrimary(IBarristaOperation* o);
	void RegisterToPrimary_After(IBarristaOperation* toReg, IBarristaOperation* afterThis);
	void RegisterToPrimary_Before(IBarristaOperation* toReg, IBarristaOperation* beforeThis);
	void RegisterToPrimary_AfterLast(IBarristaOperation* toReg);

    /* dynamic worker thread registration for one-off registration for one-off tasks */

    //registers the task to run on the same thread as the supplied operation immediately after
	void RunAfter(IBarristaOperation* toRun, IBarristaOperation* existing);
    //registers the task to run on the same thread as the supplied operation, delaying whatever came after
	void RunBefore(IBarristaOperation* toRun, IBarristaOperation* existing);

    //register the task to run at the same time as another operation on a different thread, if there already exists an operation at this time frame, it will always be run parallel
	void RunParallelTo(IBarristaOperation* toRun, IBarristaOperation* existing);

    //safe alternative to sleep, will not run any Ticks for time specified
    //CURRENTLY NO FUNCTIONAL
	void SleepOperation(IBarristaOperation* toSleep, long long timeMS);

    //for debugging, output the schedule
	void OutputSchedule();
	void RegisterDebugOutputToParallel(IBarristaOperation* duringThis);

    //for debugging, time the pop and drops
	bool TimeOperations() const { return timeOperations; }
	void TimeOperations(bool val) { timeOperations = val; }

    //Barrista does not specify a common update, but may require it for specific cases
	bool Tick();

    //to go window (BarristaToGo redirects, see IBarristaOperation for global interface)
	BarristaToGo* togo;

	struct operation_info {
		unsigned line;		//0 for primary
		long long samples_num;
		long long samples_sum;
		std::string name;
	};

	//schedule accessor
	std::vector<operation_info> Schedule_Generate();

	friend Barrista& barrista();
	friend class BarristaToGo;
	static Barrista* instance;

	static Barrista* getInstance();

	metadef(Barrista)
		m_add(bool, timeOperations);
	endmetadef;

};

//singleton external access
Barrista& barrista();