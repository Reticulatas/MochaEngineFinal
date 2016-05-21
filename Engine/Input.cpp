/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
#include "InputManager.h"
#include "MouseInputDevice.h"
#include "InputDeviceType.h"
#include "PlayerInput.h"
 

std::map<std::string, bool> lastFrameValues;

float Input::GetTriggered( int playerIndex, std::string inputName )
{
    InputManager* iManager = InputManager::getInstance();
    return iManager->Triggered(inputName,static_cast<int>(playerIndex));
}

float Input::GetHeld( int playerIndex, std::string inputName )
{
    InputManager* iManager = InputManager::getInstance();
    return iManager->Pressed(inputName,static_cast<int>(playerIndex));
}

float Input::GetValue( int playerIndex, std::string inputName )
{
    InputManager* iManager = InputManager::getInstance();
    return iManager->Value(inputName,static_cast<int>(playerIndex));
}

int Input::GetMouseX(bool gameWnd)
{
    InputManager* iManager = InputManager::getInstance();
    if(gameWnd)
        return static_cast<int>(iManager->GetMouse()->ClientX(Engine::getInstance()->GetGameWnd()->GetHWND()));
    return static_cast<int>(iManager->GetMouse()->ClientX(Engine::getInstance()->GetVRWnd()->GetHWND()));
}

int Input::GetMouseY(bool gameWnd)
{
    InputManager* iManager = InputManager::getInstance();
    if(gameWnd)
        return static_cast<int>(iManager->GetMouse()->ClientY(Engine::getInstance()->GetGameWnd()->GetHWND()));
    return static_cast<int>(iManager->GetMouse()->ClientY(Engine::getInstance()->GetVRWnd()->GetHWND()));
}

int Input::GetMouseDeltaX()
{
    InputManager* iManager = InputManager::getInstance();
    return iManager->GetMouse()->DeltaX();
}

int Input::GetMouseDeltaY()
{
    InputManager* iManager = InputManager::getInstance();
    return iManager->GetMouse()->DeltaY();
}

int Input::GetMouseWheelDeltaY()
{
    InputManager* iManager = InputManager::getInstance();
    return iManager->GetMouse()->DeltaY();
}

bool Input::IsPlayerUsingKeyboard( int playerIndex )
{
    InputManager* iManager = InputManager::getInstance();
    PlayerInput* player = iManager->GetPlayer(static_cast<int>(playerIndex));

	if (player)
		return player->InputDevices(IDT_KEYBOARD) ? true : false;
	return false;
}

bool Input::GetIsMouseInWindow(bool gameWnd)
{
    if(gameWnd)
        return InputManager::getInstance()->GetMouse()->InWindow(Engine::getInstance()->GetGameWnd()->GetHWND());
    return InputManager::getInstance()->GetMouse()->InWindow(Engine::getInstance()->GetVRWnd()->GetHWND());

}

void Input::SetMouseLock(bool val)
{
    InputManager::getInstance()->GetMouse()->Lock(val);
}

bool Input::GetLockMouse()
{
    return InputManager::getInstance()->GetMouse()->Lock();
}

void Input::SetMouseVisible(bool val)
{
    InputManager::getInstance()->GetMouse()->Visible(val);
}

bool Input::GetMouseVisible()
{
    return InputManager::getInstance()->GetMouse()->Visible();
}

bool Input::GetCtrlAltDel()
{
    return InputManager::getInstance()->CtrlAltDel();
}

float Input::GetReleased(int playerIndex, std::string inputName)
{
	InputManager* iManager = InputManager::getInstance();
	return iManager->Released(inputName, static_cast<int>(playerIndex));
}

void Input::SetMousePosition(int x, int y)
{
	InputManager* iManager = InputManager::getInstance();
	iManager->GetMouse()->SetPosition(x, y);
}

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change