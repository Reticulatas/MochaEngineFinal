#ifndef MESHIMPORT_H
#define MESHIMPORT_H

#include <stdio.h>
#include <string.h>
#if defined(__APPLE__)
#include <string>
#endif
#include <float.h>
#if !defined(__CELLOS_LV2__) && !defined(__APPLE__)
#include <malloc.h>
#endif
#include <math.h>
#include "PlatformConfigHACD.h"

#pragma warning(push)
#pragma warning(disable:4996)


namespace hacd
{

inline HaF32 fmi_computePlane(const HaF32 *A,const HaF32 *B,const HaF32 *C,HaF32 *n) // returns D
{
	HaF32 vx = (B[0] - C[0]);
	HaF32 vy = (B[1] - C[1]);
	HaF32 vz = (B[2] - C[2]);

	HaF32 wx = (A[0] - B[0]);
	HaF32 wy = (A[1] - B[1]);
	HaF32 wz = (A[2] - B[2]);

	HaF32 vw_x = vy * wz - vz * wy;
	HaF32 vw_y = vz * wx - vx * wz;
	HaF32 vw_z = vx * wy - vy * wx;

	HaF32 mag = sqrt((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

	if ( mag < 0.000001f )
	{
		mag = 0;
	}
	else
	{
		mag = 1.0f/mag;
	}

	HaF32 x = vw_x * mag;
	HaF32 y = vw_y * mag;
	HaF32 z = vw_z * mag;


	HaF32 D = 0.0f - ((x*A[0])+(y*A[1])+(z*A[2]));

  n[0] = x;
  n[1] = y;
  n[2] = z;

	return D;
}

inline void  fmi_transform(const HaF32 matrix[16],const HaF32 v[3],HaF32 t[3]) // rotate and translate this point
{
  if ( matrix )
  {
    HaF32 tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
    HaF32 ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
    HaF32 tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
    t[0] = tx;
    t[1] = ty;
    t[2] = tz;
  }
  else
  {
    t[0] = v[0];
    t[1] = v[1];
    t[2] = v[2];
  }
}

inline void  fmi_transformRotate(const HaF32 matrix[16],const HaF32 v[3],HaF32 t[3]) // rotate only
{
  if ( matrix )
  {
    HaF32 tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]);
    HaF32 ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]);
    HaF32 tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]);
    t[0] = tx;
    t[1] = ty;
    t[2] = tz;
  }
  else
  {
    t[0] = v[0];
    t[1] = v[1];
    t[2] = v[2];
  }
}

inline HaF32 fmi_normalize(HaF32 *n) // normalize this vector
{
  HaF32 dist = (HaF32)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
  if ( dist > 0.0000001f )
  {
    HaF32 mag = 1.0f / dist;
    n[0]*=mag;
    n[1]*=mag;
    n[2]*=mag;
  }
  else
  {
    n[0] = 1;
    n[1] = 0;
    n[2] = 0;
  }

  return dist;
}


inline void fmi_quatToMatrix(const HaF32 *quat,HaF32 *matrix) // convert quaterinion rotation to matrix, zeros out the translation component.
{

  HaF32 xx = quat[0]*quat[0];
  HaF32 yy = quat[1]*quat[1];
  HaF32 zz = quat[2]*quat[2];
  HaF32 xy = quat[0]*quat[1];
  HaF32 xz = quat[0]*quat[2];
  HaF32 yz = quat[1]*quat[2];
  HaF32 wx = quat[3]*quat[0];
  HaF32 wy = quat[3]*quat[1];
  HaF32 wz = quat[3]*quat[2];

  matrix[0*4+0] = 1 - 2 * ( yy + zz );
  matrix[1*4+0] =     2 * ( xy - wz );
  matrix[2*4+0] =     2 * ( xz + wy );

  matrix[0*4+1] =     2 * ( xy + wz );
  matrix[1*4+1] = 1 - 2 * ( xx + zz );
  matrix[2*4+1] =     2 * ( yz - wx );

  matrix[0*4+2] =     2 * ( xz - wy );
  matrix[1*4+2] =     2 * ( yz + wx );
  matrix[2*4+2] = 1 - 2 * ( xx + yy );

  matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (HaF32) 0.0f;
  matrix[0*4+3] = matrix[1*4+3] = matrix[2*4+3] = (HaF32) 0.0f;
  matrix[3*4+3] =(HaF32) 1.0f;

}



// minimal support math routines
// *** Support math routines
inline void fmi_getAngleAxis(HaF32 &angle,HaF32 *axis,const HaF32 *quat)
{
  //return axis and angle of rotation of quaternion
  HaF32 x = quat[0];
  HaF32 y = quat[1];
  HaF32 z = quat[2];
  HaF32 w = quat[3];

  angle = acosf(w) * 2.0f;		//this is getAngle()
  HaF32 sa = sqrtf(1.0f - w*w);
  if (sa)
  {
    axis[0] = x/sa;
    axis[1] = y/sa;
    axis[2] = z/sa;
  }
  else
  {
    axis[0] = 1;
    axis[1] = 0;
    axis[2] = 0;
  }
}

inline void fmi_setOrientationFromAxisAngle(const HaF32 axis[3],HaF32 angle,HaF32 *quat)
{
  HaF32 x,y,z,w;

  x = axis[0];
  y = axis[1];
  z = axis[2];

  // required: Normalize the axis

  const HaF32 i_length =  HaF32(1.0f) / sqrtf( x*x + y*y + z*z );

  x = x * i_length;
  y = y * i_length;
  z = z * i_length;

  // now make a clQuaternionernion out of it
  HaF32 Half = angle * HaF32(0.5);

  w = cosf(Half);//this used to be w/o deg to rad.
  const HaF32 sin_theta_over_two = sinf(Half);

  x = x * sin_theta_over_two;
  y = y * sin_theta_over_two;
  z = z * sin_theta_over_two;

  quat[0] = x;
  quat[1] = y;
  quat[2] = z;
  quat[3] = w;
}


inline void fmi_identity(HaF32 *matrix)
{
  matrix[0*4+0] = 1;    matrix[1*4+1] = 1;    matrix[2*4+2] = 1;    matrix[3*4+3] = 1;
  matrix[1*4+0] = 0;    matrix[2*4+0] = 0;    matrix[3*4+0] = 0;
  matrix[0*4+1] = 0;    matrix[2*4+1] = 0;    matrix[3*4+1] = 0;
  matrix[0*4+2] = 0;    matrix[1*4+2] = 0;    matrix[3*4+2] = 0;
  matrix[0*4+3] = 0;    matrix[1*4+3] = 0;    matrix[2*4+3] = 0;
}


inline void fmi_fromQuat(HaF32 *matrix,const HaF32 quat[4])
{
  fmi_identity(matrix);

  HaF32 xx = quat[0]*quat[0];
  HaF32 yy = quat[1]*quat[1];
  HaF32 zz = quat[2]*quat[2];
  HaF32 xy = quat[0]*quat[1];
  HaF32 xz = quat[0]*quat[2];
  HaF32 yz = quat[1]*quat[2];
  HaF32 wx = quat[3]*quat[0];
  HaF32 wy = quat[3]*quat[1];
  HaF32 wz = quat[3]*quat[2];

  matrix[0*4+0] = 1 - 2 * ( yy + zz );
  matrix[1*4+0] =     2 * ( xy - wz );
  matrix[2*4+0] =     2 * ( xz + wy );

  matrix[0*4+1] =     2 * ( xy + wz );
  matrix[1*4+1] = 1 - 2 * ( xx + zz );
  matrix[2*4+1] =     2 * ( yz - wx );

  matrix[0*4+2] =     2 * ( xz - wy );
  matrix[1*4+2] =     2 * ( yz + wx );
  matrix[2*4+2] = 1 - 2 * ( xx + yy );

  matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (HaF32) 0.0f;
  matrix[0*4+3] = matrix[1*4+3] = matrix[2*4+3] = (HaF32) 0.0f;
  matrix[3*4+3] =(HaF32) 1.0f;


}

inline void fmi_matrixToQuat(const HaF32 *matrix,HaF32 *quat) // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w
{

  HaF32 tr = matrix[0*4+0] + matrix[1*4+1] + matrix[2*4+2];

  // check the diagonal

  if (tr > 0.0f )
  {
    HaF32 s = sqrtf((tr + 1.0f) );
    quat[3] = s * 0.5f;
    s = 0.5f / s;
    quat[0] = (matrix[1*4+2] - matrix[2*4+1]) * s;
    quat[1] = (matrix[2*4+0] - matrix[0*4+2]) * s;
    quat[2] = (matrix[0*4+1] - matrix[1*4+0]) * s;

  }
  else
  {
    // diagonal is negative
    HaI32 nxt[3] = {1, 2, 0};
    HaF32  qa[4];

    HaI32 i = 0;

    if (matrix[1*4+1] > matrix[0*4+0]) i = 1;
    if (matrix[2*4+2] > matrix[i*4+i]) i = 2;

    HaI32 j = nxt[i];
    HaI32 k = nxt[j];

    HaF32 s = sqrtf( ((matrix[i*4+i] - (matrix[j*4+j] + matrix[k*4+k])) + 1.0f) );

    qa[i] = s * 0.5f;

    if (s != 0.0f ) s = 0.5f / s;

    qa[3] = (matrix[j*4+k] - matrix[k*4+j]) * s;
    qa[j] = (matrix[i*4+j] + matrix[j*4+i]) * s;
    qa[k] = (matrix[i*4+k] + matrix[k*4+i]) * s;

    quat[0] = qa[0];
    quat[1] = qa[1];
    quat[2] = qa[2];
    quat[3] = qa[3];
  }


}


inline HaF32 fmi_squared(HaF32 x) { return x*x; };

inline void fmi_decomposeTransform(const HaF32 local_transform[16],HaF32 trans[3],HaF32 rot[4],HaF32 scale[3])
{

  trans[0] = local_transform[12];
  trans[1] = local_transform[13];
  trans[2] = local_transform[14];

  scale[0] = sqrtf(fmi_squared(local_transform[0*4+0]) + fmi_squared(local_transform[0*4+1]) + fmi_squared(local_transform[0*4+2]));
  scale[1] = sqrtf(fmi_squared(local_transform[1*4+0]) + fmi_squared(local_transform[1*4+1]) + fmi_squared(local_transform[1*4+2]));
  scale[2] = sqrtf(fmi_squared(local_transform[2*4+0]) + fmi_squared(local_transform[2*4+1]) + fmi_squared(local_transform[2*4+2]));

  HaF32 m[16];
  memcpy(m,local_transform,sizeof(HaF32)*16);

  HaF32 sx = 1.0f / scale[0];
  HaF32 sy = 1.0f / scale[1];
  HaF32 sz = 1.0f / scale[2];

  m[0*4+0]*=sx;
  m[0*4+1]*=sx;
  m[0*4+2]*=sx;

  m[1*4+0]*=sy;
  m[1*4+1]*=sy;
  m[1*4+2]*=sy;

  m[2*4+0]*=sz;
  m[2*4+1]*=sz;
  m[2*4+2]*=sz;

  fmi_matrixToQuat(m,rot);

}

inline void fmi_fromScale(HaF32 *matrix,const HaF32 scale[3])
{
  fmi_identity(matrix);
  matrix[0*4+0] = scale[0];
  matrix[1*4+1] = scale[1];
  matrix[2*4+2] = scale[2];

}

inline void  fmi_multiply(const HaF32 *pA,const HaF32 *pB,HaF32 *pM)
{

  HaF32 a = pA[0*4+0] * pB[0*4+0] + pA[0*4+1] * pB[1*4+0] + pA[0*4+2] * pB[2*4+0] + pA[0*4+3] * pB[3*4+0];
  HaF32 b = pA[0*4+0] * pB[0*4+1] + pA[0*4+1] * pB[1*4+1] + pA[0*4+2] * pB[2*4+1] + pA[0*4+3] * pB[3*4+1];
  HaF32 c = pA[0*4+0] * pB[0*4+2] + pA[0*4+1] * pB[1*4+2] + pA[0*4+2] * pB[2*4+2] + pA[0*4+3] * pB[3*4+2];
  HaF32 d = pA[0*4+0] * pB[0*4+3] + pA[0*4+1] * pB[1*4+3] + pA[0*4+2] * pB[2*4+3] + pA[0*4+3] * pB[3*4+3];

  HaF32 e = pA[1*4+0] * pB[0*4+0] + pA[1*4+1] * pB[1*4+0] + pA[1*4+2] * pB[2*4+0] + pA[1*4+3] * pB[3*4+0];
  HaF32 f = pA[1*4+0] * pB[0*4+1] + pA[1*4+1] * pB[1*4+1] + pA[1*4+2] * pB[2*4+1] + pA[1*4+3] * pB[3*4+1];
  HaF32 g = pA[1*4+0] * pB[0*4+2] + pA[1*4+1] * pB[1*4+2] + pA[1*4+2] * pB[2*4+2] + pA[1*4+3] * pB[3*4+2];
  HaF32 h = pA[1*4+0] * pB[0*4+3] + pA[1*4+1] * pB[1*4+3] + pA[1*4+2] * pB[2*4+3] + pA[1*4+3] * pB[3*4+3];

  HaF32 i = pA[2*4+0] * pB[0*4+0] + pA[2*4+1] * pB[1*4+0] + pA[2*4+2] * pB[2*4+0] + pA[2*4+3] * pB[3*4+0];
  HaF32 j = pA[2*4+0] * pB[0*4+1] + pA[2*4+1] * pB[1*4+1] + pA[2*4+2] * pB[2*4+1] + pA[2*4+3] * pB[3*4+1];
  HaF32 k = pA[2*4+0] * pB[0*4+2] + pA[2*4+1] * pB[1*4+2] + pA[2*4+2] * pB[2*4+2] + pA[2*4+3] * pB[3*4+2];
  HaF32 l = pA[2*4+0] * pB[0*4+3] + pA[2*4+1] * pB[1*4+3] + pA[2*4+2] * pB[2*4+3] + pA[2*4+3] * pB[3*4+3];

  HaF32 m = pA[3*4+0] * pB[0*4+0] + pA[3*4+1] * pB[1*4+0] + pA[3*4+2] * pB[2*4+0] + pA[3*4+3] * pB[3*4+0];
  HaF32 n = pA[3*4+0] * pB[0*4+1] + pA[3*4+1] * pB[1*4+1] + pA[3*4+2] * pB[2*4+1] + pA[3*4+3] * pB[3*4+1];
  HaF32 o = pA[3*4+0] * pB[0*4+2] + pA[3*4+1] * pB[1*4+2] + pA[3*4+2] * pB[2*4+2] + pA[3*4+3] * pB[3*4+2];
  HaF32 p = pA[3*4+0] * pB[0*4+3] + pA[3*4+1] * pB[1*4+3] + pA[3*4+2] * pB[2*4+3] + pA[3*4+3] * pB[3*4+3];

  pM[0] = a;  pM[1] = b;  pM[2] = c;  pM[3] = d;

  pM[4] = e;  pM[5] = f;  pM[6] = g;  pM[7] = h;

  pM[8] = i;  pM[9] = j;  pM[10] = k;  pM[11] = l;

  pM[12] = m;  pM[13] = n;  pM[14] = o;  pM[15] = p;
}


inline void fmi_setTranslation(HaF32 *matrix,const HaF32 pos[3])
{
  matrix[12] = pos[0];  matrix[13] = pos[1];  matrix[14] = pos[2];
}


// compose this transform
inline void fmi_composeTransform(const HaF32 pos[3],const HaF32 quat[4],const HaF32 scale[3],HaF32 matrix[16])
{
  HaF32 mscale[16];
  HaF32 mrot[16];
  fmi_fromQuat(mrot,quat);
  fmi_fromScale(mscale,scale);
  fmi_multiply(mscale,mrot,matrix);
  fmi_setTranslation(matrix,pos);
}

inline HaF32 fmi_dot(const HaF32 *p1,const HaF32 *p2)
{
  return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

inline void fmi_cross(HaF32 *cross,const HaF32 *a,const HaF32 *b)
{
  cross[0] = a[1]*b[2] - a[2]*b[1];
  cross[1] = a[2]*b[0] - a[0]*b[2];
  cross[2] = a[0]*b[1] - a[1]*b[0];
}


inline HaF32 fmi_getDeterminant(const HaF32 matrix[16])
{
  HaF32 tempv[3];
  HaF32 p0[3];
  HaF32 p1[3];
  HaF32 p2[3];

  p0[0] = matrix[0*4+0];
  p0[1] = matrix[0*4+1];
  p0[2] = matrix[0*4+2];

  p1[0] = matrix[1*4+0];
  p1[1] = matrix[1*4+1];
  p1[2] = matrix[1*4+2];

  p2[0] = matrix[2*4+0];
  p2[1] = matrix[2*4+1];
  p2[2] = matrix[2*4+2];

  fmi_cross(tempv,p1,p2);

  return fmi_dot(p0,tempv);

}

inline void fmi_getSubMatrix(HaI32 ki,HaI32 kj,HaF32 pDst[16],const HaF32 matrix[16])
{
  HaI32 row, col;
  HaI32 dstCol = 0, dstRow = 0;

  for ( col = 0; col < 4; col++ )
  {
    if ( col == kj )
    {
      continue;
    }
    for ( dstRow = 0, row = 0; row < 4; row++ )
    {
      if ( row == ki )
      {
        continue;
      }
      pDst[dstCol*4+dstRow] = matrix[col*4+row];
      dstRow++;
    }
    dstCol++;
  }
}

inline void fmi_inverseTransform(const HaF32 matrix[16],HaF32 inverse_matrix[16])
{
  HaF32 determinant = fmi_getDeterminant(matrix);
  determinant = 1.0f / determinant;
  for (HaI32 i = 0; i < 4; i++ )
  {
    for (HaI32 j = 0; j < 4; j++ )
    {
      HaI32 sign = 1 - ( ( i + j ) % 2 ) * 2;
      HaF32 subMat[16];
      fmi_identity(subMat);
      fmi_getSubMatrix( i, j, subMat, matrix );
      HaF32 subDeterminant = fmi_getDeterminant(subMat);
      inverse_matrix[i*4+j] = ( subDeterminant * sign ) * determinant;
    }
  }
}



enum MeshVertexFlag
{
  MIVF_POSITION       = (1<<0),
  MIVF_NORMAL         = (1<<1),
  MIVF_COLOR          = (1<<2),
  MIVF_TEXEL1         = (1<<3),
  MIVF_TEXEL2         = (1<<4),
  MIVF_TEXEL3         = (1<<5),
  MIVF_TEXEL4         = (1<<6),
  MIVF_TANGENT        = (1<<7),
  MIVF_BINORMAL       = (1<<8),
  MIVF_BONE_WEIGHTING = (1<<9),
  MIVF_RADIUS         = (1<<10),
  MIVF_INTERP1        = (1<<11),
  MIVF_INTERP2        = (1<<12),
  MIVF_INTERP3        = (1<<13),
  MIVF_INTERP4        = (1<<14),
  MIVF_INTERP5        = (1<<15),
  MIVF_INTERP6        = (1<<16),
  MIVF_INTERP7        = (1<<17),
  MIVF_INTERP8        = (1<<18),
  MIVF_ALL = (MIVF_POSITION | MIVF_NORMAL | MIVF_COLOR | MIVF_TEXEL1 | MIVF_TEXEL2 | MIVF_TEXEL3 | MIVF_TEXEL4 | MIVF_TANGENT | MIVF_BINORMAL | MIVF_BONE_WEIGHTING )
};

class MeshVertex
{
public:
  MeshVertex(void)
  {
    mPos[0] = mPos[1] = mPos[2] = 0;
    mNormal[0] = 0; mNormal[1] = 1; mNormal[2] = 0;
    mColor = 0xFFFFFFFF;
    mTexel1[0] = mTexel1[1] = 0;
    mTexel2[0] = mTexel2[1] = 0;
    mTexel3[0] = mTexel3[1] = 0;
    mTexel4[0] = mTexel4[1] = 0;

	mInterp1[0] = mInterp1[1] = mInterp1[2] = mInterp1[3] =0;
	mInterp2[0] = mInterp2[1] = mInterp2[2] = mInterp2[3] =0;
	mInterp3[0] = mInterp3[1] = mInterp3[2] = mInterp3[3] =0;
	mInterp4[0] = mInterp4[1] = mInterp4[2] = mInterp4[3] =0;
	mInterp5[0] = mInterp5[1] = mInterp5[2] = mInterp5[3] =0;
	mInterp6[0] = mInterp6[1] = mInterp6[2] = mInterp6[3] =0;
	mInterp7[0] = mInterp7[1] = mInterp7[2] = mInterp7[3] =0;
	mInterp8[0] = mInterp8[1] = mInterp8[2] = mInterp8[3] =0;

    mTangent[0] = mTangent[1] = mTangent[2] = 0;
    mBiNormal[0] = mBiNormal[1] = mBiNormal[2] = 0;
    mWeight[0] = 1; mWeight[1] = 0; mWeight[2] = 0; mWeight[3] = 0;
    mBone[0] = mBone[1] = mBone[2] = mBone[3] = 0;
    mRadius = 0; // use for cloth simulations
  }

  bool  operator==(const MeshVertex &v) const
  {
    bool ret = false;

    if ( memcmp(this,&v,sizeof(MeshVertex)) == 0 ) ret = true;

    return ret;
  }

  HaF32          mPos[3];
  HaF32          mNormal[3];
  HaU32   		 mColor;
  HaF32          mTexel1[2];
  HaF32          mTexel2[2];
  HaF32          mTexel3[2];
  HaF32          mTexel4[2];
  HaF32          mInterp1[4];
  HaF32          mInterp2[4];
  HaF32          mInterp3[4];
  HaF32          mInterp4[4];
  HaF32          mInterp5[4];
  HaF32          mInterp6[4];
  HaF32          mInterp7[4];
  HaF32          mInterp8[4];
  HaF32          mTangent[3];
  HaF32          mBiNormal[3];
  HaF32          mWeight[4];
  unsigned short mBone[4];
  HaF32          mRadius;
};

class MeshBone
{
public:
	MeshBone(void)
	{
		mParentIndex = -1;
		mName = "";
		Identity();
	}

  void Set(const char *name,HaI32 parentIndex,const HaF32 pos[3],const HaF32 rot[4],const HaF32 scale[3])
  {
    mName = name;
    mParentIndex = parentIndex;
    mPosition[0] = pos[0];
    mPosition[1] = pos[1];
    mPosition[2] = pos[2];
    mOrientation[0] = rot[0];
    mOrientation[1] = rot[1];
    mOrientation[2] = rot[2];
    mOrientation[3] = rot[3];
    mScale[0] = scale[0];
    mScale[1] = scale[1];
    mScale[2] = scale[2];
  }

	void Identity(void)
	{
		mPosition[0] = 0;
		mPosition[1] = 0;
		mPosition[2] = 0;

		mOrientation[0] = 0;
		mOrientation[1] = 0;
		mOrientation[2] = 0;
		mOrientation[3] = 1;

    mScale[0] = 1;
    mScale[1] = 1;
    mScale[2] = 1;

	}

	void SetName(const char *name)
	{
    mName = name;
	}

	const char * GetName(void) const { return mName; };

	HaI32 GetParentIndex(void) const { return mParentIndex; };

	const HaF32 * GetPosition(void) const { return mPosition; };
	const HaF32 * GetOrientation(void) const { return mOrientation; };
  const HaF32 * GetScale(void) const { return mScale; };

  void getAngleAxis(HaF32 &angle,HaF32 *axis) const
  {
    fmi_getAngleAxis(angle,axis,mOrientation);
  }

  void setOrientationFromAxisAngle(const HaF32 axis[3],HaF32 angle)
  {
    fmi_setOrientationFromAxisAngle(axis,angle,mOrientation);
  }

	const char   *mName;
	HaI32           mParentIndex;          // array index of parent bone
	HaF32         mPosition[3];
	HaF32         mOrientation[4];
	HaF32         mScale[3];
};

class MeshEntry
{
public:
  MeshEntry(void)
  {
    mName = "";
    mBone = 0;
  }
  const char *mName;
	HaI32         mBone;         // bone this mesh is associcated
};

class MeshSkeleton
{
public:
	MeshSkeleton(void)
	{
    mName = "";
		mBoneCount = 0;
		mBones = 0;
	}

	void SetName(const char *name)
	{
    mName = name;
	}

	void SetBones(HaI32 bcount,MeshBone *bones) // memory ownership changes hands here!!!!!!!!!!
	{
		mBoneCount = bcount;
		mBones     = bones;
	}

	HaI32 GetBoneCount(void) const { return mBoneCount; };

	const MeshBone& GetBone(HaI32 index) const { return mBones[index]; };

	MeshBone * GetBonePtr(HaI32 index) const { return &mBones[index]; };

	void SetBone(HaI32 index,const MeshBone &b) { mBones[index] = b; };

	const char * GetName(void) const { return mName; };

	const char     *mName;
	HaI32             mBoneCount;
	MeshBone       *mBones;
};


class MeshAnimPose
{
public:
  MeshAnimPose(void)
  {
    mPos[0] = 0;
    mPos[1] = 0;
    mPos[2] = 0;
    mQuat[0] = 0;
    mQuat[1] = 0;
    mQuat[2] = 0;
    mQuat[3] = 0;
    mScale[0] = 1;
    mScale[1] = 1;
    mScale[2] = 1;
  }

	void SetPose(const HaF32 *pos,const HaF32 *quat,const HaF32 *scale)
	{
		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mPos[2] = pos[2];
		mQuat[0] = quat[0];
		mQuat[1] = quat[1];
		mQuat[2] = quat[2];
		mQuat[3] = quat[3];
    mScale[0] = scale[0];
    mScale[1] = scale[1];
    mScale[2] = scale[2];
	};

	void Sample(HaF32 *pos,HaF32 *quat,HaF32 *scale) const
	{
		pos[0] = mPos[0];
		pos[1] = mPos[1];
		pos[2] = mPos[2];
		quat[0] = mQuat[0];
		quat[1] = mQuat[1];
		quat[2] = mQuat[2];
		quat[3] = mQuat[3];
    scale[0] = mScale[0];
    scale[1] = mScale[1];
    scale[2] = mScale[2];
	}

  void getAngleAxis(HaF32 &angle,HaF32 *axis) const
  {
    fmi_getAngleAxis(angle,axis,mQuat);
  }

	HaF32 mPos[3];
	HaF32 mQuat[4];
  HaF32 mScale[3];
};

class MeshAnimTrack
{
public:
  MeshAnimTrack(void)
  {
    mName = "";
    mFrameCount = 0;
    mDuration = 0;
    mDtime = 0;
    mPose = 0;
  }

	void SetName(const char *name)
	{
    mName = name;
	}

	void SetPose(HaI32 frame,const HaF32 *pos,const HaF32 *quat,const HaF32 *scale)
	{
		if ( frame >= 0 && frame < mFrameCount )
			mPose[frame].SetPose(pos,quat,scale);
	}

	const char * GetName(void) const { return mName; };

	void SampleAnimation(HaI32 frame,HaF32 *pos,HaF32 *quat,HaF32 *scale) const
	{
		mPose[frame].Sample(pos,quat,scale);
	}

	HaI32 GetFrameCount(void) const { return mFrameCount; };

	MeshAnimPose * GetPose(HaI32 index) { return &mPose[index]; };

	const char *mName;
	HaI32       mFrameCount;
	HaF32     mDuration;
	HaF32     mDtime;
	MeshAnimPose *mPose;
};

class MeshAnimation
{
public:
  MeshAnimation(void)
  {
    mName = "";
    mTrackCount = 0;
    mFrameCount = 0;
    mDuration = 0;
    mDtime = 0;
    mTracks = 0;
  }


	void SetName(const char *name)
	{
    mName = name;
	}

	void SetTrackName(HaI32 track,const char *name)
	{
		mTracks[track]->SetName(name);
	}

	void SetTrackPose(HaI32 track,HaI32 frame,const HaF32 *pos,const HaF32 *quat,const HaF32 *scale)
	{
		mTracks[track]->SetPose(frame,pos,quat,scale);
	}

	const char * GetName(void) const { return mName; };

	const MeshAnimTrack * LocateTrack(const char *name) const
	{
		const MeshAnimTrack *ret = 0;
		for (HaI32 i=0; i<mTrackCount; i++)
		{
			const MeshAnimTrack *t = mTracks[i];
			if ( strcmp(t->GetName(),name) == 0 )
			{
				ret = t;
				break;
			}
		}
		return ret;
	}

	HaI32 GetFrameIndex(HaF32 t) const
	{
		t = fmodf( t, mDuration );
		HaI32 index = HaI32(t / mDtime);
		return index;
	}

	HaI32 GetTrackCount(void) const { return mTrackCount; };
	HaF32 GetDuration(void) const { return mDuration; };

	MeshAnimTrack * GetTrack(HaI32 index)
	{
		MeshAnimTrack *ret = 0;
		if ( index >= 0 && index < mTrackCount )
		{
			ret = mTracks[index];
		}
		return ret;
	};

	HaI32 GetFrameCount(void) const { return mFrameCount; };
	HaF32 GetDtime(void) const { return mDtime; };

  const char *mName;
	HaI32         mTrackCount;
	HaI32         mFrameCount;
	HaF32       mDuration;
	HaF32       mDtime;
	MeshAnimTrack **mTracks;
};



class MeshMaterial
{
public:
  MeshMaterial(void)
  {
    mName = 0;
    mMetaData = 0;
  }
  const char *mName;
  const char *mMetaData;
};

class MeshAABB
{
public:
  MeshAABB(void)
  {
    mMin[0] = FLT_MAX;
    mMin[1] = FLT_MAX;
    mMin[2] = FLT_MAX;
    mMax[0] = FLT_MIN;
    mMax[1] = FLT_MIN;
    mMax[2] = FLT_MIN;
  }

  void include(const HaF32 pos[3])
  {
    if ( pos[0] < mMin[0] ) mMin[0] = pos[0];
    if ( pos[1] < mMin[1] ) mMin[1] = pos[1];
    if ( pos[2] < mMin[2] ) mMin[2] = pos[2];
    if ( pos[0] > mMax[0] ) mMax[0] = pos[0];
    if ( pos[1] > mMax[1] ) mMax[1] = pos[1];
    if ( pos[2] > mMax[2] ) mMax[2] = pos[2];
  }
  HaF32 mMin[3];
  HaF32 mMax[3];
};

class SubMesh
{
public:
  SubMesh(void)
  {
    mMaterialName = 0;
    mMaterial     = 0;
    mVertexFlags  = 0;
    mTriCount     = 0;
    mIndices      = 0;
  }

  const char          *mMaterialName;
  MeshMaterial        *mMaterial;
  MeshAABB             mAABB;
  HaU32         mVertexFlags; // defines which vertex components are active.
  HaU32         mTriCount;    // number of triangles.
  HaU32        *mIndices;     // indexed triange list
};

class Mesh
{
public:
  Mesh(void)
  {
    mName         = 0;
    mSkeletonName = 0;
    mSkeleton     = 0;
    mSubMeshCount = 0;
    mSubMeshes    = 0;
    mVertexFlags  = 0;
    mVertexCount  = 0;
    mVertices     = 0;
  }
  const char         *mName;
  const char         *mSkeletonName;
  MeshSkeleton       *mSkeleton; // the skeleton used by this mesh system.
  MeshAABB            mAABB;
  HaU32        mSubMeshCount;
  SubMesh           **mSubMeshes;

  HaU32       mVertexFlags;  // combined vertex usage flags for all sub-meshes
  HaU32       mVertexCount;
  MeshVertex        *mVertices;

};

class MeshRawTexture
{
public:
  MeshRawTexture(void)
  {
    mName = 0;
    mData = 0;
    mWidth = 0;
    mHeight = 0;
    mBPP = 0;
  }
  const char    *mName;
  HaU8 *mData;
  HaU32   mWidth;
  HaU32   mHeight;
  HaU32		mBPP;
};

class MeshInstance
{
public:
  MeshInstance(void)
  {
    mMeshName = 0;
    mMesh     = 0;
    mPosition[0] = mPosition[1] = mPosition[2] = 0;
    mRotation[0] = mRotation[1] = mRotation[2] = mRotation[3] = 0;
    mScale[0] = mScale[1] = mScale[2] = 0;
  }
  const char  *mMeshName;
  Mesh        *mMesh;
  HaF32        mPosition[3];
  HaF32        mRotation[4]; //quaternion XYZW
  HaF32        mScale[3];
};

class MeshUserData
{
public:
  MeshUserData(void)
  {
    mUserKey = 0;
    mUserValue = 0;
  }
  const char *mUserKey;
  const char *mUserValue;
};

class MeshUserBinaryData
{
public:
  MeshUserBinaryData(void)
  {
    mName     = 0;
    mUserData = 0;
    mUserLen  = 0;
  }
  const char    *mName;
  HaU32   mUserLen;
  HaU8 *mUserData;
};

class MeshTetra
{
public:
  MeshTetra(void)
  {
    mTetraName  = 0;
    mMeshName   = 0;  // mesh the tetraheadral mesh is associated with.
    mMesh       = 0;
    mTetraCount = 0;
    mTetraData  = 0;
  }

  const char  *mTetraName;
  const char  *mMeshName;
  MeshAABB     mAABB;
  Mesh        *mMesh;
  HaU32 mTetraCount; // number of tetrahedrons
  HaF32       *mTetraData;
};

#define MESH_SYSTEM_VERSION 1 // version number of this data structure, used for binary serialization

enum MeshCollisionType
{
  MCT_BOX,
  MCT_SPHERE,
  MCT_CAPSULE,
  MCT_CONVEX,
  MCT_LAST
};

class MeshCollision
{
public:
  MeshCollision(void)
  {
    mType = MCT_LAST;
    mName = 0;
    fmi_identity(mTransform);
  }

  MeshCollisionType getType(void) const { return mType; };

  MeshCollisionType mType;
  const char       *mName;  // the bone this collision geometry is associated with.
  HaF32             mTransform[16];   // local transform.
};

class MeshCollisionBox : public MeshCollision
{
public:
  MeshCollisionBox(void)
  {
    mType = MCT_BOX;
    mSides[0] = mSides[1] = mSides[2] = 1;
  }
  HaF32 mSides[3];
};

class MeshCollisionSphere : public MeshCollision
{
public:
  MeshCollisionSphere(void)
  {
    mType = MCT_SPHERE;
    mRadius = 1;
  }
  HaF32 mRadius;
};

class MeshCollisionCapsule : public MeshCollision
{
public:
  MeshCollisionCapsule(void)
  {
    mType = MCT_CAPSULE;
    mRadius = 1;
    mHeight = 1;
  }
  HaF32  mRadius;
  HaF32  mHeight;
};

class MeshConvex
{
public:
  MeshConvex(void)
  {
    mVertexCount = 0;
    mVertices = 0;
    mTriCount = 0;
    mIndices = 0;
  }
  HaU32  mVertexCount;
  HaF32        *mVertices;
  HaU32  mTriCount;
  HaU32 *mIndices;
};

class MeshCollisionConvex : public MeshCollision, public MeshConvex
{
public:
  MeshCollisionConvex(void)
  {
    mType = MCT_CONVEX;
  }


};

class MeshCollisionRepresentation
{
public:
  MeshCollisionRepresentation(void)
  {
    mName = 0;
    mInfo = 0;
    mCollisionCount = 0;
    mCollisionGeometry = 0;
  }
  const char     *mName;
  const char     *mInfo;
  HaU32    mCollisionCount;
  MeshCollision **mCollisionGeometry;
};

class MeshSystem
{
public:
  MeshSystem(void)
  {
    mAssetName           = 0;
    mAssetInfo           = 0;
    mTextureCount        = 0;
    mTextures            = 0;
    mSkeletonCount       = 0;
    mSkeletons           = 0;
    mAnimationCount      = 0;
    mAnimations          = 0;
    mMaterialCount       = 0;
    mMaterials           = 0;
    mMeshCount           = 0;
    mMeshes              = 0;
    mMeshInstanceCount   = 0;
    mMeshInstances       = 0;
    mUserDataCount       = 0;
    mUserData            = 0;
    mUserBinaryDataCount = 0;
    mUserBinaryData      = 0;
    mTetraMeshCount      = 0;
    mTetraMeshes         = 0;
    mMeshSystemVersion   = MESH_SYSTEM_VERSION;
    mAssetVersion        = 0;
    mMeshCollisionCount  = 0;
    mMeshCollisionRepresentations = 0;
	mPlane[0] = 1;
	mPlane[1] = 0;
	mPlane[2] = 0;
	mPlane[3] = 0;
  }


  const char           *mAssetName;
  const char           *mAssetInfo;
  HaI32                   mMeshSystemVersion;
  HaI32                   mAssetVersion;
  MeshAABB              mAABB;
  HaU32          mTextureCount;          // Are textures necessary? [rgd].
  MeshRawTexture      **mTextures;              // Texture storage in mesh data is rare, and the name is simply an attribute of the material

  HaU32          mTetraMeshCount;        // number of tetrahedral meshes
  MeshTetra           **mTetraMeshes;           // tetraheadral meshes

  HaU32          mSkeletonCount;         // number of skeletons
  MeshSkeleton        **mSkeletons;             // the skeletons.

  HaU32          mAnimationCount;
  MeshAnimation       **mAnimations;

  HaU32          mMaterialCount;         // Materials are owned by this list, merely referenced later.
  MeshMaterial         *mMaterials;

  HaU32          mUserDataCount;
  MeshUserData        **mUserData;

  HaU32          mUserBinaryDataCount;
  MeshUserBinaryData  **mUserBinaryData;

  HaU32          mMeshCount;
  Mesh                **mMeshes;

  HaU32          mMeshInstanceCount;
  MeshInstance         *mMeshInstances;

  HaU32          mMeshCollisionCount;
  MeshCollisionRepresentation **mMeshCollisionRepresentations;

  HaF32                 mPlane[4];

};


class MeshImportInterface
{
public:
  virtual void        importMaterial(const char *matName,const char *metaData) = 0;        // one material
  virtual void        importUserData(const char *userKey,const char *userValue) = 0;       // carry along raw user data as ASCII strings only..
  virtual void        importUserBinaryData(const char *name,HaU32 len,const HaU8 *data) = 0;
  virtual void        importTetraMesh(const char *tetraName,const char *meshName,HaU32 tcount,const HaF32 *tetraData) = 0;

  virtual void        importAssetName(const char *assetName,const char *info) = 0;         // name of the overall asset.
  virtual void        importMesh(const char *meshName,const char *skeletonName) = 0;       // name of a mesh and the skeleton it refers to.

  virtual void        importTriangle(const char *meshName,
                                     const char *materialName,
                                     HaU32 vertexFlags,
                                     const MeshVertex &v1,
                                     const MeshVertex &v2,
                                     const MeshVertex &v3) = 0;

  virtual void        importIndexedTriangleList(const char *meshName,
                                                const char *materialName,
                                                HaU32 vertexFlags,
                                                HaU32 vcount,
                                                const MeshVertex *vertices,
                                                HaU32 tcount,
                                                const HaU32 *indices) = 0;

  virtual void        importAnimation(const MeshAnimation &animation) = 0;
  virtual void        importSkeleton(const MeshSkeleton &skeleton) = 0;
  virtual void        importRawTexture(const char *textureName,const HaU8 *pixels,HaU32 wid,HaU32 hit) = 0;
  virtual void        importMeshInstance(const char *meshName,const HaF32 pos[3],const HaF32 rotation[4],const HaF32 scale[3])= 0;

  virtual void importCollisionRepresentation(const char *name,const char *info) = 0; // the name of a new collision representation.

  virtual void importConvexHull(const char *collision_rep,    // the collision representation it is associated with
                                const char *boneName,         // the name of the bone it is associated with in a skeleton.
                                const HaF32 *transform,       // the full 4x4 transform for this hull, null if in world space.
                                HaU32 vertex_count,
                                const HaF32 *vertices,
                                HaU32 tri_count,
                                const HaU32 *indices) = 0;

  virtual void importSphere(const char *collision_rep,    // the collision representation it is associated with
                            const char *boneName,         // the name of the bone it is associated with in a skeleton.
                            const HaF32 *transform,
                            HaF32 radius) = 0;

  virtual void importCapsule(const char *collision_rep,    // the collision representation it is associated with
                                const char *boneName,         // the name of the bone it is associated with in a skeleton.
                                const HaF32 *transform,       // the full 4x4 transform for this hull, null if in world space.
                                HaF32 radius,
                                HaF32 height) = 0;

  virtual void importOBB(const char *collision_rep,    // the collision representation it is associated with
                         const char *boneName,         // the name of the bone it is associated with in a skeleton.
                         const HaF32 *transform,       // the full 4x4 transform for this hull, null if in world space.
                         const HaF32 *sides) = 0;


  virtual HaI32 getSerializeFrame(void) = 0;

  virtual void importPlane(const HaF32 *p) = 0;

};

// allows the application to load external resources.
// For example, when loading wavefront OBJ files, the materials are saved in a seperate file.
// This interface allows the application to load the resource.
class MeshImportApplicationResource
{
public:
  virtual void * getApplicationResource(const char *base_name,const char *resource_name,HaU32 &len) = 0;
  virtual void   releaseApplicationResource(void *mem) = 0;
};



class MeshImporter
{
public:
  virtual HaI32              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  virtual const char *     getExtension(HaI32 index=0) = 0; // report the default file name extension for this mesh type.
  virtual const char *     getDescription(HaI32 index=0) = 0; // report the ascii description of the import type.

  virtual bool             importMesh(const char *meshName,const void *data,HaU32 dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource) = 0;

  virtual const void * saveMeshSystem(MeshSystem * /*ms*/,HaU32 & /*dlen*/,bool /*binary*/) 
  {
	return NULL;
  }
  virtual void releaseSavedMeshSystem(const void * /*mem*/) 
  {

  }
};

enum MeshSerializeFormat
{
  MSF_EZMESH, // save it back out into ez-mesh, lossless XML format.
  MSF_OGRE3D, // save it back out into the Ogre3d XML format.
  MSF_WAVEFRONT, // save as wavefront OBJ
  MSF_PSK, // save it back out as a PSK format file.
  MSF_FBX,  // FBX import is supported by FBX output is not yet.
  MSF_ARM_XML,   // APEX render mesh
  MSF_ARM_BINARY, // apex render mesh in binary
  MSF_MSH,        // custom binary .MSH file format used by the Planetside game engine
  MSF_SCARAB_3D, // the scarab binary 3d file format.
  MSF_SCARAB_TRI,
  MSF_GR2,		// granny 2 file format.
  MSF_EZB,		// A binary representation of an ez-mesh; used for faster loading.  Should be endian aware/safe.  Avoids string parsing and tens of thousands of ascii to float conversions.
  MSF_POVRAY,	// export in POVRay format (suitable for raytracing)
  MSF_LAST
};


class MeshBoneInstance
{
public:
  MeshBoneInstance(void)
  {
    mBoneName = "";
  }

  void composeInverse(void)
  {
    fmi_inverseTransform(mTransform,mInverseTransform);
  }

  const char *mBoneName;                     // the name of the bone
  HaI32         mParentIndex;                  // the parent index
  HaF32       mLocalTransform[16];
  HaF32       mTransform[16];                // the transform in world space
  HaF32       mAnimTransform[16];            // the sampled animation transform, multiplied times the inverse root transform.
  HaF32       mCompositeAnimTransform[16];   // teh composite transform
  HaF32       mInverseTransform[16];         // the inverse transform
};

class MeshSkeletonInstance
{
public:
  MeshSkeletonInstance(void)
  {
    mName      = "";
    mBoneCount = 0;
    mBones     = 0;
  }

  const char        *mName;
  HaI32                mBoneCount;
  MeshBoneInstance  *mBones;
};

class MeshSerialize
{
public:
  MeshSerialize(MeshSerializeFormat format)
  {
    mFormat = format;
    mBaseData = 0;
    mBaseLen  = 0;
    mExtendedData = 0;
    mExtendedLen = 0;
    mSaveFileName = 0;
    fmi_identity(mExportTransform);
  }
  MeshSerializeFormat mFormat;
  HaU8      *mBaseData;
  HaU32        mBaseLen;
  HaU8      *mExtendedData;
  HaU32        mExtendedLen;
  const char         *mSaveFileName; // need to know the name of the save file name for OBJ and Ogre3d format.
  HaF32               mExportTransform[16]; // matrix transform on export
};


class MeshSystemContainer;

class VertexIndex
{
public:
  virtual HaU32    getIndex(const HaF32 pos[3],bool &newPos) = 0;  // get welded index for this HaF32 vector[3]
  virtual const HaF32 *   getVertices(void) const = 0;
  virtual const HaF32 *   getVertex(HaU32 index) const = 0;
  virtual HaU32    getVcount(void) const = 0;
};

class MeshImport
{
public:
  virtual void             addImporter(MeshImporter *importer) = 0; // add an additional importer

  virtual bool             importMesh(const char *meshName,const void *data,HaU32 dlen,MeshImportInterface *callback,const char *options) = 0;

  virtual MeshSystemContainer *     createMeshSystemContainer(void) = 0;

  virtual MeshSystemContainer *     createMeshSystemContainer(const char *meshName,
                                                              const void *data,
                                                              HaU32 dlen,
                                                              const char *options) = 0; // imports and converts to a single MeshSystem data structure

  virtual void             releaseMeshSystemContainer(MeshSystemContainer *mesh) = 0;

  virtual MeshSystem *     getMeshSystem(MeshSystemContainer *mb) = 0;

  virtual bool             serializeMeshSystem(MeshSystem *mesh,MeshSerialize &data) = 0;
  virtual void             releaseSerializeMemory(MeshSerialize &data) = 0;


  virtual HaI32              getImporterCount(void) = 0;
  virtual MeshImporter    *getImporter(HaI32 index) = 0;

  virtual MeshImporter *   locateMeshImporter(const char *fname) = 0; // based on this file name, find a matching mesh importer.

  virtual const char  *     getFileRequestDialogString(void) = 0;

  virtual void             setMeshImportApplicationResource(MeshImportApplicationResource *resource) = 0;

// convenience helper functions.
  virtual MeshSkeletonInstance *createMeshSkeletonInstance(const MeshSkeleton &sk) = 0;
  virtual bool                  sampleAnimationTrack(HaI32 trackIndex,const MeshSystem *msystem,MeshSkeletonInstance *skeleton) = 0;
  virtual void                  releaseMeshSkeletonInstance(MeshSkeletonInstance *sk) = 0;

  // apply bone weighting transforms to this vertex buffer.
  virtual void transformVertices(HaU32 vcount,
                                 const MeshVertex *source_vertices,
                                 MeshVertex *dest_vertices,
                                 MeshSkeletonInstance *skeleton) = 0;

  virtual MeshImportInterface * getMeshImportInterface(MeshSystemContainer *msc) = 0;

  virtual void gather(MeshSystemContainer *msc) = 0;

  virtual void scale(MeshSystemContainer *msc,HaF32 scale) = 0;
  virtual void rotate(MeshSystemContainer *msc,HaF32 rotX,HaF32 rotY,HaF32 rotZ) = 0; // rotate mesh system using these euler angles expressed as degrees.

  virtual VertexIndex *            createVertexIndex(HaF32 granularity) = 0;  // create an indexed vertext system for floats
  virtual void                     releaseVertexIndex(VertexIndex *vindex) = 0;
protected:
  virtual ~MeshImport(void) { };

};


#define MESHIMPORT_VERSION 11  // version 0.01  increase this version number whenever an interface change occurs.


extern MeshImport *gMeshImport; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.


MeshImport * loadMeshImporters(const char *directory); // loads the mesh import library (dll) and all available importers from the same directory.


}; // End of namespace for hacd

#ifdef PLUGINS_EMBEDDED

extern "C"
{
	hacd::MeshImport * getInterfaceMeshImport(HaI32 version_number);
	hacd::MeshImporter * getInterfaceMeshImportEzm(HaI32 version_number);
};
#endif

#pragma warning(pop)

#endif
