/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "GenericUtils.h"
#include <list>
#include <string>

void ListLogList(std::list<std::string>& l)
{
	std::list<std::string>::const_iterator iter = l.begin();
	for (; iter != l.end(); ++iter)
		Log(*iter);
}