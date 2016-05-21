/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "MemMgr.h"

MemMgr::MemMgr()
{
}

void* MemMgr::Allocate(size_t Objectsize, const char *fileName, int lineNum)
{
  return GetOA(Objectsize)->Allocate(fileName, lineNum);
}

void MemMgr::Free( void* Object )
{
  OAConfig config = mOAContainer.begin()->second->GetConfig();

  unsigned* key = reinterpret_cast<unsigned*>(
                  reinterpret_cast<char*>(Object)
                  - config.PadBytes_
                  - config.HBlockInfo_.size_);

  GetOA(*key)->Free(Object);
}


void MemMgr::FreeArray( void* Object )
{
  Object = reinterpret_cast<void*>(
    reinterpret_cast<char*>(Object) - sizeof(int));

  OAConfig config = mOAContainer.begin()->second->GetConfig();

  unsigned* key = reinterpret_cast<unsigned*>(
                  reinterpret_cast<char*>(Object)
                  - config.PadBytes_
                  - config.HBlockInfo_.size_);

  GetOA(*key)->Free(Object);
}


ObjectAllocator* MemMgr::GetOA( size_t Objectsize )
{
  Objectsize = Round_Pow2(Objectsize - 1);

  OAIter iter = mOAContainer.find(Objectsize);

  if(iter == mOAContainer.end())
    return CreateNewOA(Objectsize);

  else
    return iter->second;
}

ObjectAllocator* MemMgr::CreateNewOA( size_t Objectsize )
{
  ObjectAllocator* newOA = new ObjectAllocator(Objectsize);
  mOAContainer.insert(std::pair<size_t, ObjectAllocator*>(Objectsize, newOA));
  return newOA;
}


size_t MemMgr::Round_Pow2( size_t Objectsize )
{
  if(Objectsize <= sizeof(void*))
    return 8;

  unsigned power = 0;

  while(Objectsize)
  {
    Objectsize >>= 1;
    ++power;
  }

  return 1 << power;
}

MemMgr* MemMgr::getInstance()
{
  static MemMgr* instance  = 0;

  if(!instance)
    instance = new MemMgr();

  return instance;
}

MemMgr::~MemMgr()
{
  for(OAIter iter = mOAContainer.begin(); iter != mOAContainer.end(); ++iter)
    iter->second->~ObjectAllocator();

  mOAContainer.clear();
}

void MemMgr::DumpMemory()
{
  printf("\nDumping objects ->\n");

  for(OAIter iter = mOAContainer.begin(); iter != mOAContainer.end(); ++iter)
    iter->second->DumpMemoryInUse();

  printf("Object dump complete.\n\n");
}

void MemMgr::ValidateMemory()
{
  for(OAIter iter = mOAContainer.begin(); iter != mOAContainer.end(); ++iter)
    iter->second->ValidatePages();
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change