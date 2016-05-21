#ifndef KEYBOARDINPUTDEVICE_H
#define KEYBOARDINPUTDEVICE_H

#include "InputDevice.h"
#include "KeyboardButtonTypes.h"

class KeyboardInputDevice : public InputDevice
{
public:
	KeyboardInputDevice();
    ~KeyboardInputDevice();
	static int GetKey(const std::string& key,
					  bool isBinary = true);
	static std::string GetKey(unsigned int key,
					          bool isBinary = true);
protected:
    void updatePrevValues_();
    void updateCurrValues_();
    bool getPreviousBinaryKeyStatus_(unsigned int key) const;
    bool getCurrentBinaryKeyStatus_(unsigned int key) const;
    void initStrKeyMap_();
private:
    bool prevKeys_[KBB_TOTAL];
    bool currKeys_[KBB_TOTAL];
};

#endif