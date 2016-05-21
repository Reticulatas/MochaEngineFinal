#include "stdafx.h"
#include "PlayerInput.h"
#include "XboxInputDevice.h"
#include "KeyboardInputDevice.h"
#include "MouseInputDevice.h"
#include "InputManager.h"

PlayerInput::PlayerInput(unsigned int id)
    :   enable_(true),
        id_(id),
        keyboard_(0),
        mouse_(0)
{
}

PlayerInput::~PlayerInput()
{
    InputDeviceList::iterator inputIt = inputDevices_.begin();

    while(inputIt != inputDevices_.end())
    {
        delete *inputIt;
        inputDevices_.erase(inputIt);
        inputIt = inputDevices_.begin();
    }
}

void PlayerInput::Load()
{
    InputDeviceList::iterator it = inputDevices_.begin();
    while(it != inputDevices_.end())
    {
        (*it)->Load();
        ++it;
    }
}

void PlayerInput::Update()
{
    InputDeviceList::iterator inputIt = inputDevices_.begin();

    while(inputIt != inputDevices_.end())
    {
        (*inputIt)->Update();
        ++inputIt;
    }
}

void PlayerInput::Unload()
{
    InputDeviceList::iterator it = inputDevices_.begin();
    while(it != inputDevices_.end())
    {
        (*it)->Unload();
        ++it;
    }
}

void PlayerInput::AddInputDevice(unsigned int type)
{
    // factory needed
    switch(type)
    {
    case IDT_KEYBOARD:
        keyboard_ = InputManager::getInstance()->GetKeyboard();
        break;
    case IDT_MOUSE:
        mouse_ = InputManager::getInstance()->GetMouse();
        break;
    case IDT_XBOX:
        inputDevices_.push_back(new XboxInputDevice(id_));
        break;
    }
}

void PlayerInput::RemoveInputDevice(unsigned int type)
{
    InputDeviceList::iterator inputIt = inputDevices_.begin();

    while(inputIt != inputDevices_.end())
    {
        if((*inputIt)->Type() == type)
        {
            if(type != IDT_KEYBOARD || type != IDT_MOUSE)
                delete *inputIt;

            inputDevices_.erase(inputIt);
            return;
        }
        ++inputIt;
    }
}

//------------------------------------------------------------------------------

void PlayerInput::EnableAllDevices(bool val)
{
    InputDeviceList::iterator it = inputDevices_.begin();
    while(it != inputDevices_.end())
    {
        (*it)->Enable(val);
        ++it;
    }
}

unsigned int PlayerInput::ID() const
{
    return id_;
}

void PlayerInput::ID(unsigned int val)
{
    id_ = val;
}

PlayerInput::InputDeviceList PlayerInput::InputDevices() const
{
    InputDeviceList deviceList;
    
    if(mouse_)
        deviceList.push_back(mouse_);
    if(keyboard_)
        deviceList.push_back(keyboard_);

    InputDeviceList::const_iterator it = inputDevices_.begin();
    
    while(it != inputDevices_.end())
    {
        deviceList.push_back(*it);
        ++it;
    }
    return deviceList;
}

InputDevice* PlayerInput::InputDevices(unsigned int type) const
{
    if(type == IDT_KEYBOARD)
        return keyboard_;
    
    if(type == IDT_MOUSE)
        return mouse_;

    InputDeviceList::const_iterator inputIt = inputDevices_.begin();
    
    while(inputIt != inputDevices_.end())
    {
        if((*inputIt)->Type() == type)
            return *inputIt;
        ++inputIt;
    }
    return NULL;
}