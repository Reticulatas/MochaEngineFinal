#include <queue>
#include "metafuncs.h"

struct ThreadQueueExecutionPack
{
	meta::metafunction_method* fnc_to_exec;	//the function to execute on this node
	void* cl;							//the class to execute the function on
};

class ThreadedQueue;

class ThreadQueueNode
{
public:
	enum ThreadQueueNodeType {
		NONE, EXECUTE, BLOCK, END
	};
private:
	ThreadQueueNodeType node_type;						//type of this node
	ThreadQueueExecutionPack execution_pack;			//the pack of execution data for the function
	std::vector<ThreadQueueNode*> childNodes;			//nodes to call afterwards
	std::vector<ThreadQueueNode*> parentNodes;			//nodes that I came from (1, normally, 2+ if blocking node)
	BarristaToGo::BarristaToGoDescriptor* myDesc;		//descriptor for the function I am executing
	ThreadedQueue* myQueue;								//back reference to the queue I belong in
	std::atomic<bool> entered;										//are we in/have been in this node?
	std::atomic<bool> done;										//are we done in this node?

	/* Execute this function */
	void Execute();				

public:
	ThreadQueueNode(ThreadedQueue*);
	ThreadQueueNode(meta::metafunction_method* fnc_to_exec_, void* cl, ThreadedQueue*);

	/* Register after this node */
	ThreadQueueNode* CallAfter(meta::metafunction_method* fnc_to_exec, void* cl);
	/* Join branch with specified node and block until both branches have completed. Returns BLOCK node */
	ThreadQueueNode* JoinAndBlock(std::vector<ThreadQueueNode*> blockWithNodes);
	void CallEnd();
	void JoinBlockAndEnd(std::vector<ThreadQueueNode*> blockWithNodes);

	friend class ThreadedQueue;
};

class ThreadedQueue
{
	ThreadQueueNode* head;						//dummy node to start
	std::promise<void> end_promise;
	std::future<void> end_future;
	ThreadQueueNode* end;
	bool output_schedule;

	std::list<ThreadQueueNode*> acquiredNodes;

	void SetEnd(ThreadQueueNode* );
public:
	ThreadedQueue();
	~ThreadedQueue();

	ThreadQueueNode* GetStart();				//the head/first dummy node

	/* Execute the entire queue */
	void Execute();

	void OutputSchedule();


	friend class ThreadQueueNode;
};