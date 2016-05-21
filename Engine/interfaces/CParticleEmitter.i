%{
#include "CParticleEmitter.h"
%}

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

struct EmitterProperties
{
    unsigned mUsedParticles;
	int mWarmCount;
    float mMinLifetime;
	float mMaxLifetime;
	
	float mMinVel;
	float mMaxVel;

    float mStartSize;
    float mEndSize;
	
	float mFadeStartAmt;
    float mFadeEndAmt;
	
	Color mStartColour;
    Color mEndColour;
	
	float mEmissionAngleVariance;
	Math::Vector3 mEmissionAngle;
	
	bool mIsEmitting;
    
    float mGravityFactor;
    float mRadius; // CIRCLE EMISSION VARIABLES
	
    // ATTRACTOR
    Math::Vector3 mAttractorPos;
    bool mAttracted;
	bool mAttractorLocalSpace;
    float mAttractorFactor;
	bool mIsOneShot;
	
	bool mRandomDirection;
	bool mDeceleration;
    //BOX EMISSION VARIABLES
    bool mSpawnInGeometry;
    bool mDieInGeometry;

    // PLANE EMISSION
    Math::Vector3 mNormal;
    bool mPlaneEmit;

	ParticleEmissionType mType;
	

    
};
class CParticleEmitter : public Component
{
	CParticleEmitter();
	virtual ~CParticleEmitter();

public:

	GPUShader * GetGPUShader() const { return mComputeShader; }
	void        SetGPUShader(GPUShader * val) { mComputeShader = val; }
	ParticleBehaviour GetBehaviour() const { return mBehaviour; }
	void		GetBehaviour(ParticleBehaviour val) { mBehaviour = val; }
	
	void     TriggerOneShot(void);
	
	EmitterProperties mEmitterProperty;

};
