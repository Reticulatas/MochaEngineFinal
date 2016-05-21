#pragma once

#include "GraphicsInclude.h"
#include "MathIncludes.h"
#include "GPUShader.h"
#include "CParticleEmitter.h"
#include "CParticleRenderer.h"
#include "Event.h"
#include <map>

namespace ParticleEventList
{
	typedef Mocha::Event<void(void)> Test;
}


typedef std::pair<CParticleEmitter*, CParticleRenderer*>  ParticlePair;



struct ParticleGroup
{
	ParticlePair mPair;
	unsigned     mID;

	bool operator==(unsigned objID)
	{
		return mID == objID;
	}

};

class ParticleSystem
{
public:

	ParticleSystem(void);
	virtual ~ParticleSystem(void);

	static ParticleSystem* getInstance(void);

	void RenderParticles(CCamera* cam);

    void EditorRenderParticles(CCamera* cam);
    void GameRenderParticles(CCamera* cam);

	void AddParticleEmitter(CParticleEmitter*, CParticleRenderer*, unsigned int gameID);
	void RemoveParticleEmitter(CParticleEmitter* emitter, unsigned int gameID);
    ParticlePair GetParticleEmitter(CParticleEmitter* emitter, unsigned int gameID);


private:
    bool PreviewEmitter(CParticleEmitter* emit);

    void GPUSort(void);
    void SortBehaviours(void);

    void UpdateParticleEmitters(CCamera* cam);
    void RenderParticleEmitters(CCamera* cam);

    ID3D11BlendState*			mAlphaBlendState;

	std::map<ParticleBehaviour, std::vector< ParticleGroup > >  mParticleMap;
    std::map<ParticleBehaviour, std::vector< ParticleGroup > >  mGameParticleMap;


	ParticleEventList::Test            mTest;

    ConstantBuffer    mRenderConstantBuf;
    float mInternalTimer;
public:
	//metadef(ParticleSystem)
	//	endmetadef;




};
