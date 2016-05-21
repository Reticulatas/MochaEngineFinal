/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

//uncomment to remove all serialization routines
//#define NOSERIALIZE

#include <boost/boost/serialization/serialization.hpp>
#include <boost/boost/serialization/vector.hpp>
#include <boost/boost/serialization/list.hpp>
#include <boost/boost/serialization/map.hpp>
#include <boost/boost/serialization/nvp.hpp>
#include <boost/boost/serialization/export.hpp>
#include <fstream>
#include <boost/boost/archive/xml_iarchive.hpp>
#include <boost/boost/archive/xml_oarchive.hpp>