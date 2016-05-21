using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class DoorButton : StealthInteractableObject
{
    int isDoorUnlocked = 0; // 0 - not unlocked, 1 - tweening updates when unlocking, 2 - unlocked
    bool isButtonRed = false;
    bool isButtonActive = true;
    const float REQUEST_TIMEOUT = 0.8f;
    float requestAccessTimer;

    GameObject door;
    CMeshRenderer doorButtonMesh;
    Color doorButtonColor;

    public String doorName { get; set; }
    String doorButtonTag = "doorButton";
    public bool requiresSurveillancePlayer { get; set; }
    
    Mouse playerMouse;
    GameObject player;
    FirstPersonPlayer playerScript; // to know whether using Xbox controller
    OcuConsoleScreen ocuConsole;

    CSound sound;

    public override void OnStart()
    {
        base.OnStart();

        sound = gameObject.RequireComponent<CSound>();
        doorButtonMesh = gameObject.RequireComponent<CMeshRenderer>();
        
        if (doorName != null && doorName.Length > 0)
        {
            door = GameObject.GetGameObjectByName(doorName);
            if (requiresSurveillancePlayer)
                doorButtonMesh.GetMaterial().SetColor(SharpMocha.YELLOW);
            else
                doorButtonMesh.GetMaterial().SetColor(SharpMocha.CORNFLOWER_BLUE);
        }
        else
        {
            isButtonRed = true; // it is a default locked button
        }
        doorButtonColor = doorButtonMesh.GetMaterial().GetColor();

        playerMouse = GetScript<Mouse>(Common.GetStealthPlayerMouse() );
        player = Common.GetStealthPlayer();
        playerScript = Common.GetStealthPlayerScript();
        ocuConsole = GetScript<OcuConsoleScreen>(Common.GetSurveillancePlayerConsoleScreen());
    }
    
    public override void OnUpdate()
    {
        if (isDoorUnlocked == 2)
            return;

        float dt = FrameController.DT();

        if (isDoorUnlocked == 1) // Tweening the button when unlocked
        {
            isDoorUnlocked = 2;
        }
        else
        {
            if (!isButtonActive) // Awaiting ocu player's reply or time runs out
            {
                requestAccessTimer -= dt;
                if (requestAccessTimer <= 0.0f)
                {
                    doorButtonMesh.GetMaterial().SetColor(doorButtonColor); // Reset to original color
                    doorButtonMesh.mEmissiveColor = doorButtonColor;
                    isButtonActive = true;
                }
            }
        } // End of if door is still locked
    }

    public void AllowUnlockDoor() // called by OcuConsoleText script
    {
        doorButtonMesh.GetMaterial().SetColor(doorButtonColor.Mul(0.4f));

        DoorMain doorMainScript = GetScript<DoorMain>(door);
        doorMainScript.isLocked = false;

        isButtonActive = false;
        isDoorUnlocked = 1;
    }

    public override void OnStealthPlayerMouseEnter()
    {
        if (isButtonActive)
            doorButtonMesh.mEmissiveColor = doorButtonColor;
    }
	public override void OnStealthPlayerMouseLeave()
    {
        doorButtonMesh.mEmissiveColor = SharpMocha.BLACK;
    }
	public override void OnStealthPlayerMouseInteract()
    {
        if (isButtonActive)
        {
            String buttonSoundName = "PINPAD_INPUT.vente";// requiresSurveillancePlayer ? "BUTTON_YELLOW.vente" : "BUTTON_GREEN.vente";
            if (isButtonRed)
            {
                buttonSoundName = "PINPAD_DENY.vente";

                requestAccessTimer = REQUEST_TIMEOUT;
            }
            else
            {
                if (requiresSurveillancePlayer)
                {
                    requestAccessTimer = REQUEST_TIMEOUT;
                }
                else // just open the door immediately
                {
                    AllowUnlockDoor();
                }
            }
            // Signal to console screen
            ocuConsole.DoorButtonEvent(gameObject, (isButtonRed ? 2 : (requiresSurveillancePlayer ? 1 : 0)));

            doorButtonMesh.GetMaterial().SetColor(doorButtonColor.Mul(0.4f));
            doorButtonMesh.mEmissiveColor = SharpMocha.BLACK;
            isButtonActive = false;

            sound.PlayIndependentEvent(buttonSoundName, false, 1);
        }
    }
}