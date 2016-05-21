#include "stdafx.h"
#include "List_Allocator.h"


namespace smart_ptr_internal
{

	__forceinline static FixedSizeAllocator &RefCounterAllocator(void)
	{
		static FixedSizeAllocator s_refCounterAllocator(sizeof(unsigned));
		return s_refCounterAllocator;
	}

	unsigned *newRefCounter(unsigned value)
	{
		return new (RefCounterAllocator().Allocate()) unsigned(value);
	}

	void deleteRefCounter(unsigned *counter)
	{
		RefCounterAllocator().Free(counter);
	}

}
