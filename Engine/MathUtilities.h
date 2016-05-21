/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once


//#include <cmath>
#include <cstring>
#include <cstdio>
#include <math.h>
#include <cstdlib>

// Typedefs for consistency
typedef unsigned char      u8;
typedef char               s8;
typedef unsigned int       u32;
typedef int                s32;
typedef unsigned long long u64;
typedef long long          s64;
typedef float              f32;
typedef double             f64;

// Do not change
//static const f32 EPSILON = 0.00001f;
//static const float INFINITY = FLT_MAX;
static const double PI = 3.141592653589793;

static float DegreesToRadian( float degree)
{
  return float(degree * PI / 180);
}


inline unsigned long NextPowerOfTwo(unsigned long a)
{
	a--;
	a |= a >> 1;
	a |= a >> 2;
	a |= a >> 4;
	a |= a >> 8;
	a |= a >> 16;
#if defined _M_X64
	a |= a >> 32;
#endif
	a++;
	return a;
}

template <typename T>
inline T Minimum(T a, T b)
{
	if (a > b)
		return b;
	return a;
}