#pragma once
#include <string>

 
class EngineExt : public IRegisterable
{

public:

   static bool IsFullScreen();
   static void ToggleFullScreen();
   static void ChangeResolution(unsigned width, unsigned height);

};
 