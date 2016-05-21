/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Sound.h"
#include "Asset.h"
#include "AudioManager.h"

Sound::Sound()
	: name_(" "), inst_(nullptr), mIsLooping(false), mIsUniqueManaged(false), mIsMusic(false)
{ }
Sound::Sound(const std::string& name, FMOD::Studio::EventInstance* e)
	: name_(name), inst_(e), mIsLooping(false), mIsUniqueManaged(false), mIsMusic(false)
{
}
Sound::~Sound()
{
	
}

void Sound::Play()
{
	// Before playing the sound, always make sure volume is set to most updated volume level
	float volume = mIsMusic ? AudioManager::getInstance()->GetMusicVolume() :
		AudioManager::getInstance()->GetSoundVolume();
	SetVolume(volume);

	inst_->start();
}
void Sound::Stop(bool immediateStop)
{
	inst_->stop(immediateStop ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT);
}
void Sound::Resume()
{
	inst_->setPaused(false);
}
void Sound::Pause()
{
	inst_->setPaused(true);
}

bool Sound::IsPlaying() const
{
	FMOD_STUDIO_PLAYBACK_STATE state;
	if (!AudioManager::errorCheck_(inst_->getPlaybackState(&state)))
		return false;

	switch (state)
	{
	case FMOD_STUDIO_PLAYBACK_STOPPED:
	case FMOD_STUDIO_PLAYBACK_SUSTAINING:
	case FMOD_STUDIO_PLAYBACK_STOPPING:
		break;

	case FMOD_STUDIO_PLAYBACK_PLAYING:
	case FMOD_STUDIO_PLAYBACK_STARTING:
		return true;
	}
	return false;
}
bool Sound::IsStopped() const
{
	FMOD_STUDIO_PLAYBACK_STATE state;
	if (!AudioManager::errorCheck_(inst_->getPlaybackState(&state)))
		return false;

	switch (state)
	{
	case FMOD_STUDIO_PLAYBACK_PLAYING:
	case FMOD_STUDIO_PLAYBACK_STARTING:
		break;
	case FMOD_STUDIO_PLAYBACK_STOPPED:
	case FMOD_STUDIO_PLAYBACK_SUSTAINING:
	case FMOD_STUDIO_PLAYBACK_STOPPING:
		return true;
	}
	return false;
}
bool Sound::IsPaused() const
{
	bool state = false;
	inst_->getPaused(&state);
	return state;
}

void Sound::SetPitch(float pitch)
{
	inst_->setPitch(pitch);
}
float Sound::GetPitch() const
{
	float val = 0;
	inst_->getPitch(&val);
	return val;
}
void Sound::SetVolume(float vol)
{
	// limit value from being > 1 or < 0
	inst_->setVolume(min(max(vol, 0.0f), 1.0f));
}
float Sound::GetVolume() const
{
	float val = 0;
	inst_->getVolume(&val);
	return val;
}

void Sound::SetParam(const std::string& param, float val)
{
	AudioManager::errorCheck_(inst_->setParameterValue(param.c_str(), val));
}
float Sound::GetParam(const std::string& param) const
{
	FMOD::Studio::ParameterInstance* paramInst;
	float val;

	if (!AudioManager::errorCheck_(inst_->getParameter(param.c_str(), &paramInst)))
		return 0.f;

	paramInst->getValue(&val);
	return val;
}
bool Sound::GetParamByIndex(int index, std::string& paramName, float& paramValue) const
{
	FMOD::Studio::ParameterInstance* paramInst;
	FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	float val;

	if (!AudioManager::errorCheck_(inst_->getParameterByIndex(index, &paramInst), false))
		return false;

	if (!AudioManager::errorCheck_(paramInst->getDescription(&paramDesc), false))
		return false;

	if (!AudioManager::errorCheck_(paramInst->getValue(&val), false))
		return false;

	paramName = paramDesc.name;
	paramValue = val;
	return true;
}

void Sound::SetIs3D(int set3D, const Vector3& position)
{
	mIs3D = set3D;
	FMOD_3D_ATTRIBUTES attributes = { { 0 } };
	attributes.position.x = position.x;
	attributes.position.y = position.y;
	attributes.position.z = position.z;
	inst_->set3DAttributes(&attributes);
}