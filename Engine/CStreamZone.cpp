#include "stdafx.h"
#include "CStreamZone.h"
#include "CMapZone.h"

#define STREAMEXEMPTFLAG "DONOTSTREAM"

void CStreamZone::OnEditorUpdate(void)
{
	zone.DrawBounds();
}

void CStreamZone::OnDraw(void)
{
}

void CStreamZone::OnFree(void)
{
}

void CStreamZone::OnAlwaysUpdate(void)
{
}

void CStreamZone::OnUpdate(void)
{
	if (!this->isEnabled())
		return;
	if (streamFinished)
		return;

	switch (mode)
	{
	case ZONE_UNLOAD:
		if (mPreAccumulateUnloadObjs && objsToUnload.empty())
			PreAccumulateObjects();
		if (!hasBeenEntered && ListenerInBounds())			//you must enter the zone to deload it
			hasBeenEntered = true;
		if (!ListenerInBounds() && hasBeenEntered)			//remove this zone if the listener has left it
			BeginStream();
		break;
	default:
		if (ListenerInBounds())
			BeginStream();
		break;
	}
}

void CStreamZone::OnInit(void)
{
}

void CStreamZone::OnStart(void)
{
	zone.Init(gameObject);
}

CStreamZone::CStreamZone()
{
	mUseMarkers = false;
	mPreAccumulateUnloadObjs = false;
	isStreaming = false;
	streamFinished = false;
	forceZoneUnusableAfterStream = false;
	useOffsetFromThisPos = false;
	incrementalDeload = true;
	hasBeenEntered = false;
	offset = Math::Vector3::cZero;
}

bool CStreamZone::GetIsStreaming()
{
	return isStreaming;
}

bool CStreamZone::GetHasFinishedStreaming()
{
	return streamFinished;
}

void CStreamZone::BeginStream()
{
	if (streamFinished)
		throw MochaException("Attempt to stream after stream finished from: " + gameObject->GetName());

	CMapZone* izone = 0;

	switch (mode)
	{
	case PARALLEL_LOAD:
	case GENERIC_LOAD:
		if (!LoadPreChecks())
			return;

		if (useOffsetFromThisPos)
		{
			Math::Vector3 newOff = offset;
			newOff.y -= zone.height*0.5f;
			switch (mode)
			{
			case GENERIC_LOAD:
				Map::LoadMapAdditive(fileToStream, gameObject->transform->GetGlobalPosition() + newOff);
				break;
			case PARALLEL_LOAD:
				barrista().togo->Invoke(this, MAKEMETAMETHODVARIABLE(CStreamZone, BeginOffsetLoad), 0, 0, false, gameObject->transform->GetGlobalPosition() + newOff);
				break;
			}
		}
		else if (mUseMarkers)
		{
			if (mMarkerID.empty())
			{
				streamFinished = true;
				Log("ERROR: Cannot stream because mMarkerID is not set.");
				if (!Engine::getInstance()->GetIsEditorMode())
					MessageBox(0, L"ERROR: Cannot stream because mMarkerID is not set.", L"Operation stopped", MB_OK);
				return;
			}
			switch (mode)
			{
			case GENERIC_LOAD:
				Map::LoadMapAdditiveMergeMarkers(fileToStream, mMarkerID);
				break;
			case PARALLEL_LOAD:
				barrista().togo->Invoke(this, MAKEMETAMETHODVARIABLE(CStreamZone, BeginMarkerLoad), 0, 0, false);
				break;
			}
		}
		else
		{
			Math::Vector3 newOff = offset;
			newOff.y -= zone.height*0.5f;
			switch (mode)
			{
			case GENERIC_LOAD:
				Map::LoadMapAdditive(fileToStream, newOff);
				break;
			case PARALLEL_LOAD:
				barrista().togo->Invoke(this, MAKEMETAMETHODVARIABLE(CStreamZone, BeginOffsetLoad), 0, 0, false, newOff);
				break;
			}
		}
		break;
	case GENERIC_UNLOAD:
		if (mapZoneId.empty())
		{
			Log("Cannot stream, empty Map Zone ID set from: " + gameObject->GetName());
			return;
		}
		{
			auto* mapzones = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CMapZone>();
			if (mapzones != 0)
			{
				for (CMapZone* zone : *mapzones)
				{
					if (zone->id == mapZoneId)
						Map::DeloadZone(&zone->zone);
				}
			}
		}
		break;
	case ZONE_UNLOAD:
		//unload this zone
		if (!incrementalDeload)
		{
			if (!mPreAccumulateUnloadObjs)
				Map::DeloadZone(&zone);
			else
			{
				for (auto& gobjp : objsToUnload)
					DeloadSingle(gobjp);
			}
		}
		else
		{
			//if we haven't cached objects, do so now on the beginning of the deload
			if (!mPreAccumulateUnloadObjs && objsToUnload.empty())
				PreAccumulateObjects();
			if (!isStreaming)
			{
				//remove all scripts to begin with
				for (auto& gobjp : objsToUnload)
				{
					if (gobjp.first->HasComponent<CScript>())
						::Destroy(gobjp.first->GetComponent<CScript>());
				}
			}

			isStreaming = true;
			//de-load one at a time
			if (objsToUnload.size() != 0)
			{
				Log("Removed: " + objsToUnload.front().first->GetName() + " as number: " + ToString(objsToUnload.size()));
				DeloadSingle(objsToUnload.front());
				objsToUnload.erase(objsToUnload.begin());
				return;						//we still have objects to deload
			}
		}
	}

	isStreaming = true;

	//end stream, we are in serial
	EndStream(izone);
}

void CStreamZone::EndStream(CMapZone* zone)
{
	isStreaming = false;
	streamFinished = true;
	if (forceZoneUnusableAfterStream && zone)
	{
		zone->id.clear();
		zone->Destroy();
	}
}

void CStreamZone::SetStreamMode(StreamMode _mode)
{
	mode = _mode;
}

void CStreamZone::SetFileToStream(std::string _file)
{
	fileToStream = _file;
}

bool CStreamZone::LoadPreChecks()
{
	if (fileToStream == "")
	{
		Log("ERROR: Cannot stream from: " + gameObject->GetName() + " blank file to stream.");
		return false;
	}
	if (Engine::getInstance()->GetIsEditorMode())
	{
		streamFinished = true;
		Log("ERROR: Cannot stream in editor mode.");
		if (!Engine::getInstance()->GetIsEditorMode())
			MessageBox(0, L"Cannot stream load in editor mode.", L"Operation stopped", MB_OK);
		return false;
	}

	if (useOffsetFromThisPos && mUseMarkers)
	{
		streamFinished = true;
		Log("ERROR: Cannot stream when both useOffset and useMarkers is set.");
		if (!Engine::getInstance()->GetIsEditorMode())
			MessageBox(0, L"ERROR: Cannot stream when both useOffset and useMarkers is set.", L"Operation stopped", MB_OK);
		return false;
	}
	return true;
}

void CStreamZone::BeginMarkerLoad()
{
	Map::LoadMapAdditiveMergeMarkers(fileToStream, mMarkerID);
}

void CStreamZone::BeginOffsetLoad(Math::Vector3 offset)
{
	Map::LoadMapAdditive(fileToStream, offset);
}

bool CStreamZone::ListenerInBounds()
{
	auto* streamlist = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CStreamZoneListener>();
	if (streamlist)
	{
		for (CStreamZoneListener* listener : *streamlist)
		{
			if (zone.IsPointWithin(listener->gameObject->transform->position()))
				return true;
		}
	}
	return false;
}

void CStreamZone::PreAccumulateObjects()
{
	for (GameObject* gobj : zone.GetObjectsInBounds())
	{
		if (!gobj->GetFlag("Player") && !gobj->GetFlag(STREAMEXEMPTFLAG))		//exclude player for sanity reasons
		{
			if (gobj != gameObject)												//exclude this object
			{
				objsToUnload.push_back(std::make_pair(gobj, gobj->GetID()));
				Log("Unload Cached: " + gobj->GetName());
			}
		}
	}
}

void CStreamZone::DeloadSingle(UnloadObj& objp)
{
	if (objp.first->GetIsValidObject())
	{
		if (objp.first->GetID() == objp.second)
			::Destroy(objp.first);
	}
}

___COMP_REGISTER(CStreamZone);
meta_define(CStreamZone);
meta_define(CStreamZoneListener);
___COMP_REGISTER(CStreamZoneListener);

//original author: Nicholas Fuller

////////////////////////////////////////////////////////

void CStreamZoneListener::OnStart(void)
{
	Component::OnStart();
}

void CStreamZoneListener::OnInit(void)
{
}

void CStreamZoneListener::OnUpdate(void)
{
}

void CStreamZoneListener::OnAlwaysUpdate(void)
{
	Component::OnAlwaysUpdate();
}

void CStreamZoneListener::OnFree(void)
{
}

void CStreamZoneListener::OnDraw(void)
{
	Component::OnDraw();
}

void CStreamZoneListener::OnEditorUpdate(void)
{
	
}
////////////////////////////////////////////////////////
