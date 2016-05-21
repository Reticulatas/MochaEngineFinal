#include "stdafx.h"
#include "MouseInputDevice.h"
#include "Engine.h"

#include <iostream>

// Decrease the screen's dimension by this amount in order to detect the delta
// needed to wrap the mouse over to the other side
const static int WRAP_AREA_DIFF = 5;

const static unsigned int MouseKeyMap[MBB_TOTAL] =
{
    VK_LBUTTON,
    VK_RBUTTON,
    VK_MBUTTON,
    VK_XBUTTON1,
    VK_XBUTTON2
};

#define GENERATE_STRING(STRING) #STRING,
static const char *MouseAKeyToStrMap[] = {FOREACH_ABUTTON(GENERATE_STRING)};
static const char *MouseBKeyToStrMap[] = {FOREACH_BBUTTON(GENERATE_STRING)};
static std::map<std::string,int> MouseStrToAKeyMap;
static std::map<std::string,int> MouseStrToBKeyMap;

MouseInputDevice::MouseInputDevice()
    :   InputDevice(IDT_MOUSE),
        lock_(false),
        visible_(true),
        wrap_(false),
        deltaX_(0),
        deltaY_(0)
{
    for(unsigned int i = 0; i < MBB_TOTAL; ++i)
        prevBKeys_[i] = currBKeys_[i] = false;

    for(unsigned int i = 0; i < MAB_TOTAL; ++i)
        prevAKeys_[i] = currAKeys_[i] = 0;

    initStrKeyMap_();
}

//-----------------------------------------------------------------------------

bool MouseInputDevice::InWindow(HWND hwnd) const
{
    RECT wndRect; 
    if(!GetWindowRect(hwnd,&wndRect))
        return false;

    POINT cursorPos = {currAKeys_[MAB_X],currAKeys_[MAB_Y]};

    return PtInRect(&wndRect,cursorPos) ? true : false;
}

float MouseInputDevice::ClientX(HWND hwnd) const
{
    RECT wndRect;
    if(!GetWindowRect(hwnd,&wndRect))
        return 0.0f;

    return currAKeys_[MAB_X] - static_cast<float>(wndRect.left);
}

float MouseInputDevice::ClientY(HWND hwnd) const
{
    RECT wndRect;
    if(!GetWindowRect(hwnd,&wndRect))
        return 0.0f;

    return currAKeys_[MAB_Y] - static_cast<float>(wndRect.top);
}

int MouseInputDevice::DeltaWheelPos() const
{
    return currAKeys_[MAB_WHEEL] - prevAKeys_[MAB_WHEEL];
}

int MouseInputDevice::DeltaX() const
{
    return deltaX_;//currAKeys_[MAB_X] - prevAKeys_[MAB_X];
}

int MouseInputDevice::DeltaY() const
{
    return deltaY_;//currAKeys_[MAB_Y] - prevAKeys_[MAB_Y];
}

int MouseInputDevice::GetKey(const std::string& key,
                             bool isBinary) 
{
    if(isBinary)
    {
        std::map<std::string,int>::iterator it = MouseStrToBKeyMap.find(key);
        
        if(it != MouseStrToBKeyMap.end())
            return it->second;
        return -1;
    }
    else
    {
        std::map<std::string,int>::iterator it = MouseStrToAKeyMap.find(key);

        if(it != MouseStrToAKeyMap.end())
            return it->second;
        return -1;
    }
}

std::string MouseInputDevice::GetKey(unsigned int key,
                                     bool isBinary) 
{
    if(isBinary)
    {
        if(key >= MBB_TOTAL)
            return std::string();
        return MouseBKeyToStrMap[key];
    }
    else
    {
        if(key >= MAB_TOTAL)
            return std::string();
        return MouseAKeyToStrMap[key];
    }
}

//------------------------------------------------------------------------------

void MouseInputDevice::Lock(bool val)
{
    lock_ = val;
}

bool MouseInputDevice::Lock() const
{
    return lock_;
}

void MouseInputDevice::Visible(bool val)
{
    visible_ = val;
}

bool MouseInputDevice::Visible() const
{
    return visible_;
}

bool MouseInputDevice::Wrap() const
{
    return wrap_;
}

void MouseInputDevice::Wrap(bool val)
{
    wrap_ = val;
}

void MouseInputDevice::SetPosition(int x,int y)
{
    SetCursorPos(x,y);
}

//------------------------------------------------------------------------------

void MouseInputDevice::disableDevice_()
{
    while(ShowCursor(visible_) < 0);
}

void MouseInputDevice::load_()
{
    updateCurrValues_();
    updatePrevValues_();
}

void MouseInputDevice::eventUpdate_(HWND hwnd,
                                    UINT umsg,
                                    WPARAM wparam,
                                    LPARAM lparam)
{
    switch(umsg)
    {
    case WM_MOUSEWHEEL:
        currAKeys_[MAB_WHEEL] += GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
        break;
    };
}

void MouseInputDevice::updatePrevValues_()
{
	for (unsigned int i = 0; i < MBB_TOTAL; ++i)
		prevBKeys_[i] = currBKeys_[i];

    for(unsigned int i = 0; i < MAB_TOTAL; ++i)
        prevAKeys_[i] = currAKeys_[i];
}

void MouseInputDevice::updateCurrValues_()
{
	// Check if mouse is installed
	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	if (!GetCursorInfo(&info))
		return;

	updatePos_();
	updateButtons_();
	currAKeys_[MAB_WHEEL] = 0;

	if (Engine::getInstance()->GetIsEditorMode() && Input::GetTriggered(0, "ToggleMouse") == 1.0f)
	{
		Lock(false);
		Visible(true);
	}

	updateLock_();
	updateVisiblility_();
}

bool MouseInputDevice::getPreviousBinaryKeyStatus_(unsigned int key) const
{
    return prevBKeys_[key];
}

bool MouseInputDevice::getCurrentBinaryKeyStatus_(unsigned int key) const
{
    return currBKeys_[key];
}

float MouseInputDevice::getPreviousAnalogKeyStatus_(unsigned int key) const
{
    return static_cast<float>(prevAKeys_[key]);
}

float MouseInputDevice::getCurrentAnalogKeyStatus_(unsigned int key) const
{
    return static_cast<float>(currAKeys_[key]);
}

void MouseInputDevice::updateVisiblility_()
{
    // While cursor is still invisible
    if(visible_)
        while(ShowCursor(visible_) < 0);

    // While cursor is still visible
    else
        while(ShowCursor(visible_) >= 0);
}

void MouseInputDevice::updateLock_()
{
    if(lock_)
    {
        Engine* engine = Engine::getInstance();
        int halfWidth = static_cast<int>(engine->GetGameWnd()->GetWindowWidth()/2);
        int halfHeight = static_cast<int>(engine->GetGameWnd()->GetWindowHeight()/2);
        SetCursorPos(halfWidth,halfHeight);

        currAKeys_[MAB_X] = halfWidth;
        currAKeys_[MAB_Y] = halfHeight;
    }
}

void MouseInputDevice::updatePos_()
{
    POINT pt;
    GetCursorPos(&pt);
    currAKeys_[MAB_X] = pt.x;
    currAKeys_[MAB_Y] = pt.y;

    // If wrap is true, wrap the mouse to the other side of the screen while 
    // maintaining the same delta
    updateDelta_();

    if(wrap_)
    {
        int width = 0;
        int height = 0;
        Engine::getInstance()->GetDesktopResolution(width,height);

        // Wrap along X
        if(pt.x < WRAP_AREA_DIFF)
        {
            int remDelta = deltaX_ + (prevAKeys_[MAB_X] - WRAP_AREA_DIFF);
            currAKeys_[MAB_X] = width + remDelta;
        }
        else if(pt.x > (width - WRAP_AREA_DIFF))
        {
            int remDelta = deltaX_ - (width - prevAKeys_[MAB_X]);
            currAKeys_[MAB_X] = remDelta;
        }

        // Wrap along Y
        // Mouse moving upwards (negative delta)
        if(pt.y < WRAP_AREA_DIFF)
        {
            int remDelta = deltaY_ + (prevAKeys_[MAB_Y] - WRAP_AREA_DIFF);
            currAKeys_[MAB_Y] = height + remDelta;
        }
        // Mouse moving downwards
        else if(pt.y > (height - WRAP_AREA_DIFF))
        {
            int remDelta = deltaY_ - (height - prevAKeys_[MAB_Y]);
            currAKeys_[MAB_Y] = remDelta;
        }
        SetCursorPos(currAKeys_[MAB_X],currAKeys_[MAB_Y]);
    }
}

void MouseInputDevice::updateButtons_()
{
    for(unsigned int i = 0; i < MBB_TOTAL; ++i)
    {
        if(GetAsyncKeyState(MouseKeyMap[i]) & 0x8000)
            currBKeys_[i] = true;
        else
            currBKeys_[i] = false;
    }
}

void MouseInputDevice::updateDelta_()
{
    deltaX_ = currAKeys_[MAB_X] - prevAKeys_[MAB_X];
    deltaY_ = currAKeys_[MAB_Y] - prevAKeys_[MAB_Y];
}

void MouseInputDevice::initStrKeyMap_()
{
    for(unsigned int i = 0; i < MAB_TOTAL; ++i)
        MouseStrToAKeyMap.insert(std::pair<std::string,unsigned int>(MouseAKeyToStrMap[i],i));

    for(unsigned int i = 0; i < MBB_TOTAL; ++i)
        MouseStrToBKeyMap.insert(std::pair<std::string,unsigned int>(MouseBKeyToStrMap[i],i));
}