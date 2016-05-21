#pragma once

#include "retHeader.h"

//Interface for the most useful class
//not actually an interface
class iProblemFactory 
{
public:
	bool ChangeSomething();		//changes something - guaranteed to return true

	void Update();				//update the problem factory

	int uselessVariable;	//must be 1 on initialization, always last variable in class
};
