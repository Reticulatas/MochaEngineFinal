/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Config.h"
#include "boost/filesystem.hpp"
#include "Log.h"
#include "SerializationManager.h"

#define SAVE(var) ofs << ___GETNAME(var) << "\t" << var << std::endl;
#define LOAD(var) ifs.ignore(256,'\t'); ifs >> var;

void ConfigSerializer::Load(void)
{
	try
	{
		if (!boost::filesystem::exists(boost::filesystem::absolute("Config.ini")))
		{
			Log("Config not found, did not load");
			return;
		}
		std::ifstream ifs("Config.ini");
		if (!ifs.good())
		{
			Log("ERROR: Problem loading Config", LogEntry::Error);
			return;
		}

		LOAD(Config::initialLevel);
		LOAD(Config::showConsole);
		LOAD(Config::startFullscreen);
		LOAD(Config::loadPrefabsOnStart);
		LOAD(Config::width);
		LOAD(Config::height);
		LOAD(Config::subloadLevel);
		LOAD(Config::allowBarristaDebugStats);
    LOAD(Config::vrMode);

		ifs.close();
	}
	catch (std::exception const& e)
	{
		Log("ERROR occurred during loading config: " + std::string(e.what()), LogEntry::Error);
		return;
	}
}

void ConfigSerializer::Save(void)
{
	try
	{
		std::ofstream ofs("Config.ini");

		SAVE(Config::initialLevel);
		SAVE(Config::showConsole);
		SAVE(Config::startFullscreen);
		SAVE(Config::loadPrefabsOnStart);
		SAVE(Config::width);
		SAVE(Config::height);
		SAVE(Config::subloadLevel);
		SAVE(Config::allowBarristaDebugStats);
    SAVE(Config::vrMode);

		LogVerbose("Wrote config", LogEntry::Command);

		ofs.close();
	}
	catch (std::exception const& e)
	{
		Log("ERROR occurred during saving config: " + std::string(e.what()), LogEntry::Error);
	}
}

std::string Config::initialLevel;
bool Config::startFullscreen;
bool Config::loadPrefabsOnStart;
bool Config::showConsole;
std::string Config::subloadLevel;
int Config::width;
int Config::height;
bool Config::allowBarristaDebugStats;
bool Config::vrMode;