#include "stdafx.h"
#include "ThreadedQueue.h"
#include "meta.h"
#include "metafuncs.h"

//////////////////////////////////////////////////////////////////////////

static std::queue<ThreadQueueNode*> unusedNodes;
static std::vector<ThreadQueueNode*> usedNodes;

template <typename... Args>
ThreadQueueNode* RetrieveNode(Args&&... args)
{
	ThreadUtils::ThreadLockVisitor tv(5);
	if (unusedNodes.size())
	{
		ThreadQueueNode* node = unusedNodes.front();
		unusedNodes.pop();
		node = new(node)ThreadQueueNode(std::forward<Args>(args)...);
		usedNodes.push_back(node);
		return node;
	}
	else
	{
		ThreadQueueNode* node = new ThreadQueueNode(std::forward<Args>(args)...);
		usedNodes.push_back(node);
		return node;
	}
}

void RelinquishNode(ThreadQueueNode* node)
{
	ThreadUtils::ThreadLockVisitor tv(5);
	usedNodes.erase(std::remove(usedNodes.begin(), usedNodes.end(), node), usedNodes.end());
	unusedNodes.push(node);
}

//////////////////////////////////////////////////////////////////////////

void ThreadQueueNode::Execute()
{
	entered = true;

	//execute myself
	if (node_type == ThreadQueueNodeType::EXECUTE)
	{
		execution_pack.fnc_to_exec->invoke_with_pack_params(execution_pack.cl);
	}
	else if (node_type == ThreadQueueNodeType::BLOCK)
	{
		//block until parents are done
		for (ThreadQueueNode* parent : parentNodes)
		{
			parent->myDesc->BlockUntilFinished();
		}
	}
	else if (node_type == ThreadQueueNodeType::END)
	{
		myQueue->end_promise.set_value();
		done = true;
		return;
	}

	//execute children
	if (childNodes.size() == 1 && childNodes.front()->node_type != BLOCK)
	{
		//child inherits this descriptor (thread still running)
		childNodes.front()->myDesc = this->myDesc;
		//invalidate previous descriptor (it's future promises are broken)
		this->myDesc = 0;
		//execute in serial
		childNodes.front()->Execute();
	}
	else
	{
		//execute in parallel
		for (ThreadQueueNode* node : childNodes)
		{
			if (node->entered == false)
			{
				meta::metafunction_method* execution_func = MAKEMETAMETHODVARIABLE(ThreadQueueNode, Execute);
				if (node->node_type == BLOCK)
				{
					//I win the thread race, remove me from the queue
					node->parentNodes.erase(std::remove(node->parentNodes.begin(), node->parentNodes.end(), this), node->parentNodes.end());
				}
				BarristaToGo::BarristaToGoDescriptor* childDesc = barrista().togo->Invoke(node, execution_func, 0, 0, 0);
				node->myDesc = childDesc;
			}
		}
	}

	done = true;
}

ThreadQueueNode::ThreadQueueNode(meta::metafunction_method* fnc_to_exec_, void* cl, ThreadedQueue* tqueue) : entered(false), done(false)
{
	execution_pack.fnc_to_exec = fnc_to_exec_;
	execution_pack.cl = cl;
	node_type = EXECUTE;
	myDesc = 0;
	myQueue = tqueue;
	tqueue->acquiredNodes.push_back(this);
}

ThreadQueueNode::ThreadQueueNode(ThreadedQueue* tqueue) : entered(false), done(false)
{
	execution_pack.fnc_to_exec = 0;
	execution_pack.cl = 0;
	node_type = ThreadQueueNodeType::NONE;
	myDesc = 0;
	myQueue = tqueue;
	tqueue->acquiredNodes.push_back(this);
}

ThreadQueueNode* ThreadQueueNode::CallAfter(meta::metafunction_method* fnc_to_exec, void* cl)
{
	childNodes.push_back(RetrieveNode(fnc_to_exec, cl, myQueue));
	ThreadQueueNode* newNode = childNodes.back();
	newNode->parentNodes.push_back(this);
	return newNode;
}

ThreadQueueNode* ThreadQueueNode::JoinAndBlock(std::vector<ThreadQueueNode*> blockWithNodes)
{
	childNodes.push_back(RetrieveNode(myQueue));
	ThreadQueueNode* newNode = childNodes.back();
	newNode->node_type = ThreadQueueNodeType::BLOCK;
	for (ThreadQueueNode* node : blockWithNodes)
	{
		if (node == this)		//do not add this to the list, do it manually
			continue;
		node->childNodes.push_back(newNode);
		newNode->parentNodes.push_back(node);
	}
	newNode->parentNodes.push_back(this);
	return newNode;
}

void ThreadQueueNode::CallEnd()
{
	if (!childNodes.empty())
		throw std::exception("Cannot add END node to node with multiple children.");

	childNodes.push_back(RetrieveNode(myQueue));
	childNodes.front()->node_type = END;
	myQueue->SetEnd(childNodes.front());
}

void ThreadQueueNode::JoinBlockAndEnd(std::vector<ThreadQueueNode*> blockWithNodes)
{
	JoinAndBlock(blockWithNodes)->CallEnd();
}

//////////////////////////////////////////////////////////////////////////

ThreadQueueNode* ThreadedQueue::GetStart()
{
	return head;
}

void ThreadedQueue::Execute()
{
	if (!end)
		throw std::exception("END not set for this ThreadQueue - execution will continue indefinitely.");

	head->myDesc = barrista().togo->Invoke(head, MAKEMETAMETHODVARIABLE(ThreadQueueNode, Execute), 0, 0, 0);
	end_future.wait();
}

void ThreadedQueue::OutputSchedule() 
{
	output_schedule = true;
}

ThreadedQueue::ThreadedQueue() : end(0), output_schedule(false)
{
	head = RetrieveNode(this);
}


ThreadedQueue::~ThreadedQueue()
{
	for (ThreadQueueNode* node : acquiredNodes)
		RelinquishNode(node);
	if (!usedNodes.empty())
		throw std::exception("invalid, all nodes were not used");
}

void ThreadedQueue::SetEnd(ThreadQueueNode* node)
{
	end = node;
	this->end_promise = std::promise<void>();
	this->end_future = end_promise.get_future();
}

//////////////////////////////////////////////////////////////////////////