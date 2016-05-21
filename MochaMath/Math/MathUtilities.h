///////////////////////////////////////////////////////////////////////////////
///
/// \file Precompiled.hpp
/// Precompiled header for the math library.
/// 
/// Authors: Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

// Includes
#include <stdio.h>

#ifdef _MSC_VER
  #define ZERO_DEBUG_BREAK __debugbreak()
#else
  #define ZERO_DEBUG_BREAK
#endif

#if !defined(ZERO_ENABLE_ERROR) 
#   if defined(_DEBUG)
#       define ZERO_ENABLE_ERROR 1
#   else
#       define ZERO_ENABLE_ERROR 0
#   endif
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1600
  #define StaticAssert(name, Expression, error) \
    static_assert(Expression, error)
#else
  #define StaticAssert(name, Expression, error) \
    static int name = (sizeof(char[1 - 2 * !(Expression)]))
#endif

static int gConditionalFalseConstant = 0;

void PrintError(const char* format, ...);

#if ZERO_ENABLE_ERROR

#define UnusedParameter(param) param

#define WarnIf(Expression, ...) \
do { if((Expression)) \
  ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)

#define ErrorIf(Expression, ...) \
do { if((Expression)) \
  ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)

#define Assert(Expression, ...) \
do { if(!(Expression)) \
  ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)

#define Error(...)\
do { ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)

#define Warn(...)\
do { PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)

#define FileErrorIf(Expression, file, Line, ...) \
do { ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)

#define Verify(funccall) ErrorIf(funcall != 0);

#else

#define UnusedParameter(param)
#define WarnIf(...) ((void)0)
#define ErrorIf(...) ((void)0)
#define Assert(...) ((void)0)
#define Error(...) ((void)0)
#define Warn(...) ((void)0)
#define FileErrorIf(...) ((void)0)
#define Verify(funccall) funccall

#endif

#define ReturnIf(Expression , whatToReturn, ...) \
  do { if(Expression) {                          \
    WarnIf(Expression, __VA_ARGS__);             \
    return whatToReturn;                         \
  } } while(gConditionalFalseConstant)

#include <cstddef>
#include <cfloat>
