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
#include "MochaWindow.h"

#include "PhysicsEngine.h"
#include "FrameController.h"

class Renderer;
/* Singleton Engine class*/
class Engine
{
	Engine() : mIsInited(false) {}
	//~Engine();

  MochaWindow mGameWnd;
  MochaWindow mOculusWnd;

	bool mIsRunning;
	bool mIsMinimized;
	bool mIsInited;
	bool mIsEditorMode;
    bool mIsEditorFocused;

	/* Initialize engine and all its systems*/
	void Init(unsigned int windowW = 800, unsigned int windowH = 600, bool fullScreen = false);
	/* Run the main loop and updates */

public:

	///////////SYSTEM POINTERS /////////////
	Renderer* mRenderer;					/* Pointer to the renderer */
	FrameController* mFrameController;
	PhysicEngine* mPhysicsEngine;

	///////////PUBLIC METHODS ///////////////

	// this is the main message handler for the program
	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK OculusHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Blocks when called
	void StartFromConfig(bool run = true);
	void Start(bool showConsole, unsigned width, unsigned height, bool fullscreen, bool run = true);
	void Run();
	void RegisterEditorMode();
	void LoadInitialLevel();
	void Exit();

	//blocking calls
	void LockMainLoop();
	void UnlockMainLoop();

	////FUNCTIONALITIES/////
	void GetDesktopResolution(int& horizontal, int& vertical);	
	bool GetIsInited() const;
	bool GetIsEditorMode() const;
	bool GetIsLetterBox() const;
    bool IsGameInFocus();
    bool IsEditorWindowInFocus();

    // prevents audio to resume when in pause menu
    void SetPauseMode(bool val);
  MochaWindow* GetGameWnd();
  MochaWindow* GetVRWnd();
  MochaWindow* GetFocusedWnd();

	//Add a Safe Frame Handler to the Barrista, which will pause when the engine is in an idle state
	void RegisterSafeFrameToBarrista();
	void SignalSafeFrameContinue();
	bool IsInSameFrame();
  MochaWindow* GetWndFromHandle(const HWND& hWnd);
  Rect GetMonitorDisplayInfo(std::wstring monitorName);

  bool pauseMode_;
  bool mIsLetterBox;
	static Engine* getInstance();
};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change