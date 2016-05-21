/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once

#include <Windows.h>
#include <string>

#define GV_ASSERT_STRINGIFY(x) #x
#define GV_ASSERT_TO_STRING(x) GV_ASSERT_STRINGIFY(x)

#ifdef _DEBUG
  #define GV_ASSERT(_expression) GV_ASSERT_INNER(_expression, __FILE__, __LINE__)
  #define GV_ASSERT_MESSAGE(_expression, _message) GV_ASSERT_MESSAGE_INNER(_expression, __FILE__, __LINE__, _message)
  #define GV_ASSERT_MESSAGE_NO_BREAK(_expression, _message) GV_ASSERT_INNER_NO_BREAK(_expression, __FILE__, __LINE__, _message)

  __forceinline void AssertHelper(bool assertExpression, const char *assertMessage)
  {
    if (!assertExpression)
    {
      MessageBox(NULL, (LPCTSTR) assertMessage, (LPCTSTR) "Assertion Failed!", MB_OK);
      __asm int 3;
    }
  }
  
  __forceinline void AssertHelperNoBreak(bool assertExpression, const char *assertMessage)
  {
    if (!assertExpression)
    {
      MessageBox(NULL, (LPCTSTR) assertMessage, (LPCTSTR) "Assertion Failed!", MB_OK);
    }
  }

  #define GV_ASSERT_INNER(_expression, _file, _line) \
    AssertHelper \
    ( \
      !!( _expression ), \
      "\n\nFile: " GV_ASSERT_TO_STRING(__FILE__) \
      "\nLine: " GV_ASSERT_TO_STRING(__LINE__) \
      "\nExpression: " #_expression \
    )

  #define GV_ASSERT_INNER_NO_BREAK(_expression, _file, _line, _message) \
    AssertHelperNoBreak \
    ( \
      !!( _expression ), \
      "\n\nFile: " GV_ASSERT_TO_STRING(__FILE__) \
      "\nLine: " GV_ASSERT_TO_STRING(__LINE__) \
      "\nExpression: " #_expression \
    )

  #define GV_ASSERT_MESSAGE_INNER(_expression, _file, _line, _message) \
    AssertHelper \
    ( \
      !!( _expression ), \
      "\n\nFile: " GV_ASSERT_TO_STRING(__FILE__) \
      "\nLine: " GV_ASSERT_TO_STRING(__LINE__) \
      "\nExpression: " #_expression \
      "" \
    )
#else
  #define GV_ASSERT(_expression) 
  #define GV_ASSERT_MESSAGE(_expression, _message) 
  #define GV_ASSERT_MESSAGE_NO_BREAK(_expression, _message)
#endif
