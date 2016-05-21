/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
#include <fstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/filesystem.hpp>
#include "zip.h"
#include "unzip.h"

#include "EditorSubsystem_Interface.h"
#include "CScript.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "GameObject.h"
#include "ShaderNode.h"
#include "Serializations_Persistent.h"
#include "ThreadUtils.h"
#include "TypeRegs_ConversionHelper.h"
#include "ret.h"
#include "UVTransformNode.h"
#include "ColorNode.h"
#include "TextureNode.h"
#include "CParticleEmitter.h"
#include "CParticleRenderer.h"
#include "CLight.h"
#include "CSound.h"
#include "CFontRenderer.h"
#include "CAnimationController.h"
#include "ScriptSubsystem.h"
#include "CZone.h"
#include "ObjectFactory.h"
#include "MeshCollider.h"
#include "CForwardRenderer.h"
#include "CConvexHull.h"
#include "CReflectionProbe.h"
#include "CBloom.h"
#include "CPostProcess.h"
#include "CMotionBlur.h"
#include "CCensorFX.h"
#include "CFilmGrain.h"
#include "CChromaticAberration.h"
#include "CArtifactFX.h"
#include "CMapZone.h"
#include "IZone.h"
#include "CStreamZone.h"
#include "CStreamMarker.h"
#include "CShadowProbe.h"
#include "EditorMsgDelegators.h"

#ifndef NOSERIALIZE
//--------------------------------
//REMEMBER TO ADD SERIALIZED CLASSES HERE
BOOST_CLASS_EXPORT(CollisionShape);
BOOST_CLASS_EXPORT(Box);
BOOST_CLASS_EXPORT(Circle);
BOOST_CLASS_EXPORT(OBB);
BOOST_CLASS_EXPORT(CTransform);
BOOST_CLASS_EXPORT(GameObject);
BOOST_CLASS_EXPORT(Component);
BOOST_CLASS_EXPORT(Material);
BOOST_CLASS_EXPORT(Mesh);
BOOST_CLASS_EXPORT(MatAttribNode);
BOOST_CLASS_EXPORT(UVTransformNode);
BOOST_CLASS_EXPORT(ColorNode);
BOOST_CLASS_EXPORT(TextureNode);
BOOST_CLASS_EXPORT(ShaderNode);
BOOST_CLASS_EXPORT(MaterialImpl);
BOOST_CLASS_EXPORT(CRenderer);
BOOST_CLASS_EXPORT(IRegisterable);
BOOST_CLASS_EXPORT(CMeshRenderer);
BOOST_CLASS_EXPORT(CCamera);
BOOST_CLASS_EXPORT(CScript);
BOOST_CLASS_EXPORT(CSkinMeshRenderer);
BOOST_CLASS_EXPORT(CParticleRenderer);
BOOST_CLASS_EXPORT(CParticleEmitter);
BOOST_CLASS_EXPORT(CAnimationController);
BOOST_CLASS_EXPORT(CBoxColliderComponent);
BOOST_CLASS_EXPORT(CMeshColliderComponent);
BOOST_CLASS_EXPORT(CLight);
BOOST_CLASS_EXPORT(IMeta);
BOOST_CLASS_EXPORT(CPhysics);
BOOST_CLASS_EXPORT(CZone);
BOOST_CLASS_EXPORT(CSound);
BOOST_CLASS_EXPORT(CFontRenderer);
BOOST_CLASS_EXPORT(CForwardRenderer);
BOOST_CLASS_EXPORT(CConvexHull);
BOOST_CLASS_EXPORT(CReflectionProbe);
BOOST_CLASS_EXPORT(CPostProcess);
BOOST_CLASS_EXPORT(CBloom);
BOOST_CLASS_EXPORT(CMotionBlur);
BOOST_CLASS_EXPORT(CCensorFX);
BOOST_CLASS_EXPORT(CFilmGrain);
BOOST_CLASS_EXPORT(CChromaticAberration);
BOOST_CLASS_EXPORT(CArtifactFX);
BOOST_CLASS_EXPORT(CMapZone);
BOOST_CLASS_EXPORT(CStreamZoneListener);
BOOST_CLASS_EXPORT(CStreamZone);
BOOST_CLASS_EXPORT(CDiffuseRenderer);
BOOST_CLASS_EXPORT(CStreamMarker);
BOOST_CLASS_EXPORT(CShadowProbe);
BOOST_CLASS_VERSION(ret::SlotTable<GameObject>, 1);
#endif

StateSerializer::StateSerializer()
{
}

bool StateSerializer::SaveState( BaseState& b, std::string filename )
{
	boost::filesystem::create_directory(boost::filesystem::temp_directory_path().wstring() + L"temp");

	std::wstring mapFileName = boost::filesystem::temp_directory_path().wstring() + L"temp\\map_temp.mocha";

	HZIP hz = CreateZip(mapFileName.c_str(), 0);

	//do pre-save script serialize to generate temp\\scriptdata.flavor
	ScriptSubsystem::getInstance()->CallCScriptSerialize();

	std::wstring mochaFileName = boost::filesystem::temp_directory_path().wstring() + L"temp\\level_temp.mocha";

	std::ofstream ofs(mochaFileName);
	boost::archive::xml_oarchive oa(ofs);

	oa << BOOST_SERIALIZATION_NVP(b);

	ofs.close();

	ZipAdd(hz, L"level.mocha", (boost::filesystem::temp_directory_path().wstring() + L"temp\\level_temp.mocha").c_str());
	ZipAdd(hz, L"scriptdata.flavor", (boost::filesystem::temp_directory_path().wstring() + L"temp\\scriptdata.flavor").c_str());
	CloseZip(hz);

	//copy file over
	std::wstring wfilename(filename.begin(), filename.end());

	try{
		boost::filesystem::copy_file(boost::filesystem::path(mapFileName), boost::filesystem::path(wfilename), boost::filesystem::copy_option::overwrite_if_exists);
	}
	catch (boost::filesystem::filesystem_error& e)
	{
		Log("Could not write save to " + std::string(wfilename.begin(), wfilename.end()) + ": " + std::string(e.what()));
	}


	//kill tempts
	boost::filesystem::remove_all(boost::filesystem::temp_directory_path().wstring() + L"temp");

	LogVerbose("Wrote map file to " + filename, LogEntry::Command);

	return true;
}

bool StateSerializer::SaveCurrentState( std::string filename )
{
	return SaveState(*GSM::getInstance()->getActiveState(), filename);
}

void StateSerializer::LoadState( std::string filename, bool replaceCurrent, Math::Vector3 offset)
{
	//lock thread until this goes out of scope
	ThreadUtils::ThreadLockVisitor threadLock(2);
	Log("Loading new state from " + filename + " ...", LogEntry::Command);

	if (!boost::filesystem::exists(boost::filesystem::absolute(filename)))
	{
		throw SerializationException(filename + " not found, did not load", SerializationException::CANNOTFINDMAP);
		return;
	}

	//stop testing if we currently are
	if (EditorSS::IsTesting())
		EditorSS::EndTesting();

	//pop top state
	if (replaceCurrent)
		GSM::getInstance()->PopState();

	std::wstring wfilename(filename.begin(), filename.end());

	boost::filesystem::create_directory(boost::filesystem::temp_directory_path().wstring() + L"temp");

	HZIP hz = OpenZip(wfilename.c_str(), 0);
	ZIPENTRY ze;
	GetZipItem(hz, -1, &ze);
	int numItems = ze.index;
	for (int zi = 0; zi < numItems; ++zi)
	{
		ZIPENTRY z_e;
		GetZipItem(hz, zi, &z_e);
		UnzipItem(hz, zi, std::wstring(boost::filesystem::temp_directory_path().wstring() + L"temp\\" + std::wstring(z_e.name)).c_str());
	}

	std::ifstream ifs(boost::filesystem::temp_directory_path().wstring() + L"temp\\level.mocha");
	if (!ifs.good())
	{
		GSM::getInstance()->PushState(new BlankState());
		throw SerializationException("ERROR: Problem loading " + filename + "\nBlank state pushed", SerializationException::FAILEDTOLOAD);
		return;
	}
	boost::archive::xml_iarchive ia(ifs);

	BaseState* b = new BlankState();

	ia >> BOOST_SERIALIZATION_NVP(*b);

	ifs.close();

	//do post-load script deserialization to load temp\\scriptdata.flavor
	ScriptSubsystem::getInstance()->CallCScriptDeSerialize();

	//remove the temp dir
	boost::filesystem::remove_all(boost::filesystem::temp_directory_path().wstring() + L"temp");

	GSM::getInstance()->PushState(b, false);

	if (EditorSS::IsTesting())
		b->IsActive(true);
	else
		b->IsActive(false);

	b->filepath = filename;
	b->DoPropertyRefresh();

	CloseZip(hz);

	if (offset != Math::Vector3::cZero)
		DoOffset(b, offset);

	//call onStarts for all components
	for (GameObject*const& obj : *b->mManagedObjects)
	{
		obj->myState = b;
		obj->OnForceStart();
	}

	b->DoPropertyRefresh();

	Log("	...New state added", LogEntry::Command);

	return;
}

bool StateSerializer::DoRecoverySave()
{	
	Log("Saved Recovery State", LogEntry::Standard);
	return SaveCurrentState("Assets/Levels/Recovery Save.mocha");
}

void StateSerializer::LoadStateAdditive(std::string filename, Math::Vector3 offset)
{
	LoadState(filename, false, offset);
	BaseState* new_state = GSM::getInstance()->getActiveState();
	BaseState* cur_state = GSM::getInstance()->getStateUnderActive();

	MergeStates(new_state, cur_state, 0);
}

void StateSerializer::MergeStates(BaseState* src, BaseState* dest, std::vector<bool>* whichObjsToMerge)
{
	if (whichObjsToMerge)
	{
		if (whichObjsToMerge->size() !=(src->GetManagedObjectsSlotTable().GetAllocatedObjects()->size()))
		{
			Log("Invalid Obj Merge List passed to LoadStatePartial");
			return;
		}
	}

	std::vector<GameObject*> mergedObjects;	//not including children

	int cur_index = -1;
	for (GameObject* gobj : *src->GetManagedObjectsSlotTable().GetAllocatedObjects())
	{
		++cur_index;
		if (whichObjsToMerge != 0)
			if ((*whichObjsToMerge)[cur_index] == false)
				continue;
		if (gobj->transform->GetParent() == 0)
			mergedObjects.push_back(ObjectFactory::getInstance()->MakeNewGameObject(gobj, true, dest));
	}

	//call onStarts for all components
	for (GameObject*const& obj : mergedObjects)
	{
		obj->myState = dest;
		obj->OnForceStart(true);
	}

	dest->DoPropertyRefresh();

	GSM::getInstance()->RemoveState(src);
	return;
}

std::vector<StateObjDesc> StateSerializer::LoadStateInformation(std::string filename, bool remove_state, BaseState** ret_newstate)
{
	LoadState(filename, false);
	BaseState* new_state = GSM::getInstance()->getActiveState();


	std::vector<StateObjDesc> descs;
	for (GameObject* gobj : *new_state->GetManagedObjectsSlotTable().GetAllocatedObjects())
	{
		StateObjDesc desc;
		desc.name = gobj->GetName();
		desc.hasParent = (gobj->transform->GetParent() != 0);
		descs.push_back(desc);
	}

	if (remove_state)
		GSM::getInstance()->PopState();
	else
	{
		if (ret_newstate)
			*ret_newstate = new_state;
	}

	return descs;
}

void StateSerializer::LoadStateAdditiveMarkers(std::string filename, std::string markerID)
{
	LoadState(filename, false, Math::Vector3::cZero);
	BaseState* new_state = GSM::getInstance()->getActiveState();
	BaseState* cur_state = GSM::getInstance()->getStateUnderActive();

	//find marker in new state
	const std::list<CStreamMarker*>* markers = new_state->GetListOfComponentsOfType<CStreamMarker>();
	if (!markers)
		MAssert(false, "Could not find marker in new state from stream");
	CStreamMarker* new_marker, *cur_marker;
	new_marker = cur_marker = 0;
	for (CStreamMarker* mark : *markers)
	{
		if (mark->markerID == markerID)
		{
			new_marker = mark;
			break;
		}
	}
	if (new_marker == 0)
		MAssert(false, "Could not find marker in new state from stream");

	//find marker in current state
	markers = cur_state->GetListOfComponentsOfType<CStreamMarker>();
	if (!markers)
		MAssert(false, "Could not find marker in current state from stream");
	for (CStreamMarker* mark : *markers)
	{
		if (mark->markerID == markerID)
		{
			cur_marker = mark;
			break;
		}
	}
	if (cur_marker == 0)
		MAssert(false, "Could not find marker in current state from stream");

	Math::Vector3 offset = cur_marker->gameObject->transform->GetGlobalPosition() - new_marker->gameObject->transform->GetGlobalPosition();

	DoOffset(new_state, offset);

	//merge all objects
	MergeStates(new_state, cur_state, 0);
}

void StateSerializer::DoOffset(BaseState* new_state, Math::Vector3& offset)
{
	//do initial position offset
	auto* trans = new_state->GetListOfComponentsOfType<CTransform>();
	if (trans)
	{
		for (CTransform* ct : *trans)
			if (ct->GetParent() == 0)
				ct->localPosition += offset;
		Log("Moved: " + ToString(trans->size()) + " objects");
	}
	//fix collision
	/*auto* phys = new_state->GetListOfComponentsOfType<CPhysics>();
	if (phys)
	{
		for (CPhysics* ct : *phys)
			ct->SetPosition(Math::ToVector4(ct->gameObject->transform->localPosition));
	}*/
}


void Map::LoadMap(std::string filename)
{
	meta::metafunction_method* fnc = MAKEMETAMETHODVARIABLE(MapDelegators, Del_LoadMap);
	fnc->prepack_params(filename);
	EngineMsgDelegator::getInstance().Push(fnc, MapDelegators::getInstance());
}

void Map::LoadMapDontUnload(std::string filename)
{
	meta::metafunction_method* fnc = MAKEMETAMETHODVARIABLE(MapDelegators, Del_LoadMapNoUnload);
	fnc->prepack_params(filename);
	EngineMsgDelegator::getInstance().Push(fnc, MapDelegators::getInstance());
}

void Map::LoadOnlyMap(std::string filename)
{
	meta::metafunction_method* fnc = MAKEMETAMETHODVARIABLE(MapDelegators, Del_StateClearAndReplace);
	fnc->prepack_params(filename);
	EngineMsgDelegator::getInstance().Push(fnc, MapDelegators::getInstance());
}

void Map::UnloadMap(void)
{
	if (GSM::getInstance()->GetNumberOfStates() <= 1)
	{
		Log("Could not unload map - cannot remove only state");
		return;
	}

	meta::metafunction_method* fnc = MAKEMETAMETHODVARIABLE(MapDelegators, Del_UnloadState);
	fnc->prepack_params(GSM::getInstance()->getActiveState());
	EngineMsgDelegator::getInstance().Push(fnc, MapDelegators::getInstance());
}

void Map::ReloadMap()
{
	BaseState* s = GSM::getInstance()->getActiveState();
	ReloadMap_Aux(s);
}

void Map::ReloadMap_Aux(BaseState* s)
{
	if (s)
	{
		if (s->filepath.empty())
			Log("Could not ReloadMap, no string name set (was this map not loaded from file?)");
		else
		{
			Log("Reloading map and removing other maps....");
			LoadOnlyMap(s->filepath);
			Log("...Done reloading.");
		}
	}
	else
		Log("Could not ReloadMap because there is no active map");
}

void Map::ReloadPreviousMap()
{
	if (GSM::getInstance()->GetNumberOfStates() < 2)
	{
		Log("Could not ReloadPreviousMap because there is no previous map or there are no maps at all.");
		return;
	}

	BaseState* s = GSM::getInstance()->getStateUnderActive();
	if (s)
		ReloadMap_Aux(s);
	else
		Log("Could not ReloadPreviousMap because there is no previous map or there are no maps or the active state is the bottom state");
}

void Map::LoadMapAdditive(std::string filename, Math::Vector3 offset)
{
	meta::metafunction_method* fnc = MAKEMETAMETHODVARIABLE(MapDelegators, Del_LoadMapAdditive);
	fnc->prepack_params(filename, offset);
	EngineMsgDelegator::getInstance().Push(fnc, MapDelegators::getInstance());
}

void Map::DeloadZone(IZone* zone)
{
	std::vector<GameObject*> gobjs = zone->GetObjectsInBounds();
	//zone is no longer valid past this point - it is in the list of things to be deleted
	for (GameObject* gobj : gobjs)
		Destroy(gobj);
	Log("Removed " + ToString(gobjs.size()) + "from zone");
}

void Map::LoadMapAdditiveMergeMarkers(std::string filename, std::string markerID)
{
	meta::metafunction_method* fnc = MAKEMETAMETHODVARIABLE(MapDelegators, Del_LoadMapAdditiveMarkers);
	fnc->prepack_params(filename, markerID);
	EngineMsgDelegator::getInstance().Push(fnc, MapDelegators::getInstance());
}

SerializationException::SerializationException(std::string _msg, SERIALIZATIONEXCEPTIONTYPE _type) : msg(_msg), type(_type)
{
	Log(msg, LogEntry::Exception);
}

#pragma region Delegators

void MapDelegators::Del_StateClearAndReplace(std::string param)
{
	while (GSM::getInstance()->GetNumberOfStates() != 0)
		GSM::getInstance()->PopState();

	Del_LoadMap(param);
}

void MapDelegators::Del_UnloadState(BaseState* state)
{
	assert(state != 0);

	EngineMsgDelegator::getInstance().ClearQueue();
	GSM::getInstance()->RemoveState(state);
}

void MapDelegators::Del_LoadMapNoUnload(std::string filename)
{
	EngineMsgDelegator::getInstance().ClearQueue();
	StateSerializer s;
	s.LoadState(filename, false);
}

void MapDelegators::Del_LoadMap(std::string filename)
{
	EngineMsgDelegator::getInstance().ClearQueue();

	StateSerializer s;
	s.LoadState(filename, true);
}

MapDelegators* MapDelegators::getInstance()
{
	static MapDelegators* inst = 0;
	if (inst == 0)
		inst = new MapDelegators();
	return inst;
}

void MapDelegators::Del_LoadMapAdditive(std::string filename, Math::Vector3 offset)
{
	EngineMsgDelegator::getInstance().ClearQueue();
	StateSerializer s;
	s.LoadStateAdditive(filename, offset);
}

void MapDelegators::Del_LoadMapAdditiveMarkers(std::string filename, std::string markerID)
{
	EngineMsgDelegator::getInstance().ClearQueue();
	StateSerializer s;
	s.LoadStateAdditiveMarkers(filename, markerID);
}

#pragma endregion Delegators
