#pragma once
#include "stdafx.h"
#include <string>
#include <boost/python.hpp>
#include "def_bind.h"
#include "bindingfunctors.h"

BOOST_PYTHON_MODULE(pyMocha)
{
	using namespace boost::python;
	CreateBinds();
}

//safety comment