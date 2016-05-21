#include "stdafx.h"
#include "InputDevice.h"
#include "Log.h"

#include <iostream>
#include <fstream>
#include <float.h>

InputDevice::InputDevice(unsigned int type)
                         :  type_(type),
                            enable_(true)
{
}

InputDevice::~InputDevice()
{
}

void InputDevice::Load()
{
    load_();
}

void InputDevice::Unload()
{
    unload_();
}

float InputDevice::Pressed(unsigned int key,
                           bool isBinary) const
{
    if(!enable_)
        return 0.0f;

    if(isBinary)
        return getCurrentBinaryKeyStatus_(key) ? 1.0f : 0.0f;
    else
        return getCurrentAnalogKeyStatus_(key) > 0.0f ? 1.0f : 0.0f;
}

float InputDevice::Depressed(unsigned int key,
                             bool isBinary) const
{
    if(!enable_)
        return 0.0f;

    if(isBinary)
        return (!getCurrentBinaryKeyStatus_(key) && !getPreviousBinaryKeyStatus_(key)) ? 1.0f : 0.0f;
    else
        return (getCurrentAnalogKeyStatus_(key) < FLT_MIN && getPreviousAnalogKeyStatus_(key) < FLT_MIN) ? 1.0f : 0.0f;
}

float InputDevice::Released(unsigned int key,
                            bool isBinary) const
{
    if(!enable_)
        return 0.0f;

    if(isBinary)
        return (!getCurrentBinaryKeyStatus_(key) && getPreviousBinaryKeyStatus_(key)) ? 1.0f : 0.0f;
    else
        return (getCurrentAnalogKeyStatus_(key) < FLT_MIN && getPreviousAnalogKeyStatus_(key) > 0.0f) ? 1.0f : 0.0f;
}

float InputDevice::Triggered(unsigned int key,
                            bool isBinary) const
{
    if(!enable_)
        return 0.0f;

    if(isBinary)
        return (getCurrentBinaryKeyStatus_(key) && !getPreviousBinaryKeyStatus_(key)) ? 1.0f : 0.0f;
    else
    {
        float curr = getCurrentAnalogKeyStatus_(key);
        return (std::abs(curr) > 0.0f && std::abs(getPreviousAnalogKeyStatus_(key)) < FLT_MIN) ? curr : 0.0f;
    }
}

bool InputDevice::BinaryValue(unsigned int key) const
{
    return getCurrentBinaryKeyStatus_(key);
}

float InputDevice::AnalogValue(unsigned int key) const
{
    return getCurrentAnalogKeyStatus_(key);
}

float InputDevice::Value(unsigned int key,
                         bool isBinary) const
{
    if(!enable_)
        return 0.0f;

    if(isBinary)
        return BinaryValue(key) ? 1.0f : 0.0f;
    else
        return AnalogValue(key);
}

void InputDevice::Update()
{
    if(enable_)
    {
        updatePrevValues_();
        updateCurrValues_();
    }
}

void InputDevice::EventUpdate(HWND hwnd,
                              UINT umsg,
                              WPARAM wparam,
                              LPARAM lparam)
{
    if(enable_)
        eventUpdate_(hwnd,umsg,wparam,lparam);
}

//------------------------------------------------------------------------------

bool InputDevice::Enable() const
{
    return enable_;
}

void InputDevice::Enable(bool val, bool force /* = false */)
{
    // To prevent dramatic mouse movement when loosing focus of the window
    // we need to update the mouse position here so mouse delta is 0
    if(val != enable_ && !force)
    {
        updateCurrValues_();
        updatePrevValues_();
    }
    enable_ = val;
}

unsigned int InputDevice::Type() const
{
    return type_;
}

//-----------------------------------------------------------------------------

void InputDevice::disableDevice_()
{
}

void InputDevice::enableDevice_()
{
}

void InputDevice::load_()
{
}

void InputDevice::unload_()
{
}

float InputDevice::getPreviousAnalogKeyStatus_(unsigned int key) const
{
    return 0.f;
}

float InputDevice::getCurrentAnalogKeyStatus_(unsigned int key) const
{
    return 0.f;
}