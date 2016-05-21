#include "Helper.h"
void saveBin(HACD::HACD_API *gHACD)
{
	printf("Saving ConvexHulls to 'ConvexDecomposition.bin'\r\n");

	hacd::HaU32 hullCount = gHACD->getHullCount();
	printf("Produced %d output convex hulls.\r\n", hullCount);
	FILE *fph = fopen("ConvexDecomposition.bin", "wb");
	if (fph)
	{
		HullHeader hh;
		hh.hullCount = hullCount;
		fwrite(&hh, sizeof(hh), 1, fph);
		for (hacd::HaU32 i = 0; i < hullCount; i++)
		{
			const HACD::HACD_API::Hull *hull = gHACD->getHull(i);
			if (hull)
			{
				fwrite(&hull->mVertexCount, sizeof(hull->mVertexCount), 1, fph);
				fwrite(&hull->mTriangleCount, sizeof(hull->mTriangleCount), 1, fph);
				fwrite(hull->mVertices, sizeof(float)*hull->mVertexCount * 3, 1, fph);
				fwrite(hull->mIndices, sizeof(hacd::HaU32)*hull->mTriangleCount * 3, 1, fph);
			}
		}
		fclose(fph);
	}
	else
	{
		printf("Failed to open output file.\r\n");
	}
}

