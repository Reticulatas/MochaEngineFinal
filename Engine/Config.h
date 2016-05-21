/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once
#include <string>

struct Config
{
	static std::string initialLevel;
	static bool showConsole;
	static bool startFullscreen;
	static bool loadPrefabsOnStart;
	static std::string subloadLevel;
	static int  width;
	static int  height;
	static bool allowBarristaDebugStats;
  static bool vrMode;
};

class ConfigSerializer
{
public:
	void Load(void);
	void Save(void);
};