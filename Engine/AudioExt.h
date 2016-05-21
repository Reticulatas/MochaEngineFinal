#pragma once
#include <string>

 
class AudioExt : public IRegisterable
{

public:

	 static void SetMusicVolume(float volume);
	 static void SetSoundVolume(float volume);
	 static float GetMusicVolume();
	 static float GetSoundVolume();
	 static void MuteMusic();
	 static void MuteSound();
	 static void UnmuteMusic();
	 static void UnmuteSound();

};

 