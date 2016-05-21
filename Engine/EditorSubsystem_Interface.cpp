#include "stdafx.h"
#include "EditorSubsystem_Interface.h"
#include "EditorSubsystem.h"
#include "ThreadUtils.h"
#include "GameObject.h"
#include "Engine.h"
#include "InputManager.h"

void EditorSS::EndTesting( void )
{
	if (!Engine::getInstance()->GetIsEditorMode())
		throw std::exception("End Testing Called on non-editor engine");
	EditorSubsystem::getInstance()->EndTesting();
}

void EditorSS::BeginTesting( void )
{
	if (!Engine::getInstance()->GetIsEditorMode())
		throw std::exception("Begin Testing Called on non-editor engine");
	EditorSubsystem::getInstance()->BeginTesting();
}

bool EditorSS::IsTesting( void )
{
	if (!Engine::getInstance()->GetIsEditorMode())
		return false;
	return EditorSubsystem::getInstance()->IsTesting();
}

bool EditorSS::IsInMissionCritical( void )
{
	return ThreadUtils::IsCritSectionInUse(2);
}

