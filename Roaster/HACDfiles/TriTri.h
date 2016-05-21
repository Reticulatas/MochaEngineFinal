#ifndef TRI_TRI_H

#define TRI_TRI_H

#include "PlatformConfigHACD.h"

int tri_tri_overlap_test_3d(hacd::HaF32 p1[3], hacd::HaF32 q1[3], hacd::HaF32 r1[3],
			    hacd::HaF32 p2[3], hacd::HaF32 q2[3], hacd::HaF32 r2[3]);


int coplanar_tri_tri3d(hacd::HaF32  p1[3], hacd::HaF32  q1[3], hacd::HaF32  r1[3],
		       hacd::HaF32  p2[3], hacd::HaF32  q2[3], hacd::HaF32  r2[3],
		       hacd::HaF32  N1[3], hacd::HaF32  N2[3]);


int tri_tri_overlap_test_2d(hacd::HaF32 p1[2], hacd::HaF32 q1[2], hacd::HaF32 r1[2],
			    hacd::HaF32 p2[2], hacd::HaF32 q2[2], hacd::HaF32 r2[2]);


int tri_tri_intersection_test_3d(hacd::HaF32 p1[3], hacd::HaF32 q1[3], hacd::HaF32 r1[3],
				 hacd::HaF32 p2[3], hacd::HaF32 q2[3], hacd::HaF32 r2[3],
				 int * coplanar,
				 hacd::HaF32 source[3],hacd::HaF32 target[3]);

#endif
