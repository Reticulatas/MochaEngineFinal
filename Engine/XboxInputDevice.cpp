#include "stdafx.h"
#include "XboxInputDevice.h"
#include "XInput.h"

#include "StringUtils.h"
#include <iostream>

#define XBOX_THUMBSTICK_MIN -32768.0f
#define XBOX_THUMBSTICK_MAX 32767.0f
#define XBOX_TRIGGER_MIN 0.0f
#define XBOX_TRIGGER_MAX 255.0f

static const unsigned int XboxKeyMap[XBB_TOTAL] = 
{
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_LEFT_THUMB,
    XINPUT_GAMEPAD_RIGHT_THUMB,
    XINPUT_GAMEPAD_LEFT_SHOULDER,
    XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y
};

#define GENERATE_STRING(STRING) #STRING,
static const char *XboxAKeyToStrMap[] = {FOREACH_ABUTTON(GENERATE_STRING)};
static const char *XboxBKeyToStrMap[] = {FOREACH_BBUTTON(GENERATE_STRING)};
static std::map<std::string,int> XboxStrToAKeyMap;
static std::map<std::string,int> XboxStrToBKeyMap;

XboxInputDevice::XboxInputDevice(unsigned int& id)
:   InputDevice(IDT_XBOX),
    id_(id),
    connected_(false)
{
    for(unsigned int i = 0; i < XAB_TOTAL; ++i)
        prevAKeys_[i] = currAKeys_[i] = 0;

    for(unsigned int i = 0; i < XBB_TOTAL; ++i)
        prevBKeys_[i] = currBKeys_[i] = false;

    initStrKeyMap_();
}

float XboxInputDevice::LeftThumbX() const
{
    return prevAKeys_[XAB_LEFT_THUMB_X];
}

float XboxInputDevice::LeftThumbY() const
{
    return prevAKeys_[XAB_LEFT_THUMB_Y];
}

float XboxInputDevice::RightThumbX() const
{
    return prevAKeys_[XAB_RIGHT_THUMB_X];
}

float XboxInputDevice::RightThumbY() const
{
    return prevAKeys_[XAB_RIGHT_THUMB_Y];
}

float XboxInputDevice::LeftTrigger() const
{
    return prevAKeys_[XAB_LEFT_TRIGGER];
}

float XboxInputDevice::RightTrigger() const
{
    return prevAKeys_[XAB_RIGHT_TRIGGER];
}

int XboxInputDevice::GetKey(const std::string& key,
                            bool isBinary)
{
    if(isBinary)
    {
        std::map<std::string,int>::iterator it = XboxStrToBKeyMap.find(key);

        if(it != XboxStrToBKeyMap.end())
            return it->second;
        return -1;
    }
    else
    {
        std::map<std::string,int>::iterator it = XboxStrToAKeyMap.find(key);

        if(it != XboxStrToAKeyMap.end())
            return it->second;
        return -1;
    }
}

std::string XboxInputDevice::GetKey(unsigned int key,
                                    bool isBinary)
{
    if(isBinary)
    {
        if(key >= XBB_TOTAL)
            return std::string();
        return XboxBKeyToStrMap[key];
    }
    else
    {
        if(key >= XAB_TOTAL)
            return std::string();
        return XboxAKeyToStrMap[key];
    }
}

//------------------------------------------------------------------------------

unsigned int XboxInputDevice::ID() const
{
    return id_;
}

//------------------------------------------------------------------------------

bool XboxInputDevice::isConnected_()
{
    ZeroMemory(&state_,sizeof(XINPUT_STATE));

    DWORD Result = XInputGetState(id_,&state_);

    if(Result == ERROR_SUCCESS)
    {
        if(!connected_)
            Log("Player " + ToString(id_ + 1) + " Controller Found!",LogEntry::ScriptOutput);
        connected_ = true;
    }
    else
    {
        if(connected_)
            Log("Player " + ToString(id_ + 1) + " Controller Disconnected!",LogEntry::ScriptOutput);
        connected_ = false;
    }
    return connected_;
}

void XboxInputDevice::updatePrevValues_()
{
    for(unsigned int i = 0; i < XAB_TOTAL; ++i)
        prevAKeys_[i] = currAKeys_[i];

    for(unsigned int i = 0; i < XBB_TOTAL; ++i)
        prevBKeys_[i] = currBKeys_[i];
}

void XboxInputDevice::updateCurrValues_()
{
    if(!isConnected_())
        return;

    for(unsigned int i = 0; i < XBB_TOTAL; ++i)
        prevBKeys_[i] = currBKeys_[i];

    // If theres no change detected, do nothing
    if(prevStateID_ == state_.dwPacketNumber)
        return;

    prevStateID_ = state_.dwPacketNumber;

    updateBinaryButtons_();
    updateAnalogButtons_();
}


bool XboxInputDevice::getPreviousBinaryKeyStatus_(unsigned int key) const
{
    return prevBKeys_[key];
}

bool XboxInputDevice::getCurrentBinaryKeyStatus_(unsigned int key) const
{
    return currBKeys_[key];
}

float XboxInputDevice::getPreviousAnalogKeyStatus_(unsigned int key) const
{
    return prevAKeys_[key];
}

float XboxInputDevice::getCurrentAnalogKeyStatus_(unsigned int key) const
{
    return currAKeys_[key];
}

void XboxInputDevice::updateBinaryButtons_()
{
    for(unsigned int i = 0; i < XBB_TOTAL; ++i)
    {
        prevBKeys_[i] = currBKeys_[i];
        currBKeys_[i] = ((state_.Gamepad.wButtons & XboxKeyMap[i]) > 0) ? true : false;
    }
}

void XboxInputDevice::updateAnalogButtons_()
{
    updateThumbstick_(state_.Gamepad.sThumbLX,XAB_LEFT_THUMB_X);
    updateThumbstick_(state_.Gamepad.sThumbLY,XAB_LEFT_THUMB_Y);
    updateThumbstick_(state_.Gamepad.sThumbRX,XAB_RIGHT_THUMB_X);
    updateThumbstick_(state_.Gamepad.sThumbRY,XAB_RIGHT_THUMB_Y);
    updateTrigger_(state_.Gamepad.bLeftTrigger,XAB_LEFT_TRIGGER);
    updateTrigger_(state_.Gamepad.bRightTrigger,XAB_RIGHT_TRIGGER);
}

void XboxInputDevice::updateThumbstick_(SHORT gamePadVal,
                                        unsigned int type)
{
    if(std::abs(gamePadVal) > XBOX_THUMBSTICK_DEADZONE)
    {
        if(gamePadVal < 0)
            currAKeys_[type] = -gamePadVal / XBOX_THUMBSTICK_MIN;
        else
            currAKeys_[type] = gamePadVal / XBOX_THUMBSTICK_MAX;
    }
    else
        currAKeys_[type] = 0;
}

void XboxInputDevice::updateTrigger_(SHORT gamePadVal,
                                     unsigned int type)
{
    if(gamePadVal > XBOX_TRIGGER_DEADZONE)
        currAKeys_[type] = gamePadVal / XBOX_TRIGGER_MAX;
    else
        currAKeys_[type] = XBOX_TRIGGER_MIN;
}

void XboxInputDevice::initStrKeyMap_()
{
    for(unsigned int i = 0; i < XBB_TOTAL; ++i)
        XboxStrToBKeyMap.insert(std::pair<std::string,unsigned int>(XboxBKeyToStrMap[i],i));

    for(unsigned int i = 0; i < XAB_TOTAL; ++i)
        XboxStrToAKeyMap.insert(std::pair<std::string,unsigned int>(XboxAKeyToStrMap[i],i));
}