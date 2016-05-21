#pragma once
#include "retHeader.h"

/* 
		- Only this pointer can own this object, raw pointer may be obtained but deletion is undefined
		- Copy/Assignment is NOT considered a "transfer of ownership", there may be only one of these pointer per object
		- Deletion of the pointer delete owned object
*/

template <class T>
class RETAPI ownerPtr
{
	T* ptr;
public:
	ownerPtr( ) : ptr(0) { }
	ownerPtr( T* p ) : ptr(p) { }
	~ownerPtr( ) { if (ptr) {delete ptr;}}

	ownerPtr(const ownerPtr& p) { throw std::exception("No Copy Guarantee"); }

	void operator=(T* p) { ptr = p; }

	T& operator*( ) { return *ptr; }
	T* operator->( ) { return ptr; }
	T* operator()( ) { return ptr; }		//raw pointer retrieval
};
