/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "EditorMsgDelegators.h"
#include <string>
#include "Rectangle.h"
#include "ISubsystem.h"
#include "MochaDotNetCallbacks.h"
#include "EditorState.h"
#include "EditorSubsystem_WidgetManager.h"

class GameObject;
class CCamera;
class EditorSubsystem : public ISubsystem
{
	GameObject* editorTransformWidget;
	CCamera* editorCamera;
	GameObject* editorSoundObj;
	EditorState* editorState;
	bool isTesting;
	//the size and pos of the engine when docked

	Rect dockedBounds;
	//the size and pos of the engine when fullscreened in-editor
	Rect fullEngineBounds;

	MochaDotNetCallback mTickCallback;
	static EditorSubsystem* instance;

	

	Vector2  initialDragPos;
	Vector2  finalDragPos;
	bool beginDrag;
	bool endDrag;
public:
	EditorSubsystem();
	bool Tick();

	//if the state restarts this is called
	void OnRestart();

	EditorState* GetEditorState() const { return editorState; }
	CCamera* EditorCamera() const { return editorCamera; }
	bool IsTesting() const { return isTesting; }
	GameObject* EditorTransformWidget() const { return editorTransformWidget; }

	void SetSelectedObjects(std::vector<GameObject*> objs);

	EditorSubsystemTools::WidgetManager widgetManager;

	void BeginTesting();
	void EndTesting();

	void Register_Tick(MochaDotNetCallback _callback);

	bool CheckMouseDrag();
	void UpdateMouseDrag();

	static EditorSubsystem* getInstance();


	friend class MainContentComponent;

	int gridSize;

	metadef(EditorSubsystem)
		endmetadef;
};