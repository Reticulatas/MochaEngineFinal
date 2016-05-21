/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once

#include <malloc.h>
#include <vector>
#include <cstring>
#include <limits>
#include "Smart_pointer.h"

enum MemoryArena { MEMORY_ARENA_STACK, MEMORY_ARENA_HEAP };

// FixedSizeAllocator
//---------------------------------------------------------------------------

class FixedSizeAllocator
{
public:

	static const unsigned char PATTERN_ALIGNMENT = 0xFC;
	static const unsigned char PATTERN_ALLOCATED = 0xFD;
	static const unsigned char PATTERN_FREE = 0xFE;

	FixedSizeAllocator(unsigned dataSize = 4, unsigned pageSize = 8192, MemoryArena arena = MEMORY_ARENA_HEAP, unsigned alignment = sizeof(unsigned));
	~FixedSizeAllocator(void);

	void Reset(unsigned dataSize, unsigned pageSize = 8192, MemoryArena arena = MEMORY_ARENA_HEAP, unsigned alignment = sizeof(unsigned));

	void *Allocate(void);
	void  Free(void *p);
	void  FreeAll(void);

	unsigned DataSize(void) const;
	unsigned PageSize(void) const;
	unsigned AlignmentSize(void) const;
	unsigned BlockSize(void) const;
	unsigned BlocksPerPage(void) const;
	unsigned NumPages(void) const;
	unsigned NumBlocks(void) const;
	unsigned NumFreeBlocks(void) const;

private:

	struct BlockHeader
	{
		BlockHeader *Next; // union-ed with data
	};

	struct PageHeader
	{
		PageHeader  *Next; // followed by blocks in this page
		BlockHeader *Blocks(void) { return reinterpret_cast<BlockHeader *>(this + 1); }
	};

	void FillFreePage(PageHeader  *p);
	void FillFreeBlock(BlockHeader *p);
	void FillAllocatedBlock(BlockHeader *p);

	BlockHeader *NextBlock(BlockHeader *p);

	void *AllocatePage(void);
	void FreePage(void *);

	MemoryArena m_arena;

	PageHeader  *m_pageList;
	BlockHeader *m_freeList;

	unsigned m_dataSize;
	unsigned m_pageSize;
	unsigned m_alignmentSize;
	unsigned m_blockSize;
	unsigned m_blocksPerPage;
	unsigned m_numPages;
	unsigned m_numBlocks;
	unsigned m_numFreeBlocks;

	// disable copy & assignment
	FixedSizeAllocator(const FixedSizeAllocator &clone);
	FixedSizeAllocator &operator=(const FixedSizeAllocator &rhs);
};

// for static definitions
template <typename T, unsigned pageSize = 8192, MemoryArena arena = MemoryArena::HEAP, unsigned alignment = 4>
class BlockAllocator : public FixedSizeAllocator
{
public:
	BlockAllocator(void)
		: FixedSizeAllocator(sizeof(T), pageSize, arena, alignment)
	{ }
};

//---------------------------------------------------------------------------
// end of FixedSizeAllocator


// Allocator
//---------------------------------------------------------------------------

class Allocator
{
public:

	Allocator(unsigned pageSize = 8192, MemoryArena arena = MEMORY_ARENA_HEAP, unsigned alignment = sizeof(unsigned));

	void *Allocate(unsigned size);
	void Free(void *ptr, unsigned size);

private:

	const unsigned m_pageSize;
	const MemoryArena m_arena;
	const unsigned m_alignment;

	std::vector<shared_ptr_phy<FixedSizeAllocator>> m_allocators;

	static unsigned s_numBlockSizes;
	static unsigned s_maxBlockSize;
	static bool &BlockSizeTableInitialized(void);
	static std::vector<unsigned> &BlockSizes(void);
	static std::vector<unsigned> &BlockSizeLookup(void);
	static void InitBlockSizeTable(void);
};

//---------------------------------------------------------------------------
// end of Allocator


// STL-compatible allocator
//---------------------------------------------------------------------------

template <typename T>
class std_allocator
{
private:

	static Allocator s_allocator;

public:

	typedef T value_type;
	typedef T *pointer;
	typedef T &reference;
	typedef const T *const_pointer;
	typedef const T &const_reference;
	typedef unsigned size_type;
	typedef unsigned difference_type;
	template <typename U> struct rebind { typedef std_allocator<U> other; };

	pointer allocate(unsigned n)
	{
		return reinterpret_cast<pointer>(s_allocator.Allocate(sizeof(T)* n));
	}

	void deallocate(pointer ptr, unsigned n)
	{
		if (ptr)
			s_allocator.Free(ptr, sizeof(T)* n);
	}

	void construct(pointer p, const_reference clone)
	{
		new (p)T(clone);
	}

	void destroy(pointer p)
	{
		p->~T();
	}

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	bool operator==(const std_allocator &rhs) { return true; }
	bool operator!=(const std_allocator &rhs) { return !operator==(rhs); }
};

template <typename T>
Allocator std_allocator<T>::s_allocator;

//---------------------------------------------------------------------------
// end of STL-compatible allocator


