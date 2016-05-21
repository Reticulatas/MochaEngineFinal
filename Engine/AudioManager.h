#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "MathIncludes.h"

#include <list>
#include <set>
#include <windows.h>

class Sound;
class CAudioGeometry;

class AudioManager : public ISubsystem
{
public:
    AudioManager();
    ~AudioManager();
	
    void Load();
	void Free();
	void FreeAllEvents(); // Call to force all events to stop playing and release
	
	bool Tick(); // Update fn

	// Memory allocation is given to user to handle the freeing
	Sound* GetUniqueEvent(const std::string& eventVenteName);
	
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
	Vector3 GetPlayerAttributes() const { return mPlayerAttributes; }
    
	bool Enable() const;
    void Enable(bool val);
    
	static bool errorCheck_(FMOD_RESULT result, bool log = true);
	static AudioManager* getInstance();

private:
	void loadBankFiles_();
    void unloadBankFiles_();
	FMOD::Studio::EventDescription* LoadEventFile(const std::string& eventVenteName);
	FMOD::Studio::EventDescription* LoadEventDescription(
		const std::string& filepath, const std::string& filename);

	/*
	void SetAudio3DAttributes(const Sound* sound,
		const Math::Vector3& pos,
		const Math::Vector3& up,
		const Math::Vector3& forward,
		const Math::Vector3& vel);
	bool Is3DAudio(const Sound* sound) const;
	void CreateGeometry(const CAudioGeometry* obj, const AABB& aabb);
	void UpdateGeometry(const CAudioGeometry* obj, const AABB& aabb);
	void RemoveGeometry(const CAudioGeometry* obj);
	*/
	//typedef std::map<const CAudioGeometry*,FMOD::Geometry*> GameObjectGeometryMap;
	// void unloadGameObjectGeometry_();
    // void updateOcclusionReverb_(const FMOD_VECTOR& listener, const EventAudioMap& eventAudioMap);

	// Deprecated stuff
    // bool geometryLoaded_;
	// Math::Vector3 prevCamPos_;
	// GameObjectGeometryMap objGeomMap_;
	// EventAudioMap eventMusicMap_;
	// EventAudioMap eventSoundMap_;

    FMOD::Studio::System* studioSys_;
    FMOD::System* lowLevelSys_;
	std::list<FMOD::Studio::Bank*> bankList_;
	std::map<std::string, FMOD::Studio::EventDescription*> mEventSource;
	Vector3 mPlayerAttributes; // position of player for 3D sounds

	bool enable_;
	bool muteSound_;
	bool muteMusic_;
	float musicFaderLevel_; // current volume level for music
	float soundFaderLevel_; // current volume level for sound

    metadef(AudioManager)
        endmetadef;
};

#endif