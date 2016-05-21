using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MiniGameFourPlayersButtonController : MochaScript
{
	OcuConsoleScreen ocuConsole;
	GameObject door;
    CLight rightLight;
    CLight leftLight;

    List<MiniGameFourPlayersButton> allButtons = new List<MiniGameFourPlayersButton>();
    List<int> correctIndexOrder = new List<int>();
    List<int> correctHeights = new List<int>();
    int mLatestCorrect = 0; // index of curr working button

	CSound mSound;
    
	// Use this for initialization
	public void OnStart()
    {
        mSound = gameObject.RequireComponent<CSound>();
		ocuConsole = Common.GetConsoleScreenScript();
        door = GameObject.GetGameObjectByName(Common.prefix + "GeneticsLabDoor2");

        GameObject lightObj = GameObject.GetGameObjectByName(Common.prefix + "GLD_RightLight2");
        rightLight = lightObj.RequireComponent<CLight>();
        lightObj = GameObject.GetGameObjectByName(Common.prefix + "GLD_LeftLight2");
        leftLight = lightObj.RequireComponent<CLight>();

        correctIndexOrder.Add(0);
        correctHeights.Add(3); // 3A
        correctIndexOrder.Add(2);
        correctHeights.Add(6); // 6C
        correctIndexOrder.Add(3);
        correctHeights.Add(5); // 5D
        correctIndexOrder.Add(1);
        correctHeights.Add(3); // 3B
    }
    public int GetButtonIndex(GameObject obj)
    {
        if (allButtons.Count == 0)
        {
            string objName = "";
            for (int i = 0; i < 4; ++i)
            {
                objName = Common.prefix;
                switch (i)
                {
                    case 0: objName += "TSR2_DoorUnlockPanelStealthA"; break;
                    case 1: objName += "TSR2_DoorUnlockPanelStealthB"; break;
                    case 2: objName += "TSR2_DoorUnlockPanelStealthC"; break;
                    case 3: objName += "TSR2_DoorUnlockPanelStealthD"; break;
                }
                GameObject panel = GameObject.GetGameObjectByName(objName);
                GameObject button = Common.GetChildByName(panel, "DoorUnlockButton");
                GameObject buttonTriggered = Common.GetChildByName(button, "0");
                allButtons.Add(GetScript<MiniGameFourPlayersButton>(buttonTriggered));
            }
            
            //allButtons.Add(GetScript<MiniGameFourPlayersButton>(GameObject.GetGameObjectByName(Common.prefix + "TSR2_DoorUnlockPanelStealthB")));
            //allButtons.Add(GetScript<MiniGameFourPlayersButton>(GameObject.GetGameObjectByName(Common.prefix + "TSR2_DoorUnlockPanelStealthC")));
            //allButtons.Add(GetScript<MiniGameFourPlayersButton>(GameObject.GetGameObjectByName(Common.prefix + "TSR2_DoorUnlockPanelStealthD")));
        }

        for (int i = 0; i < allButtons.Count; ++i)
        {
            string objName = "";
            switch (i)
            {
                case 0: objName = "TSR2_DoorUnlockPanelStealthA"; break;
                case 1: objName = "TSR2_DoorUnlockPanelStealthB"; break;
                case 2: objName = "TSR2_DoorUnlockPanelStealthC"; break;
                case 3: objName = "TSR2_DoorUnlockPanelStealthD"; break;
            }
            GameObject panelParent = obj.transform.GetParent().GetParent().gameObject;
            if (objName == panelParent.GetName())
            {
                return i;
            }
        }
        Logger.Log("Bad init of minigame 4 player button, name: " + obj.GetName());
        return 0;
    }
    public string GetButtonString(int buttonIndex)
    {
        string letter = "A";
        switch (buttonIndex)
        {
            case 0: letter = "A"; break;
            case 1: letter = "B"; break;
            case 2: letter = "C"; break;
            case 3: letter = "D"; break;
        }
        return letter;
    }

    // Update is called once per frame
    public void OnUpdate()
    {
        if (Input.GetHeld(0, "ALTMOD") != 0.0f && Input.GetTriggered(0, "1") != 0.0f)
        {
            mLatestCorrect = 4;
            PassAll();
        }
	}
    public void TriggeredButton(int index, bool isStealthPlayer)
    {
        if (mLatestCorrect >= 4) // Ignore presses if already unlocked door
            return;

        if (index != correctIndexOrder[mLatestCorrect]) // Wrong sequence
        {
            // Signal to all buttons to reset to nothing
            FailAll();
            return;
        }
        else
        {
            // Check if height is wrong if this input goes in
            if (allButtons[index].activeHeight + 1 > correctHeights[mLatestCorrect])
            {
                FailAll();
                return;
            }
            else // Allow the button press
            {
                allButtons[index].CenterPull(isStealthPlayer ? 1 : 0);
            }
        }

        // If right now, latest sequence A is matched up, move the latest seq to the next
        if (allButtons[correctIndexOrder[mLatestCorrect]].activeHeight == correctHeights[mLatestCorrect])
        {
            ++mLatestCorrect;
            if (mLatestCorrect >= 4) // Successfull on all
            {
                PassAll();
            }
        }
    }

    public void FailAll()
    {
        mSound.PlayIndependentEvent("PINPAD_DENY.vente", false, 1);

        for (int i = 0; i < allButtons.Count; ++i)
        {
            allButtons[i].Fail();
        }

        mLatestCorrect = 0;
        ocuConsole.MiniGameFourPlayerEvent("", "Sequence Invalid", -1, gameObject, 0, false);
    }
    void PassAll()
    {
        for (int i = 0; i < allButtons.Count; ++i)
        {
            allButtons[i].Pass();
        }

        DoorMain doorMainScript = GetScript<DoorMain>(door);
        doorMainScript.UnlockDoor();

        leftLight.SetDiffuse(SharpMocha.GREEN);
        rightLight.SetDiffuse(SharpMocha.GREEN);

        mSound.PlayIndependentEvent("PINPAD_CONFIRM.vente", false, 1);

        ocuConsole.MiniGameFourPlayerEvent("", "", 0, gameObject, 0, true);
		ocuConsole.QueueMessage("Subject [REDACTED] : Deduction test completed", 0.0f, false, true);

		GameObject pz3room = GameObject.GetGameObjectByName("TrapSequenceRoom2");
		PuzzleBEnvironmentController pz3ScaryScript = GetScript<PuzzleBEnvironmentController>(pz3room);
		pz3ScaryScript.finished = true;
    }
}
