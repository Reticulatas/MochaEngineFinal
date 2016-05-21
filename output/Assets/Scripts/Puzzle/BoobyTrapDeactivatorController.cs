using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class BoobyTrapDeactivatorController : MochaScript
{
    class SequenceData 
    {
        public List<int> validSequence = new List<int>();
        public bool accessGranted = false;
    };

    SequenceData sequence = new SequenceData();
    List<int> playerInput = new List<int>();

    List<GameObject> panels = new List<GameObject>(); // panel buttons to decode this trap
    public List<GameObject> allPanels = new List<GameObject>(); // all the panel buttons to decode this trap

    GameObject geneticsAccessRightLightObj;
    CLight geneticsAccessRightLight;
    GameObject geneticsLabDoor;
    DoorMain doorScript; // to open to next sequence area
    DoorMain doorTerminalScript; // of the current room

    GameObject boobyTrapControllerObj;
    BoobyTrapControllerScript boobyTrapScript;
    ConsoleSequenceInitiator trapAreaScript;

    CSound sound;

    public void OnStart()
    {
        geneticsAccessRightLightObj = GameObject.GetGameObjectByName("GLD_RightLight");
        geneticsAccessRightLight = geneticsAccessRightLightObj.RequireComponent<CLight>();
        geneticsLabDoor = GameObject.GetGameObjectByName("GeneticsLabDoor");
        doorScript = GetScript<DoorMain>(geneticsLabDoor);
        GameObject doorTerminal = GameObject.GetGameObjectByName("Terminal2Door");
        doorTerminalScript = GetScript<DoorMain>(doorTerminal);
        boobyTrapControllerObj = GameObject.GetGameObjectByName("TSR_SeqScrambler");
        boobyTrapScript = GetScript<BoobyTrapControllerScript>(boobyTrapControllerObj);
        GameObject triggerArea = GameObject.GetGameObjectByName("TrapTrigger");
        trapAreaScript = GetScript<ConsoleSequenceInitiator>(triggerArea);

        //ADD SEQUENCES MANUALLY HERE
        List<int> redRoomTrapSeq = new List<int>();
        redRoomTrapSeq.Add(43); redRoomTrapSeq.Add(82); redRoomTrapSeq.Add(13); redRoomTrapSeq.Add(35);
        sequence.validSequence = redRoomTrapSeq;

        sound = gameObject.RequireComponent<CSound>();
    }

    public void OnUpdate()
    {
        // Cheat code (ctrl+shift+1)
        if (Input.GetHeld(0, "SnapGrid") != 0.0f && Input.GetHeld(0, "SnapShift") != 0.0f && 
            Input.GetTriggered(0, "1") != 0.0f)
        {
            playerInput.Clear();
            panels.Clear();
            foreach (int number in sequence.validSequence) // Unlock the sequence
            {
                CheckSequence(number, null);
            }
        }
    }

    public void ResetUponDeathOfPlayer()
    {
        // Reset all buttons back to pre-activated state
        foreach (GameObject go in allPanels)
        {
            BoobyTrapDeactivator script = GetScript<BoobyTrapDeactivator>(go);
            if (script != null)
            {
                script.Reset();
            }
        }
        playerInput.Clear();
        panels.Clear();

        trapAreaScript.isActive = true; // Allow trap to set off again
        boobyTrapScript.ResetTrap(false); // Reset all the lights
        doorTerminalScript.UnlockDoor(); // Unlock the door again
    }

    public void StartTrap()
    {
        Common.GetConsoleScreenScript().BoobyTrapEvent(0, gameObject, false);
    }
    public void TimeIsUp()
    {
        // Set lockdown state for all buttons
        foreach (GameObject go in allPanels)
        {
            BoobyTrapDeactivator script = GetScript<BoobyTrapDeactivator>(go);
            if (script != null)
            {
                script.SetLockdownState();
            }
        }

        // Release the poison gas at full blast

        // After 2-3 sec, kill the player
        Common.GetStealthPlayerCameraScript().Kill();
    }
    public bool CheckSequence(int seq, GameObject obj)
	{
        if (!sequence.accessGranted)
        {
            panels.Add(obj);
            playerInput.Add(seq);
            int isValidCode = -1;

            for(int i = 0; i < playerInput.Count; ++i)
            {
                if (playerInput[i] != sequence.validSequence[i])
                {
                    isValidCode = 0;
                    break;
                }

                if((i + 1) == sequence.validSequence.Count)
                {
                    isValidCode = 1;
                }
            }

            if (isValidCode == 0)
            {
                foreach (GameObject go in panels)
                {
                    BoobyTrapDeactivator script = GetScript<BoobyTrapDeactivator>(go);
                    if (script != null)
                    {
                        script.ChangeState(false); // Update buttons to tween feedback
                    }
                }
                // consoleScript.QueueMessage("INVALID CODE", 0.0f, false, true);
                playerInput.Clear();
                panels.Clear();
                Common.GetConsoleScreenScript().BoobyTrapEvent(-1, gameObject, false); // -1 to signal clearing the screen

                sound.PlayIndependentEvent("BUTTON_RED.vente", false, 0); // Access Granted sound
            }
            else // one more correct number code
            {
                Common.GetConsoleScreenScript().BoobyTrapEvent(seq, gameObject, false);
            }

            if (isValidCode == 1) // successfully finished trap
            {
                foreach (GameObject go in panels)
                {
                    BoobyTrapDeactivator script = GetScript<BoobyTrapDeactivator>(go);
                    if (script != null)
                    {
                        script.ChangeState(true); // Update buttons to tween feedback
                    }
                }
                geneticsAccessRightLight.SetDiffuse(SharpMocha.GREEN);
                doorScript.UnlockDoor();
                doorTerminalScript.UnlockDoor();
                boobyTrapScript.DeactivateTrap();

                Common.GetConsoleScreenScript().BoobyTrapEvent(seq, gameObject, true);

                playerInput.Clear();
                sequence.accessGranted = true;

                sound.PlayIndependentEvent("BUTTON_GREEN.vente", false, 0); // Access Granted sound
            }
            //consoleScript.QueueMessage(seq.ToString(), 0.0f, false, true);
        }

        ////Send message to console on progress HERE
        // consoleScript.QueueMessage("ACCESS ALREADY GRANTED", 0.0f, false, true);
        return true;
	}
	
}
