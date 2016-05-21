#include "stdafx.h"
#include "AudioManager.h"
#include "Log.h"
#include "GSM.h"
#include "Mesh.h"
#include "CSound.h"

#include "fmod_errors.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <fmod_studio.hpp>
#include <fmod.hpp>

#define GUID_FILE "GUIDs.txt"

AudioManager::AudioManager()
    :   enable_(true),
        // geometryLoaded_(false),
        muteSound_(false),
        muteMusic_(false),
        musicFaderLevel_(1.f),
        soundFaderLevel_(1.f),    
        studioSys_(0),
        lowLevelSys_(0)
{
}
AudioManager::~AudioManager()
{
	Free();
}

void AudioManager::Load()
{
    if(!errorCheck_(FMOD::Studio::System::create(&studioSys_)))
        return;

    // The example Studio project is authored for 5.1 sound, so set up the system output mode to match
    if(!errorCheck_(studioSys_->getLowLevelSystem(&lowLevelSys_)))
        return;

    if(!errorCheck_(lowLevelSys_->setSoftwareFormat(0,FMOD_SPEAKERMODE_5POINT1,0)))
        return;

	/*FMOD::Reverb3D* reverb;
	lowLevelSys_->createReverb3D(&reverb);

	FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_CONCERTHALL;
	FMOD_VECTOR reverbPos = { 0.f, 0.f, 0.f };
	reverb->set3DAttributes(&reverbPos, 20.f, 100.f);
	reverb->setProperties(&prop);

	lowLevelSys_->setReverbProperties(0, &prop);*/

	FMOD_STUDIO_INITFLAGS initFlags = FMOD_STUDIO_INIT_LIVEUPDATE; // FMOD_STUDIO_INIT_NORMAL;
	if (!errorCheck_(studioSys_->initialize(32, initFlags, FMOD_INIT_NORMAL, NULL)))
        return;

    loadBankFiles_();

	mPlayerAttributes = Vector3(0.f);
}
void AudioManager::Free()
{
	FreeAllEvents();

	unloadBankFiles_();

	errorCheck_(lowLevelSys_->release());
	errorCheck_(studioSys_->release());
}
void AudioManager::FreeAllEvents()
{
	StopAll();
	for (auto& eventDesc : mEventSource)
	{
		eventDesc.second->releaseAllInstances(); // Immediately stops and releases all instances of the event
	}
	// Leave the desc there, for future use
}

bool AudioManager::Tick()
{
#if 0
	if (Input::GetTriggered(0, "down") != 0.f)
	{
		SetSoundVolume(GetSoundVolume() - 0.1f);
		std::cout << "vol:" << GetSoundVolume() << std::endl;
	}
	if (Input::GetTriggered(0, "up") != 0.f)
	{
		SetSoundVolume(GetSoundVolume() + 0.1f);
		std::cout << "vol:" << GetSoundVolume() << std::endl;
	}
#endif

#if 0
    const std::list<CCamera*>* camList = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CCamera>();
    if(camList)
    {
        CCamera* cam = camList->front();
        FMOD_3D_ATTRIBUTES attributes = {{0}};
        Math::Vector3 pos(cam->gameObject->transform->GetGlobalPosition());
        attributes.position.x = pos.x;
        attributes.position.y = pos.y;
        attributes.position.z = pos.z;

        Math::Vector3 up(cam->gameObject->transform->GetUpVector());
        attributes.up.x = up.x;
        attributes.up.y = up.y;
        attributes.up.z = up.z;

        Math::Vector3 forward(cam->gameObject->transform->GetForwardVector());
        attributes.forward.x = forward.x;
        attributes.forward.y = forward.y;
        attributes.forward.z = forward.z;

        Math::Vector3 velocity(pos - prevCamPos_);
        attributes.velocity.x = velocity.x;
        attributes.velocity.y = velocity.y;
        attributes.velocity.z = velocity.z;

        prevCamPos_ = pos;
        errorCheck_(studioSys_->setListenerAttributes(&attributes));
        updateOcclusionReverb_(attributes.position,eventMusicMap_);
        updateOcclusionReverb_(attributes.position,eventSoundMap_);
    }
#endif
	const std::list<CCamera*>* camList = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CCamera>();
	if (camList)
	{
		for (auto it = camList->begin(); it != camList->end(); ++it)
		{
			if ((*it)->mCamType == CCamera::MAIN)
			{
				FMOD_3D_ATTRIBUTES attrib = { { 0 } };
				CCamera* cam = (*it);
				mPlayerAttributes = cam->gameObject->transform->GetGlobalPosition();
				attrib.position.x = mPlayerAttributes.x;
				attrib.position.y = mPlayerAttributes.y;
				attrib.position.z = mPlayerAttributes.z;

				Math::Vector3 up(cam->gameObject->transform->GetUpVector());
				attrib.up.x = up.x;
				attrib.up.y = up.y;
				attrib.up.z = up.z;

				Math::Vector3 forward(cam->gameObject->transform->GetForwardVector());
				attrib.forward.x = forward.x;
				attrib.forward.y = forward.y;
				attrib.forward.z = forward.z;

				// Math::Vector3 velocity(pos - prevCamPos_);
				// attrib.velocity.x = velocity.x;
				// attrib.velocity.y = velocity.y;
				// attrib.velocity.z = velocity.z;

				//prevCamPos_ = pos;
				errorCheck_(studioSys_->setListenerAttributes(&attrib));
				//updateOcclusionReverb_(attrib.position, eventMusicMap_);
				//updateOcclusionReverb_(attrib.position, eventSoundMap_);
			}
		}
	}

    return errorCheck_(studioSys_->update());
}

Sound* AudioManager::GetUniqueEvent(const std::string& eventVenteName)
{
	FMOD::Studio::EventDescription* eventDesc = nullptr;
	auto it = mEventSource.find(eventVenteName);
	if (it == mEventSource.end())
	{
		// Load that event and play it
		eventDesc = LoadEventFile(eventVenteName);
	}
	else
	{
		eventDesc = it->second;
	}

	FMOD::Studio::EventInstance* inst = nullptr;
	if (!errorCheck_(eventDesc->createInstance(&inst)))
		return nullptr;
	return new Sound(eventVenteName, inst);
}

void AudioManager::PlayAll()
{
	if (!enable_)
		return;

	BaseState* currState = GSM::getInstance()->getActiveState();
	if (currState)
	{
		const std::list<CSound*>* sounds = currState->GetListOfComponentsOfType<CSound>();
		if (sounds)
		{
			for (const auto& elem : *sounds) // For every csound component
			{
				for (auto& soundAsset : elem->mEventInstances) // Go through every sound event
				{
					soundAsset.second->Play();
				}
			}
		}
	}
}
void AudioManager::StopAll()
{
	if (!enable_)
		return;

	BaseState* currState = GSM::getInstance()->getActiveState();
	if (currState)
	{
		const std::list<CSound*>* sounds = currState->GetListOfComponentsOfType<CSound>();
		if (sounds)
		{
			for (const auto& elem : *sounds) // For every csound component
			{
				elem->StopAllMyEvents(false);
			}
		}
	}
}
void AudioManager::PauseAll()
{
	if (!enable_)
		return;

	BaseState* currState = GSM::getInstance()->getActiveState();
	if (currState)
	{
		const std::list<CSound*>* sounds = currState->GetListOfComponentsOfType<CSound>();
		if (sounds)
		{
			for (const auto& elem : *sounds) // For every csound component
			{
				elem->PauseAllMyEvents();
			}
		}
	}
}
void AudioManager::ResumeAll()
{
	if (!enable_)
		return;

	BaseState* currState = GSM::getInstance()->getActiveState();
	if (currState)
	{
		const std::list<CSound*>* sounds = currState->GetListOfComponentsOfType<CSound>();
		if (sounds)
		{
			for (const auto& elem : *sounds) // For every csound component
			{
				elem->ResumeAllMyEvents();
			}
		}
	}
}

void AudioManager::SetMusicVolume(float vol)
{
	if (!enable_)
		return;

	float currVol = min(max(vol, 0.0f), 1.0f);
	if (!muteMusic_) // Don't change the vol level if already set to mute
	{
		musicFaderLevel_ = currVol;
		muteMusic_ = (currVol == 0.0f);
	}

	BaseState* currState = GSM::getInstance()->getActiveState();
	if (currState)
	{
		const std::list<CSound*>* sounds = currState->GetListOfComponentsOfType<CSound>();
		if (sounds)
		{
			for (const auto& elem : *sounds) // For every csound component
			{
				for (auto& soundAsset : elem->mEventInstances) // Go through every sound event
				{
					Sound* currSound = soundAsset.second;
					if (currSound->mIsMusic)
						currSound->SetVolume(currVol); // Only set volume to currently loaded music
				}
			}
		}
	}
}
void AudioManager::SetSoundVolume(float val)
{
	if (!enable_)
		return;

	float currVol = min(max(val, 0.0f), 1.0f);
	if (!muteSound_) // Don't change the vol level if already set to mute
	{
		soundFaderLevel_ = currVol;
		muteSound_ = (currVol == 0.0f);
	}

	BaseState* currState = GSM::getInstance()->getActiveState();
	if (currState)
	{
		const std::list<CSound*>* sounds = currState->GetListOfComponentsOfType<CSound>();
		if (sounds)
		{
			for (const auto& elem : *sounds) // For every csound component
			{
				for (auto& soundAsset : elem->mEventInstances) // Go through every sound event
				{
					Sound* currSound = soundAsset.second;
					if (!currSound->mIsMusic)
						currSound->SetVolume(currVol); // Only set volume to currently loaded sounds
				}
			}
		}
	}
}
float AudioManager::GetMusicVolume() const
{
	return muteMusic_ ? 0.f : musicFaderLevel_;
}
float AudioManager::GetSoundVolume() const
{
	return muteSound_ ? 0.f : soundFaderLevel_;
}
void AudioManager::SetMuteMusic(bool val)
{
	if (!enable_)
		return;

	if (muteMusic_ == val)
		return;

	muteMusic_ = val;
	float volumeToSet = GetMusicVolume();
	if (!muteMusic_) // If set to non-mute, the volume to set is not allowed to be 0
		volumeToSet = max(0.1f, volumeToSet);
	SetMusicVolume(volumeToSet);
}
void AudioManager::SetMuteSound(bool val)
{
    if(!enable_)
        return;

    if(muteSound_ == val)
        return;

    muteSound_ = val;
	float volumeToSet = GetSoundVolume();
	if (!muteSound_) // If set to non-mute, the volume to set is not allowed to be 0
		volumeToSet = max(0.1f, volumeToSet);
	SetSoundVolume(volumeToSet);
}
bool AudioManager::GetMuteMusic() const
{
    return muteMusic_;
}
bool AudioManager::GetMuteSound() const
{
	return muteSound_;
}

bool AudioManager::Enable() const
{
    return enable_;
}
void AudioManager::Enable(bool val)
{
    enable_ = val;
}

//------------------------------------------------------------------------------

void AudioManager::loadBankFiles_()
{
    AssetType assetType("bank");
    std::vector<AvailableAsset*> assets = AvailableAsset::GetAvailableAssetsFromExtension(assetType);
   
    for(unsigned int i = 0; i < assets.size(); ++i)
    {
        FMOD::Studio::Bank* bank = 0;
        std::string filePath(assets[i]->Filepath());
        std::string ext(filePath.substr(filePath.find_last_of('.') + 1));

        if(ext != assetType)
            continue;

        if(!errorCheck_(studioSys_->loadBankFile(assets[i]->Filepath().c_str(),FMOD_STUDIO_LOAD_BANK_NORMAL,&bank)))
            continue;
		if (!errorCheck_(bank->loadSampleData())) // Immediately pre-load into memory all events from this bank
			continue;

        bankList_.push_back(bank);
    }
}
void AudioManager::unloadBankFiles_()
{
    auto bankIt = bankList_.begin();
    
    while(bankIt != bankList_.end())
    {
        errorCheck_((*bankIt)->unload());
        ++bankIt;
    }
    bankList_.clear();
}

FMOD::Studio::EventDescription* AudioManager::LoadEventFile(const std::string& eventVenteName)
{
	// Extract out file name
    unsigned int left = eventVenteName.find_last_of("\\") + 1;
    unsigned int right = eventVenteName.find_last_of(".");
    std::string filename(eventVenteName.substr(left,right - left));

	try
	{
		std::string fullPath = AssetManager::getInstance()->GetAbsolutePathOfFile(eventVenteName, false);
		return LoadEventDescription(fullPath, eventVenteName); // Add the event description into the map
	}
	catch (AssetException)
	{
		Log("ERROR: AudioManager LoadEventFile couldn't find " + eventVenteName);
		return nullptr;
	}
}
FMOD::Studio::EventDescription* AudioManager::LoadEventDescription(
	const std::string& filepath, const std::string& filename)
{
	std::cout << "Loading Sound " << filename << std::endl;
    std::ifstream file(filepath,std::ios::binary);
    if(!file.is_open())
    {
        Log("AudioManager: " + filepath + " does not exist.",LogEntry::Error);
        return nullptr;
    }

    GUID guidWIN;
    file.read(reinterpret_cast<char*>(&guidWIN),sizeof(guidWIN));

    FMOD_GUID guidFMOD;
    guidFMOD.Data1 = guidWIN.Data1;
    guidFMOD.Data2 = guidWIN.Data2;
    guidFMOD.Data3 = guidWIN.Data3;

    for(unsigned int i = 0; i < 8; ++i)
        guidFMOD.Data4[i] = guidWIN.Data4[i];

    file.close();

    FMOD::Studio::EventDescription* eventDesc = 0;
    if(!errorCheck_(studioSys_->getEventByID(&guidFMOD,&eventDesc)))
        return nullptr;

	mEventSource[filename] = eventDesc;
	return eventDesc;
}

#if 0
void AudioManager::SetAudio3DAttributes(const Sound* sound,
	const Math::Vector3& pos,
	const Math::Vector3& up,
	const Math::Vector3& forward,
	const Math::Vector3& vel)
{
	FMOD_3D_ATTRIBUTES attributes = {{0}};
	attributes.position.x = pos.x;
	attributes.position.y = pos.y;
	attributes.position.z = pos.z;

	attributes.up.x = up.x;
	attributes.up.y = up.y;
	attributes.up.z = up.z;

	attributes.forward.x = forward.x;
	attributes.forward.y = forward.y;
	attributes.forward.z = forward.z;

	attributes.velocity.x = vel.x;
	attributes.velocity.y = vel.y;
	attributes.velocity.z = vel.z;

	errorCheck_(sound->inst_->set3DAttributes(&attributes));
}

bool AudioManager::Is3DAudio(const Sound* sound) const
{
	bool val = true;
	//sound->desc_->is3D(&val);
	return val;
}

void AudioManager::CreateGeometry(const CAudioGeometry* obj,
	const AABB& aabb)
{

	if (!enable_)
		return;

	// Check for duplicates
	GameObjectGeometryMap::const_iterator it = objGeomMap_.find(obj);
	if (it != objGeomMap_.end())
		return;

	// Create AABB Geometry for fmod
	FMOD_VECTOR top0{ aabb.maxPoint.x, aabb.maxPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR top1{ aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR top2{ aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z };
	FMOD_VECTOR top3{ aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z };

	FMOD_VECTOR bot0{ aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR bot1{ aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR bot2{ aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z };
	FMOD_VECTOR bot3{ aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z };

	FMOD_VECTOR poly[4];
	FMOD::Geometry* geom;

	if (!errorCheck_(lowLevelSys_->createGeometry(6, 24, &geom)))
		return;

	// Top
	poly[0] = top0; poly[1] = top1; poly[2] = top2; poly[3] = top3;
	if (!errorCheck_(geom->addPolygon(1.0f, 1.0f, true, 4, poly, 0)))
		return;

	// Sides
	poly[0] = top0; poly[1] = top1; poly[2] = bot1; poly[3] = bot0;
	if (!errorCheck_(geom->addPolygon(1.0f, 1.0f, true, 4, poly, 0)))
		return;

	poly[0] = top1; poly[1] = top2; poly[2] = bot2; poly[3] = bot1;
	if (!errorCheck_(geom->addPolygon(1.0f, 1.0f, true, 4, poly, 0)))
		return;

	poly[0] = top2; poly[1] = top3; poly[2] = bot3; poly[3] = bot2;
	if (!errorCheck_(geom->addPolygon(1.0f, 1.0f, true, 4, poly, 0)))
		return;

	poly[0] = top3; poly[1] = top0; poly[2] = bot0; poly[3] = bot3;
	if (!errorCheck_(geom->addPolygon(1.0f, 1.0f, true, 4, poly, 0)))
		return;

	// Bottom
	poly[0] = bot0; poly[1] = bot1; poly[2] = bot2; poly[3] = bot3;
	if (!errorCheck_(geom->addPolygon(1.0f, 1.0f, true, 4, poly, 0)))
		return;

	objGeomMap_.insert(std::pair<const CAudioGeometry*, FMOD::Geometry*>(obj, geom));
}

void AudioManager::UpdateGeometry(const CAudioGeometry* obj,
	const AABB& aabb)
{
	if (!enable_)
		return;

	// Check for duplicates
	GameObjectGeometryMap::const_iterator it = objGeomMap_.find(obj);
	if (it == objGeomMap_.end())
	{
		CreateGeometry(obj, aabb);
		return;
	}

	FMOD_VECTOR top0{ aabb.maxPoint.x, aabb.maxPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR top1{ aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR top2{ aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z };
	FMOD_VECTOR top3{ aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z };

	FMOD_VECTOR bot0{ aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR bot1{ aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z };
	FMOD_VECTOR bot2{ aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z };
	FMOD_VECTOR bot3{ aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z };

	FMOD_VECTOR poly[4];

	// Top
	poly[0] = top0; poly[1] = top1; poly[2] = top2; poly[3] = top3;
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
		errorCheck_(it->second->setPolygonVertex(0, vertIndex, &poly[vertIndex]));

	// Sides
	poly[0] = top0; poly[1] = top1; poly[2] = bot1; poly[3] = bot0;
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
		errorCheck_(it->second->setPolygonVertex(1, vertIndex, &poly[vertIndex]));

	poly[0] = top1; poly[1] = top2; poly[2] = bot2; poly[3] = bot1;
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
		errorCheck_(it->second->setPolygonVertex(2, vertIndex, &poly[vertIndex]));

	poly[0] = top2; poly[1] = top3; poly[2] = bot3; poly[3] = bot2;
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
		errorCheck_(it->second->setPolygonVertex(3, vertIndex, &poly[vertIndex]));

	poly[0] = top3; poly[1] = top0; poly[2] = bot0; poly[3] = bot3;
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
		errorCheck_(it->second->setPolygonVertex(4, vertIndex, &poly[vertIndex]));

	// Bottom
	poly[0] = bot0; poly[1] = bot1; poly[2] = bot2; poly[3] = bot3;
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
		errorCheck_(it->second->setPolygonVertex(5, vertIndex, &poly[vertIndex]));
}

void AudioManager::RemoveGeometry(const CAudioGeometry* obj)
{
	if (!enable_)
		return;

	GameObjectGeometryMap::iterator it = objGeomMap_.find(obj);

	if (it != objGeomMap_.end())
	{
		it->second->release();
		objGeomMap_.erase(it);
	}
}

void AudioManager::unloadGameObjectGeometry_()
{
    for(GameObjectGeometryMap::iterator it = objGeomMap_.begin(); it != objGeomMap_.end(); ++it)
        it->second->release();
    objGeomMap_.clear();
}

void AudioManager::updateOcclusionReverb_(const FMOD_VECTOR& listener,
                                          const EventAudioMap& eventAudioMap)
{
    EventAudioMap::const_iterator it = eventAudioMap.begin();
    while(it != eventAudioMap.end())
    {
        if(Is3DAudio(*it))
        {
            FMOD_3D_ATTRIBUTES attrib;
            (*it)->inst_->get3DAttributes(&attrib);

            float direct;
            float reverb;
            lowLevelSys_->getGeometryOcclusion(&listener,&attrib.position,&direct,&reverb);
            //SetParam(*it,"Direct",direct);
            //SetParam(*it,"Reverb",reverb);
        }
        ++it;
    }
}
#endif

bool AudioManager::errorCheck_(FMOD_RESULT result, bool log)
{
	if (result != FMOD_OK)
	{
		if (log)
			Log("FMOD ERROR: " + std::string(FMOD_ErrorString(result)), LogEntry::Error);
		return false;
	}
	return true;
}
AudioManager* AudioManager::getInstance()
{
	static AudioManager instance;
	return &instance;
}

meta_define(AudioManager);