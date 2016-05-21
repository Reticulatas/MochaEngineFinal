#include "stdafx.h"
#include "CParticleEmitter.h"
#include "CParticleRenderer.h"
#include "FrameController.h"
#include "CTransform.h"
#include "MeshFactory.h"
#include "EditorSubsystem_Interface.h"
#include "CScript.h"
#include "GPUBufferManager.h"
#include "ParticleSystem.h"

static const unsigned cMaxParticleCount = 2048;

Particle::Particle()
{
    mPos = Vector3();
    mVel = Vector3();
    mAliveTime = -100.0f;
    mLifetime = -100.0f;
    mParticleSize = 1.0f;
    mColor = COLOR::WHITE;
    //mAcceleration = Vector3();
	
}

Particle::Particle(Vector3 startPos)
{
    mPos = startPos;
    mVel = Vector3();
    mAliveTime = -100.0f;
    mLifetime = -100.0f;
    mParticleSize = 1.0f;
    mColor = COLOR::WHITE;
    //mAcceleration = Vector3();

}

CParticleEmitter::CParticleEmitter()
    : mThreadCount(128)
{

}
CParticleEmitter::~CParticleEmitter()
{
}

void CParticleEmitter::OnStart()
{
	CParticleRenderer * renderer = RequireDependency<CParticleRenderer>();
	
	ParticleSystem::getInstance()->AddParticleEmitter(this, renderer, this->gameObject->GetID());

    //BOX EMISSION VARIABLE
    mEmitterProperty.mHalfLengths = gameObject->transform->scale * 0.5f;
    //mEmitterProperty.mAABBMax = mEmitterProperty.mEmitterPos + gameObject->transform->scale;

    mEmitterProperty.mEmitterPos = gameObject->transform->GetPosition(); // NEEDS TO BE UPDATED
    CalculateThreadGroups();
    mEmitterProperty.mGroupDimX = mThreadGroup[0]; // X dim
    mEmitterProperty.mGroupDimY = mThreadGroup[1]; // Y dim

    // GET COMPUTE SHADER
    Particle initArray[cMaxParticleCount];

    for (unsigned i = 0; i < cMaxParticleCount; ++i)
        initArray[i].mPos = mEmitterProperty.mEmitterPos;

    mComputeShader = GPUBufferManager::getInstance()->AddComputeShader<Particle>("MochaParticleTest.ms",
        initArray,
        GPUST_COMPOUND,
        false,                                       //Don't add to GPUManager Tick loop
        this->gameObject->GetID(),
        mEmitterProperty.mMaxParticles,
        sizeof(Particle));



    //Set up constant buffers
    ID3D11Device* pDevice = Renderer::getInstance()->GetD3D11Device();
    mComputeShader->InitialiseConstantBuffer<EmitterProperties>(pDevice, &mEmitterProperty);
}
void CParticleEmitter::OnInit()
{
	//Calculate thread groups for 1 thread 1 render process

	mEmitterProperty.mMaxParticles = cMaxParticleCount;
    mEmitterProperty.mUsedParticles = mEmitterProperty.mMaxParticles / 2;

	mEmitterProperty.mMinLifetime = 1.0f;
	mEmitterProperty.mMaxLifetime = 5.0f;

    mEmitterProperty.mMinVel = 0.5f;
	mEmitterProperty.mMaxVel = 4.5f;
	

    mEmitterProperty.mStartSize = 0.5f;
    mEmitterProperty.mEndSize = 0.5f;

    mEmitterProperty.mFadeStartAmt = 1.0f;
    mEmitterProperty.mFadeEndAmt = 0.0f;

	mEmitterProperty.mEmissionAngle = Vector3(1, 1, 1);
	mEmitterProperty.mEmissionAngleVariance = 0.2f;
    mEmitterProperty.mRandomDirection = 60;
	mEmitterProperty.mIsEmitting = 1;
    mEmitterProperty.mGravityFactor = 1.0f;

    mEmitterProperty.mCamPos = Vector3(); // NEEDS TO BE UPDATED
    mEmitterProperty.mRandomDirection = mEmitterProperty.mMaxParticles / 2;

    // ATTRACTOR
    mEmitterProperty.mAttractorPos = Vector3();
    mEmitterProperty.mAttracted = false;
    mEmitterProperty.mAttractorLocalSpace = true;
    mEmitterProperty.mAttractorFactor = 1.0f;
    //mEmitterProperty.mIsOneShot = false;
    //mEmitterProperty.mInternalTimer = mEmitterProperty.mMaxLifetime;
    //BOX EMITTER VARIABLES
    mEmitterProperty.mDeceleration = false;
    mEmitterProperty.mSpawnInGeometry = false;
    mEmitterProperty.mDieInGeometry = false;
    mEmitterProperty.mRadius = 1.0f;
    // PLANE EMISSION
    mEmitterProperty.mNormal = Vector3();
    mEmitterProperty.mPlaneEmit = false;

    mEmitterProperty.mType = PE_POINT;
    mEmitterProperty.mWarmCount = 120;

    //Colour
    mEmitterProperty.mStartColour = COLOR::WHITE;
    mEmitterProperty.mEndColour = COLOR::WHITE;

	mBehaviour = PB_DEFAULT;
	mDrawEmissionBox = false;
	mDrawBoundingBox = false;
	mCurrentParticleCount = 0;
	mAccumulatedTime = 0;
	mEBoxWidth = 0;
	mEBoxHeight = 0;

    //mPreWarm = (mEmitterProperty.mIsEmitting) ? 100 : 0;
    mNeedsPreWarm = mEmitterProperty.mIsEmitting;
    mPreWarmed = mNeedsPreWarm;
	
}

void CParticleEmitter::OnEditorUpdate()
{
    //Show emission box

    OnUpdate();

}

void CParticleEmitter::OnUpdate()
{
    if (mEmitterProperty.mType == PE_SPHERE)
    {
        //float maxScale = std::max(std::max(gameObject->transform->scale.x, gameObject->transform->scale.y),gameObject->transform->scale.z);
        //maxScale = std::max(maxScale, mEmitterProperty.mRadius);
        gameObject->transform->scale.x = mEmitterProperty.mRadius;
        gameObject->transform->scale.y = mEmitterProperty.mRadius;
        gameObject->transform->scale.z = mEmitterProperty.mRadius;

        //mEmitterProperty.mRadius = maxScale;
    }
}

void CParticleEmitter::OnFree()
{
	ParticleSystem::getInstance()->RemoveParticleEmitter(this, this->gameObject->GetID());
    //mComputeShader->Shutdown();
}

void CParticleEmitter::OnDraw()
{
    if (gameObject->mIsSelected)
    {
        
        if (mEmitterProperty.mType == PE_POINT)
            return;

        ParticlePair pair = ParticleSystem::getInstance()->GetParticleEmitter(this, this->gameObject->GetID());

        SetDebugMesh();
        std::vector<Vector3> verts = pair.second->mMesh.GetAssetPtr<Mesh>()->GetUniqueVertices();
        std::vector<int> indices = pair.second->mMesh.GetAssetPtr<Mesh>()->GetUnqiueIndices();

        Matrix4 world = gameObject->transform->GetWorldMatrix();

        for (unsigned i = 0; i < indices.size(); i += 3)
        {
            unsigned index = indices[i];
            unsigned index1 = indices[i + 1];
            unsigned index2 = indices[i + 2];

            Vector3 pos0 = TransformPoint(world, verts[index]);
            Vector3 pos1 = TransformPoint(world, verts[index1]);
            Vector3 pos2 = TransformPoint(world, verts[index2]);

            Renderer::getInstance()->DrawLinePerma(pos0, pos1, COLOR::LIME_GREEN);
            Renderer::getInstance()->DrawLinePerma(pos0, pos2, COLOR::LIME_GREEN);
            Renderer::getInstance()->DrawLinePerma(pos1, pos2, COLOR::LIME_GREEN);
        }
    }
}
void CParticleEmitter::UpdateEmitterProperties(CCamera* cam)
{
    mEmitterProperty.mEmitterPos = gameObject->transform->GetPosition();
    Vector3 angle = gameObject->transform->GetForwardVector();

    mEmitterProperty.mEmissionAngle = angle.Normalized();
    mEmitterProperty.mDT = FrameController::getInstance()->GetDeltaTime();
    mEmitterProperty.mTotalGameTime += mEmitterProperty.mDT;
    mEmitterProperty.mCamPos = cam->GetPosition();
    mEmitterProperty.mHalfLengths = gameObject->transform->scale * 0.5f;
    mNeedsPreWarm = (bool)mEmitterProperty.mIsEmitting;


    
}
void CParticleEmitter::CalculateAABBCullingData(void)
{
    mCullingData.mX = Vector3(1.0f, 0.0f, 0.0f);
    mCullingData.mY = Vector3(0.0f, 1.0f, 0.0f);
    mCullingData.mZ = Vector3(0.0f, 0.0f, 1.0f);

    // Position
    mCullingData.mOrigin = mEmitterProperty.mEmitterPos;

    if (mEmitterProperty.mRandomDirection && mEmitterProperty.mGravityFactor == 0.0f)
    {
        
    }
}
void CParticleEmitter::UpdateOneShotTimer(void)
{
    if (mEmitterProperty.mIsEmitting)
    {
        mEmitterProperty.mInternalTimer = mEmitterProperty.mMaxLifetime;
        mEmitterProperty.mInternalTimer -= FrameController::getInstance()->DT();
        mEmitterProperty.mIsEmitting = false;
    }
    else
    {
        mEmitterProperty.mInternalTimer -= FrameController::getInstance()->DT();
    }
}
void CParticleEmitter::TriggerOneShot(void)
{
    if (!mEmitterProperty.mIsEmitting && (mEmitterProperty.mInternalTimer < 0.0f))
        mEmitterProperty.mIsEmitting = true;
}
void CParticleEmitter::CalculateThreadGroups(void)
{
	int numGroups = 0;

	if (mEmitterProperty.mMaxParticles % mThreadCount != 0)
		numGroups = (mEmitterProperty.mMaxParticles / mThreadCount) + 1;
	else
		numGroups = mEmitterProperty.mMaxParticles / mThreadCount;

	double thirdRoot = pow((double)numGroups, (double)(1.0 / 3.0));
	thirdRoot = ceil(thirdRoot);

	mThreadGroup[0] = mThreadGroup[1] = mThreadGroup[2] = static_cast <unsigned> (thirdRoot);

}

void CParticleEmitter::SetDebugMesh(void)
{
    ParticlePair pair = ParticleSystem::getInstance()->GetParticleEmitter(this, this->gameObject->GetID());

    switch (mEmitterProperty.mType)
    {
    case PE_POINT:
        break;
    case PE_BOX:
        pair.second->mMesh = AssetManager::getInstance()->GetAsset("cube.bean").GetAssetPtr<Mesh>();
        break;
    case PE_SPHERE:
        pair.second->mMesh = AssetManager::getInstance()->GetAsset("Sphere.bean").GetAssetPtr<Mesh>();
        break;
    }
}



meta_define(CParticleEmitter);
___COMP_REGISTER(CParticleEmitter);
/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change