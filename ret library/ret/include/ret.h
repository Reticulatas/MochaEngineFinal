/*

	The ret(iculated) library is an effort by Nicholas Fuller 
	to provide a useful interface of common abstractions for
	game development
	Website/Blog: www.Reticulated.me

*/
/*
	This is the only file which should be included in an external project.
*/

#include "retHeader.h"

namespace ret
{
#include "SlotTable.h"
#include "ISlottable.h"
#include "refptr.h"
#include "ownerPtr.h"
#include "managedPtr.h"
#include "iMsgQueue.h"
}

//End