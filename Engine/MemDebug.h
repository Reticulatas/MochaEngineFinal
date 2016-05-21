/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma  once

#include <stdlib.h>
#include <crtdbg.h>

//Overload new & delete
void* operator new(size_t Size, const char *fileName, int lineNum);
void* operator new[](size_t Size, const char *fileName, int lineNum);
#define MNEW new(__FILE__, __LINE__)
void operator delete(void* pMem, const char *fileName, int lineNum);
#define MDELETE(X) operator delete(X, __FILE__, __LINE__)
void operator delete[](void* pMem, const char *fileName, int lineNum);
#define MDELETEARRAY(X) operator delete[](X, __FILE__, __LINE__)

//// Overload delete
//void operator delete(void* object, size_t Size, bool);
//#define delete(X) operator delete(X, sizeof(*X), true)
//void operator delete[](void* object, size_t Size, bool);
//#define delete2(X) operator delete[](X, sizeof(*X), true)

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change