#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include "InputDevice.h"
#include "KeyboardInputDevice.h"
#include "MouseInputDevice.h"

#include <list>

class PlayerInput
{
public:
    typedef std::list<InputDevice*> InputDeviceList;
public:
    PlayerInput(unsigned int id);
    ~PlayerInput();
    void Load();
    void Update();
    void Unload();
    void AddInputDevice(unsigned int type);
    void RemoveInputDevice(unsigned int type);
public:
    void EnableAllDevices(bool val);
    unsigned int ID() const;
    void ID(unsigned int val);
    InputDeviceList InputDevices() const;
    InputDevice* InputDevices(unsigned int type) const;
private:
    bool enable_;
    unsigned int id_;
    KeyboardInputDevice* keyboard_;
    MouseInputDevice* mouse_;
    InputDeviceList inputDevices_;
};

#endif 