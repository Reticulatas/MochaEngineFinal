/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


//-------------------------------BOOST SERIALIZER CONVERSIONS--------------------------
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/split_free.hpp>

#include "MathIncludes.h"
 
#include "GameObject.h"
#include "MatAttribNode.h"
#include "UVTransformNode.h"
#include "ColorNode.h"
#include "TextureNode.h"
#include "CollisionShape.h"
#include "IRegisterable.h"
#include "CParticleEmitter.h"
 
#include "MochaMath.h"
#include "Asset.h"
#include "Mesh.h"
#include "Sound.h"
#include "Shader.h"

namespace boost { namespace serialization {
template <class Archive>
void serialize(Archive &ar, Math::Vector2& t, const BOOST_PFTO unsigned int) {
ar & ___P_B_NVP(t.x);
ar & ___P_B_NVP(t.y);
}
template <class Archive>
void serialize(Archive &ar, Math::Vector3& t, const BOOST_PFTO unsigned int) {
ar & ___P_B_NVP(t.x);
ar & ___P_B_NVP(t.y);
ar & ___P_B_NVP(t.z);
}
template <class Archive>
void serialize(Archive &ar, Vector4& t, const BOOST_PFTO unsigned int) {
ar & ___P_B_NVP(t.x);
ar & ___P_B_NVP(t.y);
ar & ___P_B_NVP(t.z);
ar & ___P_B_NVP(t.w);
}
template <class Archive>
void serialize(Archive &ar, Matrix4& t, const BOOST_PFTO unsigned int) {
ar & ___P_B_NVP(t.array);
}
template <class Archive>
void serialize(Archive &ar, Matrix3& t, const BOOST_PFTO unsigned int) {
  ar & ___P_B_NVP(t.array);
}
template <class Archive>
void serialize(Archive &ar, Quaternion& t, const BOOST_PFTO unsigned int) {
  ar & ___P_B_NVP(t.data);
}
template <class Archive>
void serialize(Archive &ar, EulerAngles& t, const BOOST_PFTO unsigned int) {
  ar & ___P_B_NVP(t.Angles);
}
template <class Archive>
void serialize(Archive &ar, EmitterProperties& t, const BOOST_PFTO unsigned int ver) {
    if (ver >= 1) ar & ___P_B_NVP(t.mEmissionAngleVariance); 
	if (ver >= 1) ar & ___P_B_NVP(t.mIsEmitting);
	if (ver >= 1) ar & ___P_B_NVP(t.mMaxLifetime);
	if (ver >= 1) ar & ___P_B_NVP(t.mMinLifetime);
    if (ver >= 1) ar & ___P_B_NVP(t.mFadeStartAmt);
    if (ver >= 1) ar & ___P_B_NVP(t.mFadeEndAmt);
    if (ver >= 1) ar & ___P_B_NVP(t.mUsedParticles);
    if (ver >= 1) ar & ___P_B_NVP(t.mMinVel);
    if (ver >= 1) ar & ___P_B_NVP(t.mMaxVel);
    if (ver >= 1) ar & ___P_B_NVP(t.mStartSize);
    if (ver >= 1) ar & ___P_B_NVP(t.mEndSize);
    if (ver >= 1) ar & ___P_B_NVP(t.mStartColour);
    if (ver >= 1) ar & ___P_B_NVP(t.mEndColour);
	if (ver >= 1) ar & ___P_B_NVP(t.mGravityFactor);
    if (ver >= 1) ar & ___P_B_NVP(t.mRandomDirection);
    if (ver >= 1) ar & ___P_B_NVP(t.mAttractorPos);
    if (ver >= 1) ar & ___P_B_NVP(t.mAttracted);
    if (ver >= 1) ar & ___P_B_NVP(t.mAttractorLocalSpace);
    if (ver >= 1) ar & ___P_B_NVP(t.mAttractorFactor);
    if (ver >= 1) ar & ___P_B_NVP(t.mNormal);
    if (ver >= 1) ar & ___P_B_NVP(t.mPlaneEmit);
    if (ver >= 1) ar & ___P_B_NVP(t.mType);
    if (ver >= 1) ar & ___P_B_NVP(t.mWarmCount);
    if (ver >= 1) ar & ___P_B_NVP(t.mSpawnInGeometry);
    if (ver >= 1) ar & ___P_B_NVP(t.mDieInGeometry);
    if (ver >= 2) ar & ___P_B_NVP(t.mEmissionAngle);
    if (ver >= 2) ar & ___P_B_NVP(t.mRadius);
    if (ver >= 3) ar & ___P_B_NVP(t.mIsOneShot);
}

//template <class Archive>
//void load(Archive &ar, XMVECTOR& t, const BOOST_PFTO unsigned int) {
//	XMFLOAT4 c;
//	ar & ___P_B_NVP(c);
//	t = XMVectorSet(c.x, c.y, c.z, c.w);
//}
//template <class Archive>
//void save(Archive &ar, const XMVECTOR& t, const BOOST_PFTO unsigned int) {
//	XMFLOAT4 xmvector_c;
//	XMStoreFloat3(&xmvector_c, t);
//	ar & ___P_B_NVP(xmvector_c);
//}
} }
//BOOST_SERIALIZATION_SPLIT_FREE(XMVECTOR);