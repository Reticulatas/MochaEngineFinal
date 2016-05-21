/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "Asset.h"

#include <string>
#include <fmod_studio.hpp>

class Sound : public Asset
{
public:
	Sound();
	Sound(const std::string& name, FMOD::Studio::EventInstance* e);
	~Sound();

	void Play();
	void Stop(bool immediateStop = false);
	void Resume();
	void Pause();
	bool IsPlaying() const;
	bool IsStopped() const;
	bool IsPaused() const;

	void SetPitch(float pitch);
	float GetPitch() const;
	void SetVolume(float vol);
	float GetVolume() const;

	void SetParam(const std::string& param, float val);
	float GetParam(const std::string& param) const;
	bool GetParamByIndex(int index, std::string& paramName, float& paramValue) const;

	int GetIs3D() const { return mIs3D; }
	void SetIs3D(int set3D, const Vector3& position = Vector3(0.f));

	enum SOUND_3D_TYPE
	{
		NON_3D = 0,
		STATIC_3D,
		MOVING_3D,

		NUM_3D_TYPES
	};

	std::string name_;
	FMOD::Studio::EventInstance* inst_;
	bool mIsLooping;
	bool mIsUniqueManaged; // is gotten as an unique event
	bool mIsMusic; // true - music, false - sound effect
	int mIs3D; // if using 3D sound dependent on 3D (SOUND_3D_TYPE)

private:

	___SERIALIZE_BEGIN___
		___DEFBASE(Asset)
	___SERIALIZE_END___
};