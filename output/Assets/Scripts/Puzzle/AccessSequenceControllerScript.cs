using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class AccessSequenceControllerScript : MochaScript
{
    GameObject redButton;
	GameObject greenButton;
	GameObject blueButton;

    List<GameObject> buttons = new List<GameObject>();
    List<CLight> lights = new List<CLight>();
    DoorMain door1Script;
    DoorMain door2Script;

    List<int> sequence = new List<int>();
    List<int> playerSequence = new List<int>();
    bool accessGranted = false;
	
	GameObject consoleScreen;
	OcuConsoleScreen consoleScript;

    CLight lightAtDoor;
	GameObject geneticsAccessLeftLightObj;
	CLight geneticsAccessLeftLight;
    DeathCheckpointTriggerArea checkpointScript;
	GameObject chilloutFace;
	CMeshRenderer cFace;

    CSound sound;

    public void OnStart()
    {
        sound = gameObject.RequireComponent<CSound>();

        redButton = GameObject.GetGameObjectByName("redButton");
		greenButton = GameObject.GetGameObjectByName("greenButton");
		blueButton = GameObject.GetGameObjectByName("blueButton");
        consoleScreen = Common.GetSurveillancePlayerConsoleScreen();
		consoleScript = GetScript<OcuConsoleScreen>(consoleScreen);
		//lights = GameObject.GetGameObjectsWithTag("phase1light");
        GameObject doorObj = GameObject.GetGameObjectByName("Terminal1Door");
        door1Script = GetScript<DoorMain>(doorObj);
        doorObj = GameObject.GetGameObjectByName("Terminal2Door");
        door2Script = GetScript<DoorMain>(doorObj);
		
        GameObject checkpoint = GameObject.GetGameObjectByName("Checkpoint1a");
        checkpointScript = GetScript<DeathCheckpointTriggerArea>(checkpoint);
		chilloutFace = GameObject.GetGameObjectByName("chilloutface 1");
		cFace = (chilloutFace.RequireComponent<CMeshRenderer>());
		
        GameObject lightObj = GameObject.GetGameObjectByName("Light1a");
        lightAtDoor = lightObj.RequireComponent<CLight>();
        lightAtDoor.setEnabled(false);

        foreach (GameObject obj in GameObject.GetGameObjectsWithTag("phase1light"))
        {
            lights.Add(obj.RequireComponent<CLight>());
        }
        sequence.Add(1); sequence.Add(3); sequence.Add(2);
		if(lights.Count == 0)
        { 
        }
		
		geneticsAccessLeftLightObj = GameObject.GetGameObjectByName("GLD_LeftLight");
		geneticsAccessLeftLight = geneticsAccessLeftLightObj.RequireComponent<CLight>();
    }
    
    public void OnUpdate()
    {
        if (Input.GetHeld(0, "SnapGrid") != 0.0f && Input.GetTriggered(0, "1") != 0.0f)
        {
            playerSequence.Clear();
            buttons.Clear();
            foreach (int number in sequence) // Unlock the sequence
            {
                AddPlayerInput(number, null);
            }
        }
    }

	public void ScrambleSequence()
	{
	
	}
	public bool AddPlayerInput(int input, GameObject obj)
	{
        if (accessGranted)
            return true;

        buttons.Add(obj);
        playerSequence.Add(input);

        bool isValidCode = true;
        for(int i = 0; i < playerSequence.Count; ++i)
        {
            if(playerSequence[i] != sequence[i])
            {
                // WRONG INPUT
                playerSequence.Clear();
                isValidCode = false;

				// consoleScript.QueueMessage("INVALID CODE", 0.0f, false, true);
                break;
            }
            if(i == 2)
            {
                //Console.Write("ACCESS GRANTED!");
                accessGranted = true;
                door1Script.UnlockDoor();
                door1Script.isOneWayDoor = false; // Do not lock anymore
                door2Script.UnlockDoor();
                lightAtDoor.setEnabled(true);
                foreach (CLight light in lights)
                {
                    light.setEnabled(true);
                }
				geneticsAccessLeftLight.SetDiffuse(SharpMocha.GREEN);
                consoleScript.QueueMessage("Access sequence accepted.", 0.0f, false, true);
                buttons.Clear();

                sound.PlayIndependentEvent("BUTTON_GREEN.vente", false, 0); // Access Granted sound

                // Activate checkpoint at the door
                checkpointScript.isActive = true;
				cFace.setEnabled(true);
            }
        }
        // If incorrect, reset all buttons
        if (!isValidCode)
        {
            foreach (GameObject go in buttons)
            {
                GenericButton buttonScript = GetScript<GenericButton>(go);
                if (buttonScript != null)
                {
                    buttonScript.ResetButton();
                }
            }
            buttons.Clear();
        }

        return isValidCode;
	}

}