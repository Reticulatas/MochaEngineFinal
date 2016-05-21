#pragma once
#include <string>
#include "def.h"

struct HelloTest : public IMeta
{
	std::string Hello();
	int h;
	HelloTest() : h(10) { }

	metadef(HelloTest)
		m_add(int, h)
		endmetadef;
};