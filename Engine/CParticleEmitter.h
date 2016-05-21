#pragma once
#include "MochaMath.h"
#include "MathIncludes.h"
#include "Component.h"
#include <vector>
#include <list>
#include "IRegisterable.h"
#include "GPUShader.h"
 

enum ParticleBehaviour
{
	PB_DEFAULT = 0,
	PB_IMPLOSION,
	PB_EXPLOSION,
	PB_ORBIT,
	PB_3DORBIT,
	TOTAL_BEHAVIOURS
};
enum ParticleEmissionType
{
    PE_POINT = 0,
    PE_BOX,
    PE_SPHERE,
    TOTAL_EMITTYPES
};


struct Particle 
{
   Vector3 mPos;
   Vector3 mVel;
   float mAliveTime;
   float mLifetime;
   float mParticleSize;
   Color mColor;
   //Vector3 mAcceleration;
   //bool mIsAlive;

   Particle();
   Particle(Vector3 startPos);
};




struct EmitterProperties
{
	unsigned mGroupDimX;
	unsigned mGroupDimY;
	unsigned mMaxParticles;
    unsigned mUsedParticles;

    float mMinLifetime;
	float mMaxLifetime;

    float mMinVel;
    float mMaxVel;
	

    float mFadeStartAmt;
    float mFadeEndAmt;
    float mDT;
    float mTotalGameTime;

	float mEmissionAngleVariance;
	Vector3 mEmissionAngle;
	
	int mIsEmitting;
    Vector3 mEmitterPos;

    Vector3 mCamPos;
    int mRandomDirection;

    float mGravityFactor;
    float mRadius; // CIRCLE EMISSION VARIABLES
    float mStartSize;
    float mEndSize;

    Color mStartColour;
    Color mEndColour;
    // ATTRACTOR
    Vector3 mAttractorPos;
    int mAttracted;

    int  mAttractorLocalSpace;
    float mAttractorFactor;
    //float padding2[2];
    int   mIsOneShot;
    float mInternalTimer;

    //BOX EMISSION VARIABLES
    Vector3 mHalfLengths;
    ParticleEmissionType mType;

    int mDeceleration;
    int mSpawnInGeometry;
    int mDieInGeometry;
    int mWarmCount;

    // PLANE EMISSION
    Vector3 mNormal;
    int mPlaneEmit;

    // INHERITED VELOCITY
    Vector3 mInheritedVelocity;
    float mInheritedFactor;

    //MATRICES AND STUFF
    Matrix4 mView;
    Matrix4 mViewProj;
    float mNearPlane;
    float mFarPlane;
    float mCurrDT;
    float padding;
    

};
___SERIALIZE_CLASS(EmitterProperties, 3);



struct ParticlePropNode
{
	enum INTERPTYPE
	{
		LINEAR, INVLINEAR, 
		QUADRATIC, INVQUADRATIC,
		//LOG, INVLOG,
		CONSTANT
	};
	static float UnifiedERP(INTERPTYPE typ, float b, float e, float t)
	{
		switch (typ)
		{
		case LINEAR:
			return Math::Lerp(b, e, t);
			break;
		case INVLINEAR:
			return Math::Lerp(b, e, 1.0f - t);
			break;
		case QUADRATIC:
			return Math::Cerp(b, e, t);
			break;
		case INVQUADRATIC:
			return Math::Cerp(b, e, 1.0f - t);
			break;
		case CONSTANT:
			return e;
			break;
		}
	}

	virtual void Start(Particle& p) const = 0;
	//life time percentage is scaled to be from delay(0)->endlife(1)
	virtual void Tick(Particle& p, float lifetimeperc) const = 0;
	float delay;
	INTERPTYPE interpolation_type;

	ParticlePropNode() : delay(0.0f), interpolation_type(LINEAR) { }
};

struct PProp_ChangeVelocity : public ParticlePropNode
{
	Vector3 from;
	Vector3 to;
	void Start(Particle& p) const override	{ p.mVel = from; }
	void Tick(Particle& p, float lifetimeeperc) const override {
		p.mVel.x = UnifiedERP(interpolation_type, from.x, to.x, lifetimeeperc);
		p.mVel.y = UnifiedERP(interpolation_type, from.y, to.y, lifetimeeperc);
		p.mVel.z = UnifiedERP(interpolation_type, from.z, to.z, lifetimeeperc);
	}
};

struct PProp_ChangeAcceleration : public ParticlePropNode
{
	Vector3 from;
	Vector3 to;
	void Start(Particle& p) const override	{ p.mVel += from; }
	void Tick(Particle& p, float lifetimeeperc) const override {
		p.mVel.x += UnifiedERP(interpolation_type, from.x, to.x, lifetimeeperc);
		p.mVel.y += UnifiedERP(interpolation_type, from.y, to.y, lifetimeeperc);
		p.mVel.z += UnifiedERP(interpolation_type, from.z, to.z, lifetimeeperc);
	}
};

struct PProp_ChangeColor : public ParticlePropNode
{
	Color from;
	Color to;
	void Start(Particle& p) const override	{ p.mColor = from;  }
	void Tick(Particle& p, float lifetimeeperc) const override {
		p.mColor.r = UnifiedERP(interpolation_type, from.r, to.r, lifetimeeperc);
		p.mColor.g = UnifiedERP(interpolation_type, from.g, to.g, lifetimeeperc);
		p.mColor.b = UnifiedERP(interpolation_type, from.b, to.b, lifetimeeperc);
	}
};

struct PProp_ChangeSize : public ParticlePropNode
{
	float from;
	float to;
	void Start(Particle& p) const override	{ p.mParticleSize = from;  }
	void Tick(Particle& p, float lifetimeeperc) const override {
		p.mParticleSize = UnifiedERP(interpolation_type, from, to, lifetimeeperc);
	}
};

struct CullingData
{
    Vector3 mX, mY, mZ;
    Vector3 mOrigin;
    float mExtents[3];
};

class CParticleEmitter : public Component
{
public:

  CParticleEmitter();
  virtual ~CParticleEmitter();

  void OnStart();
  void OnUpdate();
  void OnEditorUpdate();
  void OnInit();
  void OnFree();
  void OnDraw();

  void UpdateEmitterProperties(CCamera* cam);

  void CalculateAABBCullingData(void);
  void UpdateOneShotTimer(void);
  void TriggerOneShot(void);
  GPUShader * GetGPUShader() const { return mComputeShader; }
  void        SetGPUShader(GPUShader * val) { mComputeShader = val; }
  ParticleBehaviour GetBehaviour() const { return mBehaviour; }
  void				GetBehaviour(ParticleBehaviour val) { mBehaviour = val; }

  void CalculateThreadGroups(void);
  void SetDebugMesh(void);
  template <typename T>
  T* AddProperty() {
	  static_assert<std::is_base_of<ParticlePropNode, T>::value>("Property Nodes must have base ParticlePropNode");
	  mProps.emplace_back(T t());
	  return &mProps.back();
  }

  EmitterProperties mEmitterProperty;
  ParticleBehaviour mBehaviour;
  CullingData       mCullingData;
  bool  mDrawEmissionBox;
  bool  mDrawBoundingBox;

  float mEBoxWidth;
  float mEBoxHeight;

  unsigned    mThreadGroup[3];
  unsigned    mThreadCount;

  bool mNeedsPreWarm;
  bool mPreWarmed;
  virtual void CopyFrom(const ::CParticleEmitter* c)
  {
	  //mEmitterProperty.mIsEmitting = c->mEmitterProperty.mIsEmitting;
	  //mEmitterProperty.mMaxParticles = c->mEmitterProperty.mMaxParticles;
	  //mEmitterProperty.mEmissionAngle = c->mEmitterProperty.mEmissionAngle;
	  //mEmitterProperty.mEmissionAngleVariance = c->mEmitterProperty.mEmissionAngleVariance;
      //mEmitterProperty.mGravityFactor = c->mEmitterProperty.mGravityFactor;
	  //mEmitterProperty.mMinLifetime = c->mEmitterProperty.mMinLifetime;
	  //mEmitterProperty.mMaxLifetime = c->mEmitterProperty.mMaxLifetime;
	  //mEmitterProperty.mMaxVel = c->mEmitterProperty.mMaxVel;
	  //mEmitterProperty.mMinVel = c->mEmitterProperty.mMinVel;
      //mEmitterProperty.mAttracted = c->mEmitterProperty.mAttracted;
      //mEmitterProperty.mAttractorFactor = c->mEmitterProperty.mAttractorFactor;
      //mEmitterProperty.mAttractorLocalSpace = c->mEmitterProperty.mAttractorLocalSpace;
      //mEmitterProperty.mAttractorPos = c->mEmitterProperty.mAttractorPos;
      //mEmitterProperty.mDieInGeometry = c->mEmitterProperty.mDieInGeometry;
      //mEmitterProperty.mSpawnInGeometry = c->mEmitterProperty.mSpawnInGeometry;
      //mEmitterProperty.mStartColour = c->mEmitterProperty.mStartColour;
      //mEmitterProperty.mEndColour = c->mEmitterProperty.mEndColour;
      //mEmitterProperty.mStartSize = c->mEmitterProperty.mStartSize;
      //mEmitterProperty.mEndSize = c->mEmitterProperty.mEndSize;
      mEmitterProperty = c->mEmitterProperty;
      mEmitterProperty.mTotalGameTime = 0.0f;
      mNeedsPreWarm = c->mNeedsPreWarm;
      mPreWarmed = c->mPreWarmed;
      
      mThreadGroup[0] = c->mThreadGroup[0];
      mThreadGroup[1] = c->mThreadGroup[1];
      mThreadGroup[2] = c->mThreadGroup[2];
      mThreadCount = c->mThreadCount;
	  mCurrentParticleCount = c->mCurrentParticleCount;
	  mAccumulatedTime = c->mAccumulatedTime;
	  mDrawBoundingBox = c->mDrawBoundingBox;
	  mDrawEmissionBox = c->mDrawEmissionBox;
	  mEBoxWidth = c->mEBoxWidth;
	  mEBoxHeight = c->mEBoxHeight;
      Component::CopyFrom(c);
  }
  COMPONENTCLONE(CParticleEmitter);

private:
	GPUShader * mComputeShader = nullptr;
	
	AssetHandle mOldAsset;


  // Bounding Box
  float mWidth;
  float mHeight;
  Vector3 mBBox[4];

  int mCurrentParticleCount;
  float mAccumulatedTime;


  ___SERIALIZE_BEGIN___
	  ___DEFBASE(Component)
	  ___DEFSER(mEmitterProperty, 1)
	  ___DEFSER(mDrawBoundingBox, 1)
	  ___DEFSER(mDrawEmissionBox, 1)
	  ___DEFSERONLYORLESS(mCurrentParticleCount, 1)
	  ___DEFSERONLYORLESS(mAccumulatedTime, 1)
	  ___DEFSER(mEBoxWidth, 1)
	  ___DEFSER(mEBoxHeight, 1)
    ___SERIALIZE_END___

public:
	metadef(CParticleEmitter)
		endmetadef;
};

___SERIALIZE_CLASS(CParticleEmitter, 2);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change