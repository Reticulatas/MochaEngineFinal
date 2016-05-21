/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"

#include "EditorSubsystem.h"
#include "EditorSubsystem_Interface.h"
#include "MouseInputDevice.h"
#include "StateSerializer.h"
#include "CCamera.h"
#include "Input.h"
#include "Renderer.h"
#include "GameObject.h"
#include "CTransform.h"
#include "PhysicsEngine.h"
#include "CWidget_Transform.h"
#include "CCamera.h"
#include "CSound.h"
#include "InputManager.h"
#include "FrameController.h"
#include "ObjectFactory.h"
#include "EditorState.h"
#include "AudioManager.h"
#include "Plane.h"
#include "CBloom.h"

static const float CAMSPEED = 10.0f;				//speed of the right click camera movement
static const float CAMZOOMSPEED = 5.0f;				//zoom speed of the camera using the mouse wheel
static float SPEEDMOD = 1.0f;						//speed multiplier of the right click camera movement when shift is held
static const float HIERARCHYREFRESHTIME = 10.0f;		//time in between hierarchy tree refreshes

EditorSubsystem::EditorSubsystem() : mTickCallback(0), isTesting(false), editorState(0), editorSoundObj(0), editorTransformWidget(0), editorCamera(0), beginDrag(false), endDrag(false)
{
	instance = this;
  int w,h;
  Engine::getInstance()->GetDesktopResolution(w, h);
  fullEngineBounds = Rect(12.0f,12.0f,(float)w-40.0f,(float)h-80.0f);
  OnRestart();
}

bool EditorSubsystem::Tick()
{
  static float timeTilNextSave = 0.0f;
  static float periodicHierarchyRefresh = 0.0f;
  static float scriptReloadTimer = 0.0f;

  if (Input::GetTriggered(0,"EndTesting") == 1.0f)
  {
    if (isTesting)
      EditorSS::EndTesting();
  }

  if (mTickCallback)
	  mTickCallback();

  widgetManager.Tick();

  if (CheckMouseDrag())
	  UpdateMouseDrag();

  if (isTesting)
    return true;
  if (!editorCamera)
	return true;

  return true;
}

EditorSubsystem* EditorSubsystem::getInstance()
{
  if (!Engine::getInstance()->GetIsEditorMode())
	  throw std::exception("Editor Subsystem being created while Engine is not in editor mode");
  if (!instance)
    instance = new EditorSubsystem();

  return instance;
}

void EditorSubsystem::OnRestart()
{
	//push the editor state
	if (editorState)	{
		try {
			GSM::getInstance()->RemoveState(editorState);
		}
		catch (std::exception& ) {
		}
	}
	editorState = new EditorState();
	GSM::getInstance()->PushStateToBottom(editorState);

  assert(editorCamera == 0);
  GameObject* camObj = ObjectFactory::getInstance()->MakeNewGameObject(std::string("EditorCamera"), editorState, true);
  editorCamera = camObj->AddComponent<CCamera>();
  camObj->transform->SetPosition(Vector3(0,0,-3));
  camObj->mLayerMask.SetAllLayers();
  editorCamera->mCamType = CCamera::MAIN;
  editorCamera->DispatchClickMessages = true;
  //Adding Bloom to the editor camera
  //camObj->AddComponent<CBloom>();

  //if you broke here, then you are losing the original widget that should have been unloaded before calling this
  assert(editorTransformWidget == 0);
  editorTransformWidget = ObjectFactory::getInstance()->MakeNewGameObject(std::string("EditorWidget"), editorState, true);
  editorTransformWidget->AddComponent<CWidget_Transform>();
  editorTransformWidget->GetComponent<CWidget_Transform>()->ClearAttachedObjects();
  editorTransformWidget->transform->SetPositionZ(-20.0f);

  assert(editorSoundObj == 0);
  editorSoundObj = ObjectFactory::getInstance()->MakeNewGameObject(std::string("EditorSound"), editorState, true);
  editorSoundObj->AddComponent<CSound>();

  editorCamera->setEnabled(true);
  editorState->IsActive(true);
  isTesting = false;
  PhysicEngine::getInstance()->IsActive(false);

  AudioManager::getInstance()->FreeAllEvents();
  AudioManager::getInstance()->Enable(false);

  editorState->DoPropertyRefresh();
}

void EditorSubsystem::EndTesting()
{
  //no creation of anything in this function
  //only transient
  //MessageManagerLock l;

  //clear lingering messages
  EngineMsgDelegator::getInstance().ClearQueue();

  AudioManager::getInstance()->FreeAllEvents();
  AudioManager::getInstance()->Enable(false);

  //remove testing state
  GSM::getInstance()->PopState();

  if (editorTransformWidget)
    editorTransformWidget->GetComponent<CWidget_Transform>()->ClearAttachedObjects();
  editorCamera->setEnabled(true);
  isTesting = false;
  PhysicEngine::getInstance()->IsActive(false);
  //GSM::getInstance()->getActiveState()->IsActive(false);

}

void EditorSubsystem::BeginTesting()
{
  BlankState* workingState = reinterpret_cast<BlankState*>(GSM::getInstance()->getActiveState());

  AudioManager::getInstance()->Enable(true);

  //MessageManagerLock

  //we start testing immediately
  isTesting = true;
  workingState->IsActive(false);

  //clear lingering messages we are now in mission critical
  EngineMsgDelegator::getInstance().ClearQueue();

  BlankState* runningState = new BlankState(*workingState);
  //do not do inits on this state, we assume it's valid upon copy
  GSM::getInstance()->PushState(runningState, false);
  GSM::getInstance()->getActiveState()->IsActive(true);

  //run one update loop
  runningState->UpdateState();

  editorCamera->setEnabled(false);
  PhysicEngine::getInstance()->IsActive(true);
}

bool EditorSubsystem::CheckMouseDrag()
{
	// Start the drag
	if (Input::GetHeld(0, "MouseLClick") && !beginDrag )
	{
		beginDrag = true;
		initialDragPos.x = (float)Input::GetMouseX(true);
		initialDragPos.y = (float)Input::GetMouseY(true);
	}
	// Update
	else if (beginDrag)
	{
		if (Input::GetReleased(0, "MouseLClick"))
		{
			beginDrag = false;
			endDrag = true;
		}
		//if (!(abs(initialDragPos.x - Input::GetMouseX(true) > 10) &&
		//	!(abs(initialDragPos.y - Input::GetMouseY(true) > 10))))
		//{
		//	return false;
		//}
	}
	// drag has ended
	else
	{
		beginDrag = false;
		endDrag = true;
	}

	return beginDrag;
}
void EditorSubsystem::UpdateMouseDrag()
{
	finalDragPos.x = (float)Input::GetMouseX(true);
	finalDragPos.y = (float)Input::GetMouseY(true);

	Vector3 new_pt;
	//form arbitrary plane in local space of object
	Vector3 plane_normal = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetForwardVector();
	Plane plane;
	plane.Normal = plane_normal;
	plane.Pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();
	plane.Pos += EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetForwardVector();
	//intersect with plane
	Vector3 mouse_ray = EditorSubsystem::getInstance()->EditorCamera()->ScreenToWorldPosition((int)finalDragPos.x, (int)finalDragPos.y);
	Ray3 mouse_ray3;
	mouse_ray3.dir = mouse_ray.Normalized();
	mouse_ray3.pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();

	Vector3 initial_mouse_ray = EditorSubsystem::getInstance()->EditorCamera()->ScreenToWorldPosition((int)initialDragPos.x, (int)initialDragPos.y);
	Ray3 initial_mouse_ray3;
	initial_mouse_ray3.dir = initial_mouse_ray.Normalized();
	initial_mouse_ray3.pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();

	Vector3 intersect_pt;
	Vector3 initial_intersect_pt;

	if (Math::Ray3DToPlane(mouse_ray3, plane, intersect_pt))
	{
		Math::Ray3DToPlane(initial_mouse_ray3, plane, initial_intersect_pt);
		
		Renderer::getInstance()->DrawLine(initial_intersect_pt, intersect_pt);
		//Vector3 topLeft, topRight, btmLeft, btmRight;
		//topLeft = initial_intersect_pt;
		//topRight = initial_intersect_pt;
		//topRight.x = intersect_pt.x;
		//btmRight = intersect_pt;
		//btmLeft = intersect_pt;
		//btmLeft.x = initial_intersect_pt.x;

		//Renderer::getInstance()->DrawLine(intersect_pt, initial_intersect_pt);
		//Renderer::getInstance()->DrawLine(topRight, btmLeft);
		//Renderer::getInstance()->DrawLine(btmRight, btmLeft);
		//Renderer::getInstance()->DrawLine(btmLeft, topLeft);

		//std::string pt1 = std::to_string(initial_intersect_pt.x) + " " + std::to_string(initial_intersect_pt.y) + "\n";
		//std::string pt2 = std::to_string(intersect_pt.x) + " " + std::to_string(intersect_pt.y);
		//LogVerbose(pt1 + pt2);
	}
	if (intersect_pt.Distance(&initial_intersect_pt) > 0.5f)
	{
		int x = 0;
		
	}

	
}

void EditorSubsystem::Register_Tick(MochaDotNetCallback _callback)
{
	mTickCallback = _callback;
}

void EditorSubsystem::SetSelectedObjects(std::vector<GameObject*> objs)
{
	if (editorTransformWidget)
		editorTransformWidget->GetComponent<CWidget_Transform>()->SetAttachedObjects(objs);
}

EditorSubsystem* EditorSubsystem::instance = 0;

meta_define(EditorSubsystem);