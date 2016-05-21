/******************************************************************************/
/*!
File       EventImplementation.h
 
 
Author     Joshua Chew Shi Yang
Par        email: joshuashiyang.chew@digipen.edu
 
 
Date       December 28, 2013
Brief

All content © 2014 DigiPen (USA) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


/**
* File : EventImplementation.h
* Brief : Event template implementation.
* See Event.h and unit test file for usage example.
*/

// Engine Headers //
#include "Noncopyable.h"

#if MCH_EVENT_PARAM_COUNT > 0
#define MCH_EVENT_SEP ,
#else
#define MCH_EVENT_SEP
#endif

#define MCH_EVENT_JOIN_MACRO(X, Y)	MCH_EVENT_DO_JOIN(X, Y)
#define MCH_EVENT_DO_JOIN(X, Y)		MCH_EVENT_DO_JOIN2(X, Y)
#define MCH_EVENT_DO_JOIN2(X, Y)	X##Y

namespace Mocha
{

#define MCH_EVENT_CLASS_NAME Event
#define MCH_DELEGATE_CLASS_NAME Delegate

	template< typename ReturnType MCH_EVENT_SEP MCH_EVENT_TEMPLATE_PARAMS >

	class MCH_EVENT_CLASS_NAME< ReturnType(MCH_EVENT_TEMPLATE_ARGS) >
		: Noncopyable
	{
		/***** Types *****/
	public:

		typedef MCH_DELEGATE_CLASS_NAME< ReturnType(MCH_EVENT_TEMPLATE_ARGS) > Delegate;	// Event delegate type.


		/***** Public Methods *****/
	public:
		// Empty default constructor.
		MCH_EVENT_CLASS_NAME() {}
		// Empty destructor.
		~MCH_EVENT_CLASS_NAME() {}

        //Move constructor
        //MCH_EVENT_CLASS_NAME(MCH_EVENT_CLASS_NAME&& ctr) /*: mDelegates (std::move(ctr.mDelegates)) */ {}

		// Raise an event with arguments.
		inline void Raise(MCH_EVENT_PARAMS)
		{
			if (mDelegates.size())
			{
				DelegateList::iterator it = mDelegates.begin();

				while (it != mDelegates.end())
				{
					Delegate & d = (*it);
					++it;

					d(MCH_EVENT_ARGS);
				}
			}
		}

		// Raise an event with arguments using the () operator.
		inline void operator () (MCH_EVENT_PARAMS)
		{
			Raise(MCH_EVENT_ARGS);
		}

		// Register a listener function to the event.
		inline void Register(const typename Delegate & delegate)
		{
			mDelegates.push_back(delegate);
		}

		// Increment operator used to register a listener function to the event.
		inline void operator += (const typename Delegate & delegate)
		{
			Register(delegate);
		}

		// Unregister a listener function from the event.
		inline void Unregister(const typename Delegate & delegate)
		{		
			mDelegates.remove(delegate);
		}

		// Decrement operator used to unregister a listener function from the event.
		inline void operator -= (const typename Delegate & delegate)
		{
			Unregister(delegate);
		}

		// Clears all registered event listeners.
		inline void Clear(void)
		{		
			mDelegates.clear();
		}

        // Clears all registered event listeners.
        inline unsigned Size(void)
        {
            return mDelegates.size();
        }


		/***** Static Public Methods *****/
	public:

		// Wrapper function for getting a function delegate.
		template< ReturnType(*OMethod)(MCH_EVENT_TEMPLATE_ARGS) >
		inline static typename Delegate Function()
		{
			return Delegate::FromFunction< OMethod >();
		}

		// Wrapper function for getting a class method delegate.
		template< class O, ReturnType(O::*OMethod)(MCH_EVENT_TEMPLATE_ARGS) >
		inline static typename Delegate Method(O * objectPtr)
		{
			return Delegate::FromMethod< O, OMethod >(objectPtr);
		}
		// Wrapper function for getting a class const method delegate.
		template< class O, ReturnType(O::*OMethod)(MCH_EVENT_TEMPLATE_ARGS) const >
		inline static typename Delegate CMethod(const O * objectPtr)
		{
			return Delegate::FromConstMethod< O, OMethod >(objectPtr);
		}

		/***** Internal Types *****/
	private:
		typedef std::list< typename Delegate >	DelegateList;	// Event delegate list.

		/***** Private Members *****/
	private:
		typename DelegateList					mDelegates;		// Event delegates.
		
	};
}

#undef MCH_EVENT_PARAM_COUNT
#undef MCH_EVENT_SEP
#undef MCH_EVENT_JOIN_MACRO
#undef MCH_EVENT_DO_JOIN
#undef MCH_EVENT_DO_JOIN2
#undef MCH_EVENT_CLASS_NAME
#undef MCH_DELEGATE_CLASS_NAME