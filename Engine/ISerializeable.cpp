/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "ISerializeable.h"
#include "Log.h"
#include <vector>
#include <fstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include "InputManager.h"

std::vector<ISerializeable*> iserializables;

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ISerializeable);
//BOOST_CLASS_EXPORT(InputManager);

ISerializeable::ISerializeable( bool automatic, std::string _name, void* _thisPtr ) : willBeAutoSerialized(automatic), name(_name) 
{ 
	iserializables.push_back(this); 
	//load in if this file exists
	if (boost::filesystem::exists(boost::filesystem::absolute(name + "_ISerializable.xml")))
	{
		Load();
	}
	thisPtr =  static_cast<ISerializeable*>(_thisPtr);
}

ISerializeable::ISerializeable()
{
	iserializables.push_back(this);
}

void ISerializeable::Save()
{
#ifndef NOSERIALIZE
	//try
	//{
		std::ofstream ofs(name + "_ISerializable.xml");
		if (!ofs.good())
		{
			Log("Error saving " + name);
			return;
		}
		boost::archive::xml_oarchive oa(ofs);
		ISerializeable* ser = this;
		oa << BOOST_SERIALIZATION_NVP(ser);

		Log("Wrote ISerializeable to file");
	//}
	//catch (std::exception const& e)
	//{
	//	Log("ERROR occurred during saving: " + std::string(e.what()), LogEntry::Error);
	//}
#endif
}

void ISerializeable::Load()
{
	return;
#ifndef NOSERIALIZE
	try
	{
		std::ifstream ifs(name + "_ISerializable.xml");
		if (!ifs.good())
		{
			Log("ERROR: Problem loading " + name, LogEntry::Error);
			return;
		}
		boost::archive::xml_iarchive ia(ifs);

		ISerializeable* thisRef = this;
		ia >> BOOST_SERIALIZATION_NVP(thisPtr);
	}
	catch (std::exception const& e)
	{
		Log("ERROR occurred during loading " + name + ": " + std::string(e.what()), LogEntry::Error);
		return;
	}

#endif
}

ISerializeable::~ISerializeable()
{
	iserializables.erase(std::find(iserializables.begin(), iserializables.end(), this), iserializables.end());
}

void ISerializeable_AtExit( void )
{
	std::vector<ISerializeable*>::iterator iter = iserializables.begin();
	for (; iter != iserializables.end(); ++iter)
		(*iter)->Save();
	iserializables.clear();
}
