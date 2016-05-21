#include "stdafx.h"
#include "SubloadStateVisitor.h"
#include "SubloadState.h"
#include "Config.h"
 

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
//	

SubloadStateVisitor::SubloadStateVisitor()
{
	subloadState = new SubloadState();
	GSM::getInstance()->PushState(subloadState);
}

SubloadStateVisitor::~SubloadStateVisitor()
{
	if (subloadState)
		GSM::getInstance()->RemoveState(subloadState);
}
