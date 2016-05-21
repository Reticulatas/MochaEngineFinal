#include "stdafx.h"
#include "List_Allocator.h"
#include <malloc.h>
#include <cstring>


unsigned MaxUINT(unsigned lhs, unsigned rhs)
{
	return (lhs > rhs) ? (lhs) : (rhs);
}

// FixedSizeAllocator
//---------------------------------------------------------------------------

FixedSizeAllocator::FixedSizeAllocator(unsigned dataSize, unsigned pageSize, MemoryArena arena, unsigned alignment)
	: m_pageList(nullptr)
	, m_freeList(nullptr)
{
	Reset(dataSize, pageSize, arena, alignment);
}

FixedSizeAllocator::~FixedSizeAllocator(void)
{
	FreeAll();
}

void FixedSizeAllocator::Reset(unsigned dataSize, unsigned pageSize, MemoryArena arena, unsigned alignment)
{
	FreeAll();

	// set memory arena type
	m_arena = arena;

	// order is important!
	//-----------------------------------
	m_dataSize = dataSize;
	m_pageSize = pageSize;
	m_alignmentSize = (MaxUINT(sizeof(BlockHeader), m_dataSize) % alignment) ? (alignment - MaxUINT(sizeof(BlockHeader), m_dataSize) % alignment) : (0);
	m_blockSize = MaxUINT(sizeof(BlockHeader), m_dataSize) + m_alignmentSize;
	m_blocksPerPage = (m_pageSize - sizeof(PageHeader)) / m_blockSize;
	//-----------------------------------
	// order is important!
}

void *FixedSizeAllocator::Allocate(void)
{
	// free list empty, create new page
	if (!m_freeList)
	{
		// allocate new page
		PageHeader *newPage = reinterpret_cast<PageHeader *>(AllocatePage());
		++m_numPages;
		m_numBlocks += m_blocksPerPage;
		m_numFreeBlocks += m_blocksPerPage;

		FillFreePage(newPage);

		// page list not empty, link new page
		if (m_pageList)
		{
			newPage->Next = m_pageList;
		}

		// push new page
		m_pageList = newPage;

		// link new free blocks
		BlockHeader *currBlock = newPage->Blocks();
		for (unsigned i = 0; i < m_blocksPerPage - 1; ++i)
		{
			currBlock->Next = NextBlock(currBlock);
			currBlock = NextBlock(currBlock);
		}
		currBlock->Next = nullptr; // last block

		// push new blocks
		m_freeList = newPage->Blocks();
	}

	// pop free block
	BlockHeader *freeBlock = m_freeList;
	m_freeList = m_freeList->Next;
	--m_numFreeBlocks;

	FillAllocatedBlock(freeBlock);

	return freeBlock;
}

void FixedSizeAllocator::Free(void *p)
{
	if (p)
	{
		// retrieve block header
		BlockHeader *block = reinterpret_cast<BlockHeader *>(p);

		FillFreeBlock(block);

		// push block
		block->Next = m_freeList;
		m_freeList = block;
		++m_numFreeBlocks;
	}
}

void FixedSizeAllocator::FreeAll(void)
{
	// free all pages
	PageHeader *pageWalker = m_pageList;
	while (pageWalker)
	{
		PageHeader *currPage = pageWalker;
		pageWalker = pageWalker->Next;

		FreePage(currPage);
	}

	// release pointers
	m_pageList = nullptr;
	m_freeList = nullptr;

	// re-init stats
	m_numPages = 0;
	m_numBlocks = 0;
	m_numFreeBlocks = 0;
}

unsigned FixedSizeAllocator::DataSize(void) const { return m_dataSize; }
unsigned FixedSizeAllocator::PageSize(void) const { return m_pageSize; }
unsigned FixedSizeAllocator::BlockSize(void) const { return m_blockSize; }
unsigned FixedSizeAllocator::BlocksPerPage(void) const { return m_blocksPerPage; }
unsigned FixedSizeAllocator::NumPages(void) const { return m_numPages; }
unsigned FixedSizeAllocator::NumBlocks(void) const { return m_numBlocks; }
unsigned FixedSizeAllocator::NumFreeBlocks(void) const { return m_numFreeBlocks; }

void FixedSizeAllocator::FillFreePage(PageHeader *p)
{
	// page header
	p->Next = nullptr;

	// blocks
	BlockHeader *currBlock = p->Blocks();
	for (unsigned i = 0; i < m_blocksPerPage; ++i)
	{
		FillFreeBlock(currBlock);
		currBlock = NextBlock(currBlock);
	}
}

void FixedSizeAllocator::FillFreeBlock(BlockHeader *p)
{
	// block header + data
	std::memset
		(
		p,
		PATTERN_FREE,
		m_blockSize - m_alignmentSize
		);

	// alignment
	std::memset
		(
		reinterpret_cast<char *>(p)+m_blockSize - m_alignmentSize,
		PATTERN_ALIGNMENT,
		m_alignmentSize
		);
}

void FixedSizeAllocator::FillAllocatedBlock(BlockHeader *p)
{
	// block header + data
	std::memset
		(
		p,
		PATTERN_ALLOCATED,
		m_blockSize - m_alignmentSize
		);

	// alignment
	std::memset
		(
		reinterpret_cast<char *>(p)+m_blockSize - m_alignmentSize,
		PATTERN_ALIGNMENT,
		m_alignmentSize
		);
}

FixedSizeAllocator::BlockHeader *FixedSizeAllocator::NextBlock(BlockHeader *p)
{
	return
		reinterpret_cast<BlockHeader *>(reinterpret_cast<char *>(p)+m_blockSize);
}

void *FixedSizeAllocator::AllocatePage(void)
{
	switch (m_arena)
	{
	case MEMORY_ARENA_STACK: return _malloca(m_pageSize);
	case MEMORY_ARENA_HEAP: return malloc(m_pageSize);
	}
	return nullptr;
}

void FixedSizeAllocator::FreePage(void *page)
{
	switch (m_arena)
	{
	case MEMORY_ARENA_STACK: _freea(page); break;
	case MEMORY_ARENA_HEAP:  free(page); break;
	}
}

//---------------------------------------------------------------------------
// end of FixedSizeAllocator


// Allocator
//---------------------------------------------------------------------------

unsigned Allocator::s_numBlockSizes;
unsigned Allocator::s_maxBlockSize;

bool &Allocator::BlockSizeTableInitialized(void)
{
	static bool s_blockSizeTableInitialized = false;
	return s_blockSizeTableInitialized;
}

std::vector<unsigned> &Allocator::BlockSizes(void)
{
	static std::vector<unsigned> s_blockSizes;
	return s_blockSizes;
}

std::vector<unsigned> &Allocator::BlockSizeLookup(void)
{
	static std::vector<unsigned> s_blockSizeLookup;
	return s_blockSizeLookup;
}

void Allocator::InitBlockSizeTable(void)
{
	bool &blockSizeTableInitialized = BlockSizeTableInitialized();

	if (blockSizeTableInitialized)
		return;

	// one-time initialization
	static const unsigned s_blockSizesData[] =
	{
		// 4-increments
		4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
		52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96,

		// 32-increments
		128, 160, 192, 224, 256, 288, 320, 352, 384,
		416, 448, 480, 512, 544, 576, 608, 640,

		// 64-increments
		704, 768, 832, 896, 960, 1024
	};
	std::vector<unsigned> &blockSizes = BlockSizes();
	blockSizes.assign(s_blockSizesData, s_blockSizesData + sizeof(s_blockSizesData) / sizeof(s_blockSizesData[0]));
	s_numBlockSizes = blockSizes.size();
	s_maxBlockSize = blockSizes[s_numBlockSizes - 1];

	// initialize block size lookup
	std::vector<unsigned> &blockSizeLookup = BlockSizeLookup();
	blockSizeLookup.resize(s_maxBlockSize + 1);
	unsigned j = 0;
	for (unsigned i = 0; i <= s_maxBlockSize; ++i)
	{
		if (i > blockSizes[j])
			++j;

		blockSizeLookup[i] = j;
	}

	blockSizeTableInitialized = true;
}

Allocator::Allocator(unsigned pageSize, MemoryArena arena, unsigned alignment)
	: m_pageSize(pageSize)
	, m_arena(arena)
	, m_alignment(alignment)
{
	InitBlockSizeTable();
	m_allocators.resize(s_numBlockSizes);
}

void *Allocator::Allocate(unsigned size)
{
	if (size <= s_maxBlockSize)
	{
		std::vector<unsigned> &blockSizeLookup = BlockSizeLookup();
		unsigned index = blockSizeLookup[size];
		shared_ptr_phy<FixedSizeAllocator> &allocator = m_allocators[index];
		if (!allocator)
			allocator.reset(new FixedSizeAllocator(BlockSizes()[index], m_pageSize, m_arena, m_alignment));

		return allocator->Allocate();
	}
	else
		return malloc(size);
}


void Allocator::Free(void *ptr, unsigned size)
{
	if (ptr && size <= s_maxBlockSize)
	{
		std::vector<unsigned> &blockSizeLookup = BlockSizeLookup();
		unsigned index = blockSizeLookup[size];
		shared_ptr_phy<FixedSizeAllocator> &allocator = m_allocators[index];
		

		allocator->Free(ptr);
	}
	else
		free(ptr);
}

//---------------------------------------------------------------------------
// end of Allocator

