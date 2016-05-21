/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include <crtdbg.h>
#include "MemMgr.h"

void* operator new(size_t memSize, const char *fileName, int lineNum)
{
    return MemMgr::getInstance()->Allocate(memSize, fileName , lineNum);
}

void* operator new[](size_t memSize, const char *fileName, int lineNum)
{
    return MemMgr::getInstance()->Allocate(memSize, fileName, lineNum);
}

void operator delete(void* object, const char *fileName, int lineNum)
{
  MemMgr::getInstance()->Free(object);
}

void operator delete[](void* object, const char *fileName, int lineNum)
{
  MemMgr::getInstance()->FreeArray(object);
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change