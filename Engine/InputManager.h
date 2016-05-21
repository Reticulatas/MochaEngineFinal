/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include "ISerializeable.h"
#include "ISubsystem.h"
#include "InputDevice.h"
#include "InputDeviceType.h"

#include <map>
#include <windows.h>
#include <bitset>

/* SUB SYSTEM FOR INPUT MANAGEMENT */
/* Do not interface with directly, use   instead */

#define GAMEPAD_INPUT_DEADZONE 8000
#define GAMEPAD_TRIGGER_THRESHOLD 30

struct InputManagerLock
{
	~InputManagerLock(void);
	InputManagerLock(void);
private:
	bool maintainInActive;
};

class InputDevice;
class MouseInputDevice;
class KeyboardInputDevice;
class PlayerInput;

class InputManager : public ISubsystem, public ISerializeable
{
private:
    struct InputDeviceKey
    {
        bool isBinaryKey_;
        unsigned int keyCode_;
        InputDeviceType type_;
        bool operator==(const InputDeviceKey& val);
    };

    typedef std::list<InputDeviceKey> InputDeviceKeyList;

    struct PlayerKeyData
    {
        PlayerInput* playerInput_;
        InputDeviceKeyList keyList_;
    };

    typedef std::map<int,PlayerKeyData> IDPlayerKeyDataMap;
    typedef std::map<std::string,IDPlayerKeyDataMap> ActionPlayerKeyMap;
    typedef std::list<PlayerInput*> PlayerList;
    typedef std::list<InputDeviceType> FormatList;
public:
    //void EventUpdate(HWND hwnd,
    //                 UINT umsg,
    //                 WPARAM wparam,
    //                 LPARAM lparam);
    PlayerInput* AddPlayer();
    void RemovePlayer(unsigned int id);
    //void RegisterActionKeys(const std::string& action,
    //                        const InputDeviceKeyList& keys,
    //                        int playerID = -1);
    //void UnregisterKey(const std::string& action,
    //                   const InputDeviceKeyList& keys,
    //                   int playerID = -1);
    
    float Triggered(const std::string& action) const;
    float Triggered(const std::string& action,
                    int playerId) const;
    float Released(const std::string& action) const;
    float Released(const std::string& action,
                   int playerId) const;
    float Pressed(const std::string& action) const;
    float Pressed(const std::string& action,
                  int playerId) const;
    float Depressed(const std::string& action) const;
    float Depressed(const std::string& action,
                    int playerId) const;
    float Value(const std::string& action) const;
    float Value(const std::string& action,
                int playerId) const;
	bool Tick();
	void Save(void);
    void Load(void);
    static InputManager* getInstance();
public:
    bool CtrlAltDel() const;
    void CtrlAltDel(bool val);
    PlayerInput* GetPlayer(unsigned int id) const;
    unsigned int GetNumberOfPlayers() const;
    MouseInputDevice* GetMouse() const;
    KeyboardInputDevice* GetKeyboard() const;
    void Enable(bool val);
private:
	InputManager();
	~InputManager();
    void loadDevices_();
    void unloadDevices_();
    float keyPressCheck_(const std::string& action,
                         InputDevice::KeyStatusFunc func) const;

    float keyPressCheck_(const std::string& action,
                         int playerID,
                         InputDevice::KeyStatusFunc func) const;
    std::string getKeyStr_(unsigned int type,
                           unsigned int keyCode,
                           bool isBinary) const;

    int getKeyVal_(unsigned int type,
                   const std::string& keyStr,
                   bool isBinary) const;
private:
    bool enable_;
    bool ctrlAltDel_;
    InputDevice* keyboard_;
    InputDevice* mouse_;
    std::queue<unsigned int> availPlayerIds_;
    ActionPlayerKeyMap actionPlayerKeyMap_;
    PlayerList playerList_;
    FormatList formatList_;

    metadef(InputManager)
        endmetadef;
};

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	
//	Date	-	Change