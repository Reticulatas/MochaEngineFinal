/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "ObjectAllocator.h"

/******************************************************************************/
/*!

  \brief
    Creates a stack of size ObjectSize and initializes private data.

  \param ObjectSize 
    Size of an object.

  \param config
    Config option of the ObjectAllocator

*/
/******************************************************************************/
ObjectAllocator::ObjectAllocator(size_t ObjectSize) throw(OAException)
{
    /* Initialize page and free list to NULL */
  mPageList = NULL;
  mFreeList = NULL;

  /* Initialize mConfig and mStats */
  mStats.ObjectSize_ = ObjectSize;

  unsigned padbytes = 4;
  mConfig = OAConfig(false, DEFAULT_OBJECTS_PER_PAGE, DEFAULT_MAX_PAGES, true,
                     padbytes, OAConfig::HeaderBlockInfo(OAConfig::hbExtended, 
                     sizeof(unsigned) + sizeof(char*) + sizeof(int)));

  /* Assigns alignment if user defined */
  if(mConfig.Alignment_ > 1)
    SetAlignment();

  /* Calculates page size */
  mStats.PageSize_ = mConfig.ObjectsPerPage_ * (ObjectSize + 
                   2 * mConfig.PadBytes_ + mConfig.HBlockInfo_.size_) + 
                   (mConfig.ObjectsPerPage_ - 1) * mConfig.InterAlignSize_ + 
                   sizeof(char*) + mConfig.LeftAlignSize_;

  /* Calculates an aligned block size */
  mStats.BlockSize_ = mStats.ObjectSize_ + 2 * mConfig.PadBytes_ 
                    + mConfig.InterAlignSize_ + mConfig.HBlockInfo_.size_;

  ++mStats.PagesInUse_;
  mStats.MostObjects_ = mStats.FreeObjects_ = mConfig.ObjectsPerPage_;
  
  /* Allocate memory for a page if OA enabled*/
  if(!mConfig.UseCPPMemManager_)
    mPageList = AllocNewPage();
}
/******************************************************************************/
/*!

  \brief
    Aligns the memory to specified number of bytes.

*/
/******************************************************************************/
void ObjectAllocator::SetAlignment()
{
  /* Calculates left alignment */
  unsigned alignNum =  (mConfig.HBlockInfo_.size_ + mConfig.PadBytes_ + 
                      sizeof(char*)) % mConfig.Alignment_;

  if(alignNum)
    mConfig.LeftAlignSize_ = mConfig.Alignment_ - alignNum;

  /* Calculates inter alignment */
  alignNum = (mConfig.HBlockInfo_.size_ + mConfig.PadBytes_ + 
            mConfig.PadBytes_ + mStats.ObjectSize_) % mConfig.Alignment_;

  if(alignNum)
    mConfig.InterAlignSize_ = mConfig.Alignment_ - alignNum;
}
/******************************************************************************/
/*!

  \brief
    Deletes all the memory allocated.

*/
/******************************************************************************/
ObjectAllocator::~ObjectAllocator() throw()
{
  /* Loops through all the pages and deletes memory */
  if(!mConfig.UseCPPMemManager_)
  {
    while(mStats.PagesInUse_--)
    {
      mFreeList = mPageList;

      GenericObject* pGO = reinterpret_cast<GenericObject*>(mPageList);

      if(pGO->Next)
        mPageList = reinterpret_cast<char*>(pGO->Next);

      delete [] mFreeList;
    }
  }
}
/******************************************************************************/
/*!

  \brief
    Takes an object from the free list and give it to the client.
    Throws an exception if the object can't be allocated.

  \param label 
    Label of the object

  \exception OAException
    text message indicating the reason for the exception

*/
/******************************************************************************/
void * ObjectAllocator::Allocate(const char *fileName, int lineNum) throw(OAException)
{
  /* If OA is enabled */
  if(!mConfig.UseCPPMemManager_)
  {
    /* Checks if there are free blocks */
    if(mStats.ObjectsInUse_ < mConfig.ObjectsPerPage_ * mStats.PagesInUse_)
    {
      return AllocNewBlock(fileName, lineNum);
    }

    /* Checks if there are enough pages */
    else if(mStats.PagesInUse_ < mConfig.MaxPages_ || !mConfig.DebugOn_)
    {
      char* newPage = AllocNewPage();

      /* Links pages together */
      GenericObject* pGO = reinterpret_cast<GenericObject*>(newPage);
      pGO->Next = reinterpret_cast<GenericObject*>(mPageList);
      mPageList = newPage;

      /* Increment pages in use and free objects */
      ++mStats.PagesInUse_;
      mStats.FreeObjects_ += mConfig.ObjectsPerPage_ ;

      return AllocNewBlock(fileName, lineNum);
    }

    /* Throws exception if debug enabled and not enough memory */
    if(mConfig.DebugOn_)
      throw OAException(OAException::E_NO_PAGES, 
                        "allocate_new_page: No system memory available.");
  }

  /* If OA is disabled */
  else
  {
      /* use new directly */
      char* newAlloc;

      try
      {
          newAlloc = new char[mStats.ObjectSize_];
      }
      catch(std::bad_alloc)
      {
          throw OAException(OAException::E_NO_MEMORY,
              "allocate_new_block: No system memory available.");
      }

      ++mStats.Allocations_;
      ++mStats.ObjectsInUse_;

      if(mStats.ObjectsInUse_ > mStats.MostObjects_)
          mStats.MostObjects_ = mStats.ObjectsInUse_;

      return newAlloc;
  }

  return NULL;
}
/******************************************************************************/
/*!

  \brief
    Assigns a free block for the user.

  \return
    Pointer to the free block.

*/
/******************************************************************************/
char* ObjectAllocator::AllocNewBlock(const char *fileName, int line)
{
  /* Modify relevant stats */
  ++mStats.ObjectsInUse_;
  ++mStats.Allocations_;
  --mStats.FreeObjects_;

  if(mStats.ObjectsInUse_ > mStats.MostObjects_)
      mStats.MostObjects_ = mStats.ObjectsInUse_;

  char* newAlloc = mFreeList;

  /* Reassign free list */
  GenericObject* pGO = reinterpret_cast<GenericObject*>(mFreeList);
  mFreeList = reinterpret_cast<char*>(pGO->Next);

  /* Set allocated pattern to object */
  memset(newAlloc, ALLOCATED_PATTERN, mStats.ObjectSize_);

  /* Basic Header */
  if(mConfig.HBlockInfo_.type_ == OAConfig::hbBasic || 
     mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
  {
    memset(newAlloc - sizeof(char) - mConfig.PadBytes_,1, sizeof(char));

    unsigned* allocNum = reinterpret_cast<unsigned*>(newAlloc 
                       - mConfig.BASIC_HEADER_SIZE 
               - mConfig.PadBytes_);

    *allocNum = mStats.Allocations_;

    /* Extended Header */
    if(mConfig.HBlockInfo_.type_ == OAConfig::hbExtended)
    {
      short* useCounter = reinterpret_cast<short*>(newAlloc 
                          - mConfig.HBlockInfo_.size_ 
                          - mConfig.PadBytes_ 
                          + mConfig.HBlockInfo_.additional_);

      ++*useCounter;

      // Assign object size in header block
      unsigned* size = reinterpret_cast<unsigned*>(newAlloc 
        - mConfig.HBlockInfo_.size_ 
        - mConfig.PadBytes_ );

      *size = mStats.ObjectSize_;

      // Assign filename location memory was allocated
      char** file = reinterpret_cast<char**>(newAlloc 
                   - mConfig.HBlockInfo_.size_ 
                   - mConfig.PadBytes_ + sizeof(unsigned));

      *file = const_cast<char*>(fileName);

      int* lineNum = reinterpret_cast<int*>(newAlloc 
                    - mConfig.HBlockInfo_.size_ 
                    - mConfig.PadBytes_
                    + sizeof(unsigned)
                    + sizeof(char*));

      *lineNum = line;
    }
  }
  /* External Header */
  else if(mConfig.HBlockInfo_.type_ == OAConfig::hbExternal)
  {
    MemBlockInfo** extHeader = reinterpret_cast<MemBlockInfo**>(newAlloc
                               - mConfig.EXTERNAL_HEADER_SIZE
                               - mConfig.PadBytes_);

    *extHeader = new MemBlockInfo;
    (*extHeader)->in_use = true;
    (*extHeader)->alloc_num = mStats.Allocations_;
    //(*extHeader)->label = const_cast<char*>(label);
  }

  return newAlloc;
}
/******************************************************************************/
/*!

  \brief
    Returns an object to the free list for the client.
    Throws an exception if the the object can't be freed.

  \param Object 
    The object to be freed

  \return
    Pointer to allocated block

  \exception OAException
    text message indicating the reason for the exception

*/
/******************************************************************************/
void ObjectAllocator::Free( void *Object ) throw(OAException)
{
  /* If OA is enabled */
  if(!mConfig.UseCPPMemManager_)
  {
    GenericObject* pGO = reinterpret_cast<GenericObject*>(Object);

    /* Debug checks if enabled */
    if(mConfig.DebugOn_)
    {
      if(IsOnFreeList(pGO))
        throw OAException(OAException::E_MULTIPLE_FREE,
                          "FreeObject: Object has already been freed.");

      else if(IsOnBadBoundary(pGO))
        throw OAException(OAException::E_BAD_BOUNDARY,
                          "FreeObject: Object has bad boundary.");

      else if(HasCorruptedBlock(pGO))
        throw OAException(OAException::E_CORRUPTED_BLOCK,
                          "FreeObject: Object has a corrupted block.");
    }

    /* Using Headers */
    if(mConfig.HBlockInfo_.size_)
    {
      /* External Header */
      if(mConfig.HBlockInfo_.type_ ==  OAConfig::hbExternal)
      {
        /* Move to MemBlockInfo location and deletes it */
        char* pChar = reinterpret_cast<char*>(pGO) - mConfig.PadBytes_ 
          - mConfig.HBlockInfo_.size_;

        MemBlockInfo** extHeader = reinterpret_cast<MemBlockInfo**>
          (pChar);
        delete(*extHeader);

        /* Reset header blocks to 0 */
        memset(pChar, 0, mConfig.HBlockInfo_.size_);
      }

      /* Basic or extended header */
      else
      {
        /* Reset header blocks to 0 */
        memset(reinterpret_cast<char*>(pGO) - sizeof(char) - 
          mConfig.PadBytes_, 0, sizeof(char));

        /* Reset allocations to 0 */
        unsigned* allocNum = reinterpret_cast<unsigned*> 
          (reinterpret_cast<char*>(pGO) 
          - mConfig.BASIC_HEADER_SIZE 
          - mConfig.PadBytes_);

        *allocNum = 0;

        if(mConfig.HBlockInfo_.type_ ==  OAConfig::hbExtended)
        {
          unsigned* size = reinterpret_cast<unsigned*>
                        (reinterpret_cast<char*>(pGO) 
                        - mConfig.HBlockInfo_.size_ 
                        - mConfig.PadBytes_ );

          *size = 0;

          char** file = reinterpret_cast<char**>
                        (reinterpret_cast<char*>(pGO) 
                        - mConfig.HBlockInfo_.size_ 
                        - mConfig.PadBytes_ + sizeof(unsigned));

          *file = 0;

          int* lineNum = reinterpret_cast<int*>
                         (reinterpret_cast<char*>(pGO)
                         - mConfig.HBlockInfo_.size_ 
                         - mConfig.PadBytes_
                         + sizeof(unsigned)
                         + sizeof(char*));

          *lineNum = 0;
        }
      }
    }

    /* Sets Freed Pattern to object */
    memset(pGO, FREED_PATTERN, mStats.ObjectSize_);

    /* Links the free list */
    if(!mFreeList)
    {
      pGO->Next = NULL;
      mFreeList = reinterpret_cast<char*>(pGO);
    }

    else
    {
      pGO->Next = reinterpret_cast<GenericObject*>(mFreeList);
      mFreeList = reinterpret_cast<char*>(Object);
    }

    /* Modify relevant stats */
    ++mStats.FreeObjects_;
    ++mStats.Deallocations_;
    --mStats.ObjectsInUse_;
  }

  /* If OA is disabled */
  else
  {
        delete [] reinterpret_cast<char*>(Object);
        ++mStats.Deallocations_;
  }
}
/******************************************************************************/
/*!

  \brief
    Calls the callback fn for each block still in use.

  \param fn 
    The callback function.

  \return 
    The number of objects that are still in use.

*/
/******************************************************************************/
unsigned ObjectAllocator::DumpMemoryInUse( )
{
  if(mStats.ObjectsInUse_)
  {
    GenericObject* pageList = reinterpret_cast<GenericObject*>(mPageList);
    
    int numPages = mStats.PagesInUse_;

    while(numPages--)
    {
      /* first object's location on a page */
      unsigned char* pChar = reinterpret_cast<unsigned char*>(pageList) 
                           + sizeof(char*) + mConfig.LeftAlignSize_ 
                           + mConfig.HBlockInfo_.size_ 
                           + mConfig.PadBytes_;

      for(unsigned i = 0; i < mConfig.ObjectsPerPage_; ++i)
      {
        /* If object is in use, dump memory */
        if( *(pChar -  mConfig.PadBytes_ - 1) )
          DumpCallback(pChar, mStats.ObjectSize_);

        if(i + 1 < mConfig.ObjectsPerPage_)
          pChar += mStats.BlockSize_;
      }

      pageList = pageList->Next;
    }

    return mStats.ObjectsInUse_;
  }

  else
    return mStats.ObjectsInUse_;
}
/******************************************************************************/
/*!

  \brief
    Calls the callback fn for each block that is potentially corrupted

  \param fn 
    The callback function.

  \return 
    The number of corrupted pages.

*/
/******************************************************************************/
unsigned ObjectAllocator::ValidatePages( )
{
  /* corrupt counter */
  int corruptCount = 0,
      numPages = mStats.PagesInUse_;

  GenericObject* pageList = reinterpret_cast<GenericObject*>(mPageList);

  /* Check all pages */
  while(numPages--)
  {
    /* first object's location on a page */
    unsigned char* pChar = reinterpret_cast<unsigned char*>(pageList) 
                         + sizeof(char*) + mConfig.LeftAlignSize_ 
                         + mConfig.HBlockInfo_.size_;

    /* Check all objects */
    for(unsigned i = 0; i < mConfig.ObjectsPerPage_; ++i)
    {
      /* Check all pad bytes */
      for(unsigned j = 0; j < mConfig.PadBytes_; ++j)
      {
          /* Callback if pad is corrupted */
          if(pChar[j] != PAD_PATTERN || 
             pChar[j + mStats.ObjectSize_ + mConfig.PadBytes_] 
             != PAD_PATTERN)
          {
              ++corruptCount;
              ValidateCallback(pChar, mStats.ObjectSize_);
              break;
          }
      }

      if(i + 1 < mConfig.ObjectsPerPage_)
        pChar += mStats.BlockSize_;
    }

    pageList = pageList->Next;
  }

  return corruptCount;
}
/******************************************************************************/
/*!

  \brief
    Frees all empty pages

  \return 
    The number of pages freed.

*/
/******************************************************************************/
unsigned ObjectAllocator::FreeEmptyPages( void )
{
  int pagesFreed = 0,
      numPages = mStats.PagesInUse_;

  GenericObject* pageList = reinterpret_cast<GenericObject*>(mPageList);
  GenericObject* prevList = NULL;;
  char* toDelete = NULL;

  /* Check all pages */
  while(numPages--)
  {
    /* first object's location on a page */
    unsigned char* pChar = reinterpret_cast<unsigned char*>(pageList)
                         + 2 * sizeof(char*) + mConfig.LeftAlignSize_
                         + mConfig.HBlockInfo_.size_ + mConfig.PadBytes_;

    /* Check all objects */
    for(unsigned i = 0; i < mConfig.ObjectsPerPage_; ++i)
    {
      /* Checks if page is free */
      if(*pChar == FREED_PATTERN || *pChar == UNALLOCATED_PATTERN)
      {
        if(i + 1 < mConfig.ObjectsPerPage_)
          pChar += mStats.BlockSize_;

        /* If whole page is free, assign for deletion */
        if(i + 1 == mConfig.ObjectsPerPage_)
          toDelete = reinterpret_cast<char*>(pageList);
      }
      /* if object in use, stop checking */
      else
        break;
    }

    /* if there is a page to delete */
    if(toDelete)
    {
      /* Link pages */
      if(prevList)
        prevList->Next = pageList->Next;
      else
        mPageList = reinterpret_cast<char*>(pageList->Next);

      pageList = pageList->Next;

      ReassignFreeList(reinterpret_cast<GenericObject*>(toDelete 
                     + sizeof(char*) + mConfig.PadBytes_ 
                     + mConfig.LeftAlignSize_ 
                     + mConfig.HBlockInfo_.size_));

      delete [] toDelete;
      toDelete = NULL;

      /* Modify relevant stats */
      ++pagesFreed;
      --mStats.PagesInUse_;
      mStats.FreeObjects_ -= mConfig.ObjectsPerPage_;
    }
    /* if no page to delete */
    else
    {
      /* move to next page */
      prevList = pageList;
      pageList = pageList->Next;
    }
  }

  return pagesFreed;
}
/******************************************************************************/
/*!

  \brief
    Enabled or disables Testing/Debugging/Statistic methods.

  \param State 
    Enabled if true or disabled if false.

*/
/******************************************************************************/
void ObjectAllocator::SetDebugState( bool State )
{
  mConfig.DebugOn_ = State;
}
/******************************************************************************/
/*!

  \brief
    Gets the free list.
  
  \return 
    The free list.

*/
/******************************************************************************/
const void * ObjectAllocator::GetFreeList( void ) const
{
  return mFreeList;
}
/******************************************************************************/
/*!

  \brief
    Gets the page list.
  
  \return 
    The page list.

*/
/******************************************************************************/
const void * ObjectAllocator::GetPageList( void ) const
{
  return mPageList;
}
/******************************************************************************/
/*!

  \brief
    Gets config option.
  
  \return 
    The config.

*/
/******************************************************************************/
OAConfig ObjectAllocator::GetConfig( void ) const
{
  return mConfig;
}
/******************************************************************************/
/*!

  \brief
    Gets stats of ObjectAllocator.

  \return
    The stats.

*/
/******************************************************************************/
OAStats ObjectAllocator::GetStats( void ) const
{
  return mStats;
}
/******************************************************************************/
/*!

  \brief
    Reassigns the free list before deleting a page.

  \param pGO
    Pointer to the first object in the page to delete.

*/
/******************************************************************************/
void ObjectAllocator::ReassignFreeList(GenericObject* pGO)
{
  GenericObject* freeList = reinterpret_cast<GenericObject*>(mFreeList);
  int numObj = mConfig.ObjectsPerPage_;

  /* Checks all objects */
  while(numObj--)
  {
    while(freeList)
    {
      /* Finds the objects in the page to delete and
         removes them from the free list */
      if(freeList == pGO)
      {
        freeList = freeList->Next;
        mFreeList = reinterpret_cast<char*>(freeList);
        break;
      }

      else if(freeList->Next == pGO)
      {
        freeList->Next = freeList->Next->Next;
        break;
      }

      freeList = freeList->Next;
    }

    if(numObj)
        pGO = reinterpret_cast<GenericObject*>(reinterpret_cast<char*>(pGO) 
                                               + mStats.BlockSize_);

    freeList = reinterpret_cast<GenericObject*>(mFreeList);
  }
}
/******************************************************************************/
/*!

  \brief
    Checks for double free.

  \param pGO
    Object to check.

  \return
    True if fails the check, false if it passes.

*/
/******************************************************************************/
bool ObjectAllocator::IsOnFreeList( GenericObject* pGO)
{
  if(mFreeList)
  {
    GenericObject* freeGO = reinterpret_cast<GenericObject*>(mFreeList);

    while(freeGO)
    {
      if(freeGO == pGO)
        return true;

      freeGO = freeGO->Next;
    }
  }

  return false;
}
/******************************************************************************/
/*!

  \brief
    Checks if object to free is on bad boundary.

  \param pGO
    Object to check.

  \return
    True if fails the check, false if it passes.

*/
/******************************************************************************/
bool ObjectAllocator::IsOnBadBoundary(GenericObject* pGO)
{
  int numPages = mStats.PagesInUse_;

  GenericObject* pageList = reinterpret_cast<GenericObject*>(mPageList);

  /* first object's location on a page */
  char* pChar = reinterpret_cast<char*>(pGO) - (mConfig.PadBytes_ 
                                      + mConfig.HBlockInfo_.size_ 
                                      + mConfig.LeftAlignSize_ 
                                      + sizeof(char*));

  /* Checks all pages */
  while(numPages--)
  {
    unsigned offset = reinterpret_cast<unsigned>(pChar) 
                    - reinterpret_cast<unsigned>(pageList);

    if(offset <= mStats.PageSize_ - sizeof(char*) 
                                  - mConfig.LeftAlignSize_)
    {
      if(offset % mStats.BlockSize_ || offset 
         == mStats.PageSize_ - sizeof(char*) - mConfig.LeftAlignSize_)
        return true;
    }

    pageList = pageList->Next;
  }
  
  return false;
}
/******************************************************************************/
/*!

  \brief
    Checks if object to free has corrupted block.

  \param pGO
    Object to check.

  \return
    True if fails the check, false if it passes.

*/
/******************************************************************************/
bool ObjectAllocator::HasCorruptedBlock(GenericObject* pGO)
{
  unsigned char* pChar = reinterpret_cast<unsigned char*>(pGO) 
                       - mConfig.PadBytes_;

  /* Check if pad bytes are corrupted */
  for(unsigned i = 0; i < mConfig.PadBytes_; ++i)
  {
    if( *(pChar + i) != PAD_PATTERN || 
        *(pChar + i + mStats.ObjectSize_ + mConfig.PadBytes_) 
          != PAD_PATTERN)
      return true;
  }

  return false;
}
/******************************************************************************/
/*!

  \brief
    Allocates a new page of memory.

  \return
    The newly allocated page.

*/
/******************************************************************************/
char* ObjectAllocator::AllocNewPage()
{
  /* Allocates memory for a page */
  char* newPage;

  try
  {
    newPage = new char[mStats.PageSize_];
  }
  catch(std::bad_alloc &)
  {
    throw OAException(OAException::E_NO_MEMORY,
                    "allocate_new_page: No system memory available.");
  }

  /* Sets next pointer to zero */
  GenericObject* pGO = reinterpret_cast<GenericObject*>(newPage);
  pGO->Next = NULL;

  char* pList = newPage + sizeof(char*);

  /* Left Alignment */
  memset(pList, ALIGN_PATTERN, mConfig.LeftAlignSize_);
  pList += mConfig.LeftAlignSize_;

  for(unsigned i = 0; i < mConfig.ObjectsPerPage_; ++i)
  {
    /* Header */
    memset(pList, 0, mConfig.HBlockInfo_.size_);
    pList += mConfig.HBlockInfo_.size_;

    /* Padding */
    memset(pList, PAD_PATTERN, mConfig.PadBytes_);
    pList += mConfig.PadBytes_;

    /* Data */
    memset(pList, UNALLOCATED_PATTERN, mStats.ObjectSize_);
    pGO = reinterpret_cast<GenericObject*>(pList);

    if(!mFreeList)
      pGO->Next = NULL;
    else
      pGO->Next = reinterpret_cast<GenericObject*>(mFreeList);

    mFreeList = reinterpret_cast<char*>(pGO);
    pList += mStats.ObjectSize_;

    /* Padding */
    memset(pList, PAD_PATTERN, mConfig.PadBytes_);
    pList += mConfig.PadBytes_;

    /* Inter Alignment */
    if(i < mConfig.ObjectsPerPage_ - 1)
    {
      memset(pList, ALIGN_PATTERN, mConfig.InterAlignSize_);
      pList += mConfig.InterAlignSize_;
    }
  }

  return newPage;
}

void ObjectAllocator::DumpCallback( const void *block, size_t actual_size )
{
  size_t size = actual_size;
  // limit to 16 bytes
  if (actual_size > 16)
    size = 16;
  
  unsigned char *data = const_cast<unsigned char*>(static_cast<const unsigned char *>(block));

  char** fileName = reinterpret_cast<char**>(data
                    - mConfig.PadBytes_ 
                    - mConfig.HBlockInfo_.size_
                    + sizeof(unsigned));

  int* lineNum = reinterpret_cast<int*>(data
                 - mConfig.PadBytes_ 
                 - mConfig.HBlockInfo_.size_
                 + sizeof(unsigned) + sizeof(char*));

  printf("\n%s(%i)\n", *fileName, *lineNum);

  printf("Block at 0x%p, %u bytes long.\n", block, static_cast<unsigned>(actual_size));

  // If we were passed a NULL pointer, do nothing
  if (!block)
    return;

  printf(" Data: <");
  for (unsigned int i = 0; i < size; i++)
  {
    unsigned char c = *data++;
    if (c > 31 && c < 128) // printable range
      printf("%c", c);
    else
      printf(" ");
  }
  printf(">");

  data = const_cast<unsigned char*>(static_cast<const unsigned char *>(block));
  for (unsigned int i = 0; i < size; i++)
    printf(" %02X", static_cast<int>(*data++));
  printf("\n");
}

void ObjectAllocator::ValidateCallback( const void *block, size_t actual_size )
{
  printf("Block at 0x%p, %u bytes long.\n\n", block, static_cast<unsigned>(actual_size));

  if (!block)
    return;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change