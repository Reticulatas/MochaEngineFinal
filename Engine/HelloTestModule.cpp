#include "stdafx.h"
#include <string>
#include "meta.h"
#include "HelloTestModule.h"

std::string HelloTest::Hello()
{
	return "Hello";
}

meta_define(HelloTest);
//safety