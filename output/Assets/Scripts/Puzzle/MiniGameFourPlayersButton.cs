using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MiniGameFourPlayersButton : StealthInteractableObject 
{
	int[] center = new int[6];
    public int activeHeight = -1;
    int maxHeight = 6;
	bool started = false;
    public bool passed = false; // If door is unlocked
    public bool failed = false; // If currently reseting from red screen
    float failTimer = 2.0f;

    float randomTimer = 0.0f;
    const float timeToRandomise = 0.08f;

    GameObject panelParent;
    MiniGameFourPlayersButtonController controllerScript;
    int mMyIndex = -1;
	
	CSound mSound;
    
	// Use this for initialization
	public override void OnStart()
    {
        base.OnStart();

		mSound = gameObject.RequireComponent<CSound>();
        controllerScript = GetScript<MiniGameFourPlayersButtonController>(
            GameObject.GetGameObjectByName(Common.prefix + "TSR2_DoorUnlockPanelStealthA"));
        mMyIndex = controllerScript.GetButtonIndex(gameObject);

        panelParent = gameObject.transform.GetParent().GetParent().gameObject;

        RandomCenter(); // Initialize the column puzzle
    }
	
	// Update is called once per frame
    public override void OnUpdate()
    {
        if (failed)
        {
            float dt = FrameController.DT();
            failTimer -= dt;

            if (failTimer <= 1.0f)
            {
                if (failTimer < 0.0f) // Time to reset to show starting button color
                {
                    failed = false;
                    failTimer = 2.5f;
                    randomTimer = 0.0f;
                    Reset();
                }
                else // Flashing random button colors to animate resetting
                {
                    randomTimer += dt;
                    if (randomTimer >= timeToRandomise)
                    {
                        randomTimer -= timeToRandomise;

                        // Reset all to white
                        for (int i = 0; i < maxHeight; i++)
                        {
                            int color = MMath.GetRandomInt(1, 3); // [1,2]
                            center[i] = color;
                        }
                        RefreshColorsOnBlocks(false);
                    }
                }
            }
        }
	}

    public override void OnStealthPlayerMouseEnter()
    {

    }
    public override void OnStealthPlayerMouseLeave()
    {

    }
    public override void OnStealthPlayerMouseInteract()
    {
        // Check if button is facing player
        Vector3 buttonForward = panelParent.transform.GetForwardVector();
        if (Vector3.Dot(buttonForward, Common.GetStealthPlayerCamera().transform.GetForwardVector()) >= 0.0f)
        {
            // Tell controller that I am clicked on
            controllerScript.TriggeredButton(mMyIndex, true); // CenterPull(1);
        }
    }
	
    // The action when either player clicks on the game (0 - from Oculus, 1 - from Stealth)
	public void CenterPull(int pullFrom)
    {
		if (failed || passed)
		{
            return; // If error or already completed
        }

        mSound.PlayIndependentEvent("PINPAD_INPUT.vente", false, 1);
        // change to colour code
        pullFrom += 1; // 1 is oculus color, 2 is stealth color
        if (activeHeight == -1)
        {
            ++activeHeight;
            RefreshColorsOnBlocks(true);
            return;
        }

        if (center[activeHeight] == pullFrom) // Correct person pressing it
        {
            ++activeHeight;
            RefreshColorsOnBlocks(true);
        }
        else
        {
            controllerScript.FailAll();
        }
    }
    // Call to change what appears on console screen
    void RefreshColorsOnBlocks(bool hideCenter, bool isSentToConsoleScreen = true)
    {
        // Color center
        for (int i = 0; i <= 5; i++)
        {
            Color clrOfBlock = GetColorOfBlock(i, hideCenter);
            ChangeButtonColor(panelParent, "Col2", i.ToString(), clrOfBlock);
        }
        if (isSentToConsoleScreen)
        {
            Common.GetConsoleScreenScript().MiniGameFourPlayerEvent(
                ConvertGameToString(hideCenter), controllerScript.GetButtonString(mMyIndex), 
                mMyIndex, controllerScript.gameObject, activeHeight, passed);
        }
    }

    public void Pass() // called by the button that initiated the unlocking
    {
        passed = true;
        for (int j = 0; j < maxHeight; j++)
        {
			center[j] = 3; // Set green
            if (j >= activeHeight)
                center[j] = 0; // Set white for the rest above
        }
        RefreshColorsOnBlocks(false, false); // Don't send to console
    }
    public void Fail()
    {
        failed = true;
        activeHeight = -1;
        for (int j = 0; j < maxHeight; j++)
        {
			center[j] = -1;
        }
        RefreshColorsOnBlocks(false, false); // Don't send to console
    }
	void Reset()
    {
        activeHeight = -1; // do not show any button
        RandomCenter();
		RefreshColorsOnBlocks(true, true);
    }
	void RandomCenter()
    {
        int totalBlues = maxHeight/2;
        int totalYellows = maxHeight/2;
        int randNum;
        for (int i = 0; i < center.Length; i++)
        {
			randNum = MMath.GetRandomInt(1,3);
			
			//Equal Random
            if (totalBlues <= 0)
                randNum = 2;
            else if (totalYellows <= 0)
                randNum = 1;
			
            if (randNum == 1)
                totalBlues -= 1;
            if (randNum == 2)
                totalYellows -= 1;
			
            center[i] = randNum;
        }
    }
	
    string ConvertGameToString(bool hideCenter)
	{
		string gameConverted = "";
		for (int i = maxHeight-1; i >= 0; i--)
		{
            Color clrOfBlock = GetColorOfBlock(i, hideCenter);
			gameConverted += Common.ConvertColorToStringTag(clrOfBlock) + "\\b\\n"; // 1 (hacked) newline between each row
		}
		return gameConverted;
	}
    Color GetColorOfBlock(int height, bool hideCenter)
    {
        Color clrOfBlock = ColorIntToColor(0); // White
        if (!(hideCenter && height > activeHeight)) // Hide all blocks above currently active block
        {
            clrOfBlock = ColorIntToColor(center[height]);
            if (!failed && !passed && height < activeHeight)
                clrOfBlock = clrOfBlock.Mul(0.1f); // Darken selected blocks, if not passed or failed
        }
        return clrOfBlock;
    }

    Color ColorIntToColor(int code)
    {
        if (code == 0)
            return SharpMocha.WHITE;
        else if (code == 1)
            return SharpMocha.BLUE;
        else if (code == 2)
            return SharpMocha.YELLOW;
        else if (code == 3)
            return SharpMocha.GREEN;
        else if (code == -1)
            return SharpMocha.RED;
        return SharpMocha.WHITE;
    }
	void ChangeButtonColor(GameObject pad, string columnNumber, string buttonNumber, Color color)
	{
		GameObject column;
		GameObject button;

        column = Common.GetChildByName(pad, columnNumber);
        button = Common.GetChildByName(column, buttonNumber);
        button.GetComponent<CMeshRenderer>().GetMaterial().SetColor(color);
	}
}
