#include "stdafx.h"
#include "KeyboardInputDevice.h"

#include <winuser.h>
#include <iostream>

const static unsigned int KeyboardKeyMap[KBB_TOTAL] =
{
    VK_BACK,
    VK_TAB,
    VK_CLEAR,
    VK_RETURN,
    VK_SHIFT,
    VK_CONTROL,
    VK_MENU,
    VK_PAUSE,
    VK_CAPITAL,
    VK_ESCAPE,
    VK_SPACE,
    VK_PRIOR,
    VK_NEXT,
    VK_END,
    VK_HOME,
    VK_LEFT,
    VK_UP,
    VK_RIGHT,
    VK_DOWN,
    VK_SNAPSHOT,
    VK_INSERT,
    VK_DELETE,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A,
    0x4B,
    0x4C,
    0x4D,
    0x4E,
    0x4F,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5A,
    VK_LWIN,
    VK_RWIN,
    VK_NUMPAD0,
    VK_NUMPAD1,
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,
    VK_MULTIPLY,
    VK_ADD,
    VK_SEPARATOR,
    VK_SUBTRACT,
    VK_DECIMAL,
    VK_DIVIDE,
    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_F11,
    VK_F12,
    VK_F13,
    VK_F14,
    VK_F15,
    VK_F16,
    VK_F17,
    VK_F18,
    VK_F19,
    VK_F20,
    VK_F21,
    VK_F22,
    VK_F23,
    VK_F24,
    VK_NUMLOCK,
    VK_SCROLL,
    VK_LSHIFT,
    VK_RSHIFT,
    VK_LCONTROL,
    VK_RCONTROL
};

#define GENERATE_STRING(STRING) #STRING,
static const char *KeyboardBKeyToStrMap[] = {FOREACH_BBUTTON(GENERATE_STRING)};
static std::map<std::string,int> KeyboardStrToBKeyMap;

KeyboardInputDevice::KeyboardInputDevice()
    :   InputDevice(IDT_KEYBOARD)
{
    for(unsigned int i = 0; i < KBB_TOTAL; ++i)
        prevKeys_[i] = currKeys_[i] = false;
    initStrKeyMap_();
}

KeyboardInputDevice::~KeyboardInputDevice()
{
}

int KeyboardInputDevice::GetKey(const std::string& key,
                                bool)
{
    std::map<std::string,int>::iterator it = KeyboardStrToBKeyMap.find(key);
    
    if(it != KeyboardStrToBKeyMap.end())
        return it->second;
    return -1;
}

std::string KeyboardInputDevice::GetKey(unsigned int key,
                                        bool) 
{
    if(key >= KBB_TOTAL)
        return std::string();
    return KeyboardBKeyToStrMap[key];
}

//------------------------------------------------------------------------------

void KeyboardInputDevice::updatePrevValues_()
{
    for(unsigned int i = 0; i < KBB_TOTAL; ++i)
        prevKeys_[i] = currKeys_[i];
}

void KeyboardInputDevice::updateCurrValues_()
{
    for(unsigned int i = 0; i < KBB_TOTAL; ++i)
    {
        if(GetAsyncKeyState(KeyboardKeyMap[i]) & 0x8000)
            currKeys_[i] = true;
        else
            currKeys_[i] = false;
    }
}

bool KeyboardInputDevice::getPreviousBinaryKeyStatus_(unsigned int key) const
{
    return prevKeys_[key];
}

bool KeyboardInputDevice::getCurrentBinaryKeyStatus_(unsigned int key) const
{
    return currKeys_[key];
}

void KeyboardInputDevice::initStrKeyMap_()
{
    for(unsigned int i = 0; i < KBB_TOTAL; ++i)
        KeyboardStrToBKeyMap.insert(std::pair<std::string,unsigned int>(KeyboardBKeyToStrMap[i],i));
}