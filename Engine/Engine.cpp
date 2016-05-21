/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include <thread>
#include "AssetManager.h"
#include "AvailableAssets.h"
#include "InputManager.h"
#include "MouseInputDevice.h"
#include "KeyboardInputDevice.h"
#include "CScript.h"
#include "EditorSubsystem.h"
#include "EditorSubsystem_Interface.h"
#include "AudioManager.h"
#include "ISerializeable.h"
#include "Prefab.h"
#include <stdlib.h>
#include "SpeechRecognition.h"
#include "Config.h"
#include "SubloadStateVisitor.h"
#include "meta.h"
#include "GameState.h"
#include "ScriptSubsystem.h"
#include "CrashHandler.h"
#include "AnimationSubSystem.h"
#include "RenderTasks.h"
#include "GPUBufferManager.h"
#include "SteamViveVR.h"

// Get the horizontal and vertical screen sizes in pixel
void Engine::GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

/// Checks if the mouse cursor is over the window.
/// @param hWnd        [IN] window handle
/// @param mx            [IN] x mouse coordinate
/// @param my            [IN] y mouse coordinate
/// @param inClientSpace [IN] indicates if we check coordinate in 
///                                        client or screen space.
/// @return true if the point is over the window area.
const bool IsMouseOverWindow(HWND hWnd, const int mx, const int my,
	const bool inClientSpace = false)
{
	RECT windowRect;

	// Get the window in screen space
	::GetWindowRect(hWnd, &windowRect);

	if (inClientSpace)
	{
		POINT offset;
		offset.x = offset.y = 0;
		ClientToScreen(hWnd, &offset);

		// Offset the window to client space
		windowRect.left -= offset.x;
		windowRect.top -= offset.y;
		// NOTE: left and top should now be 0, 0
		windowRect.right -= offset.x;
		windowRect.bottom -= offset.y;
	}

	// Test if mouse over window
	POINT cursorPos = { mx, my };
	return PtInRect(&windowRect, cursorPos) != 0;
}

void Engine::Run()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			mIsRunning = false;
	}

	ThreadUtils::EnterCritSection();
	ThreadUtils::EnterCritSection(1);

	if (!mIsEditorMode)
		GSM::getInstance()->getActiveState()->IsActive(true);

	barrista().Tick();

	ThreadUtils::LeaveCritSection();
	ThreadUtils::LeaveCritSection(1);

	//process the message queue
	EngineMsgDelegator::getInstance().ProcessQueue();

	//last minute input collection
	POINT p;
	GetCursorPos(&p);
	//InputManager::getInstance()->GetMouse()->MouseInWindow(IsMouseOverWindow(*Engine::getInstance()->GetHwnd(), p.x, p.y));
}

/* Handle OS Messages */
LRESULT CALLBACK Engine::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (Engine::getInstance()->GetIsInited())
  {
    switch (message)
    {
    case WM_DESTROY:
    {
      ChangeDisplaySettings(NULL, 0);
      PostQuitMessage(0);
      return 0;
    }
      break;

      // Window is resizing
    case WM_SIZE:
    {
      if (wParam != SIZE_MINIMIZED)
      {
        if (hWnd == mGameWnd.GetHWND())
          mGameWnd.Letterboxing(LOWORD(lParam), HIWORD(lParam));


        if (mRenderer)
          mRenderer->OnResize();
        mIsMinimized = false;
      }
      else if (wParam == SIZE_MINIMIZED)
      {
        if (!mIsMinimized && hWnd == mGameWnd.GetHWND())
        {
          ChangeDisplaySettings(NULL, 0);
          AudioManager::getInstance()->PauseAll();
          AudioManager::getInstance()->Enable(false);
          InputManager::getInstance()->Enable(false);
        }
        mIsMinimized = true;
      }
    }
      break;

      case WM_ACTIVATE:
      {
          if (!mIsEditorMode && (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE))
          {
              InputManager::getInstance()->Enable(true);
              AudioManager::getInstance()->Enable(true);

              if (!pauseMode_)
                  AudioManager::getInstance()->ResumeAll();


          }
          //mGameWnd.SetFocus(true);
          //mIsEditorFocused = true;
          //mGameWnd.SetFocus(true);
          //LogVerbose("Activated Focused");

          //window in focus
          break;
      }

    case WM_KILLFOCUS:
    {
        if (!mIsEditorMode && !IsGameInFocus())
        {
            AudioManager::getInstance()->PauseAll();
            InputManager::getInstance()->Enable(false);
            AudioManager::getInstance()->Enable(false);
            InputManager::getInstance()->CtrlAltDel(false);
        }

        if (GetForegroundWindow() && !IsGameInFocus())// && GetWndFromHandle(hWnd)->IsFullScreen())
        {
            if (!mIsEditorMode)
            {
                ChangeDisplaySettings(NULL, 0);
                // Minimize both windows
                ShowWindow(mGameWnd.GetHWND(), SW_MINIMIZE);
                ShowWindow(mOculusWnd.GetHWND(), SW_MINIMIZE);
            }
            //else
            //  ShowWindow(hWnd, SW_MINIMIZE);

            InputManager::getInstance()->CtrlAltDel(false);
        }

        // detects ctrl+alt+delete
        else if (!GetForegroundWindow())
        {
            InputManager::getInstance()->CtrlAltDel(true);
        }
        //mGameWnd.SetFocus(false);
        //mIsEditorFocused = false;
        //mGameWnd.SetFocus(false);
        ScriptSubsystem* ss = ScriptSubsystem::getInstance(false);
      mGameWnd.SetFocus(false);
      mIsEditorFocused = false;
      //mGameWnd.SetFocus(false);
      //LogVerbose("Unfocused");
        if (ss)
            ss->CallUnFocus();
        //LogVerbose("Unfocused");
        break;
    }

    case WM_SETFOCUS:
    {
      if (GetWndFromHandle(hWnd)->IsFullScreen())
        GetWndFromHandle(hWnd)->SetFullScreen(GetWndFromHandle(hWnd)->IsFullScreen());

        if (!mIsEditorMode)
        {
            if (mGameWnd.IsFullScreen())
              Input::SetMouseVisible(false);
            InputManager::getInstance()->Enable(true);
            AudioManager::getInstance()->Enable(true);

            if (!pauseMode_)
                AudioManager::getInstance()->ResumeAll();

            InputManager::getInstance()->CtrlAltDel(false);

            // Maximize both windows
            ShowWindow(mOculusWnd.GetHWND(), SW_MAXIMIZE);
            //ShowWindow(mGameWnd.GetHWND(), SW_MAXIMIZE);     
        }

        //mGameWnd.SetFocus(true);
        //mGameWnd.SetFocus(true);
        //LogVerbose("Set Focused");
        ScriptSubsystem* ss = ScriptSubsystem::getInstance(false);
        if (ss)
            ss->CallFocus();
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      break;
    
    case WM_MOUSEMOVE:
      break;

    case WM_MOUSELEAVE:
    case WM_MOUSEWHEEL:
      InputManager::getInstance()->GetMouse()->EventUpdate(hWnd, message, wParam, lParam);
      break;
    }
  }

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Engine::Init(unsigned int windowW, unsigned int windowH, bool fullScreen)
{
	mIsRunning = true;
	mIsMinimized = false;
	mIsEditorMode = false;
	mIsLetterBox = false;

	std::cout <<
		" __ __				 				" << std::endl <<
		"|  |  |            | 				" << std::endl <<
		"|  |  |   __   __  |       __,		" << std::endl <<
		"|  |  |  /  \\_/    |/ \\   /  |		" << std::endl <<
		"|  |  |_/\\__/ \\___/|   |_/\\_/|_/  " << std::endl <<
		"_____________________________________" << std::endl;

  // Need to init OVR first for DirectMode to work
  // vrMode 1 - SteamVR
  // vrMode 0 - Oculus
  VR* vr = 0;
  if (Config::vrMode)
    vr = SteamVR::getInstance();
  else
    vr = OculusVR::getInstance(); 
  vr->Init();

  mOculusWnd.Init(L"subject REDACTED - Oculus", windowW, windowH, false);
  mGameWnd.Init(L"subject REDACTED", windowW, windowH, fullScreen);
  //mOculusWnd.SetFullScreen(true);

	/// Init Systems ///
	/* Task Manager */
	//TaskManager::getInstance();

	//Begin thread safety routines
	ThreadUtils::Startup();

	// Init the Crash Handler
	CRASH_HANDLER::Init();

	//start the sub load state from the config
	//does not work!
	//SubloadStateVisitor ssv;
	//Renderer::getInstance()->Draw();

	// Load the minimal data from available assets to determine working set in parallel
	(new Task_AvailableAsset_Refresh())->Tick();

	/* Renderer */
  mRenderer = Renderer::getInstance();
  mRenderer->Init(vr);

	FrameController::getInstance()->BeginTimeSample("AssetManager");
	AssetManager::getInstance()->Init();
	FrameController::getInstance()->EndTimeSample("AssetManager", true);

	/* Physics Engine */
	this->mPhysicsEngine = PhysicEngine::getInstance();

	this->mFrameController = FrameController::getInstance();
	this->mFrameController->CapOn(120.f);

	FrameController::getInstance()->BeginTimeSample("AudioManager");
	AudioManager::getInstance()->Load();
	FrameController::getInstance()->EndTimeSample("AudioManager", true);

	InputManager::getInstance()->Load();

	PrefabContainer::getInstance();

	GSM::getInstance()->PushState(new BlankState());

	//StateSerializer ss;
	//ss.SaveCurrentState("TestMap.mocha");

	//generate main loop in the barrista
	barrista().RegisterToPrimary(GSM::getInstance());
	//barrista().RegisterToPrimary_AfterLast(ScriptSubsystem::getInstance());
	//barrista().RegisterToPrimary_After(Renderer::getInstance(), GSM::getInstance());
	barrista().RegisterToPrimary_AfterLast(AnimationSubSystem::getInstance());
	barrista().RegisterToPrimary_AfterLast(AudioManager::getInstance());
  barrista().RegisterDebugOutputToParallel(AudioManager::getInstance());
  //barrista().RegisterToPrimary_AfterLast(FrustumCullingTask::getInstance());// , AnimationSubSystem::getInstance());
  barrista().RegisterToPrimary_AfterLast(GPUBufferManager::getInstance());//, FrustumCullingTask::getInstance());
  barrista().RegisterToPrimary_AfterLast(Renderer::getInstance());//, FrustumCullingTask::getInstance());
  barrista().RegisterToPrimary_AfterLast(mFrameController);
	//barrista().RegisterToPrimary_AfterLast(MInt::getInstance());  
  barrista().RegisterToPrimary_AfterLast(InputManager::getInstance());

	barrista().RegisterToPrimary_AfterLast(mPhysicsEngine);
	

	AssetManager::getInstance()->assChecker = MAKEMETAMETHODVARIABLE(AssetManager, CheckForChangedAssets);
	//barrista().togo->Invoke(AssetManager::getInstance(), AssetManager::getInstance()->assChecker, GSM::getInstance(), 5000, true);

	barrista().OutputSchedule();

	mIsInited = true;
}

Engine* Engine::getInstance()
{
	static Engine* instance = 0;

	if (!instance)
		instance = new Engine();

	return instance;
}

void Engine::StartFromConfig(bool run)
{
	ConfigSerializer s;
	s.Load();

	if (Config::showConsole)
	{
		ConsoleAdapter* c = new ConsoleAdapter();
		c->CreateConsole(CONSOLETYPE_e::BOTH);
	}

	Engine::getInstance()->Init(Config::width, Config::height, Config::startFullscreen);

	if (run)
		Engine::getInstance()->Run();

}

void Engine::Start(bool showConsole, unsigned width, unsigned height, bool fullscreen, bool run )
{
	ConfigSerializer s;
	s.Load();

	if (showConsole)
	{
		ConsoleAdapter* c = new ConsoleAdapter();
		c->CreateConsole(CONSOLETYPE_e::BOTH);
	}

	Engine::getInstance()->Init(width, height, fullscreen);

	if (run)
		Engine::getInstance()->Run();
}
//////////////////////////////////////////////////////////////////////////
// Safe Frame

struct Task_SafeFrame : public Task
{
	static Task_SafeFrame* instance;
	std::mutex mutex;
	bool inSafeFrame;

	virtual bool Tick()
	{
		/*
		{
			inSafeFrame = true;
			mutex.lock();
			//safe frame
		}
		mutex.lock();
		inSafeFrame = false;*/
		return true;
	}

	void SignalEndOfSafeFrame()
	{
		mutex.unlock();
	}

	Task_SafeFrame() : inSafeFrame(false) {
		instance = this;
		mutex.lock();
	}

	metadef(Task_SafeFrame)
		endmetadef;
};
Task_SafeFrame* Task_SafeFrame::instance = 0;
meta_define(Task_SafeFrame);

void Engine::RegisterSafeFrameToBarrista()
{
	if (Task_SafeFrame::instance != 0)
		return;
	barrista().RegisterToPrimary(new Task_SafeFrame());
}

bool Engine::IsInSameFrame()
{
	return Task_SafeFrame::instance->inSafeFrame;
}

void Engine::SignalSafeFrameContinue()
{
	Task_SafeFrame::instance->SignalEndOfSafeFrame();
}

bool Engine::GetIsInited() const
{
	return mIsInited;
}

void Engine::RegisterEditorMode()
{
	mIsEditorMode = true;
	barrista().RegisterToPrimary(EditorSubsystem::getInstance());
	if (!GetIsInited())
		throw std::exception("Editor started before engine fully init'ed.");
}

bool Engine::GetIsEditorMode() const
{
	return mIsEditorMode;
}

MochaWindow* Engine::GetGameWnd()
{
  return &mGameWnd;
}

MochaWindow* Engine::GetVRWnd()
{
  return &mOculusWnd;
}

MochaWindow* Engine::GetWndFromHandle(const HWND& hWnd)
{
  if (hWnd == mOculusWnd.GetHWND())
    return &mOculusWnd;
  
  return &mGameWnd;
}

MochaWindow* Engine::GetFocusedWnd()
{
  return GetWndFromHandle(GetFocus());
}

Rect Engine::GetMonitorDisplayInfo(std::wstring monitorName)
{
  IDXGIFactory* pFactory;
  CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
  unsigned adapterIndex = 0;
  IDXGIAdapter* pAdapter;
  IDXGIOutput* pFoundOutput = 0;
  DXGI_OUTPUT_DESC outDesc;

  //int adapterNum = 0, outadapterNum;
  //SteamVR::getInstance()->GetHMD()->GetDXGIOutputInfo(&adapterNum, &outadapterNum);

  // Iterate through adapters.
  while (pFactory->EnumAdapters(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND)
  {
    unsigned outputIndex = 0;
    IDXGIOutput* pOutput;

    // Iterate through outputs.
    while (pAdapter->EnumOutputs(outputIndex, &pOutput) != DXGI_ERROR_NOT_FOUND)
    {
      pOutput->GetDesc(&outDesc);
      WCHAR* outputName = outDesc.DeviceName;

      // Try and match the first part of the display name.
      // For example an outputName of "\\.\DISPLAY1" might
      // correspond to a displayName of "\\.\DISPLAY1\Monitor0".
      // If two monitors are setup in ’duplicatemode then they will
      // have the same ’displaypart in their display name.

      if (monitorName.find(outputName) != std::wstring::npos)
      {
        pFoundOutput = pOutput;
        break;
      }

      ++outputIndex;
    }

    ++adapterIndex;
  }

  Rect monitorRect;
  monitorRect.x = (float)outDesc.DesktopCoordinates.left;
  monitorRect.y = (float)outDesc.DesktopCoordinates.top;
  monitorRect.width = (float)outDesc.DesktopCoordinates.right - monitorRect.x;
  monitorRect.height = (float)outDesc.DesktopCoordinates.bottom - monitorRect.y;

  return monitorRect;
}

void Engine::LoadInitialLevel()
{
	if (Config::initialLevel != "")
	{
		StateSerializer ss;
		ss.LoadState(Config::initialLevel);
	}
	else
		GSM::getInstance()->PushState(new BlankState());
}

void Engine::LockMainLoop()
{
	ThreadUtils::EnterCritSection();
}

void Engine::UnlockMainLoop()
{
	ThreadUtils::LeaveCritSection();
}

bool Engine::IsGameInFocus()
{
    //HWND h = GetForegroundWindow();
    HWND h = GetFocus();
    return h && (h == mGameWnd.GetHWND() || h == mOculusWnd.GetHWND());
}
bool Engine::IsEditorWindowInFocus()
{
    return mIsEditorFocused;
}
void Engine::SetPauseMode(bool val)
{
    pauseMode_ = val;

    if(!val)
    {
        AudioManager::getInstance()->Enable(true);
        AudioManager::getInstance()->ResumeAll();
    }
}

void Engine::Exit()
{
	exit(0);
}

bool Engine::GetIsLetterBox() const
{
  return mIsLetterBox;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
