/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "MochaWindow.h"

unsigned int MochaWindow::GetWindowWidth() const
{
	return mWindowWidth;
}

unsigned int MochaWindow::GetWindowHeight() const
{
	return mWindowHeight;
}

bool MochaWindow::IsFullScreen()
{
	return mIsFullScreen;
}

float MochaWindow::AspectRatio() const
{
	return static_cast<float>(mWindowWidth) / mWindowHeight;
}

void MochaWindow::ChangeResolution(unsigned windowW, unsigned windowH)
{
	mSavedResolutionWidth = windowW;
	mSavedResolutionHeight = windowH;
}

/* Toggle Fullscreen*/
void MochaWindow::SetFullScreen(bool fullscreen)
{
	mIsFullScreen = fullscreen;

	if (mIsFullScreen)
	{
		unsigned posX = 0,
			width = mSavedResolutionWidth,
			height = mSavedResolutionHeight;

		if (this == Engine::getInstance()->GetVRWnd())
		{
			mIsFullScreen = false;
			posX = mWindowWidth;
			width = mWindowWidth;
			height = mWindowHeight;
		}

		SetWindowLongPtr(mHWND, GWL_STYLE,
			WS_SYSMENU |
			WS_POPUP |
			WS_CLIPCHILDREN |
			WS_CLIPSIBLINGS |
			WS_VISIBLE);
		MoveWindow(mHWND, posX, 0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN), TRUE);

		mScreenSettings.dmSize = sizeof(DEVMODE);
		mScreenSettings.dmPelsWidth = width;
		mScreenSettings.dmPelsHeight = height;
		mScreenSettings.dmBitsPerPel = 32;
		mScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		ChangeDisplaySettings(&mScreenSettings, 0);
	}
	else
	{
		ChangeDisplaySettings(NULL, 0);

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = mSavedResolutionWidth;
		rect.bottom = mSavedResolutionHeight;

		SetWindowLongPtr(mHWND, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
		MoveWindow(mHWND, 0, 0, mSavedResolutionWidth, mSavedResolutionHeight, TRUE);

    DrawMenuBar(mHWND);
	}
}

void MochaWindow::SetWindowTitle(const char* title)
{
	static const unsigned TITLESIZE = 30;
	if (strlen(title) + 1 > TITLESIZE)
		throw std::exception("Title too long");
	wchar_t wtext[TITLESIZE];
	unsigned ret;
	mbstowcs_s(&ret, wtext, title, strlen(title) + 1);//Plus null
	LPWSTR ptr = wtext;
	SetWindowText(mHWND, ptr);
}

/*Create Create a window*/
void MochaWindow::CreateGameWindow()
{
	int posX, posY;

	// Get the instance of this application.
	mHInstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	mWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	mWndClass.lpfnWndProc = WndProc;
	mWndClass.cbClsExtra = 0;
	mWndClass.cbWndExtra = 0;
	mWndClass.hInstance = mHInstance;
	mWndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	mWndClass.hIconSm = mWndClass.hIcon;
	mWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	mWndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	mWndClass.lpszMenuName = NULL;
	mWndClass.lpszClassName = mApplicationName;
	mWndClass.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&mWndClass);

	// Determine the resolution of the clients desktop screen.
	unsigned int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	unsigned int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (mWindowWidth < screenWidth && mWindowWidth != 0)
		screenWidth = mWindowWidth;

	if (mWindowWidth > screenWidth)
		mWindowWidth = screenWidth;

	if (mWindowHeight < screenHeight && mWindowHeight != 0)
		screenHeight = mWindowHeight;

	if (mWindowHeight > screenHeight)
		mWindowHeight = screenHeight;

  if (this == Engine::getInstance()->GetGameWnd() && Engine::getInstance()->GetIsLetterBox())
    Letterboxing(screenWidth, screenHeight);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (this->mIsFullScreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		ZeroMemory(&mScreenSettings, sizeof(mScreenSettings));
		mScreenSettings.dmSize = sizeof(mScreenSettings);
		mScreenSettings.dmPelsWidth = screenWidth;
		mScreenSettings.dmPelsHeight = screenHeight;
		mScreenSettings.dmBitsPerPel = 32;
		mScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&mScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		mWndCentre.x = posX;
		mWndCentre.y = posY;
	}

	// Create the window with the screen settings and get the handle to it.
	//if(this->mIsFullScreen)
	//{
	//	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
	//			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	//			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	//}
	//else

	// If oculus window, position to oculus monitor and size
	if (this == Engine::getInstance()->GetVRWnd())
	{
		Rect oculusDisplay = Engine::getInstance()->GetMonitorDisplayInfo(L"\\\\.\\DISPLAY2\\Monitor0");
		posX = (int)oculusDisplay.x;
		posY = (int)oculusDisplay.y;
		mWindowWidth = screenWidth = (unsigned)oculusDisplay.width;
		mWindowHeight = screenHeight = (unsigned)oculusDisplay.height;
		mWndCentre.x = screenWidth / 2;
		mWndCentre.y = screenHeight / 2;
	}

	mHWND = CreateWindowEx(WS_EX_APPWINDOW, mApplicationName, mApplicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, mHInstance, NULL);

	ClientToScreen(mHWND, &mWndCentre);

	//if (this == Engine::getInstance()->GetOculusWnd() && OculusVR::getInstance()->IsConnected())
	//  ShowWindow(mHWND, SW_HIDE);
	//else
	{
    if ((int)mWindowWidth == GetSystemMetrics(SM_CXSCREEN))
		  ShowWindow(mHWND, SW_MAXIMIZE);
		SetForegroundWindow(mHWND);
		SetFocus(true);
	}

	// Hide the mouse cursor.
	ShowCursor(!mIsFullScreen);
	UpdateWindow(mHWND);
};

void MochaWindow::Init(LPCWSTR appName, unsigned int windowW, unsigned int windowH, bool fullScreen)
{
	mApplicationName = appName;
	mWindowWidth = windowW;
	mWindowHeight = windowH;
	mSavedResolutionWidth = windowW;
	mSavedResolutionHeight = windowH;
	mViewportWidth = windowW;
	mViewportHeight = windowH;
	mFixedAspectRatio = 16.f / 9.f;
	mIsFullScreen = fullScreen;

	CreateGameWindow();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		ChangeDisplaySettings(NULL, 0);
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		ChangeDisplaySettings(NULL, 0);
		//ISerializeable_AtExit();
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return Engine::getInstance()->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}

HWND& MochaWindow::GetHWND(void)
{
	return mHWND;
}

void MochaWindow::Letterboxing(unsigned newWidth, unsigned newHeight)
{
  if (IsFullScreen())
  {
    newWidth = mSavedResolutionWidth;
    newHeight = mSavedResolutionHeight;
  }

	mWindowWidth = newWidth;
	if (newHeight)
		mWindowHeight = newHeight;

	mViewportWidth = mWindowWidth;
	mViewportHeight = (unsigned)(mWindowWidth / mFixedAspectRatio + .5f);

	if (mViewportHeight > mWindowHeight)
	{
		mViewportHeight = mWindowHeight;
		mViewportWidth = (unsigned)(mViewportHeight * mFixedAspectRatio + .5f);
	}
}

unsigned MochaWindow::GetViewportWidth() const
{
	return mViewportWidth;
}

unsigned MochaWindow::GetViewportHeight() const
{
	return mViewportHeight;
}

float MochaWindow::FixedAspectRatio() const
{
	return mFixedAspectRatio;
}

void MochaWindow::FixedAspectRatio(const float& ratio)
{
	mFixedAspectRatio = ratio;
}

float MochaWindow::GetScreenOffsetX() const
{
	if (mWindowHeight == mViewportHeight && mWindowWidth != mViewportWidth)
		return (mWindowWidth - mViewportWidth) * .5f;
	else
		return 0.f;
}

float MochaWindow::GetScreenOffsetY() const
{
	if (mWindowHeight != mViewportHeight && mWindowWidth == mViewportWidth)
		return (mWindowHeight - mViewportHeight) * .5f;
	else
		return 0.f;
}

unsigned int MochaWindow::GetSavedResolutionHeight() const
{
	return mSavedResolutionHeight;
}

unsigned int MochaWindow::GetSavedResolutionWidth() const
{
	return mSavedResolutionWidth;
}

unsigned MochaWindow::GetHwndAsInt()
{
	return (unsigned)mHWND;
}

std::string MochaWindow::GetWindowName()
{
	std::wstring ws = mApplicationName;
	std::string s(ws.begin(), ws.end());
	return s;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
