#include "stdafx.h"
#include "AudioExt.h"
#include "AudioManager.h"

void AudioExt::SetMusicVolume( float volume )
{
	AudioManager::getInstance()->SetMusicVolume(volume);
}

void AudioExt::SetSoundVolume( float volume )
{
	AudioManager::getInstance()->SetSoundVolume(volume);
}

float AudioExt::GetMusicVolume()
{
	return AudioManager::getInstance()->GetMusicVolume();
}

float AudioExt::GetSoundVolume()
{
	return AudioManager::getInstance()->GetSoundVolume();
}

void AudioExt::MuteMusic()
{
	AudioManager::getInstance()->SetMuteMusic(true);
}

void AudioExt::MuteSound()
{
	AudioManager::getInstance()->SetMuteSound(true);
}

void AudioExt::UnmuteMusic()
{
	AudioManager::getInstance()->SetMuteMusic(false);
}

void AudioExt::UnmuteSound()
{
	AudioManager::getInstance()->SetMuteSound(false);
}