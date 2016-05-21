
%{
#include "Engine.h"
	%}

class Engine
{
private:
	Engine();
	~Engine();
public:
	void StartFromConfig(bool run = true);
	void Start(bool showConsole, unsigned width, unsigned height, bool fullscreen, bool run = true);
	void Run();
	void RegisterEditorMode();
	void LoadInitialLevel();

	void LockMainLoop();
	void UnlockMainLoop();

	void GetDesktopResolution(int& horizontal, int& vertical);
	bool GetIsInited() const;
	bool GetIsEditorMode() const;
	void Exit();

	MochaWindow* GetGameWnd();
	MochaWindow* GetVRWnd();
	MochaWindow* GetFocusedWnd();

	//Add a Safe Frame Handler to the Barrista, which will pause when the engine is in an idle state
	void RegisterSafeFrameToBarrista();
	void SignalSafeFrameContinue();
	bool IsInSameFrame();
    bool IsGameInFocus();
	bool IsEditorWindowInFocus();
    void SetPauseMode(bool val);
	static Engine* getInstance();

	bool mIsLetterBox;
};