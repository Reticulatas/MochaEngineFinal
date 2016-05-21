/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

 
#include "AssetHandle.h"
#include "Sound.h"
#include "AudioManager.h"
 
#include <map>
#include <string>

class Sound;

DEPRECATED struct AudioAsset
{
    bool is3D_;
    Sound* handle_;
    typedef std::map<std::string,float> SoundParameterList;
    SoundParameterList paramList_;

    ___SERIALIZE_BEGIN___
        ___DEFSER(is3D_,1)
        ___DEFSER(handle_,1)
        ___DEFSER(paramList_,1)
    ___SERIALIZE_END___
};

___SERIALIZE_CLASS(AudioAsset,2);

class CSound : public Component
{
public:
	CSound(); 
    ~CSound();

	// 0 - off 3D, 1 - use static gameObject position, 2 - dyanmic position will always update
	void PlayIndependentEvent(const std::string& eventVenteName, bool loop, int set3D);
	void PlayIndependentEventStatic3D(const std::string& eventVenteName, const Vector3& pos, bool loop); // assumed to be 3D, with given position
	unsigned int GetUniqueEvent(const std::string& eventVenteName, int set3D); // Returns the eventId to refer back to this event
	void PlayUniqueEvent(unsigned int eventId, bool loop);
	void PlayUniqueEvent(unsigned int eventId, const Vector3& pos, bool loop); // play at specific given position (for 3D)
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

	typedef std::map<unsigned int, Sound*> AudioEventMap;

private:
    void OnStart();
    void OnUpdate();
    void OnAlwaysUpdate();
    void OnFree();
    void OnInit();

	// Returns the iterator pointing to the position that took its place after erasing
	AudioEventMap::iterator FreeEventInstance(AudioEventMap::iterator eventIt);

public:
	//typedef std::map<std::string, AudioAsset> AudioHandleList;
	DEPRECATED std::map<std::string, AudioAsset> audioHandleList_;

	unsigned int mAvailIndex;
	AudioEventMap mEventInstances;

	virtual void CopyFrom(const ::CSound* c);

    COMPONENTCLONE(CSound);

    ___SERIALIZE_BEGIN___
        ___DEFBASE(Component)
        ___DEFSERONLY(audioHandleList_,2)
    ___SERIALIZE_END___

public:
    metadef(CSound)
    endmetadef;

	virtual void OnEditorStart(void);

	virtual void OnEditorFree(void);

};
 
___SERIALIZE_CLASS(CSound, 3);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change