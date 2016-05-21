
%{
#include "AudioManager.h"
	%}

class AudioManager : public ISubsystem
{
public:
	// These work on all currently loaded event instances in all CSound components in the game
	void PlayAll();
	void StopAll();
	void ResumeAll();
	void PauseAll();
    
	void SetMusicVolume(float vol);
	void SetSoundVolume(float vol);
    float GetMusicVolume() const; // returns 0 if muted
    float GetSoundVolume() const; // returns 0 if muted
    void SetMuteMusic(bool val);
	void SetMuteSound(bool val);
    bool GetMuteMusic() const;
	bool GetMuteSound() const;

	static AudioManager* getInstance();

private:	
    AudioManager();
    ~AudioManager();
};
