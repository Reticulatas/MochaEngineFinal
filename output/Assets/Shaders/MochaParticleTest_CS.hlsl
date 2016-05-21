#include "MochaParticleBehaviour.hlsl"
#define THREAD_BLOCK 256
cbuffer EmitterProperties : register(b0)
{
    uint mGroupDimX;
    uint mGroupDimY;
    uint mMaxParticles;
    uint mUsedParticles;

    float mMinLifetime;
    float mMaxLifetime;
    float mMinVel;
    float mMaxVel;
    

    float mFadeStartAmt;
    float mFadeEndAmt;
    float mDT;
    float mTotalGameTime;

    float  mEmissionAngleVariance;
    float3 mEmissionAngle;

    bool  mIsEmitting;
    float3 mEmitterPos;

    float3 mCamPos;
    int    mRandomDirection;

    float mGravityFactor;
    float mRadius; // CIRCLE EMISSION VARIABLES
    float mStartSize;
    float mEndSize;

    float4 mStartColour;
    float4 mEndColour;

    // ATTRACTOR
    float3 mAttractorPos;
    int    mAttracted;

    int  mAttractorLocalSpace;
    float mAttractorFactor;
    float2 padding2;

    //BOX EMISSION VARIABLES
    float3 mHalfLengths;
    int    mType;

    int mDeceleration;
    int mSpawnInGeometry;
    int mDieInGeometry;
    int mWarmCount;

    // PLANE EMISSION
    float3 mNormal;
    int    mPlaneEmit;

    // INHERITED VELOCITY
    float3 mInheritedVelocity;
    float  mInheritedFactor;

    //MATRICES AND STUFF
    matrix mView;
    matrix mViewProj;
    float mNearPlane;
    float mFarPlane;
    float2 padding;
};

RWStructuredBuffer<Particle> srcParticleBuffer : register(u0);

Texture2D Normal : register(t0);
Texture2D Depth : register(t1);

SamplerState pointSampler   : register(s0);
SamplerState samAnisotropic : register(s1);
//float Depth()

EmitterProperties AssignEmitterProperties(void)
{
    EmitterProperties temp;

    temp.mGroupDimX = mGroupDimX;
    temp.mGroupDimY = mGroupDimY;
    temp.mMaxParticles = mMaxParticles;
    temp.mUsedParticles = mUsedParticles;
    temp.mMinLifetime = mMinLifetime;
    temp.mMaxLifetime = mMaxLifetime;
    temp.mMaxVel = mMaxVel;
    temp.mMinVel = mMinVel;
    temp.mFadeStartAmt = mFadeStartAmt;
    temp.mFadeEndAmt = mFadeEndAmt;
    temp.mDT = mDT;
    temp.mTotalGameTime = mTotalGameTime;
    temp.mEmissionAngleVariance = mEmissionAngleVariance;
    temp.mEmissionAngle = mEmissionAngle;
    temp.mIsEmitting = mIsEmitting;
    temp.mEmitterPos = mEmitterPos;

    temp.mCamPos = mCamPos;
    temp.mRandomDirection = mRandomDirection;
    temp.mGravityFactor = mGravityFactor;
    temp.mRadius = mRadius; 
    temp.mStartSize = mStartSize;
    temp.mEndSize = mEndSize;
    temp.mStartColour = mStartColour;
    temp.mEndColour = mEndColour;
    temp.mAttractorPos = mAttractorPos;
    temp.mAttracted = mAttracted;
    temp.mAttractorLocalSpace = mAttractorLocalSpace;
    temp.mAttractorFactor = mAttractorFactor;
    temp.mHalfLengths = mHalfLengths;
    temp.mType = mType;
    temp.mDeceleration = mDeceleration;
    temp.mSpawnInGeometry = mSpawnInGeometry;
    temp.mDieInGeometry = mDieInGeometry;
    temp.mWarmCount = mWarmCount;

    // PLANE EMISSION
    temp.mNormal = mNormal;
    temp.mPlaneEmit = mPlaneEmit;

    // INHERITED VELOCITY
    temp.mInheritedVelocity = mInheritedVelocity;
    temp.mInheritedFactor = mInheritedFactor;
    return temp;
}

[numthreads(THREAD_BLOCK, 1, 1)]
void CS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex, uint3 threadID : SV_DispatchThreadID)
{

    EmitterProperties temp = AssignEmitterProperties();

	//Calculate the ID of this Thread
    uint ID = groupID.x * THREAD_BLOCK + groupID.y * mGroupDimX * THREAD_BLOCK + groupID.z * mGroupDimX * mGroupDimY * THREAD_BLOCK + groupIndex;
	
    Particle p = srcParticleBuffer[ID];

    //Get it into view space
    //float4 projPos = mul(float4(p.mPos, 1.0f), mViewProj);
    //float4 viewSpacePos = mul(float4(p.mPos, 1.0f), mView);
    //// Unproject position to use as texCoords
    //float2 texCoords = viewSpacePos.xy;
    //texCoords.y *= -1;
    //texCoords = texCoords * .5f + .5f;

    //float sceneDepth = Depth.Sample(pointSampler, texCoords).r;
    //float4 normal = Normal.Sample(pointSampler, texCoords);
    //float linearDepth = mFarPlane / (sceneDepth - mNearPlane);
    //
    //float depthDiff = linearDepth - projPos.z;
    //bool ref = false;
    //if ((depthDiff) < 0.0f && (depthDiff) > -0.1f)
    //    ref = true;
    //
    //float3 reflectVec = reflect(p.mVel, normal.xyz);
    //
    ////if (ref)
    //    p.mVel = reflectVec;
    
    switch (mType)
    {
    case 0:
        srcParticleBuffer[ID] = DefaultBehaviour(temp, p, ID);
        break;
    case 1:
        srcParticleBuffer[ID] = BoxBehaviour(temp, p, ID);
        break;
    case 2:
        srcParticleBuffer[ID] = SphereBehaviour(temp, p, ID);
        break;
    default:
        break;
    }



}