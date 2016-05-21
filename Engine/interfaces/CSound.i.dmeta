%{
#include "CSound.h"
%}

namespace std {
%template(SoundParameterList)map<string,float>;
}

struct AudioAsset
{
    bool is3D_;
    Sound* handle_;
    std::map<string,float> paramList_;
};

namespace std {
%template(SoundHandleList)map<string,AudioAsset>;
}

%rename(PlayIndependentEvent3D) CSound::PlayIndependentEvent(const std::string& eventVenteName, const Math::Vector3& pos, bool loop);


class CSound : public Component 
{
	CSound();
	~CSound();

public:
	// 0 - off 3D, 1 - use static gameObject position, 2 - dyanmic position will always update
	void PlayIndependentEvent(const std::string& eventVenteName, bool loop, int set3D);
	void PlayIndependentEventStatic3D(const std::string& eventVenteName, const Math::Vector3& pos, bool loop); // assumed to be 3D, with given position
	unsigned int GetUniqueEvent(const std::string& eventVenteName, int set3D); // Returns the eventId to refer back to this event
	void PlayUniqueEvent(unsigned int eventId, bool loop);
	void PlayUniqueEvent(unsigned int eventId, const Math::Vector3& pos, bool loop); // play at specific given position (for 3D)
	void PauseUniqueEvent(unsigned int eventId);
	void ResumeUniqueEvent(unsigned int eventId);
	void StopUniqueEvent(unsigned int eventId, bool toRelease = true); // true to release the unique event, making the id invalid
	void SetUniqueEventParam(unsigned int eventId, const std::string& param, float val);
	float GetUniqueEventParam(unsigned int eventId, const std::string& param);
	bool GetUniqueEventIsPlaying(unsigned int eventId);
	bool GetUniqueEventIsStopped(unsigned int eventId);
	bool GetUniqueEventIsPaused(unsigned int eventId);

	void StopAllGlobalEvents(); // Only stops all loaded events, does not free them
	void StopAllMyEvents(bool toRelease); // Whether to free and invalidate all my event ids
	void ResumeAllMyEvents();
	void PauseAllMyEvents();

	/*
	void Play(const std::string& sound = std::string());
	void Stop(const std::string& sound = std::string());
	void Pause(const std::string& sound = std::string());
    void Loop(bool val,
              const std::string& sound = std::string());
    void Volume(float val,
                const std::string& sound = std::string());
    float Volume(const std::string& sound = std::string());
    void Pitch(float val,
               const std::string& sound = std::string());
    float Pitch(const std::string& sound = std::string());
    bool IsPlaying(const std::string& sound = std::string());
    bool IsLooping(const std::string& sound = std::string());
    bool IsPaused(const std::string& soundFile = std::string());
    void PlayAll();
    void StopAll();
    void PauseAll();
    void LoopAll(bool val);
    void ResumeAll();
    void RemoveSound(const std::string& soundFile);
    bool SetParam(const std::string& param,
                  float val,
                  const std::string& sound = std::string());
    float GetParam(const std::string& param,
                   const std::string& sound = std::string()) const;
    AudioAsset* AddSound(const std::string& soundFile);

	std::map<string,AudioAsset> audioHandleList_;
	*/
};
