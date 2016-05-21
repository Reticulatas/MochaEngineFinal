/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include "IRegisterable.h"

 
class Input : public IRegisterable
{
public:
	static float GetReleased(int playerIndex, std::string inputName);
	/*! \brief If the button/stick in question has been triggered, return it's Magnitude, else return 0.0f */
	 static float GetTriggered(int playerIndex, std::string inputName);
	/*! \brief If the button/stick in question is being held, return it's Magnitude, else return 0.0f */
	 static float GetHeld(int playerIndex, std::string inputName);
	/*! \brief Get raw value of the button/stick in question (For Keyboard, this will be 1.0f = true, 0.0f = false */
	 static float GetValue(int playerIndex, std::string inputName);
	/*! \brief Returns the Mouse X position relative to the top left corner of the screen MAX (ScreenWidth) */
	 static int GetMouseX(bool gameWnd);
	/*! \brief Returns the Mouse Y position relative to the top left corner of the screen MAX (ScreenHeight) */
	 static int GetMouseY(bool gameWnd);
	/*! \brief Returns the change in the mouse X position since the last frame */
	 static int GetMouseDeltaX();
	/*! \brief Returns the change in the mouse Y position since the last frame */
	 static int GetMouseDeltaY();
	/*! \brief Returns the change in the mouse wheel's movement - in multiples or divisions of 120 */
	 static int GetMouseWheelDeltaY();
     /*! \brief Locks the cursor to the center of the window*/
     static void SetMouseLock(bool val);
     /*! \brief set the cursor to a point of the window*/
     static void SetMousePosition(int x, int y);
     /*! \brief Gets info if the cursor to the center of the window*/
     static bool GetLockMouse();
     /*! \brief Sets the mouse's visibility*/
     static void SetMouseVisible(bool val);
     /*! \brief Gets the mouse's visibility*/
     static bool GetMouseVisible();
     /*! \brief Gets info if the cursor is a the window*/
     static bool GetIsMouseInWindow(bool gameWnd);
	 /*! \brief If the specified player is set to use the keyboard, returns true. If it's a gamepad, return false */
     static bool IsPlayerUsingKeyboard(int playerIndex);
     /*! \brief If Ctrl Alt Del was detected*/
     static bool GetCtrlAltDel();
};
 

//_REG_STATIC_METHOD(float, Input, GetTriggered, "uint8 playerIndex, string inputName")
//#define _REG_STATIC_METHOD(returnVal, classname, funcName, arguments) r = engine->RegisterGlobalFunction(#returnVal " " #funcName "(" arguments ")", asFUNCTION(classname::funcName), asCALL_CDECL); ___AS___

/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change