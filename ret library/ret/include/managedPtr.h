#pragma once
#include "retHeader.h"

/* 
	Abstractions of STL containers for safe DLL exporting
		
		- Manually allocates the underlying type on construction and removes it on destruction
		- Only this pointer can own this object, raw pointer may be obtained but deletion is undefined
		- Safe for DLL export and general use
		- Copy/Assignment is considered a "transfer of ownership"
*/

template <class T>
class RETAPI managedPtr
{
	T* ptr;
public:
	template <typename ...Args>
	managedPtr( Args&... args ) : ptr(new T(std::forward<Args>(args)...)) { }
	~managedPtr( ) { if (ptr) {delete ptr;}}

	managedPtr(managedPtr& p) { ptr = p.ptr; p.ptr = 0; }

	T& operator*( ) { return *ptr; }
	T* operator->( ) { return ptr; }
	T* operator()( ) { return ptr; }		//raw pointer retrieval
};
