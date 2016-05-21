/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "IRegisterable.h"
#include <windows.h>

//-----------------HELPERS--------------------

//-----------------CALLBACKS--------------------

/* Called via the event::unload proc, prompts user for cancellation then kills GUI */
void GUIOnExit()
{
	GSM::getInstance()->PopState();
}

//-----------------HANDLERS---------------------

DWORD WINAPI T_GUIBegin(void* param)
{
	//JuceAccessor::getInstance().StartEditor();
	return 0;
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change