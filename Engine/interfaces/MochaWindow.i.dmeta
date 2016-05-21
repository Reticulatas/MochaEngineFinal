
%{
#include "MochaWindow.h"
%}

class MochaWindow
{
public:
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
	bool GetFocus()const;
	HWND& GetHWND(void);
	unsigned GetHwndAsInt();
};