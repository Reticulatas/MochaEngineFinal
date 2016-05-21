#ifndef FLOAT_MATH_LIB_H

#define FLOAT_MATH_LIB_H

#include "PlatformConfigHACD.h"

/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

// a set of routines that let you do common 3d math
// operations without any vector, matrix, or quaternion
// classes or templates.
//
// a vector (or point) is a 'HaF32 *' to 3 floating point numbers.
// a matrix is a 'HaF32 *' to an array of 16 floating point numbers representing a 4x4 transformation matrix compatible with D3D or OGL
// a quaternion is a 'HaF32 *' to 4 floats representing a quaternion x,y,z,w
//
//


#include <float.h>

namespace hacd
{
enum FM_ClipState
{
  FMCS_XMIN       = (1<<0),
  FMCS_XMAX       = (1<<1),
  FMCS_YMIN       = (1<<2),
  FMCS_YMAX       = (1<<3),
  FMCS_ZMIN       = (1<<4),
  FMCS_ZMAX       = (1<<5),
};

enum FM_Axis
{
  FM_XAXIS   = (1<<0),
  FM_YAXIS   = (1<<1),
  FM_ZAXIS   = (1<<2)
};

enum LineSegmentType
{
  LS_START,
  LS_MIDDLE,
  LS_END
};


const HaF32 FM_PI = 3.1415926535897932384626433832795028841971693993751f;
const HaF32 FM_DEG_TO_RAD = ((2.0f * FM_PI) / 360.0f);
const HaF32 FM_RAD_TO_DEG = (360.0f / (2.0f * FM_PI));

//***************** Float versions
//***
//*** vectors are assumed to be 3 floats or 3 doubles representing X, Y, Z
//*** quaternions are assumed to be 4 floats or 4 doubles representing X,Y,Z,W
//*** matrices are assumed to be 16 floats or 16 doubles representing a standard D3D or OpenGL style 4x4 matrix
//*** bounding volumes are expressed as two sets of 3 floats/HaF64 representing bmin(x,y,z) and bmax(x,y,z)
//*** Plane equations are assumed to be 4 floats or 4 doubles representing Ax,By,Cz,D

FM_Axis fm_getDominantAxis(const HaF32 normal[3]);
FM_Axis fm_getDominantAxis(const HaF64 normal[3]);

void fm_decomposeTransform(const HaF32 local_transform[16],HaF32 trans[3],HaF32 rot[4],HaF32 scale[3]);
void fm_decomposeTransform(const HaF64 local_transform[16],HaF64 trans[3],HaF64 rot[4],HaF64 scale[3]);

void  fm_multiplyTransform(const HaF32 *pA,const HaF32 *pB,HaF32 *pM);
void  fm_multiplyTransform(const HaF64 *pA,const HaF64 *pB,HaF64 *pM);

void  fm_inverseTransform(const HaF32 matrix[16],HaF32 inverse_matrix[16]);
void  fm_inverseTransform(const HaF64 matrix[16],HaF64 inverse_matrix[16]);

void  fm_identity(HaF32 matrix[16]); // set 4x4 matrix to identity.
void  fm_identity(HaF64 matrix[16]); // set 4x4 matrix to identity.

void  fm_inverseRT(const HaF32 matrix[16], const HaF32 pos[3], HaF32 t[3]); // inverse rotate translate the point.
void  fm_inverseRT(const HaF64 matrix[16],const HaF64 pos[3],HaF64 t[3]); // inverse rotate translate the point.

void  fm_transform(const HaF32 matrix[16], const HaF32 pos[3], HaF32 t[3]); // rotate and translate this point.
void  fm_transform(const HaF64 matrix[16],const HaF64 pos[3],HaF64 t[3]); // rotate and translate this point.

HaF32  fm_getDeterminant(const HaF32 matrix[16]);
HaF64 fm_getDeterminant(const HaF64 matrix[16]);

void fm_getSubMatrix(HaI32 ki,HaI32 kj,HaF32 pDst[16],const HaF32 matrix[16]);
void fm_getSubMatrix(HaI32 ki,HaI32 kj,HaF64 pDst[16],const HaF32 matrix[16]);

void  fm_rotate(const HaF32 matrix[16],const HaF32 pos[3],HaF32 t[3]); // only rotate the point by a 4x4 matrix, don't translate.
void  fm_rotate(const HaF64 matri[16],const HaF64 pos[3],HaF64 t[3]); // only rotate the point by a 4x4 matrix, don't translate.

void  fm_eulerToMatrix(HaF32 ax,HaF32 ay,HaF32 az,HaF32 matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)
void  fm_eulerToMatrix(HaF64 ax,HaF64 ay,HaF64 az,HaF64 matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)

void  fm_getAABB(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 bmin[3],HaF32 bmax[3]);
void  fm_getAABB(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF64 bmin[3],HaF64 bmax[3]);

void  fm_getAABBCenter(const HaF32 bmin[3],const HaF32 bmax[3],HaF32 center[3]);
void  fm_getAABBCenter(const HaF64 bmin[3],const HaF64 bmax[3],HaF64 center[3]);

void fm_transformAABB(const HaF32 bmin[3],const HaF32 bmax[3],const HaF32 matrix[16],HaF32 tbmin[3],HaF32 tbmax[3]);
void fm_transformAABB(const HaF64 bmin[3],const HaF64 bmax[3],const HaF64 matrix[16],HaF64 tbmin[3],HaF64 tbmax[3]);

void  fm_eulerToQuat(HaF32 x,HaF32 y,HaF32 z,HaF32 quat[4]); // convert euler angles to quaternion.
void  fm_eulerToQuat(HaF64 x,HaF64 y,HaF64 z,HaF64 quat[4]); // convert euler angles to quaternion.

void  fm_quatToEuler(const HaF32 quat[4],HaF32 &ax,HaF32 &ay,HaF32 &az);
void  fm_quatToEuler(const HaF64 quat[4],HaF64 &ax,HaF64 &ay,HaF64 &az);

void  fm_eulerToQuat(const HaF32 euler[3],HaF32 quat[4]); // convert euler angles to quaternion. Angles must be radians not degrees!
void  fm_eulerToQuat(const HaF64 euler[3],HaF64 quat[4]); // convert euler angles to quaternion.

void  fm_scale(HaF32 x,HaF32 y,HaF32 z,HaF32 matrix[16]); // apply scale to the matrix.
void  fm_scale(HaF64 x,HaF64 y,HaF64 z,HaF64 matrix[16]); // apply scale to the matrix.

void  fm_eulerToQuatDX(HaF32 x,HaF32 y,HaF32 z,HaF32 quat[4]); // convert euler angles to quaternion using the fucked up DirectX method
void  fm_eulerToQuatDX(HaF64 x,HaF64 y,HaF64 z,HaF64 quat[4]); // convert euler angles to quaternion using the fucked up DirectX method

void  fm_eulerToMatrixDX(HaF32 x,HaF32 y,HaF32 z,HaF32 matrix[16]); // convert euler angles to quaternion using the fucked up DirectX method.
void  fm_eulerToMatrixDX(HaF64 x,HaF64 y,HaF64 z,HaF64 matrix[16]); // convert euler angles to quaternion using the fucked up DirectX method.

void  fm_quatToMatrix(const HaF32 quat[4],HaF32 matrix[16]); // convert quaterinion rotation to matrix, translation set to zero.
void  fm_quatToMatrix(const HaF64 quat[4],HaF64 matrix[16]); // convert quaterinion rotation to matrix, translation set to zero.

void  fm_quatRotate(const HaF32 quat[4],const HaF32 v[3],HaF32 r[3]); // rotate a vector directly by a quaternion.
void  fm_quatRotate(const HaF64 quat[4],const HaF64 v[3],HaF64 r[3]); // rotate a vector directly by a quaternion.

void  fm_getTranslation(const HaF32 matrix[16],HaF32 t[3]);
void  fm_getTranslation(const HaF64 matrix[16],HaF64 t[3]);

void  fm_setTranslation(const HaF32 *translation,HaF32 matrix[16]);
void  fm_setTranslation(const HaF64 *translation,HaF64 matrix[16]);

void  fm_multiplyQuat(const HaF32 *qa,const HaF32 *qb,HaF32 *quat);
void  fm_multiplyQuat(const HaF64 *qa,const HaF64 *qb,HaF64 *quat);

void  fm_matrixToQuat(const HaF32 matrix[16],HaF32 quat[4]); // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w
void  fm_matrixToQuat(const HaF64 matrix[16],HaF64 quat[4]); // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w

HaF32 fm_sphereVolume(HaF32 radius); // return's the volume of a sphere of this radius (4/3 PI * R cubed )
HaF64 fm_sphereVolume(HaF64 radius); // return's the volume of a sphere of this radius (4/3 PI * R cubed )

HaF32 fm_cylinderVolume(HaF32 radius,HaF32 h);
HaF64 fm_cylinderVolume(HaF64 radius,HaF64 h);

HaF32 fm_capsuleVolume(HaF32 radius,HaF32 h);
HaF64 fm_capsuleVolume(HaF64 radius,HaF64 h);

HaF32 fm_distance(const HaF32 p1[3],const HaF32 p2[3]);
HaF64 fm_distance(const HaF64 p1[3],const HaF64 p2[3]);

HaF32 fm_distanceSquared(const HaF32 p1[3],const HaF32 p2[3]);
HaF64 fm_distanceSquared(const HaF64 p1[3],const HaF64 p2[3]);

HaF32 fm_distanceSquaredXZ(const HaF32 p1[3],const HaF32 p2[3]);
HaF64 fm_distanceSquaredXZ(const HaF64 p1[3],const HaF64 p2[3]);

HaF32 fm_computePlane(const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3],HaF32 *n); // return D
HaF64 fm_computePlane(const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3],HaF64 *n); // return D

HaF32 fm_distToPlane(const HaF32 plane[4],const HaF32 pos[3]); // computes the distance of this point from the plane.
HaF64 fm_distToPlane(const HaF64 plane[4],const HaF64 pos[3]); // computes the distance of this point from the plane.

HaF32 fm_dot(const HaF32 p1[3],const HaF32 p2[3]);
HaF64 fm_dot(const HaF64 p1[3],const HaF64 p2[3]);

void  fm_cross(HaF32 cross[3],const HaF32 a[3],const HaF32 b[3]);
void  fm_cross(HaF64 cross[3],const HaF64 a[3],const HaF64 b[3]);

void  fm_computeNormalVector(HaF32 n[3],const HaF32 p1[3],const HaF32 p2[3]); // as P2-P1 normalized.
void  fm_computeNormalVector(HaF64 n[3],const HaF64 p1[3],const HaF64 p2[3]); // as P2-P1 normalized.

bool  fm_computeWindingOrder(const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3]); // returns true if the triangle is clockwise.
bool  fm_computeWindingOrder(const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3]); // returns true if the triangle is clockwise.

HaF32  fm_normalize(HaF32 n[3]); // normalize this vector and return the distance
HaF64  fm_normalize(HaF64 n[3]); // normalize this vector and return the distance

void  fm_matrixMultiply(const HaF32 A[16],const HaF32 B[16],HaF32 dest[16]);
void  fm_matrixMultiply(const HaF64 A[16],const HaF64 B[16],HaF64 dest[16]);

void  fm_composeTransform(const HaF32 position[3],const HaF32 quat[4],const HaF32 scale[3],HaF32 matrix[16]);
void  fm_composeTransform(const HaF64 position[3],const HaF64 quat[4],const HaF64 scale[3],HaF64 matrix[16]);

HaF32 fm_computeArea(const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3]);
HaF64 fm_computeArea(const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3]);

void  fm_lerp(const HaF32 p1[3],const HaF32 p2[3],HaF32 dest[3],HaF32 lerpValue);
void  fm_lerp(const HaF64 p1[3],const HaF64 p2[3],HaF64 dest[3],HaF64 lerpValue);

bool  fm_insideTriangleXZ(const HaF32 test[3],const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3]);
bool  fm_insideTriangleXZ(const HaF64 test[3],const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3]);

bool  fm_insideAABB(const HaF32 pos[3],const HaF32 bmin[3],const HaF32 bmax[3]);
bool  fm_insideAABB(const HaF64 pos[3],const HaF64 bmin[3],const HaF64 bmax[3]);

bool  fm_insideAABB(const HaF32 obmin[3],const HaF32 obmax[3],const HaF32 tbmin[3],const HaF32 tbmax[3]); // test if bounding box tbmin/tmbax is fully inside obmin/obmax
bool  fm_insideAABB(const HaF64 obmin[3],const HaF64 obmax[3],const HaF64 tbmin[3],const HaF64 tbmax[3]); // test if bounding box tbmin/tmbax is fully inside obmin/obmax

HaU32 fm_clipTestPoint(const HaF32 bmin[3],const HaF32 bmax[3],const HaF32 pos[3]);
HaU32 fm_clipTestPoint(const HaF64 bmin[3],const HaF64 bmax[3],const HaF64 pos[3]);

HaU32 fm_clipTestPointXZ(const HaF32 bmin[3],const HaF32 bmax[3],const HaF32 pos[3]); // only tests X and Z, not Y
HaU32 fm_clipTestPointXZ(const HaF64 bmin[3],const HaF64 bmax[3],const HaF64 pos[3]); // only tests X and Z, not Y


HaU32 fm_clipTestAABB(const HaF32 bmin[3],const HaF32 bmax[3],const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3],HaU32 &andCode);
HaU32 fm_clipTestAABB(const HaF64 bmin[3],const HaF64 bmax[3],const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3],HaU32 &andCode);


bool     fm_lineTestAABBXZ(const HaF32 p1[3],const HaF32 p2[3],const HaF32 bmin[3],const HaF32 bmax[3],HaF32 &time);
bool     fm_lineTestAABBXZ(const HaF64 p1[3],const HaF64 p2[3],const HaF64 bmin[3],const HaF64 bmax[3],HaF64 &time);

bool     fm_lineTestAABB(const HaF32 p1[3],const HaF32 p2[3],const HaF32 bmin[3],const HaF32 bmax[3],HaF32 &time);
bool     fm_lineTestAABB(const HaF64 p1[3],const HaF64 p2[3],const HaF64 bmin[3],const HaF64 bmax[3],HaF64 &time);


void  fm_initMinMax(const HaF32 p[3],HaF32 bmin[3],HaF32 bmax[3]);
void  fm_initMinMax(const HaF64 p[3],HaF64 bmin[3],HaF64 bmax[3]);

void  fm_initMinMax(HaF32 bmin[3],HaF32 bmax[3]);
void  fm_initMinMax(HaF64 bmin[3],HaF64 bmax[3]);

void  fm_minmax(const HaF32 p[3],HaF32 bmin[3],HaF32 bmax[3]); // accmulate to a min-max value
void  fm_minmax(const HaF64 p[3],HaF64 bmin[3],HaF64 bmax[3]); // accmulate to a min-max value


HaF32 fm_solveX(const HaF32 plane[4],HaF32 y,HaF32 z); // solve for X given this plane equation and the other two components.
HaF64 fm_solveX(const HaF64 plane[4],HaF64 y,HaF64 z); // solve for X given this plane equation and the other two components.

HaF32 fm_solveY(const HaF32 plane[4],HaF32 x,HaF32 z); // solve for Y given this plane equation and the other two components.
HaF64 fm_solveY(const HaF64 plane[4],HaF64 x,HaF64 z); // solve for Y given this plane equation and the other two components.

HaF32 fm_solveZ(const HaF32 plane[4],HaF32 x,HaF32 y); // solve for Z given this plane equation and the other two components.
HaF64 fm_solveZ(const HaF64 plane[4],HaF64 x,HaF64 y); // solve for Z given this plane equation and the other two components.

bool  fm_computeBestFitPlane(HaU32 vcount,     // number of input data points
                     const HaF32 *points,     // starting address of points array.
                     HaU32 vstride,    // stride between input points.
                     const HaF32 *weights,    // *optional point weighting values.
                     HaU32 wstride,    // weight stride for each vertex.
                     HaF32 plane[4]);

bool  fm_computeBestFitPlane(HaU32 vcount,     // number of input data points
                     const HaF64 *points,     // starting address of points array.
                     HaU32 vstride,    // stride between input points.
                     const HaF64 *weights,    // *optional point weighting values.
                     HaU32 wstride,    // weight stride for each vertex.
                     HaF64 plane[4]);

bool  fm_computeCentroid(HaU32 vcount,     // number of input data points
						 const HaF32 *points,     // starting address of points array.
						 HaU32 vstride,    // stride between input points.
						 HaF32 *center);

bool  fm_computeCentroid(HaU32 vcount,     // number of input data points
						 const HaF64 *points,     // starting address of points array.
						 HaU32 vstride,    // stride between input points.
						 HaF64 *center);


HaF32  fm_computeBestFitAABB(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 bmin[3],HaF32 bmax[3]); // returns the diagonal distance
HaF64 fm_computeBestFitAABB(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF64 bmin[3],HaF64 bmax[3]); // returns the diagonal distance

HaF32  fm_computeBestFitSphere(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 center[3]);
HaF64  fm_computeBestFitSphere(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF64 center[3]);

bool fm_lineSphereIntersect(const HaF32 center[3],HaF32 radius,const HaF32 p1[3],const HaF32 p2[3],HaF32 intersect[3]);
bool fm_lineSphereIntersect(const HaF64 center[3],HaF64 radius,const HaF64 p1[3],const HaF64 p2[3],HaF64 intersect[3]);

bool fm_intersectRayAABB(const HaF32 bmin[3],const HaF32 bmax[3],const HaF32 pos[3],const HaF32 dir[3],HaF32 intersect[3]);
bool fm_intersectLineSegmentAABB(const HaF32 bmin[3],const HaF32 bmax[3],const HaF32 p1[3],const HaF32 p2[3],HaF32 intersect[3]);

bool fm_lineIntersectsTriangle(const HaF32 rayStart[3],const HaF32 rayEnd[3],const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3],HaF32 sect[3]);
bool fm_lineIntersectsTriangle(const HaF64 rayStart[3],const HaF64 rayEnd[3],const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3],HaF64 sect[3]);

bool fm_rayIntersectsTriangle(const HaF32 origin[3],const HaF32 dir[3],const HaF32 v0[3],const HaF32 v1[3],const HaF32 v2[3],HaF32 &t);
bool fm_rayIntersectsTriangle(const HaF64 origin[3],const HaF64 dir[3],const HaF64 v0[3],const HaF64 v1[3],const HaF64 v2[3],HaF64 &t);

bool fm_raySphereIntersect(const HaF32 center[3],HaF32 radius,const HaF32 pos[3],const HaF32 dir[3],HaF32 distance,HaF32 intersect[3]);
bool fm_raySphereIntersect(const HaF64 center[3],HaF64 radius,const HaF64 pos[3],const HaF64 dir[3],HaF64 distance,HaF64 intersect[3]);

void fm_catmullRom(HaF32 out_vector[3],const HaF32 p1[3],const HaF32 p2[3],const HaF32 p3[3],const HaF32 *p4, const HaF32 s);
void fm_catmullRom(HaF64 out_vector[3],const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3],const HaF64 *p4, const HaF64 s);

bool fm_intersectAABB(const HaF32 bmin1[3],const HaF32 bmax1[3],const HaF32 bmin2[3],const HaF32 bmax2[3]);
bool fm_intersectAABB(const HaF64 bmin1[3],const HaF64 bmax1[3],const HaF64 bmin2[3],const HaF64 bmax2[3]);


// computes the rotation quaternion to go from unit-vector v0 to unit-vector v1
void fm_rotationArc(const HaF32 v0[3],const HaF32 v1[3],HaF32 quat[4]);
void fm_rotationArc(const HaF64 v0[3],const HaF64 v1[3],HaF64 quat[4]);

HaF32  fm_distancePointLineSegment(const HaF32 Point[3],const HaF32 LineStart[3],const HaF32 LineEnd[3],HaF32 intersection[3],LineSegmentType &type,HaF32 epsilon);
HaF64 fm_distancePointLineSegment(const HaF64 Point[3],const HaF64 LineStart[3],const HaF64 LineEnd[3],HaF64 intersection[3],LineSegmentType &type,HaF64 epsilon);


bool fm_colinear(const HaF64 p1[3],const HaF64 p2[3],const HaF64 p3[3],HaF64 epsilon=0.999);               // true if these three points in a row are co-linear
bool fm_colinear(const HaF32  p1[3],const HaF32  p2[3],const HaF32 p3[3],HaF32 epsilon=0.999f);

bool fm_colinear(const HaF32 a1[3],const HaF32 a2[3],const HaF32 b1[3],const HaF32 b2[3],HaF32 epsilon=0.999f);  // true if these two line segments are co-linear.
bool fm_colinear(const HaF64 a1[3],const HaF64 a2[3],const HaF64 b1[3],const HaF64 b2[3],HaF64 epsilon=0.999);  // true if these two line segments are co-linear.

enum IntersectResult
{
  IR_DONT_INTERSECT,
  IR_DO_INTERSECT,
  IR_COINCIDENT,
  IR_PARALLEL,
};

IntersectResult fm_intersectLineSegments2d(const HaF32 a1[3], const HaF32 a2[3], const HaF32 b1[3], const HaF32 b2[3], HaF32 intersectionPoint[3]);
IntersectResult fm_intersectLineSegments2d(const HaF64 a1[3],const HaF64 a2[3],const HaF64 b1[3],const HaF64 b2[3],HaF64 intersectionPoint[3]);

IntersectResult fm_intersectLineSegments2dTime(const HaF32 a1[3], const HaF32 a2[3], const HaF32 b1[3], const HaF32 b2[3],HaF32 &t1,HaF32 &t2);
IntersectResult fm_intersectLineSegments2dTime(const HaF64 a1[3],const HaF64 a2[3],const HaF64 b1[3],const HaF64 b2[3],HaF64 &t1,HaF64 &t2);

// Plane-Triangle splitting

enum PlaneTriResult
{
  PTR_ON_PLANE,
  PTR_FRONT,
  PTR_BACK,
  PTR_SPLIT,
};

PlaneTriResult fm_planeTriIntersection(const HaF32 plane[4],    // the plane equation in Ax+By+Cz+D format
                                    const HaF32 *triangle, // the source triangle.
                                    HaU32 tstride,  // stride in bytes of the input and output *vertices*
                                    HaF32        epsilon,  // the co-planer epsilon value.
                                    HaF32       *front,    // the triangle in front of the
                                    HaU32 &fcount,  // number of vertices in the 'front' triangle
                                    HaF32       *back,     // the triangle in back of the plane
                                    HaU32 &bcount); // the number of vertices in the 'back' triangle.


PlaneTriResult fm_planeTriIntersection(const HaF64 plane[4],    // the plane equation in Ax+By+Cz+D format
                                    const HaF64 *triangle, // the source triangle.
                                    HaU32 tstride,  // stride in bytes of the input and output *vertices*
                                    HaF64        epsilon,  // the co-planer epsilon value.
                                    HaF64       *front,    // the triangle in front of the
                                    HaU32 &fcount,  // number of vertices in the 'front' triangle
                                    HaF64       *back,     // the triangle in back of the plane
                                    HaU32 &bcount); // the number of vertices in the 'back' triangle.


void fm_intersectPointPlane(const HaF32 p1[3],const HaF32 p2[3],HaF32 *split,const HaF32 plane[4]);
void fm_intersectPointPlane(const HaF64 p1[3],const HaF64 p2[3],HaF64 *split,const HaF64 plane[4]);

PlaneTriResult fm_getSidePlane(const HaF32 p[3],const HaF32 plane[4],HaF32 epsilon);
PlaneTriResult fm_getSidePlane(const HaF64 p[3],const HaF64 plane[4],HaF64 epsilon);


void fm_computeBestFitOBB(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 *sides,HaF32 matrix[16],bool bruteForce=true);
void fm_computeBestFitOBB(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF64 *sides,HaF64 matrix[16],bool bruteForce=true);

void fm_computeBestFitOBB(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 *sides,HaF32 pos[3],HaF32 quat[4],bool bruteForce=true);
void fm_computeBestFitOBB(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF64 *sides,HaF64 pos[3],HaF64 quat[4],bool bruteForce=true);

void fm_computeBestFitABB(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 *sides,HaF32 pos[3]);
void fm_computeBestFitABB(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF64 *sides,HaF64 pos[3]);


//** Note, if the returned capsule height is less than zero, then you must represent it is a sphere of size radius.
void fm_computeBestFitCapsule(HaU32 vcount,const HaF32 *points,HaU32 pstride,HaF32 &radius,HaF32 &height,HaF32 matrix[16],bool bruteForce=true);
void fm_computeBestFitCapsule(HaU32 vcount,const HaF64 *points,HaU32 pstride,HaF32 &radius,HaF32 &height,HaF64 matrix[16],bool bruteForce=true);


void fm_planeToMatrix(const HaF32 plane[4],HaF32 matrix[16]); // convert a plane equation to a 4x4 rotation matrix.  Reference vector is 0,1,0
void fm_planeToQuat(const HaF32 plane[4],HaF32 quat[4],HaF32 pos[3]); // convert a plane equation to a quaternion and translation

void fm_planeToMatrix(const HaF64 plane[4],HaF64 matrix[16]); // convert a plane equation to a 4x4 rotation matrix
void fm_planeToQuat(const HaF64 plane[4],HaF64 quat[4],HaF64 pos[3]); // convert a plane equation to a quaternion and translation

inline void fm_doubleToFloat3(const HaF64 p[3],HaF32 t[3]) { t[0] = (HaF32) p[0]; t[1] = (HaF32)p[1]; t[2] = (HaF32)p[2]; };
inline void fm_floatToDouble3(const HaF32 p[3],HaF64 t[3]) { t[0] = (HaF64)p[0]; t[1] = (HaF64)p[1]; t[2] = (HaF64)p[2]; };


void  fm_eulerMatrix(HaF32 ax,HaF32 ay,HaF32 az,HaF32 matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)
void  fm_eulerMatrix(HaF64 ax,HaF64 ay,HaF64 az,HaF64 matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)


HaF32  fm_computeMeshVolume(const HaF32 *vertices,HaU32 tcount,const HaU32 *indices);
HaF64 fm_computeMeshVolume(const HaF64 *vertices,HaU32 tcount,const HaU32 *indices);


#define FM_DEFAULT_GRANULARITY 0.001f  // 1 millimeter is the default granularity

class fm_VertexIndex
{
public:
  virtual HaU32          getIndex(const HaF32 pos[3],bool &newPos) = 0;  // get welded index for this HaF32 vector[3]
  virtual HaU32          getIndex(const HaF64 pos[3],bool &newPos) = 0;  // get welded index for this HaF64 vector[3]
  virtual const HaF32 *   getVerticesFloat(void) const = 0;
  virtual const HaF64 *  getVerticesDouble(void) const = 0;
  virtual const HaF32 *   getVertexFloat(HaU32 index) const = 0;
  virtual const HaF64 *  getVertexDouble(HaU32 index) const = 0;
  virtual HaU32          getVcount(void) const = 0;
  virtual bool            isDouble(void) const = 0;
  virtual bool            saveAsObj(const char *fname,HaU32 tcount,HaU32 *indices) = 0;
};

fm_VertexIndex * fm_createVertexIndex(HaF64 granularity,bool snapToGrid); // create an indexed vertex system for doubles
fm_VertexIndex * fm_createVertexIndex(HaF32 granularity,bool snapToGrid);  // create an indexed vertext system for floats
void             fm_releaseVertexIndex(fm_VertexIndex *vindex);



#if 0 // currently disabled

class fm_LineSegment
{
public:
  fm_LineSegment(void)
  {
    mE1 = mE2 = 0;
  }

  fm_LineSegment(HaU32 e1,HaU32 e2)
  {
    mE1 = e1;
    mE2 = e2;
  }

  HaU32 mE1;
  HaU32 mE2;
};


// LineSweep *only* supports doublees.  As a geometric operation it needs as much precision as possible.
class fm_LineSweep
{
public:

 virtual fm_LineSegment * performLineSweep(const fm_LineSegment *segments,
                                   HaU32 icount,
                                   const HaF64 *planeEquation,
                                   fm_VertexIndex *pool,
                                   HaU32 &scount) = 0;


};

fm_LineSweep * fm_createLineSweep(void);
void           fm_releaseLineSweep(fm_LineSweep *sweep);

#endif

class fm_Triangulate
{
public:
  virtual const HaF64 *       triangulate3d(HaU32 pcount,
                                             const HaF64 *points,
                                             HaU32 vstride,
                                             HaU32 &tcount,
                                             bool consolidate,
                                             HaF64 epsilon) = 0;

  virtual const HaF32  *       triangulate3d(HaU32 pcount,
                                             const HaF32  *points,
                                             HaU32 vstride,
                                             HaU32 &tcount,
                                             bool consolidate,
                                             HaF32 epsilon) = 0;
};

fm_Triangulate * fm_createTriangulate(void);
void             fm_releaseTriangulate(fm_Triangulate *t);


const HaF32 * fm_getPoint(const HaF32 *points,HaU32 pstride,HaU32 index);
const HaF64 * fm_getPoint(const HaF64 *points,HaU32 pstride,HaU32 index);

bool   fm_insideTriangle(HaF32 Ax, HaF32 Ay,HaF32 Bx, HaF32 By,HaF32 Cx, HaF32 Cy,HaF32 Px, HaF32 Py);
bool   fm_insideTriangle(HaF64 Ax, HaF64 Ay,HaF64 Bx, HaF64 By,HaF64 Cx, HaF64 Cy,HaF64 Px, HaF64 Py);
HaF32  fm_areaPolygon2d(HaU32 pcount,const HaF32 *points,HaU32 pstride);
HaF64 fm_areaPolygon2d(HaU32 pcount,const HaF64 *points,HaU32 pstride);

bool  fm_pointInsidePolygon2d(HaU32 pcount,const HaF32 *points,HaU32 pstride,const HaF32 *point,HaU32 xindex=0,HaU32 yindex=1);
bool  fm_pointInsidePolygon2d(HaU32 pcount,const HaF64 *points,HaU32 pstride,const HaF64 *point,HaU32 xindex=0,HaU32 yindex=1);

HaU32 fm_consolidatePolygon(HaU32 pcount,const HaF32 *points,HaU32 pstride,HaF32 *dest,HaF32 epsilon=0.999999f); // collapses co-linear edges.
HaU32 fm_consolidatePolygon(HaU32 pcount,const HaF64 *points,HaU32 pstride,HaF64 *dest,HaF64 epsilon=0.999999); // collapses co-linear edges.


bool fm_computeSplitPlane(HaU32 vcount,const HaF64 *vertices,HaU32 tcount,const HaU32 *indices,HaF64 *plane);
bool fm_computeSplitPlane(HaU32 vcount,const HaF32 *vertices,HaU32 tcount,const HaU32 *indices,HaF32 *plane);

void fm_nearestPointInTriangle(const HaF32 *pos,const HaF32 *p1,const HaF32 *p2,const HaF32 *p3,HaF32 *nearest);
void fm_nearestPointInTriangle(const HaF64 *pos,const HaF64 *p1,const HaF64 *p2,const HaF64 *p3,HaF64 *nearest);

HaF32  fm_areaTriangle(const HaF32 *p1,const HaF32 *p2,const HaF32 *p3);
HaF64 fm_areaTriangle(const HaF64 *p1,const HaF64 *p2,const HaF64 *p3);

void fm_subtract(const HaF32 *A,const HaF32 *B,HaF32 *diff); // compute A-B and store the result in 'diff'
void fm_subtract(const HaF64 *A,const HaF64 *B,HaF64 *diff); // compute A-B and store the result in 'diff'

void fm_multiply(HaF32 *A,HaF32 scaler);
void fm_multiply(HaF64 *A,HaF64 scaler);

void fm_add(const HaF32 *A,const HaF32 *B,HaF32 *sum);
void fm_add(const HaF64 *A,const HaF64 *B,HaF64 *sum);

void fm_copy3(const HaF32 *source,HaF32 *dest);
void fm_copy3(const HaF64 *source,HaF64 *dest);

// re-indexes an indexed triangle mesh but drops unused vertices.  The output_indices can be the same pointer as the input indices.
// the output_vertices can point to the input vertices if you desire.  The output_vertices buffer should be at least the same size
// is the input buffer.  The routine returns the new vertex count after re-indexing.
HaU32  fm_copyUniqueVertices(HaU32 vcount,const HaF32 *input_vertices,HaF32 *output_vertices,HaU32 tcount,const HaU32 *input_indices,HaU32 *output_indices);
HaU32  fm_copyUniqueVertices(HaU32 vcount,const HaF64 *input_vertices,HaF64 *output_vertices,HaU32 tcount,const HaU32 *input_indices,HaU32 *output_indices);

bool    fm_isMeshCoplanar(HaU32 tcount,const HaU32 *indices,const HaF32 *vertices,bool doubleSided); // returns true if this collection of indexed triangles are co-planar!
bool    fm_isMeshCoplanar(HaU32 tcount,const HaU32 *indices,const HaF64 *vertices,bool doubleSided); // returns true if this collection of indexed triangles are co-planar!

bool    fm_samePlane(const HaF32 p1[4],const HaF32 p2[4],HaF32 normalEpsilon=0.01f,HaF32 dEpsilon=0.001f,bool doubleSided=false); // returns true if these two plane equations are identical within an epsilon
bool    fm_samePlane(const HaF64 p1[4],const HaF64 p2[4],HaF64 normalEpsilon=0.01,HaF64 dEpsilon=0.001,bool doubleSided=false);

void    fm_OBBtoAABB(const HaF32 obmin[3],const HaF32 obmax[3],const HaF32 matrix[16],HaF32 abmin[3],HaF32 abmax[3]);

// a utility class that will tesseleate a mesh.
class fm_Tesselate
{
public:
  virtual const HaU32 * tesselate(fm_VertexIndex *vindex,HaU32 tcount,const HaU32 *indices,HaF32 longEdge,HaU32 maxDepth,HaU32 &outcount) = 0;
};

fm_Tesselate * fm_createTesselate(void);
void           fm_releaseTesselate(fm_Tesselate *t);

void fm_computeMeanNormals(HaU32 vcount,       // the number of vertices
                           const HaF32 *vertices,     // the base address of the vertex position data.
                           HaU32 vstride,      // the stride between position data.
                           HaF32 *normals,            // the base address  of the destination for mean vector normals
                           HaU32 nstride,      // the stride between normals
                           HaU32 tcount,       // the number of triangles
                           const HaU32 *indices);     // the triangle indices

void fm_computeMeanNormals(HaU32 vcount,       // the number of vertices
                           const HaF64 *vertices,     // the base address of the vertex position data.
                           HaU32 vstride,      // the stride between position data.
                           HaF64 *normals,            // the base address  of the destination for mean vector normals
                           HaU32 nstride,      // the stride between normals
                           HaU32 tcount,       // the number of triangles
                           const HaU32 *indices);     // the triangle indices


bool fm_isValidTriangle(const HaF32 *p1,const HaF32 *p2,const HaF32 *p3,HaF32 epsilon=0.00001f);
bool fm_isValidTriangle(const HaF64 *p1,const HaF64 *p2,const HaF64 *p3,HaF64 epsilon=0.00001f);

};

#endif
