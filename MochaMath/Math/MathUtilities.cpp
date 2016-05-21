///////////////////////////////////////////////////////////////////////////////
///
/// \file Utilities.hpp
/// Implementation of the utilities.
/// 
/// Authors: Trevor Sundberg
/// Copyright 2010-2013, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////

// Includes
#include "MathUtilities.h"
#include <Windows.h>

#pragma comment(lib, "user32.lib")

void PrintError(const char* format, ...)
{
  va_list vl;
  va_start(vl, format);

  // Change to the error color
  HANDLE console = GetStdHandle(STD_ERROR_HANDLE);
  SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);

  vfprintf(stderr, format, vl);
  
  // Change back to the normal color
  SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  // Only popup the console upon the first error
  static bool firstError = true;
  if (firstError == true)
  {
    // Bring the console window to the front
    HWND hwnd = GetConsoleWindow();
    SetForegroundWindow(hwnd);

    // We won't popup the console anymore (popups with every error gets annoying!)
    firstError = false;
  }

  va_end(vl);
}