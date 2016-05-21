#pragma once

#include "IZone.h"
#include "Component.h"

class CMapZone;

class CStreamZoneListener : public Component
{
public:
	virtual void CopyFrom(const ::CStreamZoneListener* c)
	{
		Component::CopyFrom(c);
	}

	COMPONENTCLONE(CStreamZoneListener);

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
	___SERIALIZE_END___

	virtual void OnStart(void);
	virtual void OnInit(void);
	virtual void OnUpdate(void);
	virtual void OnAlwaysUpdate(void);
	virtual void OnFree(void);
	virtual void OnDraw(void);
	virtual void OnEditorUpdate(void);

	metadef(CStreamZoneListener)
		m_addbase(Component)
		endmetadef
};
___SERIALIZE_CLASS(CStreamZoneListener, 1);

class CStreamZone : public Component
{
public:
	enum StreamMode {
		GENERIC_LOAD,			//load new zone in serial
		GENERIC_UNLOAD,			//unload zone when this zone touched
		PARALLEL_LOAD,			//load new zone in parallel
		ZONE_UNLOAD				//remove this zone when listener leaves
	};

	IZone zone;

	std::string mapZoneId;
	StreamMode mode;
	std::string fileToStream;
	bool forceZoneUnusableAfterStream;
	bool useOffsetFromThisPos;
	Math::Vector3 offset;

	//markers
	bool mUseMarkers;
	std::string mMarkerID;

	//pre accumulate unload
	bool mPreAccumulateUnloadObjs;				//only for zone_unload
	typedef std::pair<GameObject*, unsigned> UnloadObj;
	std::vector<UnloadObj> objsToUnload;
	bool incrementalDeload;				//slowly deload zone, one object every frame
	bool hasBeenEntered;

	bool GetIsStreaming();
	bool GetHasFinishedStreaming();

	void SetStreamMode(StreamMode _mode);
	void SetFileToStream(std::string _file);

	bool ListenerInBounds();

	virtual void CopyFrom(const ::CStreamZone* c)
	{
		//position = c->position;
		zone = c->zone;
		zone.Init(gameObject);
		mapZoneId = c->mapZoneId;
		mode = c->mode;
		fileToStream = c->fileToStream;
		forceZoneUnusableAfterStream = c->forceZoneUnusableAfterStream;
		useOffsetFromThisPos = c->useOffsetFromThisPos;
		offset = c->offset;
		mUseMarkers = c->mUseMarkers;
		mMarkerID = c->mMarkerID;
		mPreAccumulateUnloadObjs = c->mPreAccumulateUnloadObjs;
		incrementalDeload = c->incrementalDeload;
		hasBeenEntered = c->hasBeenEntered;
		Component::CopyFrom(c);
	}

	CStreamZone();

	COMPONENTCLONE(CStreamZone);
private:

	bool isStreaming;
	bool streamFinished;

	void BeginStream();
	void EndStream(CMapZone* zone = 0);

	void BeginOffsetLoad(Math::Vector3 offset);
	void BeginMarkerLoad();

	bool LoadPreChecks();
	void PreAccumulateObjects();
	void DeloadSingle(UnloadObj& objp);

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(zone, 1)
		___DEFSER(mapZoneId, 2)
		___DEFSER(mode, 2)
		___DEFSER(fileToStream, 2)
		___DEFSER(forceZoneUnusableAfterStream, 2)
		___DEFSER(useOffsetFromThisPos, 2)
		___DEFSER(offset, 2)
		___DEFSER(mMarkerID, 3)
		___DEFSER(mUseMarkers,3)
		___DEFSER(mPreAccumulateUnloadObjs, 4)
		___DEFSER(incrementalDeload, 5)
		___DEFSER(hasBeenEntered, 6)
	___SERIALIZE_END___

		metadef(CStreamZone)
		m_addbase(Component)
		endmetadef;

	virtual void OnStart(void);

	virtual void OnInit(void);

	virtual void OnUpdate(void);

	virtual void OnAlwaysUpdate(void);

	virtual void OnFree(void);

	virtual void OnDraw(void);

	virtual void OnEditorUpdate(void);

};

___SERIALIZE_CLASS(CStreamZone, 6);

//Original Author: Nicholas Fuller





