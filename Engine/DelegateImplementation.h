/******************************************************************************/
/*!
File       DelegateImplementation.h
 
 
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
* File : DelegateImplementation.h
* Brief :
* Templated delegate object that can store a function pointer or a member function pointer.
* Reference: http://www.codeproject.com/KB/cpp/ImpossiblyFastCppDelegate.aspx
*/

/* 
*  This ensures when templated delegates are created on compile time, proper commas will be in place
*/

#if MCH_DELEGATE_PARAM_COUNT > 0
#define MCH_DELEGATE_SEP ,
#else
#define MCH_DELEGATE_SEP
#endif

/* Reference: http://stackoverflow.com/questions/1597007/creating-c-macro-with-and-line-token-concatenation-with-positioning-macr
* For why I need all the weird join wrappers
*/

#define MCH_DELEGATE_JOIN_MACRO(X, Y)	MCH_DELEGATE_DO_JOIN(X, Y)
#define MCH_DELEGATE_DO_JOIN(X, Y)		MCH_DELEGATE_DO_JOIN2(X, Y)
#define MCH_DELEGATE_DO_JOIN2(X, Y)		X##Y

namespace Mocha
{

#define MCH_DELEGATE_CLASS_NAME Delegate
	//#define MCH_DELEGATE_CLASS_NAME MCH_DELEGATE_JOIN_MACRO( Delegate, MCH_DELEGATE_PARAM_COUNT )

	template <typename ReturnType MCH_DELEGATE_SEP MCH_DELEGATE_TEMPLATE_PARAMS>

	class MCH_DELEGATE_CLASS_NAME< ReturnType(MCH_DELEGATE_TEMPLATE_ARGS) >
	{
		/***** Types *****/
	public:

		typedef ReturnType(MCH_DELEGATE_CALLTYPE * SignatureType)(MCH_DELEGATE_TEMPLATE_ARGS);


		/***** Public Methods *****/
	public:
		// Get a function delegate.
		template< ReturnType(*TMethod)(MCH_DELEGATE_TEMPLATE_ARGS) >
		static MCH_DELEGATE_CLASS_NAME FromFunction()
		{
			return FromStub(NULL, &FunctionStub< TMethod >);
		}
		// Get a class method delegate.
		template< class T, ReturnType(T::*TMethod)(MCH_DELEGATE_TEMPLATE_ARGS) >
		static MCH_DELEGATE_CLASS_NAME FromMethod(T * objectPtr)
		{
			return FromStub(objectPtr, &MethodStub< T, TMethod >);
		}
		// Get a class const method delegate.
		template< class T, ReturnType(T::*TMethod)(MCH_DELEGATE_TEMPLATE_ARGS) const >
		static MCH_DELEGATE_CLASS_NAME FromConstMethod(T const * objectPtr)
		{
			return FromStub(const_cast< T * >(objectPtr), &ConstMethodStub< T, TMethod >);
		}
		// Bracket operator used to trigger delegate.
		ReturnType operator()(MCH_DELEGATE_PARAMS) const
		{
			return (*mStubPtr)(mObjectPtr MCH_DELEGATE_SEP MCH_DELEGATE_ARGS);
		}
		// Check if delegate is empty.
		operator bool(void) const
		{
			return mStubPtr != NULL;
		}
		// Logical negation operator.
		bool operator ! (void) const
		{
			return !(operator bool());
		}
		// For STL container storage.
		bool operator < (const MCH_DELEGATE_CLASS_NAME & rhs) const
		{
			return (mObjectPtr < rhs.mObjectPtr) && (mStubPtr < rhs.mStubPtr);
		}

		// Compares two delegates if they are the same.
		bool operator == (const MCH_DELEGATE_CLASS_NAME & rhs) const
		{
			return (rhs.mObjectPtr == mObjectPtr) && (rhs.mStubPtr == mStubPtr);
		}

		/***** Internal Types *****/
	private:
		// Function callback type.
		typedef ReturnType(MCH_DELEGATE_CALLTYPE * StubType)(void * objectPtr MCH_DELEGATE_SEP MCH_DELEGATE_PARAMS);

		/***** Private Methods *****/
	private:
		// Constructor.
		MCH_DELEGATE_CLASS_NAME() : mObjectPtr(NULL),
			mStubPtr(NULL) {}
		// Overloaded constructor.
		MCH_DELEGATE_CLASS_NAME(void * objPtr, StubType stubPtr) : mObjectPtr(objPtr),
			mStubPtr(stubPtr) {}
		// Get the delegate 'stub'
		static MCH_DELEGATE_CLASS_NAME FromStub(void * objectPtr, StubType stubType)
		{
			return MCH_DELEGATE_CLASS_NAME(objectPtr, stubType);
		}
		// Call the delegate function 'stub'.
		template< ReturnType(*TMethod)(MCH_DELEGATE_TEMPLATE_ARGS) >
		static ReturnType MCH_FASTCALL FunctionStub(void * MCH_DELEGATE_SEP MCH_DELEGATE_PARAMS)
		{
			return (TMethod)(MCH_DELEGATE_ARGS);
		}
		// Call the delegate method 'stub'.
		template< class T, ReturnType(T::*TMethod)(MCH_DELEGATE_TEMPLATE_ARGS) >
		static ReturnType MCH_DELEGATE_CALLTYPE MethodStub(void * objectPtr MCH_DELEGATE_SEP MCH_DELEGATE_PARAMS)
		{
			T * p = static_cast< T * >(objectPtr);
 			return (p->*TMethod)(MCH_DELEGATE_ARGS);
		}

		// Call the delegate const method 'stub'.
		// TParam :	T			Object type.
		// TParam :	TMethod		Function pointer type.
		// Param : 	stubType	Calling member const function (stub) type.
		// Return :				Returns the member const function's return data.
		template< class T, ReturnType(T::*TMethod)(MCH_DELEGATE_TEMPLATE_ARGS) const >
		static ReturnType MCH_DELEGATE_CALLTYPE ConstMethodStub(void * objectPtr MCH_DELEGATE_SEP MCH_DELEGATE_PARAMS)
		{
			T const * p = static_cast< T * >(objectPtr);
			return (p->*TMethod)(MCH_DELEGATE_ARGS);
		}

		/***** Private Members *****/
	private:
		void	 	* mObjectPtr;	// Object pointer.
		StubType 	  mStubPtr;		// Function mapping pointer.
	};
}

#undef MCH_DELEGATE_SEP
#undef MCH_DELEGATE_JOIN_MACRO
#undef MCH_DELEGATE_DO_JOIN
#undef MCH_DELEGATE_DO_JOIN2
#undef MCH_DELEGATE_CLASS_NAME