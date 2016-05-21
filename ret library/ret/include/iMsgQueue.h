/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#pragma once

class iMsgQueueListener
{
public:
	virtual void OnReceiveMessage(void* param) = 0;
};


//Generic queue message that calls the listener function
template <typename T>
class iMsg
{
protected:
	iMsgQueueListener* receiver;
	T param;
private:
	bool		isUnique;
public:
	bool GetIsUnique() const { return isUnique; }
	iMsgQueueListener* Receiver() const { return receiver; }
	T Param() const { return param; }
	iMsg(iMsgQueueListener* listener, T& param_) : receiver(listener), param(param_) { }
	iMsg(iMsgQueueListener* listener) : receiver(listener) { }
};


//Unique message imply only one of the type may be in the queue at a time
//In the event a duplicate tries to be pushed, it will be rejected
template <typename T>
class iUniqueMsg : public iMsg<T>
{
	const type_info	typeInfo;
public:
	iUniqueMsg(void) : iMsg<T>(), isUnique(true), typeInfo(typeid(*this)) {}
	type_info const& GetTypeInfo(void) const { return typeInfo; }
};

//When the queue hits this message it will cease bulk processing
template <typename T>
class iMsgStopProcessing : public iMsg<T>
{
public:
	iMsgStopProcessing(void) : iMsg<T>() { }
};

template <typename T>
class iMsgQueue
{
private:
	std::queue<iMsg<T>>		msgQueue;
	std::list<type_info>	queuedUniques;
public:
	/* Process every message in the queue */
	virtual void ProcessQueue(void)
	{
		while (!msgQueue.empty())
		{
			//this is a STOP msg
			if (msgQueue.front().Receiver() == 0)
				break;
			ProcessNext();
		}
	}
	/* Process next message in the queue */
	virtual void ProcessNext(void)
	{
		iMsg<T>* msg = &msgQueue.front();
		msg->Receiver()->OnReceiveMessage(&msg->Param());
		msgQueue.pop();
	}
	/* Push new message into the queue */
	virtual void Push(iMsg<T>& msg)
	{
		if (msg.GetIsUnique())
		{
			iUniqueMsg<T>* iu = static_cast<iUniqueMsg<T>*>(&msg);
		}
		msgQueue.push(msg);
	}
	/* Immediately remove all messages without processing */
	void Clear(void)
	{
		while (msgQueue.size())
			msgQueue.pop();
	}

	unsigned Size(void)
	{
		return (unsigned)msgQueue.size();
	}
};

