using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class DoorUnlockSequence : MochaScript 
{	
	float failTime = 2.5f;
	int[] center = new int[6];
    int activeHeight;
    int maxHeight = 6;
	bool started = false;
    bool passed = false; // If door is unlocked
    bool failed = false; // If currently reseting from red screen

    float randomTimer = 0.0f;
    const float timeToRandomise = 0.08f;
	
	DoorSequenceButton stealthDoorButton;
    GameObject stealthIndicators;
    OcuConsoleScreen ocuConsole;
	//GameObject surveillanceIndicators;
	GameObject door;
	
	CSound mSound;
    
	// Use this for initialization
	public void OnStart()
    {
		mSound = gameObject.RequireComponent<CSound>();
		ocuConsole = GetScript<OcuConsoleScreen>(Common.GetSurveillancePlayerConsoleScreen());
		
        RandomCenter();
    }
	
	// Update is called once per frame
	public void OnUpdate ()
    {
        if (failed == true)
        {
            float dt = FrameController.DT();
            failTime -= dt;

            if (failTime <= 1.0f)
            {
                if (failTime < 0.0f) // Time to reset to show starting button color
                {
                    failed = false;
                    failTime = 2.5f;
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
	
	public void CenterPull(int pullFrom)
    {
		// change to colour code
		pullFrom += 1;
		
        if (stealthIndicators == null || failed || passed)
		{
            return; // If error or already completed
        }
		
        if (started == false) // First time trigger by stealth P
        {
            started = true;
			RefreshColorsOnBlocks(true);
			return;
        }

        if (center[activeHeight] == pullFrom)
        {
			// center[activeHeight] = 3; // Make next center bar green
            activeHeight++;
			if(activeHeight >= maxHeight)
				Pass();
            else
			    RefreshColorsOnBlocks(true);
        }
        else
        {
            Fail();
        }
    }

    public void Pass()
    {
        passed = true;
        for (int j = 0; j <= maxHeight-1; j++)
        {
			center[j] = 3; // Set green
        }
		RefreshColorsOnBlocks(false);
		
		DoorMain doorMainScript = GetScript<DoorMain>(door);
        doorMainScript.isLocked = false;
		
		stealthDoorButton.completed = true;
		
		mSound.PlayIndependentEvent("PINPAD_CONFIRM.vente", false, 1);
    }

    public void Fail()
    {
        failed = true;
        for (int j = 0; j <= maxHeight-1; j++)
        {
			center[j] = -1;
        }
		RefreshColorsOnBlocks(false);
		
		mSound.PlayIndependentEvent("PINPAD_DENY.vente", false, 1);
    }
	
	public void Reset()
    {
        activeHeight = 0;
        RandomCenter();
		RefreshColorsOnBlocks(true);
    }
	
	public void Off()
    {
        activeHeight = -1;
		RefreshColorsOnBlocks(true);
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
	
	public void SetParticipants(GameObject _go, GameObject _goParent, string doorName)
    {
		if(_goParent != stealthIndicators)
		{
			if(stealthIndicators != null)
			{
				if(!stealthDoorButton.completed)
				{
					Off();
				}
			}
			
			started = false;
			passed = false;
			
			stealthDoorButton = GetScript<DoorSequenceButton>(_go);
			
			stealthIndicators = _goParent;
			
			GameObject door_;
			door_ = GameObject.GetGameObjectByName(doorName);
			door = door_;
			
			Reset();
		}
    }
	
	void RefreshColorsOnBlocks(bool hideCenter = false)
	{
		// Color center
		for(int i = 0; i <= 5; i++)
		{
            Color clrOfBlock = GetColorOfBlock(i, hideCenter);
            ChangeButtonColor(stealthIndicators, "Col2", i.ToString(), clrOfBlock);
		}
        ocuConsole.DoorMiniGameEvent(ConvertGameToString(hideCenter), gameObject, passed);
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
                clrOfBlock = clrOfBlock.Mul(0.06f); // Darken selected blocks, if not passed or failed
        }
        return clrOfBlock;
    }
	
	void ChangeButtonColor(GameObject pad, string columnNumber, string buttonNumber, Color color)
	{
		GameObject column;
		GameObject button;
		
		column = Common.GetChildByName(pad, columnNumber);
		button = Common.GetChildByName(column, buttonNumber);
		button.GetComponent<CMeshRenderer>().mEmissiveColor = color;
	}
	
	Color ColorIntToColor(int code)
	{
		if(code == 0)
			return SharpMocha.WHITE;
		else if(code == 1)
			return SharpMocha.BLUE;
		else if(code == 2)
			return SharpMocha.YELLOW;
		else if(code == 3)
			return SharpMocha.GREEN;
		else if(code == -1)	
			return SharpMocha.RED;
		return SharpMocha.WHITE;
	}
}
