using Microsoft.Xna.Framework;
using MochaInterface;using System;

public class PinPadController : MochaScript
{
    public string CODE { get; set; } // door num is 951H
    int currDigitIndex = 0;
	bool isDoorUnlocked = false;

    bool isFlashingRed = false;
    int flashingState = 0; // 0-normal, 1-red, 2-base clr
    const float FLASH_DURATION = 0.3f;
    float flashTimer;
    int flashCount = 0;

    // own object is the pin pad display
    CFontRenderer pinPadDisplay;
    CMeshRenderer pinPadDisplayBg;
    Color baseDisplayColor;
    public String pinPadObjName { get; set;} // the digit after the name is the number it represents
    public String doorName { get; set; } // the door to open
    DoorMain doorScript; // script for door to unlock it
    // GameObject doorTop; // script to use is "Door". the doors to unlock
    // GameObject doorBtm;
    // public String doorBtmName { get; set;}
    // public String doorTopName { get; set;}
    
    Mouse playerMouse;
    GameObject player;
    FirstPersonPlayer playerScript; // to know whether using Xbox controller

    CSound sound;

	public void OnStart()
    {
        sound = gameObject.RequireComponent<CSound>();

        pinPadDisplay = gameObject.RequireComponent<CFontRenderer>();
        pinPadDisplay.mText = "---";
        pinPadDisplayBg = gameObject.transform.GetParent().gameObject.RequireComponent<CMeshRenderer>();
        baseDisplayColor = pinPadDisplayBg.GetMaterial().GetColor();

        GameObject door = GameObject.GetGameObjectByName(doorName);
        if (door != null)
            doorScript = GetScript<DoorMain>(door);
        // doorTop = GameObject.GetGameObjectByName(doorBtmName);
        // doorBtm = GameObject.GetGameObjectByName(doorTopName);

        playerMouse = GetScript<Mouse>(Common.GetStealthPlayerMouse() );
        player = Common.GetStealthPlayer();
        playerScript = GetScript<FirstPersonPlayer>(player);
    }
    public void OnUpdate()
    {
        if (isDoorUnlocked)
            return; // Do not need any more updates once it has unlocked the doors

        float dt = FrameController.DT();
        // If flashing red, do not allow inputs
        if (isFlashingRed)
        {
            flashTimer -= dt; // update timer
            if (flashTimer <= 0.0f)
            {
                flashTimer += FLASH_DURATION; // reset the timer
                if (flashingState == 1) // if need to set red
                {
                    pinPadDisplayBg.GetMaterial().SetColor(SharpMocha.RED);
                    flashingState = 2;
                }
                else
                {
                    flashingState = 1;
                    pinPadDisplayBg.GetMaterial().SetColor(baseDisplayColor);
                    if (flashCount++ >= 3)
                    {
                        // Clear the string
                        pinPadDisplay.mText = "---";
                        currDigitIndex = 0;

                        isFlashingRed = false;
                    }
                }
            }
        }
        else
        {
            // Check if input 3 digits
            if (currDigitIndex >= 3)
            {
                if (pinPadDisplay.mText == CODE) // if correct
                {
                    // Show green screen
                    pinPadDisplayBg.GetMaterial().SetColor(SharpMocha.GREEN);

                    // Unlock the doors
                    doorScript.isLocked = false;
                    /*Door top = GetScript<Door>(doorTop);
                    top.isLocked = false;
                    Door btm = GetScript<Door>(doorBtm);
                    btm.isLocked = false;*/

                    isDoorUnlocked = true;

                    sound.PlayIndependentEvent("PINPAD_CONFIRM.vente", false, 0);
                }
                else
                {
                    // Flash red
                    isFlashingRed = true;
                    flashingState = 1;
                    flashTimer = 0.0f; // First update will trigger set to red
                    flashCount = 0;

                    sound.PlayIndependentEvent("PINPAD_DENY.vente", false, 0);
                }
            }

            // Only raycast and check if player is near the keypad
            Vector3 playerToKeypad = (Vector3)gameObject.transform.position - (Vector3)player.transform.position;
            if (Vector3.Dot(playerToKeypad, playerToKeypad) < 8.0f * 8.0f)
            {
                GameObject pickedObject = playerMouse.RayCastWithTag(Common.tagForPinPad);
                if (pickedObject != null)
                {
                    // Mouse over of keypad effect

                    if (playerScript.IsStealthPlayerInteracting())
                    {
                        String digit = pickedObject.GetName().Substring(pinPadObjName.Length);
                        if (digit.Length != 1)
                            Logger.Log("pinpad's digit is more than 1 digit!");

                        InputPinPad(Convert.ToInt32(digit));
                    }
                }
            } // End of checking if player is near to keypad
        }
    }

    public void InputPinPad(int digit)
    {
        char[] currString = pinPadDisplay.mText.ToCharArray();
        currString[currDigitIndex++] = (char)('0' + digit);
        pinPadDisplay.mText = new string(currString); // Update the digit to display

        sound.PlayIndependentEvent("PINPAD_INPUT.vente", false, 0);
    }
}