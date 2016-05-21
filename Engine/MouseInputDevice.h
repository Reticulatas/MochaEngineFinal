#ifndef MOUSEINPUTDEVICE_H
#define MOUSEINPUTDEVICE_H

#include "InputDevice.h"
#include "MouseButtonTypes.h"

class MouseInputDevice : public InputDevice
{
public:
    MouseInputDevice();
    void Load();
    void Unload();
    bool InWindow(HWND hwnd) const;
    float ClientX(HWND hwnd) const;
    float ClientY(HWND hwnd) const;
    int DeltaWheelPos() const;
    int DeltaX() const;
    int DeltaY() const;
    static int GetKey(const std::string& key,
                      bool isBinary = true);
    static std::string GetKey(unsigned int key,
                              bool isBinary = true);
public:
    bool Visible() const;
    void Visible(bool val);
    bool Lock() const;
    void Lock(bool val);
    bool Wrap() const;
    void Wrap(bool val);
	void SetPosition(int x, int y);
protected:
    void disableDevice_();
    void load_();
    void eventUpdate_(HWND hwnd,
                      UINT umsg,
                      WPARAM wparam,
                      LPARAM lparam);
    void updatePrevValues_();
    void updateCurrValues_();
    bool getPreviousBinaryKeyStatus_(unsigned int key) const;
    bool getCurrentBinaryKeyStatus_(unsigned int key) const;
    float getPreviousAnalogKeyStatus_(unsigned int key) const;
    float getCurrentAnalogKeyStatus_(unsigned int key) const;
    void updateVisiblility_();
    void updateLock_();
    void updatePos_();
    void updateButtons_();
    void updateDelta_();
    void initStrKeyMap_();
    int wrapValue_(int currVal,
                   int minVal,
                   int maxVal);
private:
    bool visible_;
    bool lock_;
    bool wrap_;
    int deltaX_;
    int deltaY_;
    int prevAKeys_[MAB_TOTAL];
    int currAKeys_[MAB_TOTAL];
    bool prevBKeys_[MBB_TOTAL];
    bool currBKeys_[MBB_TOTAL];
};

#endif