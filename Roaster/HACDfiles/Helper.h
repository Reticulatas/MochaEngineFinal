#pragma once
#include "HACD.h"
#define HEADER_VERSION 100
struct HullHeader
{
	HullHeader(void)
	{
		header[0] = 'H';
		header[1] = 'A';
		header[2] = 'C';
		header[3] = 'D';
		versionNumber = HEADER_VERSION;
		hullCount = 0;
	}
	unsigned char header[4];
	hacd::HaU32		versionNumber;
	hacd::HaU32		hullCount;	// number of convex hulls
};

void saveBin(HACD::HACD_API *gHACD);