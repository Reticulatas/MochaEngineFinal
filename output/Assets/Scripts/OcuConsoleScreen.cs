using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections;
using System.Collections.Generic;

public class OcuConsoleScreen : InteractableObjectBaseClass
{
    CFontRenderer consoleText;
    const String defaultText = "System ready.";
    const String defaultHeaderText = "System: ";
    Vector3 mCenterPosForText; // Holds the center of the console screen for text, (-10,25)
    List<GameObject> mExtraTexts; // For the right side of the screen

    String backupHeaderText = "";
    LinkedList<String> backupTextToDisplay;

    const float mDarkenAmount = 0.35f;
    bool mMessageHasColorTags = false; // Used to signal for next queue message to parse the latest message to darken the color tags
    bool mPrevMessageHasColorTags = false;
    Color mDColorOfText = new Color(0.137f, 0.706f, 0.294f, 1.0f);
    public Color mColorOfText { get; set; } // FF2EFF00
    const String mBlinkingChar = "<#1,1,1,1#>\\b";
    float mBlinkingTimer;
    bool mIsBlinkingCharVisible = false;

    float mDWidthLimitBeforeNewling = 4.0f;
    public float mWidthLimitBeforeNewling { get { return mDWidthLimitBeforeNewling; }
        set { mDWidthLimitBeforeNewling = value; } }

    GameObject mHeaderTextObj;
    CFontRenderer mHeaderFont;
    String mHeaderText;
    int hasHeaderTextChanged; // 0 - no change, 1 - new message, 2 - typing out text
    int currHeaderIndexDisplayed;
    float mTimerForHeader;

    LinkedList<Tuple<String, float, bool> > mMessageQueue; // text, timer, hasColoredTags
    String mNextMessageString;
    float mNextMessageTimer;

    LinkedList<String> mTextToDisplay = new LinkedList<String>();
    int hasConsoleTextChanged; // 0 - no change, 1 - new message, 2 - typing out text
    String firstLineTextToDisplay;
    String btmTextToDisplay;
    int currTextIndexDisplayed; // used to index to the whole text, to get the next char
    float eachCharTimer = 0.0f;

    int mDMaxLines = 10;
    int mMaxLines { get { return mDMaxLines; }
        set { mDMaxLines = value; } }

    CONSOLE_EVENT currEvent;
    GameObject savedObject;
    float eventTimer;
    int eventState;
    int eventState2;

    Mouse playerMouse;
    GameObject player;
    OcuConsoleBlinkingLight mBlinkingLight;

    CSound sound;

    enum CONSOLE_EVENT
    {
        NONE = 0,
        DOOR_REQUEST,
		DOOR_MINIGAME_TURN,
        BOOBY_TRAP,
        MINIGAME_FOUR_PLAYERS,

        NUM_CONSOLE_EVENT
    };

    public void OnStart()
    {
        sound = gameObject.RequireComponent<CSound>();

        consoleText = Common.GetSurveillancePlayerConsoleText().RequireComponent<CFontRenderer>();
        // consoleText.mFontSize = 60;
        // consoleText.mFontScale = 0.0024f;

        mBlinkingTimer = 0.0f;
        consoleText.mText = "";
        firstLineTextToDisplay = "";
        btmTextToDisplay = "";

        playerMouse = GetScript<Mouse>(Common.GetStealthPlayerMouse());
        player = Common.GetStealthPlayer();
        mBlinkingLight = GetScript<OcuConsoleBlinkingLight>(Common.GetConsoleBlinkingLight());

        mCenterPosForText = Common.GetConsoleCam().transform.position; // console: -10, 25
        mCenterPosForText.Z = consoleText.gameObject.transform.position.z;

        mExtraTexts = new List<GameObject>();

        if (mColorOfText == null || mColorOfText.Comp(SharpMocha.BLACK))
        {
            mColorOfText = mDColorOfText;
        }

        mHeaderTextObj = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
        mHeaderTextObj.SetName("ConsoleHeaderText");
        Vector3 headerPos = mCenterPosForText;
        headerPos.X -= 3.3f; //-13.40, 26.8
        headerPos.Y += 1.8f;
        mHeaderTextObj.transform.SetPosition(headerPos);
        mHeaderFont = mHeaderTextObj.GetComponent<CFontRenderer>();
        mHeaderFont.mText = "";
        mHeaderFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.LEFT_JUSTIFIED;

        ChangeHeader(defaultHeaderText);

        mNextMessageTimer = 0.0f;
        mMessageQueue = new LinkedList<Tuple<String, float, bool> >();
        QueueMessage("System Initializing...", 0.0f, false, false);
        QueueMessage("Enabling Camera Systems.", 0.7f, false, false);
        QueueMessage("Enabling Map Systems.", 0.4f, false, false);
        QueueMessage("Enabling Door Security Systems.", 0.3f, false, false);
        QueueMessage("System Ready.", 0.7f, false, false);

        mTextToDisplay.AddFirst("> ");
        UpdateConsoleTextFromQueue();
        hasConsoleTextChanged = 0;
    }

    public void CreateSavedState()
    {
        // Backup all the queues
        LinkedList<Tuple<String, float, bool>> bMessageQueue = new LinkedList<Tuple<String, float, bool>>(mMessageQueue);
        String bNextMessageString = mNextMessageString;
        float bNextMessageTimer = mNextMessageTimer;

        LinkedList<String> bTextToDisplay = new LinkedList<String>(mTextToDisplay);
        int bHasConsoleTextChanged = hasConsoleTextChanged; // 0 - no change, 1 - new message, 2 - typing out text
        String bFirstLineTextToDisplay = firstLineTextToDisplay;
        String bBtmTextToDisplay = btmTextToDisplay;
        int bCurrTextIndexDisplayed = currTextIndexDisplayed; // used to index to the whole text, to get the next char

        // Force show immediately
        QueueMessage("", 0.0f, false, true);
        backupTextToDisplay = mTextToDisplay; // Save the list of texts after all are forced on screen

        // Restore back original stuff
        mMessageQueue = bMessageQueue;
        mNextMessageString = bNextMessageString;
        mNextMessageTimer = bNextMessageTimer;

        mTextToDisplay = bTextToDisplay;
        hasConsoleTextChanged = bHasConsoleTextChanged; // 0 - no change, 1 - new message, 2 - typing out text
        firstLineTextToDisplay = bFirstLineTextToDisplay;
        btmTextToDisplay = bBtmTextToDisplay;
        currTextIndexDisplayed = bCurrTextIndexDisplayed; // used to index to the whole text, to get the next char

        // Save the header text
        backupHeaderText = mHeaderText;
    }
    public void RestoreToSavedState()
    {
        if (backupTextToDisplay != null)
        {
            mTextToDisplay = backupTextToDisplay;
            UpdateConsoleTextFromQueue();

            ResetToDefaultState();

            ChangeHeader(backupHeaderText);
        }
    }

    public void ChangeHeader(String text)
    {
        if (mHeaderText != text)
        {
            hasHeaderTextChanged = 2;
            mHeaderText = text;
            currHeaderIndexDisplayed = 0;
            mTimerForHeader = 0.0f;

            mHeaderFont.mText = "";
        }
    }
    public void QueueMessage(String text, float timeToWait, bool hasColorTags, bool showImmediately)
    {
        if (showImmediately) // Immediately add all messages in queue into the console
        {
            bool doesLastMessageHaveColor = mPrevMessageHasColorTags; // Store if what's going to be top most console text has color tag
            mPrevMessageHasColorTags = false; // Prevent all below texts to be parsed for nothing

            var currNode = mMessageQueue.Last;
            while (currNode != null)
            {
                if (doesLastMessageHaveColor && currNode == mMessageQueue.First) // If at the top most text, and it was color tags
                {
                    mPrevMessageHasColorTags = true; // Set to make this text be darkened-parsed
                }
                AddMessage(currNode.Value.Item1);

                currNode = currNode.Previous;
            }

            mMessageQueue.Clear();
            UpdateAllTextImmediately();
        }
        // Add the newest message at the queue
        mMessageQueue.AddFirst(new Tuple<String, float, bool>(text, timeToWait, hasColorTags));
    }
    void AddMessage(String text)
    {
        if (mPrevMessageHasColorTags) // current old top of console text has color tags
        {
            String coloredText = mTextToDisplay.First.Value; // colored text that needs to be darken
            String correctedText = "";

            int index = 0;
            int startIndex = -1;
            while (index != -1)
            {
                if (index > coloredText.Length)
                    break;

                startIndex = coloredText.IndexOf("<#", index);
                int endIndex = coloredText.IndexOf("#>", index);

                if (startIndex == -1)
                    break;

                int firstComma = coloredText.IndexOf(',', startIndex + 2);
                int secondComma = coloredText.IndexOf(',', firstComma + 1);
                int thirdComma = coloredText.IndexOf(',', secondComma + 1);

                float r = Convert.ToSingle(coloredText.Substring(startIndex + 2, firstComma - (startIndex + 2)));
                float g = Convert.ToSingle(coloredText.Substring(firstComma + 1, secondComma - (firstComma + 1)));
                float b = Convert.ToSingle(coloredText.Substring(secondComma + 1, thirdComma - (secondComma + 1)));
                float a = Convert.ToSingle(coloredText.Substring(thirdComma + 1, endIndex - (thirdComma + 1)));
                Color clrTag = new Color(r, g, b, a);
                clrTag = clrTag.Mul(mDarkenAmount);

                correctedText += coloredText.Substring(index, startIndex - index);
                correctedText += Common.ConvertColorToStringTag(clrTag);

                index = endIndex + 2;
            }
            if (startIndex == -1)
            {
                correctedText += coloredText.Substring(index);
            }
            mTextToDisplay.First.Value = correctedText; // Replace the old top of console with darkened tags

            mPrevMessageHasColorTags = false;
        }

        hasConsoleTextChanged = 1;
        String dateTimeStr = DateTime.Now.ToString("HH:mm:ss");
        String textLine = "> [" + dateTimeStr + "] : " + text;

        // Process the message to ensure it does not exceed the width limit
        consoleText.mText = textLine;
        consoleText.ClampTextWithinWidth(mWidthLimitBeforeNewling); // Force to be within width limits
        textLine = consoleText.mText;

        mTextToDisplay.AddFirst(textLine);

        UpdateConsoleTextFromQueue();
    }

    // Updates the text to be updated on console screen from the queue of strings
    void UpdateConsoleTextFromQueue()
    {
        firstLineTextToDisplay = ""; // Clear the text
        btmTextToDisplay = "";
        String currText = consoleText.mText;

        // First entry in queue is the last line in console screen
        int index = 0;
        for (int i = mMaxLines; i < mTextToDisplay.Count; ++i)
        {
            mTextToDisplay.RemoveLast(); // Remove last lines, until reached max num of lines
        }
        Color darkenedColor = mColorOfText.Mul(mDarkenAmount);
        String darkenedColorStr = Common.ConvertColorToStringTag(darkenedColor); // Color to change from darkened (for all lines other than first)

        var currNode = mTextToDisplay.First;
        while (currNode != null)
        {
            if (index == 0) // The topmost line
            {
                firstLineTextToDisplay = Common.ConvertColorToStringTag(mColorOfText) + currNode.Value;
            }
            else
            {
                // Holds all the lines except the topmost
                btmTextToDisplay = btmTextToDisplay + darkenedColorStr + currNode.Value + "\\n";
            }

            currNode = currNode.Next;
            ++index;
        }
        consoleText.mText = "";

        hasConsoleTextChanged = 2;
        currTextIndexDisplayed = 0; // Reset back to 0th index
    }
    void UpdateAllTextImmediately()
    {
        hasConsoleTextChanged = 0;
        consoleText.mText = firstLineTextToDisplay + (mIsBlinkingCharVisible ? mBlinkingChar : "") + 
            "\\n" + btmTextToDisplay;
    }

    String GrabOneCharFromString(String inputStr, int startingIndex)
    {
        String textLeft = inputStr.Substring(startingIndex);
        if (textLeft.StartsWith("<#"))
        {
            int endIndex = textLeft.IndexOf("#>", 2);
            if (endIndex < 0) // If can't find it somehow
            {
                Logger.Log("OcuConsoleScreen text can't find #> end");
                textLeft = textLeft.Substring(0, 1);
            }
            else // copy over the whole <# #> color tag chunk
            {
                textLeft = textLeft.Substring(0, endIndex + 2);
            }
        }
        else if (textLeft.StartsWith("\\"))
        {
            textLeft = textLeft.Substring(0, 2); // take the \ and 1 char after that
        }
        else // Usual case, to just take the next char
        {
            textLeft = textLeft.Substring(0, 1);
        }
        return textLeft;
    }
    public void OnUpdate()
    {
        float dt = FrameController.DT();
        if (hasConsoleTextChanged == 2)
        {
            const float minTimeForEachCharacter = 0.01f;
            const float maxTimeForEachCharacter = 0.07f;
            eachCharTimer -= dt;
            if (eachCharTimer <= 0.0f)
            {
                eachCharTimer = MMath.GetRandomLimitedFloat(minTimeForEachCharacter, maxTimeForEachCharacter);
                if (currTextIndexDisplayed >= firstLineTextToDisplay.Length) // Finished typing all messages
                {
                    UpdateAllTextImmediately();
                }
                else
                {
                    String textLeft = GrabOneCharFromString(firstLineTextToDisplay, currTextIndexDisplayed);
                    if (textLeft == " ")
                        eachCharTimer = 0.1f; // longer pause after a space
                    
                    currTextIndexDisplayed += textLeft.Length; // Advance the index by amount of text copied over
                    String currPartOfFirstLine = firstLineTextToDisplay.Substring(0, currTextIndexDisplayed);
                    consoleText.mText = currPartOfFirstLine + mBlinkingChar + "\\n" + btmTextToDisplay;
                }
            }
        }
        else if (hasConsoleTextChanged == 0) // Screen is idling
        {
            // Checking if there are incoming messages waiting
            if (mMessageQueue.Count > 0)
            {
                if (mNextMessageTimer <= 0.0f) // First time triggering
                {
                    mNextMessageString = mMessageQueue.Last.Value.Item1;
                    mNextMessageTimer = mMessageQueue.Last.Value.Item2;
                    mPrevMessageHasColorTags = mMessageHasColorTags;
                    mMessageHasColorTags = mMessageQueue.Last.Value.Item3;
                }

                mNextMessageTimer -= dt;
                if (mNextMessageTimer <= 0.0f) // Time to show next message
                {
                    AddMessage(mNextMessageString);
                    mMessageQueue.RemoveLast();
                }
            }
        }

        if (hasHeaderTextChanged == 2)
        {
            float timeForEachCharacter = 0.1f;
            mTimerForHeader -= dt;
            if (mTimerForHeader <= 0.0f)
            {
                mTimerForHeader = timeForEachCharacter;
                if (currHeaderIndexDisplayed >= mHeaderText.Length)
                {
                    hasHeaderTextChanged = 0;
                    mHeaderFont.mText = mHeaderText;
                }
                else
                {
                    String textLeft = GrabOneCharFromString(mHeaderText, currHeaderIndexDisplayed);

                    currHeaderIndexDisplayed += textLeft.Length;
                    mHeaderFont.mText += textLeft;
                }
            }
        }

        mBlinkingTimer -= dt;
        if (mBlinkingTimer <= 0.0f)
        {
            mBlinkingTimer = 0.3f;
            mIsBlinkingCharVisible = !mIsBlinkingCharVisible;
            if (hasConsoleTextChanged == 0) // Nothing is typing out, then blink the char
            {
                UpdateAllTextImmediately();
            }
        }

#if ZERO
        if (hasNewTextToDisplay == 1) // Trigger 1 frame
        {
            consoleText.mText = "";
            timer = 0.0f;
            hasNewTextToDisplay = 2;

            mBlinkingLight.TriggerBlinkingLight(true); // start blinking
        }
        else if (hasNewTextToDisplay == 2)
        {
            float timeForEachCharacter = 0.1f;
            timer -= dt;
            if (timer <= 0.0f)
            {
                timer = timeForEachCharacter;

                consoleText.mText = firstLineTextToDisplay.Substring(0, consoleText.mText.Length+1);
            }
        }
		else if (hasNewTextToDisplay == 3)
		{
			consoleText.mText = firstLineTextToDisplay;
		}

        if (hasNewTextToDisplay != 0 && consoleText.mText.Length == firstLineTextToDisplay.Length)
        {
            hasNewTextToDisplay = 0; // end of typing out

            mBlinkingLight.TriggerBlinkingLight(false); // stop blinking
        }
#endif
        UpdateEvent(dt);
    }

    void UpdateEvent(float dt)
    {
        switch (currEvent)
        {
            case CONSOLE_EVENT.BOOBY_TRAP:
                {
                    CFontRenderer alertTextFont = mExtraTexts[0].GetComponent<CFontRenderer>();
                    if (eventState == 0) // waiting for codes to show
                    {
                        eventTimer -= dt;
                        if (eventTimer <= 0.0f)
                        {
                            eventState = 1; // Show the countdown timer
                            eventTimer = 100.0f;
                            alertTextFont.setEnabled(true);
                        }
                    }
                    else if (eventState == 1)
                    {
                        eventTimer -= dt;
                        if (eventTimer <= 0.0f)
                        {
                            eventState = 2;
                            eventTimer = 0.0f;

                            BoobyTrapDeactivatorController boobyTrapScript = GetScript<BoobyTrapDeactivatorController>(savedObject, false);
                            if (boobyTrapScript != null)
                            {
                                boobyTrapScript.TimeIsUp();
                            }
                            QueueMessage("Lockdown is in effect.\\n<#1,0,0,1#>Now cleansing Terminal 2 of organic matter.", 0.0f, true, true);
                        }
                        alertTextFont.mText = "Lockdown in\\n<#1,0,0,1#>" + eventTimer.ToString("0.00"); // 2 decimal places
                    }
                }
                break;
        }
    }
    public override void OnTriggered()
    {
        switch (currEvent)
        {
            case CONSOLE_EVENT.DOOR_REQUEST:
                if (savedObject != null)
                {
                    if (hasConsoleTextChanged == 0) // if all the text has displayed, then allow touch to open door
                    {
                        // Open the door, then 
                        DoorButton buttonScript = GetScript<DoorButton>(savedObject);
                        if (buttonScript != null)
                            buttonScript.AllowUnlockDoor();

                        EndOfDoorButtonEvent();
                    }
                    else // If still typing out, first click immediately shows all the text
                    {
                        UpdateAllTextImmediately();
                    }
                }
                break;
			case CONSOLE_EVENT.DOOR_MINIGAME_TURN:
				if (savedObject != null)
                {
					// Open the door, then 
					DoorUnlockSequence doorUnlockSeq = GetScript<DoorUnlockSequence>(savedObject);
					if (doorUnlockSeq != null)
						doorUnlockSeq.CenterPull(0);
                }
                break;
            case CONSOLE_EVENT.MINIGAME_FOUR_PLAYERS:
                if (savedObject != null && eventState != -1 && eventState2 != -1) // If one of the buttons is active
                {
                    MiniGameFourPlayersButtonController minigameScript = GetScript<MiniGameFourPlayersButtonController>(savedObject);
                    if (minigameScript != null)
                        minigameScript.TriggeredButton(eventState, false); // Click on screen to perform action on game
                }
                break;
        }
    }
    public override void OnMouseOver()
    {
        CFontRenderer sub = GetSubtitleFont();
        if (!sub.isEnabled())
        {
            sub.setEnabled(true);
            sub.mText = "Click to Interact";
        }

        switch (currEvent)
        {
            case CONSOLE_EVENT.DOOR_MINIGAME_TURN:
                {
                    CFontRenderer fontOfInput = mExtraTexts[1].GetComponent<CFontRenderer>();
                    fontOfInput.mColor = mColorOfText; // Change to green when mouse-overed
                }
                break;
            case CONSOLE_EVENT.MINIGAME_FOUR_PLAYERS:
                {
                    CFontRenderer fontOfInput = mExtraTexts[1].GetComponent<CFontRenderer>();
                    fontOfInput.mColor = mColorOfText; // Change to green when mouse-overed
                }
                break;
        }
    }
    public override void OnMouseOverOff()
    {
        GetSubtitleFont().setEnabled(false);

        switch (currEvent)
        {
            case CONSOLE_EVENT.DOOR_MINIGAME_TURN:
                {
                    CFontRenderer fontOfInput = mExtraTexts[1].GetComponent<CFontRenderer>();
                    fontOfInput.mColor = SharpMocha.WHITE; // Return to normal color
                }
                break;
            case CONSOLE_EVENT.MINIGAME_FOUR_PLAYERS:
                {
                    CFontRenderer fontOfInput = mExtraTexts[1].GetComponent<CFontRenderer>();
                    fontOfInput.mColor = SharpMocha.WHITE; // Return to normal color
                }
                break;
        }
    }

    void ResetToDefaultState()
    {
        ChangeHeader(defaultHeaderText);

        savedObject = null;

        foreach (GameObject gObj in mExtraTexts)
        {
            gObj.Destroy();
        }
        mExtraTexts.Clear();

        currEvent = CONSOLE_EVENT.NONE;
    }
    GameObject CreateDividerText()
    {
        GameObject dottedLine = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
        Vector3 headerPos = mCenterPosForText;
        headerPos.X += 0.8f;
        headerPos.Y += 1.6f;
        dottedLine.transform.SetPosition(headerPos);
        CFontRenderer dottedLineTextFont = dottedLine.GetComponent<CFontRenderer>();
        dottedLineTextFont.mText = "|\\n|\\n|\\n|\\n|\\n|\\n|\\n|\\n|\\n|\\n|";
        dottedLineTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.CENTER_JUSTIFIED;
        dottedLineTextFont.mNewlineSpacing = 1.5f;
        return dottedLine;
    }
    // List of Events to be sent to console
    public void DoorButtonEvent(GameObject obj, int greenYellowOrRedButton) // Co-op button press to unlock door
    {
        String message = "";
        switch (greenYellowOrRedButton)
        {
            case 0: message = "<#1,1,0,1#>Door Override Detected."; break;
            case 1:
                ChangeHeader("DOOR SYSTEM:");
                message = "<#1,1,0,1#>Door Access Requested. Touch to Confirm."; 
                break;
            case 2: message = "<#1,0,0,1#>Warning! Invalid Door Access Requested!"; break;
        }
        QueueMessage(message, 0.0f, true, true);
        savedObject = obj;

        currEvent = CONSOLE_EVENT.DOOR_REQUEST;
    }
    void EndOfDoorButtonEvent()
    {
        QueueMessage("<#0,1,0,1#>Access Granted. Door Opening...", 0.0f, true, true);
        
        ResetToDefaultState();
    }
	public void DoorMiniGameEvent(string newMsg, GameObject obj, bool isUnlocked) // Co-op mini-game
	{
        if (isUnlocked)
        {
            // If the door is unlocked, then game has ended.
            QueueMessage("<#0,1,0,1#>AAccess Granted. Door Opening...", 0.0f, true, true);

            ResetToDefaultState();
        }
        else
        {
            savedObject = obj;

            if (currEvent != CONSOLE_EVENT.DOOR_MINIGAME_TURN) // First time trigger into minigame
            {
                ChangeHeader("DOOR SYSTEM:");
                QueueMessage("Door Access Requested.", 0.0f, false, true);
                QueueMessage("Unlock Sequence Initiating...\\n<#1,1,0,1#>Yellow " + Common.ConvertColorToStringTag(mColorOfText) +
                    "external input required.", 0.6f, true, false);
                currEvent = CONSOLE_EVENT.DOOR_MINIGAME_TURN;

                // Create game screen column itself
                GameObject headerText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 headerPos = mCenterPosForText;
                headerPos.X += 2.4f;
                headerPos.Y += 1.0f;
                headerText.transform.SetPosition(headerPos);
                CFontRenderer headerTextFont = headerText.GetComponent<CFontRenderer>();
                headerTextFont.mText = newMsg;
                headerTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.CENTER_JUSTIFIED;
                headerTextFont.mNewlineSpacing = 1.5f;

                // Create "Input ..." feedback at the btm
                GameObject btmText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 btmPos = mCenterPosForText;
                btmPos.X = headerPos.X;
                btmPos.Y -= 1.0f;
                btmText.transform.SetPosition(btmPos);
                CFontRenderer btmTextFont = btmText.GetComponent<CFontRenderer>();
                btmTextFont.mText = "Touch to enter your input";
                btmTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.CENTER_JUSTIFIED;
                btmTextFont.mFontScale = 0.002f;

                // Create divider
                GameObject dottedLine = CreateDividerText();

                mExtraTexts.Add(headerText);
                mExtraTexts.Add(btmText);
                mExtraTexts.Add(dottedLine);
            }
            else
            {
                CFontRenderer headerTextFont = mExtraTexts[0].GetComponent<CFontRenderer>();
                headerTextFont.mText = newMsg; // Update the main game column
            }
        }
	}
    public void BoobyTrapEvent(int numberCode, GameObject obj, bool isDeactivated) // pass in -1 to reset
    {
        if (isDeactivated) // End of trap
        {
            QueueMessage("Override successful. Lockdown deactivated.", 0.0f, false, true);
            QueueMessage("Laboratory access granted.", 1.6f, false, false);

            ResetToDefaultState();
        }
        else // Start
        {
            if (currEvent != CONSOLE_EVENT.BOOBY_TRAP) // First time trigger into booby trap
            {
                savedObject = obj;
                eventTimer = 5.0f;
                eventState = 0;

                ChangeHeader("Lockdown System:");
                QueueMessage("Intruder detected! Lockdown initiated.", 0.0f, false, true);
                QueueMessage("Override codes:\\n20, 43, 55, 82, 13, 59, 94, 35, 11", 0.6f, false, false);
                currEvent = CONSOLE_EVENT.BOOBY_TRAP;

                // Create an alert screen
                GameObject alertText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 alertPos = mCenterPosForText;
                alertPos.X += 2.4f;
                alertPos.Y += 1.5f;
                alertText.transform.SetPosition(alertPos);
                CFontRenderer alertTextFont = alertText.GetComponent<CFontRenderer>();
                alertTextFont.mText = "";
                alertTextFont.mFontScale = 0.0032f;
                alertTextFont.mNewlineSpacing = 1.1f;
                alertTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.CENTER_JUSTIFIED;
                alertTextFont.setEnabled(false);

                // Create game screen column itself
                GameObject headerText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 headerPos = mCenterPosForText;
                headerPos.X += 2.4f;
                headerPos.Y += 0.6f;
                headerText.transform.SetPosition(headerPos);
                CFontRenderer headerTextFont = headerText.GetComponent<CFontRenderer>();
                headerTextFont.mText = "Code: ";
                headerTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.RIGHT_JUSTIFIED;

                // Create "Input ..." feedback at the btm
                GameObject btmText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 btmPos = headerPos;
                btmText.transform.SetPosition(btmPos);
                CFontRenderer btmTextFont = btmText.GetComponent<CFontRenderer>();
                btmTextFont.mText = "__\\n__\\n__\\n__";
                btmTextFont.mFontScale = 0.003f;
                btmTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.LEFT_JUSTIFIED;
                btmTextFont.mNewlineSpacing = 1.5f;

                // Create divider
                GameObject dottedLine = CreateDividerText();

                mExtraTexts.Add(alertText);
                mExtraTexts.Add(headerText);
                mExtraTexts.Add(btmText);
                mExtraTexts.Add(dottedLine);
            }
            else
            {
                CFontRenderer btmTextFont = mExtraTexts[2].GetComponent<CFontRenderer>();
                if (numberCode < 0) // Clear the screen back to __
                {
                    btmTextFont.mText = "__\\n__\\n__\\n__";
                }
                else // Add another correct number
                {
                    int indexToInsert = btmTextFont.mText.IndexOf("__");
                    if (indexToInsert >= 0)
                    {
                        string newString = btmTextFont.mText.Remove(indexToInsert, 2);
                        btmTextFont.mText = newString.Insert(indexToInsert, "<#0,1,0,1#>" + numberCode.ToString() + "<#1,1,1,1#>");
                    }
                }
            }
        } // End of if in activated state
    }
    public void MiniGameFourPlayerEvent(string gameString, string seqText, int buttonIndex, 
        GameObject buttonObj, int currHeight, bool isUnlocked)
    {
        if (isUnlocked)
        {
            // If the door is unlocked, then game has ended.
            QueueMessage("Access Granted. Door Opening...", 0.0f, false, true);

            ResetToDefaultState();
        }
        else
        {
            if (currEvent != CONSOLE_EVENT.MINIGAME_FOUR_PLAYERS) // First time trigger into minigame
            {
				GameObject pz3room = GameObject.GetGameObjectByName("TrapSequenceRoom2");
				PuzzleBEnvironmentController pz3ScaryScript = GetScript<PuzzleBEnvironmentController>(pz3room);
				pz3ScaryScript.activated = true;
                savedObject = buttonObj; // Save the last pressed button, so surveillance player can input into that button

                ChangeHeader("DOOR SYSTEM:");
                QueueMessage("Door Access Requested. \\nUnlock Sequence Initiating...", 0.0f, false, true);
                QueueMessage("<#1,1,0,1#>Yellow " + Common.ConvertColorToStringTag(mColorOfText) +
                    "external input required.", 0.6f, true, false);
                currEvent = CONSOLE_EVENT.MINIGAME_FOUR_PLAYERS;

                GameObject sequenceText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 sequencePos = mCenterPosForText;
                sequencePos.X += 2.4f;
                sequencePos.Y += 1.5f;
                sequenceText.transform.SetPosition(sequencePos);
                CFontRenderer sequenceTextFont = sequenceText.GetComponent<CFontRenderer>();
                sequenceTextFont.mText = "Code Sequence: \\n3A - 6C - 5D - 3B";
                sequenceTextFont.mFontScale = 0.0032f;
                sequenceTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.CENTER_JUSTIFIED;
                sequenceTextFont.mNewlineSpacing = 1.1f;

                GameObject currButtonText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 currButtonPos = sequencePos;
                currButtonPos.X += 0.4f;
                currButtonPos.Y -= 0.7f;
                currButtonText.transform.SetPosition(currButtonPos);
                CFontRenderer currButtonTextFont = currButtonText.GetComponent<CFontRenderer>();
                //currButtonTextFont.mText = "Sequence A: ";
                currButtonTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.RIGHT_JUSTIFIED;
                currButtonTextFont.mNewlineSpacing = 1.1f;

                // Create game screen column itself
                GameObject headerText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 headerPos = currButtonPos;
                headerPos.X += 0.05f;
                headerText.transform.SetPosition(headerPos);
                CFontRenderer headerTextFont = headerText.GetComponent<CFontRenderer>();
                headerTextFont.mFontScale = 0.003f;
                headerTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.LEFT_JUSTIFIED;
                headerTextFont.mNewlineSpacing = 1.5f;

                // Create "Input ..." feedback at the btm
                GameObject btmText = ObjectFactory.getInstance().Instantiate(Common.nameForFixedWidthText);
                Vector3 btmPos = sequencePos;
                btmPos.Y = headerPos.Y - 2.0f;
                btmText.transform.SetPosition(btmPos);
                CFontRenderer btmTextFont = btmText.GetComponent<CFontRenderer>();
                btmTextFont.mText = "Touch to enter your input";
                btmTextFont.mJustification = CFontRenderer.FONT_JUSTIFICATION.CENTER_JUSTIFIED;
                btmTextFont.mFontScale = 0.002f;

                // Create divider
                GameObject dottedLine = CreateDividerText();

                mExtraTexts.Add(headerText);
                mExtraTexts.Add(btmText);
                mExtraTexts.Add(dottedLine);
                mExtraTexts.Add(sequenceText);
                mExtraTexts.Add(currButtonText);
            }

            // Update minigame based on what is keyed in
            CFontRenderer textFont = mExtraTexts[0].GetComponent<CFontRenderer>();
            textFont.mText = gameString; // Update the main game column

            textFont = mExtraTexts[4].GetComponent<CFontRenderer>(); // the current button text
            CFontRenderer headerSeqFont = mExtraTexts[3].GetComponent<CFontRenderer>();
            if (buttonIndex == -1)
            {
                textFont.mText = seqText;

                string headerSeqText = "Code Sequence: \\n3A - 6C - 5D - 3B";
                headerSeqFont.mText = headerSeqText; // Reset code sequence coloring when got it wrong
            }
            else
            {
                string headerSeqText = "";
                bool toChangeText = false;
                switch (buttonIndex)
                {
                    case 0: // A is done
                        if (currHeight == 3)
                        {
                            toChangeText = true;
                            headerSeqText = "Code Sequence: \\n<#0,1,0,1#>3A<#1,1,1,1#> - 6C - 5D - 3B";
                        }
                        break;
                    case 2: // C is done
                        if (currHeight == 6)
                        {
                            toChangeText = true;
                            headerSeqText = "Code Sequence: \\n<#0,1,0,1#>3A - 6C<#1,1,1,1#> - 5D - 3B";
                        }
                        break;
                    case 3: // D is done
                        if (currHeight == 5)
                        {
                            toChangeText = true;
                            headerSeqText = "Code Sequence: \\n<#0,1,0,1#>3A - 6C - 5D<#1,1,1,1#> - 3B";
                        }
                        break;
                    case 1: // B is done
                        if (currHeight == 3)
                        {
                            toChangeText = true;
                            headerSeqText = "Code Sequence: \\n<#0,1,0,1#>3A - 6C - 5D - 3B";
                        }
                        break;
                }
                if (toChangeText)
                {
                    headerSeqFont.mText = headerSeqText;
                }

                textFont.mText = "Sequence " + seqText + ": ";
            }

            eventState2 = currHeight;
            eventState = buttonIndex;
        }
    }
}
