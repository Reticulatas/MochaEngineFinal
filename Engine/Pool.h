/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
template <typename T, unsigned pageSize = 8192, MemoryArena arena = MemoryArena::MEMORY_ARENA_HEAP, unsigned alignment = 4>
class Pooled
{
private:

	static BlockAllocator<T, pageSize, arena, alignment> s_allocator;

public:

	void *operator new(unsigned size)
	{
		return s_allocator.Allocate();
	}

	void operator delete(void *ptr)
	{
		s_allocator.Free(ptr);
	}
};

template <typename T, unsigned pageSize, MemoryArena arena, unsigned alignment>
BlockAllocator<T, pageSize, arena, alignment> Pooled<T, pageSize, arena, alignment>::s_allocator;