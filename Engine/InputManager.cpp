/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "InputManager.h"
#include "XInput.h"
#include "MouseInputDevice.h"
#include "KeyboardInputDevice.h"
#include "XboxInputDevice.h"
#include "PlayerInput.h"

#include <windowsx.h>
#include <algorithm>

#define CONFIG_MAXPLAYERS 10
#define CONFIG_INPUTMANAGER_FILENAME "ConfigInput.csv"

bool InputManager::InputDeviceKey::operator==(const InputDeviceKey& val)
{
    return  (type_ == val.type_) && 
            (keyCode_ == val.keyCode_) && 
            (isBinaryKey_ == val.isBinaryKey_);
}

//-----------------------------------------------------------------------------

PlayerInput* InputManager::AddPlayer()
{
    if(playerList_.size() > CONFIG_MAXPLAYERS)
        return 0;

    PlayerInput* player = new PlayerInput(availPlayerIds_.front());
    availPlayerIds_.pop();
    playerList_.push_back(player);
    return player;
}

void InputManager::RemovePlayer(unsigned int id)
{
    PlayerList::iterator it = playerList_.begin();
    for(; it != playerList_.end(); ++it)
    {
        if((*it)->ID() == id)
        {
            delete *it;
            playerList_.erase(it);
            availPlayerIds_.push(id);
            break;
        }
    }
}

float InputManager::Triggered(const std::string& action) const
{
    return keyPressCheck_(action,&InputDevice::Triggered);
}

float InputManager::Triggered(const std::string& action,
                              int playerId) const
{
    return keyPressCheck_(action,playerId,&InputDevice::Triggered);
}

float InputManager::Released(const std::string& action) const
{
    return keyPressCheck_(action,&InputDevice::Released);
}

float InputManager::Released(const std::string& action,
                             int playerId) const
{
    return keyPressCheck_(action,playerId,&InputDevice::Released);
}

float InputManager::Pressed(const std::string& action) const
{
    if(!enable_)
        return 0.0f;
    return keyPressCheck_(action,&InputDevice::Pressed);
}

float InputManager::Pressed(const std::string& action,
                            int playerId) const
{
    if(!enable_)
        return 0.0f;
    return keyPressCheck_(action,playerId,&InputDevice::Pressed);
}

float InputManager::Depressed(const std::string& action) const
{
    if(!enable_)
        return 0.0f;
    return keyPressCheck_(action,&InputDevice::Depressed);
}

float InputManager::Depressed(const std::string& action,
                              int playerId) const
{
    if(!enable_)
        return 0.0f;
    return keyPressCheck_(action,playerId,&InputDevice::Depressed);
}

float InputManager::Value(const std::string& action) const
{
    if(!enable_)
        return 0.0f;
    return keyPressCheck_(action,&InputDevice::Value);
}

float InputManager::Value(const std::string& action,
                          int playerId) const
{
    if(!enable_)
        return 0.0f;
    return keyPressCheck_(action,playerId,&InputDevice::Value);
}

bool InputManager::Tick(void)
{
    MSG msg;
    if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(msg.message == WM_QUIT)
        return false;

    PlayerList::iterator playerIt = playerList_.begin();

    keyboard_->Update();
    mouse_->Update();

    while(playerIt != playerList_.end())
    {
        (*playerIt)->Update();
        ++playerIt;
    }
    return true;
}

void InputManager::Save(void)
{
    std::ofstream file(CONFIG_INPUTMANAGER_FILENAME);
    if(!file.good())
    {
        Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " could not be created",LogEntry::Error);
        return;
    }

    if(formatList_.size() == 0)
    {
        formatList_.push_back(IDT_KEYBOARD);
        formatList_.push_back(IDT_MOUSE);
        formatList_.push_back(IDT_XBOX);
    }

    // Write format
    file << "#Action";

    FormatList::iterator formatIt = formatList_.begin();
    for(unsigned int i = 0; i < formatList_.size(); ++i,++formatIt)
    {
        if(*formatIt == IDT_KEYBOARD)
            file << ",#keyboard";
        else if(*formatIt == IDT_MOUSE)
            file << ",#mouse";
        else if(*formatIt == IDT_XBOX)
            file << ",#xbox";
    }
    file << std::endl;

    PlayerList::iterator playerIt = playerList_.begin();
    unsigned int playerCounter = 0;
    while(playerIt != playerList_.end())
    {
        // Begin player definition
        file << ">>>";
        for(unsigned int i = 0; i < formatList_.size(); ++i)
            file << ",";
        file << std::endl;

        // Player comment
        file << "#Player " << playerCounter;
        for(unsigned int i = 0; i < formatList_.size(); ++i)
            file << ",";
        file << std::endl;

        // Iterate through action keys for this player
        ActionPlayerKeyMap::iterator apkMapIt = actionPlayerKeyMap_.begin();
        while(apkMapIt != actionPlayerKeyMap_.end())
        {
            IDPlayerKeyDataMap& ipkdMap(apkMapIt->second);
            IDPlayerKeyDataMap::iterator ipkdIt = ipkdMap.find((*playerIt)->ID());

            // This player is not mapped to this action
            if(ipkdIt == ipkdMap.end())
                continue;

            // Write the action string
            file << apkMapIt->first;

            // Loop through all keys registered to this action
            InputDeviceKeyList& idkList(ipkdIt->second.keyList_);
            FormatList::iterator formatIt = formatList_.begin();

            while(formatIt != formatList_.end())
            {
                file << ",";

                // Write the key enum strings
                InputDeviceKeyList::iterator idkListIt = idkList.begin();
                while(idkListIt != idkList.end())
                {
                    if(*formatIt == idkListIt->type_)
                        file << getKeyStr_(idkListIt->type_,idkListIt->keyCode_,idkListIt->isBinaryKey_);
                    ++idkListIt;
                }
                ++formatIt;
            }
            file << std::endl;
            ++apkMapIt;
        }

        file << "<<<";

        for(unsigned int i = 0; i < formatList_.size(); ++i)
            file << ",";

        file << std::endl;
        ++playerCounter;
        ++playerIt;
    }
    file.close();

    unloadDevices_();
}

void InputManager::Load(void)
{
    std::ifstream file(CONFIG_INPUTMANAGER_FILENAME);
    if(!file.good())
    {
        Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " not found, not loading any input settings",LogEntry::Error);
        return;
    }

    std::string line;
    PlayerInput* lastPlayer = 0;

    if(file.eof())
    {
        Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " contains nothing",LogEntry::Error);
        return;
    }

    // Record format
    std::getline(file,line);
    unsigned int left = line.find_first_of('#');
    unsigned right = line.find_first_of(',',left);

    // skip #Action
    // read sections
    formatList_.clear();
    do
    {
        left = line.find_first_of('#',right);
        right = min(min(line.find_first_of(',',left),line.find_first_of(' ',left)),line.length());

        std::string section(line.substr(left,right - left));
        if(section.length() == 0)
        {
            Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " has an invalid format with an invalid device type",LogEntry::Error);
            return;
        }

        std::transform(section.begin(),section.end(),section.begin(),::tolower);

        if(section == "#keyboard")
            formatList_.push_back(IDT_KEYBOARD);
        else if(section == "#xbox")
            formatList_.push_back(IDT_XBOX);
        else if(section == "#mouse")
            formatList_.push_back(IDT_MOUSE);
        else
        {
            Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " has an invalid format with an invalid device type " + section,LogEntry::Error);
            return;
        }
    }
    while(right != line.length());

    if(formatList_.size() == 0)
    {
        Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " has no defined format",LogEntry::Error);
        return;
    }

    // parse file with given format
    while(!file.eof())
    {
        std::getline(file,line);

        //skip comments and blank lines 
        if(line.empty() || line[0] == '#')
            continue;

        left = line.find_first_not_of(' ');
        right = min(min(line.find_first_of(',',left),line.find_first_of(' ',left)),line.length());
        std::string action(line.substr(left,right - left));

        // begin player definition
        if(action == ">>>")
        {
            if(lastPlayer)
            {
                Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " has a nested player definition.",LogEntry::Error);
                return;
            }

            lastPlayer = AddPlayer();
        }
        else if(action == "<<<")
        {
            lastPlayer = 0;
        }
        else
        {
            // Read according to format
            right = line.find_first_of(',',left);
            action = line.substr(left,right - left);
            std::transform(action.begin(),action.end(),action.begin(),::tolower);

            if(action.length() == 0)
            {
                Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " has an empty action mapping",LogEntry::Error);
                return;
            }

            ActionPlayerKeyMap::iterator actKeyIt = actionPlayerKeyMap_.find(action);

            // Action mapping does not exist
            if(actKeyIt == actionPlayerKeyMap_.end())
                actKeyIt = actionPlayerKeyMap_.insert(std::pair<std::string,IDPlayerKeyDataMap>(action,IDPlayerKeyDataMap())).first;

            IDPlayerKeyDataMap::iterator ipkdMapIt = actKeyIt->second.find(lastPlayer->ID());
            if(ipkdMapIt == actKeyIt->second.end())
                ipkdMapIt = actKeyIt->second.insert(std::pair<int,PlayerKeyData>(lastPlayer->ID(),PlayerKeyData())).first;

            // Read action and keys
            FormatList::iterator formatIt = formatList_.begin();
            for(unsigned int i = 0; i < formatList_.size(); ++i,++formatIt)
            {
                left = line.find_first_not_of(' ',right + 1);
                right = min(min(line.find_first_of(',',left),line.find_first_of(' ',left)),line.length());

                // No keys bound
                if((left == right) || (left == line.npos))
                    continue;

                if(lastPlayer->InputDevices(*formatIt) == 0)
                    lastPlayer->AddInputDevice(*formatIt);

                bool isBinary = true;
                std::string key(line.substr(left,right - left));
                std::transform(key.begin(),key.end(),key.begin(),::toupper);
                int result = getKeyVal_(*formatIt,key,isBinary);

                if(result == -1)
                {
                    // Try analog
                    isBinary = false;
                    result = getKeyVal_(*formatIt,key,isBinary);

                    if(result == -1)
                    {
                        Log("ERROR: " + std::string(CONFIG_INPUTMANAGER_FILENAME) + " contains an invalid key map for " + action,LogEntry::Error);
                        return;
                    }
                }

                InputDeviceKey iDeviceKey;
                iDeviceKey.type_ = *formatIt;
                iDeviceKey.isBinaryKey_ = isBinary;
                iDeviceKey.keyCode_ = result;

                PlayerKeyData& playerData(ipkdMapIt->second);
                playerData.playerInput_ = lastPlayer;
                playerData.keyList_.push_back(iDeviceKey);
            }
        }
    }

    file.close();

    loadDevices_();
}

InputManager* InputManager::getInstance()
{
    static InputManager* instance = 0;
    if(instance == 0)
        instance = new InputManager();
    return instance;
}

//-----------------------------------------------------------------------------

bool InputManager::CtrlAltDel() const
{
    return ctrlAltDel_;
}

void InputManager::CtrlAltDel(bool val)
{
    ctrlAltDel_ = val;
}

PlayerInput* InputManager::GetPlayer(unsigned int id) const
{
    if(id >= playerList_.size())
    {
        Log("Failed to get player - index out of bounds",LogEntry::Error);
        return 0;
    }

    PlayerList::const_iterator it = playerList_.begin();
    while(it != playerList_.end())
    {
        if((*it)->ID() == id)
            return *it;
        ++it;
    }
    return 0;
}

unsigned int InputManager::GetNumberOfPlayers() const
{
	return (unsigned int)playerList_.size();
}

MouseInputDevice* InputManager::GetMouse() const
{
    return reinterpret_cast<MouseInputDevice*>(mouse_);
}

KeyboardInputDevice* InputManager::GetKeyboard() const
{
    return reinterpret_cast<KeyboardInputDevice*>(keyboard_);
}

void InputManager::Enable(bool val)
{
    enable_ = val;
    keyboard_->Enable(val);
    mouse_->Enable(val);

    PlayerList::iterator playerIt = playerList_.begin();
    while(playerIt != playerList_.end())
    {
        (*playerIt)->EnableAllDevices(val);
        ++playerIt;
    }
}

//-----------------------------------------------------------------------------

InputManager::InputManager(void) 
    :   ISerializeable(true,"InputManager",this),
        ctrlAltDel_(false),
        enable_(true)
{
    mouse_ = new MouseInputDevice();
    keyboard_ = new KeyboardInputDevice();
    for(unsigned int i = 0; i < CONFIG_MAXPLAYERS; ++i)
        availPlayerIds_.push(i);
}

InputManager::~InputManager()
{
    delete mouse_;
    mouse_ = 0;
    delete keyboard_;
    keyboard_ = 0;

    PlayerList::iterator playerIt = playerList_.begin();
    while(playerIt != playerList_.end())
    {
        delete *playerIt;
        ++playerIt;
    }
    playerList_.clear();
}

void InputManager::loadDevices_()
{
    keyboard_->Load();
    mouse_->Load();
    PlayerList::iterator it = playerList_.begin();
    
    while(it != playerList_.end())
    {
        (*it)->Load();
        ++it;
    }
}

void InputManager::unloadDevices_()
{
    keyboard_->Unload();
    mouse_->Unload();
    PlayerList::iterator it = playerList_.begin();

    while(it != playerList_.end())
    {
        (*it)->Unload();
        ++it;
    }
}

float InputManager::keyPressCheck_(const std::string& action,
                                   InputDevice::KeyStatusFunc func) const
{
    std::string actionLower(action);
    std::transform(action.begin(),action.end(),actionLower.begin(),::tolower);
    ActionPlayerKeyMap::const_iterator apkMapIt = actionPlayerKeyMap_.find(actionLower);

    if(apkMapIt == actionPlayerKeyMap_.end())
        return false;

    const IDPlayerKeyDataMap& ipkdList(apkMapIt->second);
    IDPlayerKeyDataMap::const_iterator ipkdMapIt = ipkdList.begin();
    IDPlayerKeyDataMap::const_iterator ipkdMapEnd = ipkdList.end();

    // loop through all players with keys registered to this action
    while(ipkdMapIt != ipkdMapEnd)
    {
        const InputDeviceKeyList& ideviceKeyList(ipkdMapIt->second.keyList_);
        InputDeviceKeyList::const_iterator idkListIt = ideviceKeyList.begin();
        InputDeviceKeyList::const_iterator idkListEnd = ideviceKeyList.end();
        PlayerInput* playerInput = ipkdMapIt->second.playerInput_;

        // loop through all keys of the current player registered to this key
        // the moment a true value is detected, break
        while(idkListIt != idkListEnd)
        {
            // Remove type_ from inputdevicekey
            // Create a triggered function in player input
            const InputDeviceKey& idk(*idkListIt);
            float val = 0;

            if(idk.type_ == IDT_KEYBOARD)
            {
                val = (keyboard_->*func)(idk.keyCode_,idk.isBinaryKey_);

                if(std::abs(val) > 0)
                    return val;
            }

            else if(idk.type_ == IDT_MOUSE)
            {
                val = (mouse_->*func)(idk.keyCode_,idk.isBinaryKey_);

                if(std::abs(val) > 0)
                    return val;
            }

            else
            {
                val = (playerInput->InputDevices(idk.type_)->*func)(idk.keyCode_,idk.isBinaryKey_);

                if(std::abs(val) > 0)
                    return val;
            }
            ++idkListIt;
        }
        ++ipkdMapIt;
    }
    return 0;
}

float InputManager::keyPressCheck_(const std::string& action,
                                   int playerID,
                                   InputDevice::KeyStatusFunc func) const
{
    std::string actionLower(action);
    std::transform(action.begin(),action.end(),actionLower.begin(),::tolower);
    ActionPlayerKeyMap::const_iterator apkMapIt = actionPlayerKeyMap_.find(actionLower);

    if(apkMapIt == actionPlayerKeyMap_.end())
        return false;

    const IDPlayerKeyDataMap& ipkdMap(apkMapIt->second);
    IDPlayerKeyDataMap::const_iterator ipkdMapIt = ipkdMap.find(playerID);
    IDPlayerKeyDataMap::const_iterator ipkdMapEnd = ipkdMap.end();

    // find player with the same id
    if(ipkdMapIt != ipkdMapEnd)
    {
        const InputDeviceKeyList& ideviceKeyList(ipkdMapIt->second.keyList_);
        InputDeviceKeyList::const_iterator idkListIt = ideviceKeyList.begin();
        InputDeviceKeyList::const_iterator idkListEnd = ideviceKeyList.end();
        PlayerInput* playerInput = ipkdMapIt->second.playerInput_;

        // loop through all keys of the current player registered to this key
        // the moment a true value is detected, break
        while(idkListIt != idkListEnd)
        {
            const InputDeviceKey& idk(*idkListIt);
            float val = (playerInput->InputDevices(idk.type_)->*func)(idk.keyCode_,idk.isBinaryKey_);

            if(std::abs(val) > 0)
                return val;
            ++idkListIt;
        }
    }
    return 0;
}

std::string InputManager::getKeyStr_(unsigned int type,
                                     unsigned int keyCode,
                                     bool isBinary) const
{
    switch(type)
    {
    case IDT_KEYBOARD:
        return KeyboardInputDevice::GetKey(keyCode,isBinary);
    case IDT_MOUSE:
        return MouseInputDevice::GetKey(keyCode,isBinary);
    case IDT_XBOX:
        return XboxInputDevice::GetKey(keyCode,isBinary);
    }
    return std::string();
}

int InputManager::getKeyVal_(unsigned int type,
                             const std::string& keyStr,
                             bool isBinary) const
{
    switch(type)
    {
    case IDT_KEYBOARD:
        return KeyboardInputDevice::GetKey(keyStr,isBinary);
    case IDT_MOUSE:
        return MouseInputDevice::GetKey(keyStr,isBinary);
    case IDT_XBOX:
        return XboxInputDevice::GetKey(keyStr,isBinary);
    }
    return -1;
}


/*
InputManagerLock::~InputManagerLock( void )
{
	if (!maintainInActive)
		InputManager::getInstance()->Enable(true);
}

InputManagerLock::InputManagerLock( void )
{
	maintainInActive = false;
	if (InputManager::getInstance()->Enable() == false)
		maintainInActive = true;
	InputManager::getInstance()->Enable(false);
}
*/


meta_define(InputManager);
