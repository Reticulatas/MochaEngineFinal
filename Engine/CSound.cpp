/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CSound.h"
#include "Log.h"
#include "GameObject.h"
#include "EditorSubsystem.h"

CSound::CSound()
{
	mAvailIndex = 1;
}

CSound::~CSound()
{
}

void CSound::PlayIndependentEvent(const std::string& eventVenteName, bool loop, int set3D)
{
	int eventId = GetUniqueEvent(eventVenteName, set3D);
	if (eventId == 0)
		return; // This is an error, means file name was invalid
	Sound* currSound = mEventInstances[eventId];
	currSound->mIsUniqueManaged = false;
	PlayUniqueEvent(eventId, loop);
}
void CSound::PlayIndependentEventStatic3D(const std::string& eventVenteName, const Vector3& pos, bool loop)
{
	int eventId = GetUniqueEvent(eventVenteName, Sound::STATIC_3D);
	if (eventId == 0)
		return; // This is an error, means file name was invalid
	Sound* currSound = mEventInstances[eventId];
	currSound->mIsUniqueManaged = false;
	PlayUniqueEvent(eventId, pos, loop);
}
unsigned int CSound::GetUniqueEvent(const std::string& eventVenteName, int set3D)
{
	Sound* newSound = AudioManager::getInstance()->GetUniqueEvent(eventVenteName);
	if (newSound)
	{
		newSound->mIsUniqueManaged = true;
		if (set3D)
		{
			newSound->SetIs3D(set3D, gameObject->transform->GetPosition());
		}
		mEventInstances[mAvailIndex] = newSound;
		return mAvailIndex++;
	}
	return 0;
}
void CSound::PlayUniqueEvent(unsigned int eventId, bool loop)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		currSound->mIsLooping = loop;
		currSound->Play();
	}
}
void CSound::PlayUniqueEvent(unsigned int eventId, const Vector3& pos, bool loop)
{
	Sound* currSound = mEventInstances[eventId];
	currSound->SetIs3D(Sound::STATIC_3D, pos); // Set 3D position of sound
	PlayUniqueEvent(eventId, loop);
}
void CSound::PauseUniqueEvent(unsigned int eventId)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		currSound->Pause();
	}
}
void CSound::ResumeUniqueEvent(unsigned int eventId)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		currSound->Resume();
	}
}
void CSound::StopUniqueEvent(unsigned int eventId, bool toRelease)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		currSound->Stop(true);
		currSound->mIsLooping = false;
		currSound->mIsUniqueManaged = !toRelease; // signal to allow release
	}
}
void CSound::SetUniqueEventParam(unsigned int eventId, const std::string& param, float val)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		currSound->SetParam(param, val);
	}
	return;
}
float CSound::GetUniqueEventParam(unsigned int eventId, const std::string& param)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		return currSound->GetParam(param);
	}
	return 0.f;
}
bool CSound::GetUniqueEventIsPlaying(unsigned int eventId)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		return currSound->IsPlaying();
	}
	return false;
}
bool CSound::GetUniqueEventIsStopped(unsigned int eventId)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		return currSound->IsStopped();
	}
	return false;
}
bool CSound::GetUniqueEventIsPaused(unsigned int eventId)
{
	Sound* currSound = mEventInstances[eventId];
	if (currSound)
	{
		return currSound->IsPaused();
	}
	return false;
}

void CSound::StopAllGlobalEvents()
{
	AudioManager::getInstance()->StopAll();
}
void CSound::StopAllMyEvents(bool toRelease)
{
	for (auto eventIt = mEventInstances.begin(); eventIt != mEventInstances.end();)
	{
		Sound* currSound = eventIt->second;
		if (toRelease) // If to release, free the event from the map
		{
			eventIt = FreeEventInstance(eventIt);
			continue;
		}
		else // otherwise, just stop playing it
		{
			currSound->Stop(true);
			currSound->mIsLooping = false;
		}
		++eventIt;
	}

	if (toRelease)
	{
		mEventInstances.clear();
	}
}
void CSound::ResumeAllMyEvents()
{
	for (auto eventIt = mEventInstances.begin(); eventIt != mEventInstances.end(); ++eventIt)
	{
		eventIt->second->Resume();
	}
}
void CSound::PauseAllMyEvents()
{
	for (auto eventIt = mEventInstances.begin(); eventIt != mEventInstances.end(); ++eventIt)
	{
		eventIt->second->Pause();
	}
}

#if 0
void CSound::Play(const std::string& sound)
{
    if(!audioSetter_(sound,&AudioManager::Play))
    {
        cachedHandle_ = AddSound(sound);
        AudioManager::getInstance()->Play(cachedHandle_->handle_);
    }
}

void CSound::Stop(const std::string& sound)
{
    if(!audioSetter_(sound,&AudioManager::Stop))
    {
        cachedHandle_ = AddSound(sound);
        AudioManager::getInstance()->Stop(cachedHandle_->handle_);
    }
}

void CSound::Pause(const std::string& sound)
{
    if(!audioSetter_(sound,&AudioManager::Pause))
    {
        cachedHandle_ = AddSound(sound);
        AudioManager::getInstance()->Pause(cachedHandle_->handle_);
    }
}

void CSound::Loop(bool val,
                  const std::string& sound)
{
    if(!audioSetter_(sound,val,&AudioManager::SetLoop))
    {
        cachedHandle_ = AddSound(sound);
        AudioManager::getInstance()->SetLoop(cachedHandle_->handle_,val);
    }
}

void CSound::Volume(float val,
                    const std::string& sound)
{
    if(!audioSetter_(sound,val,&AudioManager::SetVolume))
    {
        cachedHandle_ = AddSound(sound);
        AudioManager::getInstance()->SetVolume(cachedHandle_->handle_,val);
    }
}

float CSound::Volume(const std::string& sound)
{
    return audioGetter_(sound,&AudioManager::GetVolume);
}

void CSound::Pitch(float val,
                   const std::string& sound)
{
    if(!audioSetter_(sound,val,&AudioManager::SetPitch))
    {
        cachedHandle_ = AddSound(sound);
        AudioManager::getInstance()->SetPitch(cachedHandle_->handle_,val);
    }
}

float CSound::Pitch(const std::string& sound)
{
    return audioGetter_(sound,&AudioManager::GetVolume);
}

bool CSound::IsPlaying(const std::string& sound)
{
    return audioGetter_(sound,&AudioManager::IsPlaying);
}

bool CSound::IsLooping(const std::string& sound)
{
    return audioGetter_(sound,&AudioManager::GetLoop);
}

bool CSound::IsPaused(const std::string& sound)
{
    return audioGetter_(sound,&AudioManager::IsPaused);
}

void CSound::PlayAll()
{
    allAudioSetter_(&AudioManager::Play);
}

void CSound::StopAll()
{

    allAudioSetter_(&AudioManager::Stop);
}

void CSound::PauseAll()
{
    allAudioSetter_(&AudioManager::Pause);
}

void CSound::LoopAll(bool val)
{
    allAudioSetter_(val,&AudioManager::SetLoop);
}

void CSound::ResumeAll()
{
    allAudioSetter_(&AudioManager::Resume);
}

void CSound::RemoveSound(const std::string& sound)
{
    AudioHandleList::iterator it = audioHandleList_.find(sound);

    if(it != audioHandleList_.end())
    {
        AudioManager::getInstance()->FreeAudio(it->second.handle_);
        audioHandleList_.erase(it);
    }
}

bool CSound::SetParam(const std::string& param,
                      float val,
                      const std::string& sound)
{
    AudioAsset* asset;

    // Use cached handle
    if(cachedHandle_ && sound.size() == 0)
        asset = cachedHandle_;
    else
        asset = &audioHandleList_.find(sound)->second;

    AudioAsset::SoundParameterList::iterator it = asset->paramList_.find(param);
    
    if(it != asset->paramList_.end())
    {
        AudioManager::getInstance()->SetParam(asset->handle_,param,val);
        return true;
    }
    return false;
}

float CSound::GetParam(const std::string& param,
                       const std::string& sound) const
{
    const AudioAsset* asset;

    // Use cached handle
    if(cachedHandle_ && sound.size() == 0)
        asset = cachedHandle_;
    else
        asset = &audioHandleList_.find(sound)->second;

    AudioAsset::SoundParameterList::const_iterator it = asset->paramList_.find(param);

    if(it != asset->paramList_.end())
        return AudioManager::getInstance()->GetParam(asset->handle_,param);
    return 0.0f;
}

AudioAsset* CSound::AddSound(const std::string& sound)
{
    // Check for duplicates
    AudioHandleList::iterator it = audioHandleList_.find(sound);

    if(it != audioHandleList_.end())
        return &it->second;

    AvailableAsset* aa = AvailableAsset::GetAvailableAssetFromFileName(sound);
    Sound* handle = AudioManager::getInstance()->GetAudio(aa->Filename());
    if(handle)
    {
        AudioAsset asset;
        asset.handle_ = handle;
        readAssetInfo_(asset);
        return &audioHandleList_.insert(std::pair<std::string,AudioAsset>(sound,asset)).first->second;
    }
    std::cout << "fail" << std::endl;
    return 0;
}
#endif

//------------------------------------------------------------------------------

void CSound::OnStart()
{
}

void CSound::OnUpdate()
{
	for (auto eventIt = mEventInstances.begin(); eventIt != mEventInstances.end(); )
	{
		Sound* currSound = eventIt->second;
		
		bool toDelete = false;
		// get sound's current state of playing
		FMOD_STUDIO_PLAYBACK_STATE currState;
		if (!currSound)
		{
			eventIt = mEventInstances.erase(eventIt);
			Log("ERROR: null sound* in eventMap of CSound!");
			continue;
		}

		if (!currSound->inst_)
		{
			toDelete = true;
			Log("ERROR: null sound eventInstance in eventMap of CSound!");
		}
		else
		{
			if (!AudioManager::errorCheck_(currSound->inst_->getPlaybackState(&currState))) // If there's an error, delete it
				toDelete = true;

			int sound3DType = currSound->GetIs3D();
			switch (sound3DType)
			{
				// If sound is not 3D, always set it to player's position
			case Sound::NON_3D: currSound->SetIs3D(sound3DType, AudioManager::getInstance()->GetPlayerAttributes()); break;
				// If sound is moving 3D, always set it to current position
			case Sound::MOVING_3D: currSound->SetIs3D(sound3DType, gameObject->transform->GetPosition()); break;
			}

			if (currSound->mIsLooping)
			{
				// if ended playing, then play it again
				if (currState == FMOD_STUDIO_PLAYBACK_STOPPED)
					currSound->inst_->start();
			}
			else
			{
				// if ended playing, then remove it
				if (currState == FMOD_STUDIO_PLAYBACK_STOPPED && !currSound->mIsUniqueManaged)
				{
					toDelete = true;
				}
			}
		}
		if (toDelete)
		{
			eventIt = FreeEventInstance(eventIt);
			continue;
		}

		++eventIt;
	}

#if 0
    AudioHandleList::iterator it = audioHandleList_.begin();
    CTransform* trans(this->gameObject->transform);
    Math::Vector3 pos(trans->GetGlobalPosition());
    Math::Vector3 up(trans->GetUpVector());
    Math::Vector3 forward(trans->GetForwardVector());
    Math::Vector3 vel(pos - prevPos_);

    while(it != audioHandleList_.end())
    {
        if(it->second.is3D_)
        {
            AudioManager::getInstance()->SetAudio3DAttributes(it->second.handle_,
                                                              pos,
                                                              up,
                                                              forward,
                                                              vel);
        }
        ++it;
    }
    prevPos_ = pos;
#endif
}

CSound::AudioEventMap::iterator CSound::FreeEventInstance(AudioEventMap::iterator eventIt)
{
	Sound* currSound = eventIt->second;
	currSound->inst_->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	currSound->inst_->release();
	currSound->inst_ = nullptr;
	delete currSound;
	currSound = nullptr;
	return mEventInstances.erase(eventIt);
}

void CSound::OnAlwaysUpdate()
{
}

void CSound::OnFree()
{
	for (auto eventIt = mEventInstances.begin(); eventIt != mEventInstances.end(); )
	{
		eventIt = FreeEventInstance(eventIt);
	}
	mEventInstances.clear();
}

void CSound::OnInit()
{
    
}

void CSound::CopyFrom(const ::CSound* c)
{
	mAvailIndex = c->mAvailIndex;
	for (const auto& elem : c->mEventInstances)
	{
		// Recreate the event instances
		int eventId = GetUniqueEvent(elem.second->name_, false);
		mEventInstances[eventId]->mIsUniqueManaged = elem.second->mIsUniqueManaged;
		mEventInstances[eventId]->mIsLooping = elem.second->mIsLooping;
		mEventInstances[eventId]->mIsMusic = elem.second->mIsMusic;
		mEventInstances[eventId]->mIs3D = elem.second->mIs3D;
	}

	Component::CopyFrom(c);
}

void CSound::OnEditorStart(void)
{
	EditorSubsystem::getInstance()->widgetManager.RegisterWidget(gameObject, EditorSubsystemTools::WidgetManager::SOUND);
}

void CSound::OnEditorFree(void)
{
	EditorSubsystem::getInstance()->widgetManager.DeRegisterWidget(gameObject);
}

___COMP_REGISTER(CSound);
meta_define(CSound);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change