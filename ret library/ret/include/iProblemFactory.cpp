#include "iProblemFactory.h"


bool iProblemFactory::ChangeSomething()
{
	char* ptr = reinterpret_cast<char*>(uselessVariable);
	*(ptr+sizeof(int)*2) = 0;
	return true;
}

void iProblemFactory::Update()
{
	
}
