#include "stdafx.h"
#include "ParticleSystem.h"
#include "GSM.h"
#include "EditorSubsystem.h"
ParticleSystem::ParticleSystem(void)
{
	for (int i = 0; i < TOTAL_BEHAVIOURS; ++i)
	{
		mParticleMap[(ParticleBehaviour)i].reserve(16);
        mGameParticleMap[(ParticleBehaviour)i].reserve(16);
	}
    ID3D11Device* pDevice = Renderer::getInstance()->GetD3D11Device();

    mRenderConstantBuf.CreateBuffer<ParticleRenderConstants>(pDevice);

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    blendDesc.IndependentBlendEnable = false;
    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    pDevice->CreateBlendState(&blendDesc, &mAlphaBlendState);
    mInternalTimer = 0.0f;
}
ParticleSystem::~ParticleSystem(void)
{

}
ParticleSystem* ParticleSystem::getInstance()
{
	static ParticleSystem* instance = 0;

	if (!instance)
		instance = new ParticleSystem();

	return instance;
}

void ParticleSystem::RenderParticles(CCamera* cam)
{
    mInternalTimer += FrameController::getInstance()->DT();



    if (Engine::getInstance()->GetIsEditorMode() && !EditorSubsystem::getInstance()->IsTesting())
    {
        EditorRenderParticles(cam);
        //UpdateParticleEmitters(cam);
        //RenderParticleEmitters(cam);
    }
    else
        GameRenderParticles(cam);
    mInternalTimer = 0.0f;
    //EditorRenderParticles(cam);
}

void ParticleSystem::AddParticleEmitter(CParticleEmitter* emitter, CParticleRenderer* render, unsigned int objID)
{
	ParticleGroup particles;
	particles.mPair.first  = emitter;
	particles.mPair.second = render;
	particles.mID = objID;
	

    if (Engine::getInstance()->GetIsEditorMode() && !EditorSubsystem::getInstance()->IsTesting())
    {
        mParticleMap[emitter->GetBehaviour()].push_back(particles);
    }
    else
    {
        mGameParticleMap[emitter->GetBehaviour()].push_back(particles);
    }
}

void ParticleSystem::RemoveParticleEmitter(CParticleEmitter* emitter, unsigned int gameID)
{
    if (Engine::getInstance()->GetIsEditorMode() && !EditorSubsystem::getInstance()->IsTesting())
    {
        std::vector< ParticleGroup >::iterator toRemove =
            std::find(mParticleMap[emitter->GetBehaviour()].begin(),
            mParticleMap[emitter->GetBehaviour()].end(),
            gameID);

        if (toRemove != mParticleMap[emitter->GetBehaviour()].end())
            mParticleMap[emitter->GetBehaviour()].erase(toRemove);
    }
    else
    {
        std::vector< ParticleGroup >::iterator toRemove =
            std::find(mGameParticleMap[emitter->GetBehaviour()].begin(),
            mGameParticleMap[emitter->GetBehaviour()].end(),
            gameID);

        if (toRemove != mGameParticleMap[emitter->GetBehaviour()].end())
            mGameParticleMap[emitter->GetBehaviour()].erase(toRemove);
    }

}

ParticlePair ParticleSystem::GetParticleEmitter(CParticleEmitter* emitter, unsigned int gameID)
{
    std::vector< ParticleGroup >::iterator toRemove =
        std::find(mParticleMap[emitter->GetBehaviour()].begin(),
        mParticleMap[emitter->GetBehaviour()].end(),
        gameID);

    if (toRemove != mParticleMap[emitter->GetBehaviour()].end())
        return toRemove->mPair;
    else
        return toRemove->mPair;
}

void ParticleSystem::EditorRenderParticles(CCamera* cam)
{
    ID3D11DeviceContext* pDeviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
    ID3D11ShaderResourceView* ppSRVNULL[3] = { nullptr, nullptr, nullptr };

    InputLayouts::SetInputLayout(InputLayouts::InputLayout_Null);
    Renderer::getInstance()->SetBlendState(CRenderer::ALPHA_BLEND);
    Renderer::getInstance()->EnableDepthWriting(false);

    //for (unsigned state = 0; state < 2; ++state)
    //{
    //    if (state == 1)
    //        Renderer::getInstance()->SetBlendState(CRenderer::ADDITIVE_BLEND);

        for (int i = 0; i < TOTAL_BEHAVIOURS; ++i)
        {
            for (unsigned j = 0; j < mParticleMap[(ParticleBehaviour)i].size(); ++j)
            {
                // Binding

                CParticleEmitter * emitter = mParticleMap[(ParticleBehaviour)i][j].mPair.first;
                CParticleRenderer * render = mParticleMap[(ParticleBehaviour)i][j].mPair.second;
                
                //if (state == 0 && render->mBlendState != CRenderer::ALPHA_BLEND)
                //    continue;
                //if (state == 1 && render->mBlendState != CRenderer::ADDITIVE_BLEND)
                //    continue;

                if (!PreviewEmitter(emitter) || !emitter->isEnabled())// || !render->isEnabled())
                    continue;

                GPUShader * tmp = mParticleMap[(ParticleBehaviour)i][j].mPair.first->GetGPUShader();
                
                emitter->UpdateEmitterProperties(cam);

                if (!emitter->mNeedsPreWarm)
                    emitter->mPreWarmed = false;
                //Updating/Binding RENDER constant buffer

                //****** PARTICLE UPDATE START ******//
                pDeviceContext->CSSetShader(tmp->GetComputeShader(), nullptr, 0);
                pDeviceContext->CSSetUnorderedAccessViews(0, 1, &tmp->GetCompoundGPUBuffer()->mUAVBufViewer, nullptr);

                std::vector<ID3D11ShaderResourceView*> srv;
                // Check for constant buffer 
                if (tmp->GetConstantBuffer())
                {
                    //Updating/Binding UPDATE constant buffer
                    ConstantBufferManager::BindParticleUpdateConstantBuffer(cam, emitter->mEmitterProperty, tmp->mCBuffer.mConstBuffer);

                    srv.push_back(Renderer::getInstance()->mNormalsBuffer.GetShaderResourceView());
                    srv.push_back(Renderer::getInstance()->mDepthStencilBuffer.GetShaderResourceView());
                    pDeviceContext->CSSetShaderResources(0, srv.size(), srv.data());


                }
                //****** PRE WARM START ******//
                if (!emitter->mPreWarmed && emitter->mNeedsPreWarm)
                {
                    int cachedWarmCount = emitter->mEmitterProperty.mWarmCount;
                    for (int i = 0; i < cachedWarmCount; ++i)
                        pDeviceContext->Dispatch(emitter->mThreadGroup[0], emitter->mThreadGroup[1], emitter->mThreadGroup[2]);
                    emitter->mPreWarmed = true;
                }
                //****** PRE WARM END ******//
                // Dispatching the update to Compute shader
                //if (mInternalTimer > 0.01666666666667f)
                    pDeviceContext->Dispatch(emitter->mThreadGroup[0], emitter->mThreadGroup[1], emitter->mThreadGroup[2]);
                // Unbind UAV
                pDeviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, nullptr);

                //****** PARTICLE UPDATE END ******//

                ConstantBufferManager::BindParticleRenderConstantBuffer(cam, mRenderConstantBuf.mConstBuffer);

                render->mMaterial.BindAttributes(render->mMaterial.GetShader());
                pDeviceContext->PSSetShaderResources(1, srv.size(), srv.data());
                //Bind Shader Resource View (SRV) (no updating, just drawing) for Vertex shader
                pDeviceContext->VSSetShaderResources(0, 1, &tmp->GetCompoundGPUBuffer()->mSRVBufViewer);

                //****** PARTICLE RENDER START ******//
                //pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
                Renderer::getInstance()->SetPrimitiveType(POINT_LIST);
                pDeviceContext->Draw(emitter->mEmitterProperty.mUsedParticles, 0);
                //****** PARTICLE RENDER END ******//
                if (emitter->mEmitterProperty.mIsOneShot)
                    emitter->UpdateOneShotTimer();
            }
        }
    //}

    pDeviceContext->VSSetShader(0, nullptr, 0);
    pDeviceContext->GSSetShader(0, nullptr, 0);
    pDeviceContext->PSSetShader(0, nullptr, 0);
    pDeviceContext->CSSetShader(0, nullptr, 0);
    InputLayouts::SetInputLayout(InputLayouts::InputLayout_Vert);
    pDeviceContext->PSSetShaderResources(1, 2, ppSRVNULL);
    pDeviceContext->VSSetShaderResources(0, 1, ppSRVNULL);
    Renderer::getInstance()->EnableDepthWriting(true);
    Renderer::getInstance()->SetPrimitiveType(TRIANGLE_LIST);
}

void ParticleSystem::GameRenderParticles(CCamera* cam)
{
    ID3D11DeviceContext* pDeviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
    ID3D11ShaderResourceView* ppSRVNULL[3] = { nullptr, nullptr, nullptr };

    InputLayouts::SetInputLayout(InputLayouts::InputLayout_Null);
    Renderer::getInstance()->SetBlendState(CRenderer::ALPHA_BLEND);
    Renderer::getInstance()->EnableDepthWriting(false);

    //for (unsigned state = 0; state < 2; ++state)
    //{
    //    if (state == 1)
    //        Renderer::getInstance()->SetBlendState(CRenderer::ADDITIVE_BLEND);

    for (int i = 0; i < TOTAL_BEHAVIOURS; ++i)
    {
        for (unsigned j = 0; j < mGameParticleMap[(ParticleBehaviour)i].size(); ++j)
        {
            // Binding

            CParticleEmitter * emitter = mGameParticleMap[(ParticleBehaviour)i][j].mPair.first;
            CParticleRenderer * render = mGameParticleMap[(ParticleBehaviour)i][j].mPair.second;

            if (!emitter->isEnabled())//
                continue;

            GPUShader * tmp = mGameParticleMap[(ParticleBehaviour)i][j].mPair.first->GetGPUShader();

            emitter->UpdateEmitterProperties(cam);

            if (!emitter->mNeedsPreWarm)
                emitter->mPreWarmed = false;
            //Updating/Binding RENDER constant buffer

            //****** PARTICLE UPDATE START ******//
            pDeviceContext->CSSetShader(tmp->GetComputeShader(), nullptr, 0);
            pDeviceContext->CSSetUnorderedAccessViews(0, 1, &tmp->GetCompoundGPUBuffer()->mUAVBufViewer, nullptr);

            std::vector<ID3D11ShaderResourceView*> srv;
            // Check for constant buffer 
            if (tmp->GetConstantBuffer())
            {
                //Updating/Binding UPDATE constant buffer
                ConstantBufferManager::BindParticleUpdateConstantBuffer(cam, emitter->mEmitterProperty, tmp->mCBuffer.mConstBuffer);

                srv.push_back(Renderer::getInstance()->mNormalsBuffer.GetShaderResourceView());
                srv.push_back(Renderer::getInstance()->mDepthStencilBuffer.GetShaderResourceView());
                pDeviceContext->CSSetShaderResources(0, srv.size(), srv.data());


            }
            //****** PRE WARM START ******//
            if (!emitter->mPreWarmed && emitter->mNeedsPreWarm)
            {
                int cachedWarmCount = emitter->mEmitterProperty.mWarmCount;
                for (int i = 0; i < cachedWarmCount; ++i)
                    pDeviceContext->Dispatch(emitter->mThreadGroup[0], emitter->mThreadGroup[1], emitter->mThreadGroup[2]);
                emitter->mPreWarmed = true;
            }
            //****** PRE WARM END ******//
            // Dispatching the update to Compute shader
            //if (mInternalTimer > 0.01666666666667f)
                pDeviceContext->Dispatch(emitter->mThreadGroup[0], emitter->mThreadGroup[1], emitter->mThreadGroup[2]);
            // Unbind UAV
            pDeviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, nullptr);

            //****** PARTICLE UPDATE END ******//

            ConstantBufferManager::BindParticleRenderConstantBuffer(cam, mRenderConstantBuf.mConstBuffer);

            render->mMaterial.BindAttributes(render->mMaterial.GetShader());
            pDeviceContext->PSSetShaderResources(1, srv.size(), srv.data());
            //Bind Shader Resource View (SRV) (no updating, just drawing) for Vertex shader
            pDeviceContext->VSSetShaderResources(0, 1, &tmp->GetCompoundGPUBuffer()->mSRVBufViewer);

            //****** PARTICLE RENDER START ******//
            //pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            Renderer::getInstance()->SetPrimitiveType(POINT_LIST);
            pDeviceContext->Draw(emitter->mEmitterProperty.mUsedParticles, 0);
            //****** PARTICLE RENDER END ******//
            if (emitter->mEmitterProperty.mIsOneShot)
                emitter->UpdateOneShotTimer();
        }
    }
    //}

    pDeviceContext->VSSetShader(0, nullptr, 0);
    pDeviceContext->GSSetShader(0, nullptr, 0);
    pDeviceContext->PSSetShader(0, nullptr, 0);
    pDeviceContext->CSSetShader(0, nullptr, 0);
    InputLayouts::SetInputLayout(InputLayouts::InputLayout_Vert);
    pDeviceContext->PSSetShaderResources(1, 2, ppSRVNULL);
    pDeviceContext->VSSetShaderResources(0, 1, ppSRVNULL);
    Renderer::getInstance()->EnableDepthWriting(true);
    Renderer::getInstance()->SetPrimitiveType(TRIANGLE_LIST);
}

bool ParticleSystem::PreviewEmitter(CParticleEmitter* emit)
{
    return emit->gameObject->mIsSelected;
}
void ParticleSystem::SortBehaviours(void)
{
    for (int i = 0; i < TOTAL_BEHAVIOURS; ++i)
    {
        for (unsigned j = 0; j < mParticleMap[(ParticleBehaviour)i].size(); ++j)
        {
            CParticleEmitter * emitter = mParticleMap[(ParticleBehaviour)i][j].mPair.first;
            if (emitter->mBehaviour != (ParticleBehaviour)i)
            {
                CParticleRenderer * render = mParticleMap[(ParticleBehaviour)i][j].mPair.second;

                //Remove from current behaviour container
                std::find(mParticleMap[(ParticleBehaviour)i].begin(),
                    mParticleMap[(ParticleBehaviour)i].end(), 
                    mParticleMap[(ParticleBehaviour)i][j].mID);
            }
        }
    }

}
void ParticleSystem::UpdateParticleEmitters(CCamera* cam)
{
    ID3D11DeviceContext* pDeviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
    ID3D11ShaderResourceView* ppSRVNULL[3] = { nullptr, nullptr, nullptr };


    //Renderer::getInstance()->SetBlendState(CRenderer::ALPHA_BLEND);


    for (int i = 0; i < TOTAL_BEHAVIOURS; ++i)
    {
        for (unsigned j = 0; j < mParticleMap[(ParticleBehaviour)i].size(); ++j)
        {
            CParticleEmitter * emitter = mParticleMap[(ParticleBehaviour)i][j].mPair.first;
            CParticleRenderer * render = mParticleMap[(ParticleBehaviour)i][j].mPair.second;
            GPUShader * tmp = mParticleMap[(ParticleBehaviour)i][j].mPair.first->GetGPUShader();
            emitter->mEmitterProperty.mEmitterPos = emitter->gameObject->transform->GetPosition();
            Vector3 angle = emitter->gameObject->transform->GetForwardVector();
            emitter->mEmitterProperty.mEmissionAngle = angle.Normalized();
            emitter->mEmitterProperty.mDT = FrameController::getInstance()->GetDeltaTime();
            emitter->mEmitterProperty.mTotalGameTime += emitter->mEmitterProperty.mDT;
            emitter->mEmitterProperty.mCamPos = cam->GetPosition();
            emitter->mEmitterProperty.mHalfLengths = emitter->gameObject->transform->scale * 0.5f;
            emitter->mNeedsPreWarm = (bool)emitter->mEmitterProperty.mIsEmitting;

            if (!emitter->mNeedsPreWarm)
                emitter->mPreWarmed = false;

            //Updating/Binding RENDER constant buffer
            ConstantBufferManager::BindParticleRenderConstantBuffer(cam, mRenderConstantBuf.mConstBuffer);
            pDeviceContext->IASetInputLayout(nullptr);
            render->mMaterial.BindAttributes(render->mMaterial.GetShader());
            pDeviceContext->GSSetShader(render->mGS->GetSubShader(), nullptr, 0);


            //****** PARTICLE UPDATE START ******//
            pDeviceContext->CSSetShader(tmp->GetComputeShader(), nullptr, 0);
            pDeviceContext->CSSetUnorderedAccessViews(0, 1, &tmp->GetCompoundGPUBuffer()->mUAVBufViewer, nullptr);

            // Check for constant buffer 
            if (tmp->GetConstantBuffer())
            {
                //Updating/Binding UPDATE constant buffer
                ConstantBufferManager::BindParticleUpdateConstantBuffer(cam, emitter->mEmitterProperty, tmp->mCBuffer.mConstBuffer);
                std::vector<ID3D11ShaderResourceView*> srv;
                srv.push_back(Renderer::getInstance()->mNormalsBuffer.GetShaderResourceView());
                srv.push_back(Renderer::getInstance()->mDepthStencilBuffer.GetShaderResourceView());
                pDeviceContext->CSSetShaderResources(0, srv.size(), srv.data());
            }
            //****** PRE WARM START ******//
            if (!emitter->mPreWarmed && emitter->mNeedsPreWarm)
            {
                int cachedWarmCount = emitter->mEmitterProperty.mWarmCount;
                for (int i = 0; i < cachedWarmCount; ++i)
                    pDeviceContext->Dispatch(emitter->mThreadGroup[0], emitter->mThreadGroup[1], emitter->mThreadGroup[2]);
                emitter->mPreWarmed = true;
            }
            //****** PRE WARM END ******//

            // Dispatching the update to Compute shader
            pDeviceContext->Dispatch(emitter->mThreadGroup[0], emitter->mThreadGroup[1], emitter->mThreadGroup[2]);

            // Unbind UAV
            pDeviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, nullptr);

            //****** PARTICLE UPDATE END ******//


            //****** PARTICLE SORT START ******//



            //****** PARTICLE SORT END ******//



            //Bind Shader Resource View (SRV) (no updating, just drawing) for Vertex shader
            //pDeviceContext->VSSetShaderResources(0, 1, &tmp->GetCompoundGPUBuffer()->mSRVBufViewer);
            ////****** PARTICLE RENDER START ******//
            ////pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            //Renderer::getInstance()->SetPrimitiveType(POINT_LIST);
            //pDeviceContext->Draw(emitter->mEmitterProperty.mMaxParticles, 0);
            //****** PARTICLE RENDER END ******//



        }
    }
}

void ParticleSystem::RenderParticleEmitters(CCamera* cam)
{
    ID3D11DeviceContext* pDeviceContext = Renderer::getInstance()->GetD3D11DeviceContext();
    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
    ID3D11ShaderResourceView* ppSRVNULL[3] = { nullptr, nullptr, nullptr };


    Renderer::getInstance()->SetBlendState(CRenderer::ALPHA_BLEND);
    Renderer::getInstance()->SetPrimitiveType(POINT_LIST);

    for (int i = 0; i < TOTAL_BEHAVIOURS; ++i)
    {
        for (unsigned j = 0; j < mParticleMap[(ParticleBehaviour)i].size(); ++j)
        {


            CParticleEmitter * emitter = mParticleMap[(ParticleBehaviour)i][j].mPair.first;
            CParticleRenderer * render = mParticleMap[(ParticleBehaviour)i][j].mPair.second;
            GPUShader * tmp = emitter->GetGPUShader();

            //Updating/Binding RENDER constant buffer
            ConstantBufferManager::BindParticleRenderConstantBuffer(cam, mRenderConstantBuf.mConstBuffer);
            pDeviceContext->IASetInputLayout(nullptr);
            render->mMaterial.BindAttributes(render->mMaterial.GetShader());
            pDeviceContext->GSSetShader(render->mGS->GetSubShader(), nullptr, 0);


            //Bind Shader Resource View (SRV) (no updating, just drawing) for Vertex shader
            pDeviceContext->VSSetShaderResources(0, 1, &tmp->GetCompoundGPUBuffer()->mSRVBufViewer);
            //****** PARTICLE RENDER START ******//
            //pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            
            pDeviceContext->Draw(emitter->mEmitterProperty.mMaxParticles, 0);
            //****** PARTICLE RENDER END ******//



        }
    }
}

