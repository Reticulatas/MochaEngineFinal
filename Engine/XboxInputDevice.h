#ifndef XBOXINPUTDEVICE_H
#define XBOXINPUTDEVICE_H

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput.lib")

#include "InputDevice.h"
#include "XboxButtonTypes.h"

#define XBOX_THUMBSTICK_DEADZONE 8000
#define XBOX_TRIGGER_DEADZONE 0

class XboxInputDevice : public InputDevice
{
public:
    XboxInputDevice(unsigned int& id);
    float LeftThumbX() const;
    float LeftThumbY() const;
    float RightThumbX() const;
    float RightThumbY() const;
    float LeftTrigger() const;
    float RightTrigger() const;
    static int GetKey(const std::string& key,
                      bool isBinary = true);
    static std::string GetKey(unsigned int key,
                              bool isBinary = true);
public:
    unsigned int ID() const;
protected:
    bool isConnected_();
    void updatePrevValues_();
    void updateCurrValues_();
    bool getPreviousBinaryKeyStatus_(unsigned int key) const;
    bool getCurrentBinaryKeyStatus_(unsigned int key) const;
    float getPreviousAnalogKeyStatus_(unsigned int key) const;
    float getCurrentAnalogKeyStatus_(unsigned int key) const;
private:
    void updateBinaryButtons_();
    void updateAnalogButtons_();
    void updateThumbstick_(SHORT gamePadVal,
                           unsigned int type);
    void updateTrigger_(SHORT gamePadVal,
                           unsigned int type);
    void initStrKeyMap_();
private:
    bool connected_;
    unsigned int& id_;
    DWORD prevStateID_;
    XINPUT_STATE state_;
    float prevAKeys_[XAB_TOTAL];
    float currAKeys_[XAB_TOTAL];
    bool prevBKeys_[XBB_TOTAL];
    bool currBKeys_[XBB_TOTAL];
};
#endif