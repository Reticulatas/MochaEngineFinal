/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include <map>
#include "ObjectAllocator.h"

class MemMgr
{
public:

  MemMgr();
  ~MemMgr();

  // Template functions
  void* Allocate(size_t Objectsize, const char *fileName, int lineNum);
  void Free(void* Object);
  void FreeArray(void* Object);

  // Gets OA of the closest size, creates a new OA if not enough space
  ObjectAllocator* GetOA(size_t Objectsize);  

  // Creates a new object allocator of size == Objectsize)
  ObjectAllocator* CreateNewOA(size_t Objectsize);

  // Rounds up to the closest power of 2
  size_t Round_Pow2(size_t Objectsize);

  void DumpMemory();      // Dumps memory in use
  void ValidateMemory();  // Checks for corrupted blocks

  static MemMgr* getInstance();

private:

  std::map<size_t, ObjectAllocator*> mOAContainer;
  typedef std::map<size_t,ObjectAllocator*>::iterator OAIter;
};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change