/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Engine.h"
#include "Config.h"
#include "Console.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	Engine::getInstance()->StartFromConfig();

	return 0;
}

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change