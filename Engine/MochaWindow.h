/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once

// include the basic windows header files and the Direct3D header files
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

class MochaWindow
{
	LPCWSTR mApplicationName;				/* String representing the app name */
	HINSTANCE mHInstance;					/* Handle if the instance */
	HWND mHWND;							/* window handle */
	WNDCLASSEX mWndClass;
	DEVMODE mScreenSettings;
	POINT mWndCentre;

	unsigned int mWindowWidth;
	unsigned int mWindowHeight;
	unsigned mSavedResolutionWidth;
	unsigned mSavedResolutionHeight;
	unsigned mViewportWidth;
	unsigned mViewportHeight;
	float mFixedAspectRatio;
	bool mIsFullScreen;
	bool mFocused = true;
	void CreateGameWindow();

public:

	void Init(LPCWSTR appName, unsigned int windowW = 800, unsigned int windowH = 600, bool fullScreen = false);

	void ChangeResolution(unsigned int windowW, unsigned int windowH);		/* Change the width and height of the window */
	void SetFullScreen(bool fullscreen);									/* Set True to put the game on FullScreen */
	void SetWindowTitle(const char* title);									/* Change the title of the application */
	unsigned int GetWindowWidth() const;									/* Get Width of the application window */
	unsigned int GetSavedResolutionHeight() const;									/* Get Height of the window */
	unsigned int GetSavedResolutionWidth() const;
	unsigned int GetWindowHeight() const;
	unsigned GetViewportWidth() const;
	unsigned GetViewportHeight() const;
	bool IsFullScreen();													/* Return true if fullscreen */
	float AspectRatio() const;
	float FixedAspectRatio() const;
	void FixedAspectRatio(const float& ratio);
	float GetScreenOffsetX() const;
	float GetScreenOffsetY() const;
	void Letterboxing(unsigned newWidth, unsigned newHeight);

	bool GetFocus() const { return mFocused; }
	void SetFocus(bool val) { mFocused = val; }
	std::string GetWindowName();

	HWND& GetHWND(void);
	unsigned GetHwndAsInt();
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change