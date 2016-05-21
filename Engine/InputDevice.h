#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "InputDeviceType.h"

#include <intsafe.h>
#include <wtypes.h>
#include <string>

class InputDevice
{
public:
    typedef float(InputDevice::* KeyStatusFunc)(unsigned int,bool) const;
public:
    InputDevice(unsigned int type);
    virtual ~InputDevice();
    void Load();
    void Unload();
    float Pressed(unsigned int key, 
                 bool isBinary) const;
    float Depressed(unsigned int key,
                   bool isBinary) const;
    float Released(unsigned int key,
                  bool isBinary) const;
    float Triggered(unsigned int key,
                   bool isBinary) const;
    bool BinaryValue(unsigned int key) const;
    float AnalogValue(unsigned int key) const;
    float Value(unsigned int key,
                bool isBinary) const;
    void Update();
    void EventUpdate(HWND hwnd,
                     UINT umsg,
                     WPARAM wparam,
                     LPARAM lparam);
public:
    bool Enable() const;
    void Enable(bool val, bool force = false);
    unsigned int Type() const;
protected:
    virtual void disableDevice_();
    virtual void enableDevice_();
    virtual void load_();
    virtual void unload_();
    // Used for event triggered device updates
    // All changes will be applied on main update function
    virtual void eventUpdate_(HWND,UINT,WPARAM,LPARAM){}
    virtual void updatePrevValues_() = 0;
    virtual void updateCurrValues_() = 0;
    virtual bool getPreviousBinaryKeyStatus_(unsigned int key) const = 0;
    virtual bool getCurrentBinaryKeyStatus_(unsigned int key) const = 0;
    virtual float getPreviousAnalogKeyStatus_(unsigned int key) const;
    virtual float getCurrentAnalogKeyStatus_(unsigned int key) const;
private:
    bool enable_;
    unsigned int type_;
};

#endif