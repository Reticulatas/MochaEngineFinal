/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"

void MAssert(bool cond, std::string msg)
{
	if (!cond)
		MessageBox(NULL, std::wstring(msg.begin(), msg.end()).c_str(), L"Mocha Assert", MB_OK);
	throw MochaException(msg);
}
