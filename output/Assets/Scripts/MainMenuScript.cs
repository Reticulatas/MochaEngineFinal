using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MainMenuScript : MochaScript
{
    const string menuPointerName = "menuPointer";
    GameObject menuPointer; // menuPointer.png
    int menuPointerIndex; // Which option is it pointing at
    Vector3 menuPointerInitPos;
    const float amountYToMove = 1.13f; // For main menu

    int mOptionsPointerIndex = 0; // 0 - fullscreen, 1 - volume, 2 - mute
    static Vector3 mOptionsPointerInitPos = new Vector3(-6.75f, 0.73f, -0.11f);
    const float mOptionsAmountYToMove = 1.1f; // for options

    const string screenImageName = "screenImage"; // for fullscreen main menu images
    GameObject screenImage;
    CDiffuseRenderer screenImageMesh;
    const string fadeScreenName = "fadeScreen";
    GameObject fadeScreen;
    float fadeOpacity = 1.0f;
    int fadeState = 0; // 0 - idle, 1 - fading black, 2 - fading transparent
    float fadeTotalTime = 0.7f;

    enum Items
    {
        I_START = 0,
        I_HOWTOPLAY,
        I_OPTIONS,
        I_CREDITS,
        I_QUIT,

        I_TOTAL
    };
    bool mLoadingFirstTime;

    int mPrevMainMenuState;
    int mMainMenuState;
    int mHowToPlayState = 0; // 0 - first page, 1 - second page

    CSound mSound;

	public void OnStart()
	{
        mSound = gameObject.RequireComponent<CSound>();
        mSound.PlayIndependentEvent("MUSIC_HUB_LOOP.vente", false, 0);

        menuPointer = GameObject.GetGameObjectByName(Common.prefix + menuPointerName);
        Common.SetOpacityToGameObject(0.0f, menuPointer);
        menuPointerInitPos = new Vector3(-2.75f, 1.23f, -0.11f);
        menuPointer.transform.SetPosition(menuPointerInitPos);
        screenImage = GameObject.GetGameObjectByName(Common.prefix + screenImageName);
        screenImageMesh = screenImage.RequireComponent<CDiffuseRenderer>();
        fadeScreen = GameObject.GetGameObjectByName(Common.prefix + fadeScreenName);
        Common.SetOpacityToGameObject(fadeOpacity, fadeScreen); // Start with full black

        Common.GetMenuFullscreenX().setEnabled(false);
        Common.GetMenuMuteX().setEnabled(false);
        Common.GetMenuVolumeFont().setEnabled(false);

        mLoadingFirstTime = true;
        mPrevMainMenuState = -1;
        mMainMenuState = -1;
	}

    void UpdateFadeScreen(float dt)
    {
        switch (fadeState)
        {
            case 0: break;
            case 1: // Fading to black
                fadeOpacity += 1.0f / fadeTotalTime * dt;
                if (fadeOpacity >= 1.0f)
                {
                    fadeOpacity = 1.0f;
                    fadeState = 0; // Stop fading
                }
                Common.SetOpacityToGameObject(fadeOpacity, fadeScreen);
                break;
            case 2: // Fading to transparent
                fadeOpacity -= 1.0f / fadeTotalTime * dt;
                if (fadeOpacity <= 0.0f)
                {
                    fadeOpacity = 0.0f;
                    fadeState = 0; // Stop fading
                }
                Common.SetOpacityToGameObject(fadeOpacity, fadeScreen);
                break;
        }
    }
    
    bool CheckIfPressBack()
    {
        if (Input.GetTriggered(0, "MenuBack") != 0.0f)
        {
            fadeState = 1; // Fade black
            menuPointerIndex = -1; // Back to main menu page
            return true;
        }
        return false;
    }
    void PositionMenuPointerForConfirmation(bool isAtYes)
    {
        menuPointerIndex = isAtYes ? 0 : 1; 

        float xPos = (isAtYes) ? -2.45f : 0.65f;
        menuPointer.transform.SetPosition(new Vector3(xPos, -0.3f, menuPointerInitPos.Z));
    }
    public static void PositionOptionsPointer(GameObject pointerObj, int index, Vector3 offset)
    {
        Vector3 currPointerPos = mOptionsPointerInitPos + offset;
        currPointerPos.Y -= mOptionsAmountYToMove * (float)index;
        pointerObj.transform.SetPosition(currPointerPos);
    }
    public static void SetVolumeText()
    {
        float currVolume = AudioManager.getInstance().GetSoundVolume();
        currVolume *= 100.0f; // [0, 100]
        string volumeText = currVolume.ToString("0") + "%";
        Common.GetMenuVolumeFont().mText = volumeText;
    }
    public static void ExitTheGame()
    {
        MochaCore.CoffeeConstructor.Exit();
        //Engine.getInstance().Exit();
        //Environment.Exit(0);
    }
    // The trigger point when screen is black, and state is changed
    void TriggerOnStartForMainMenu()
    {
        fadeState = 2; // Fade transparent
        mLoadingFirstTime = false;
        Common.SetOpacityToGameObject(0.0f, menuPointer);

        Common.GetMenuFullscreenX().setEnabled(false);
        Common.GetMenuMuteX().setEnabled(false);
        Common.GetMenuVolumeFont().setEnabled(false);

        switch (mMainMenuState)
        {
            case -1:
                // Set pointer back to where it was clicked into
                menuPointerIndex = (mPrevMainMenuState == -1) ? 0 : mPrevMainMenuState;

                screenImageMesh.GetMaterial().SetTexture("Menu_Main.png");
                Common.SetOpacityToGameObject(1.0f, menuPointer);
                Vector3 currPointerPos = menuPointerInitPos;
                currPointerPos.Y -= (float)menuPointerIndex  * amountYToMove;
                menuPointer.transform.SetPosition(currPointerPos);

                fadeTotalTime = 0.25f; // Faster fade within main menu
                break;
            case (int)Items.I_START:
                Common.Reset();
                Map.LoadOnlyMap("Assets/Levels/tutorial_week12.mocha");
                break;
            case (int)Items.I_HOWTOPLAY:
                if (mPrevMainMenuState != mMainMenuState) // from main menu
                {
                    mHowToPlayState = 0;
                }
                else // from page 1
                {
                    mHowToPlayState = (mHowToPlayState == 0) ? 1 : 0;
                }

                string pngFile = (mHowToPlayState == 0) ? "howtoplay_Stealth.png" : "howtoplay_Surveillance.png";
                screenImageMesh.GetMaterial().SetTexture(pngFile);
                break;
            case (int)Items.I_OPTIONS:
                Common.SetOpacityToGameObject(1.0f, menuPointer);
                mOptionsPointerIndex = 0;
                PositionOptionsPointer(menuPointer, mOptionsPointerIndex, Vector3.Zero);

                Common.GetMenuFullscreenX().setEnabled(Engine.getInstance().GetGameWnd().IsFullScreen());
                Common.GetMenuMuteX().setEnabled(AudioManager.getInstance().GetMuteSound());
                Common.GetMenuVolumeFont().setEnabled(true);
                SetVolumeText();

                screenImageMesh.GetMaterial().SetTexture("Menu_options.png");
                break;
            case (int)Items.I_CREDITS:
                screenImageMesh.GetMaterial().SetTexture("menu_credits.png");
                break;
            case (int)Items.I_QUIT:
                Common.SetOpacityToGameObject(1.0f, menuPointer);
                screenImageMesh.GetMaterial().SetTexture("menu_cod.png");
                
                PositionMenuPointerForConfirmation(false); // default at no
                break;
        }
    }
    void UpdateMainMenu(float dt)
    {
        switch (mMainMenuState)
        {
            case -1:
                UpdateMainPageInMainMenu(dt);
                break;
            case (int)Items.I_START: break; // Nothing, already in another level
            case (int)Items.I_HOWTOPLAY:
                // Click A or B to move forward or back
                if (mHowToPlayState == 0) // first page
                {
                    CheckIfPressBack();
                    if (Input.GetTriggered(0, "MenuSelect") != 0.0f || Common.TriggeredRight())
                    {
                        fadeState = 1; // Fade black
                    }
                }
                else // mHowToPlayState == 1 // second page
                {
                    if (Input.GetTriggered(0, "MenuBack") != 0.0f || Common.TriggeredLeft())
                    {
                        fadeState = 1; // Fade black
                    }
                }
                break;
            case (int)Items.I_OPTIONS:
                if (UpdateOptionsMenu(dt, menuPointer, ref mOptionsPointerIndex, Vector3.Zero, mSound))
                {
                    fadeState = 1; // Fade black
                    menuPointerIndex = -1; // Back to main menu page
                }
                break;
            case (int)Items.I_CREDITS:
                CheckIfPressBack();
                break;
            case (int)Items.I_QUIT:
                // Special case for heading back to main menu (no fading)
                if (Input.GetTriggered(0, "MenuBack") != 0.0f)
                {
                    menuPointerIndex = -1; // Back to main menu page
                    mPrevMainMenuState = mMainMenuState;
                    mMainMenuState = menuPointerIndex;
                    mLoadingFirstTime = true;
                    return;
                }
                if (Input.GetTriggered(0, "MenuSelect") != 0.0f)
                {
                    if (menuPointerIndex == 0) // YES, exit game immediately
                    {
                        ExitTheGame();
                    }
                    else // NO
                    {
                        menuPointerIndex = -1; // Back to main menu page
                        mPrevMainMenuState = mMainMenuState;
                        mMainMenuState = menuPointerIndex;
                        mLoadingFirstTime = true;
                    }
                }

                if (Common.TriggeredUp() || Common.TriggeredDown() || Common.TriggeredLeft() || Common.TriggeredRight())
                {
                    if (Common.TriggeredDown() || Common.TriggeredRight())
                    {
                        PositionMenuPointerForConfirmation(false);
                    }
                    else
                    {
                        PositionMenuPointerForConfirmation(true);
                    }
                }
                break;
        } 
    }
    public static bool UpdateOptionsMenu(float dt, GameObject pointerObj, ref int index, Vector3 offset, CSound sound)
    {
        if (Input.GetTriggered(0, "MenuBack") != 0.0f)
            return true;

        if (Input.GetTriggered(0, "MenuSelect") != 0.0f)
        {
            switch (index)
            {
                case 0: // Fullscreen
                    {
                        bool isFullscreen = Engine.getInstance().GetGameWnd().IsFullScreen();
                        Engine.getInstance().GetGameWnd().SetFullScreen(!isFullscreen);
                        Common.GetMenuFullscreenX().setEnabled(!isFullscreen);
                    }
                    break;
                case 2: // Mute
                    {
                        bool isMute = AudioManager.getInstance().GetMuteSound();
                        //AudioManager.getInstance().SetMuteMusic(!isMute);
                        AudioManager.getInstance().SetMuteSound(!AudioManager.getInstance().GetMuteSound());

                        Common.GetMenuMuteX().setEnabled(!isMute);

                        SetVolumeText();
                    }
                    break;
            }
        }
        if (index == 1) // over volume
        {
            if (Common.TriggeredLeft() || Common.TriggeredRight())
            {
                float currVolume = AudioManager.getInstance().GetSoundVolume();
                if (Common.TriggeredLeft())
                {
                    currVolume -= 0.1f;
                    if (currVolume < 0.0f)
                        currVolume = 0.0f;
                }
                else
                {
                    currVolume += 0.1f;
                    if (currVolume > 1.0f)
                        currVolume = 1.0f;

                    if (AudioManager.getInstance().GetMuteSound())
                    {
                        AudioManager.getInstance().SetMuteSound(false);
                        Common.GetMenuMuteX().setEnabled(false);
                    }
                }
                AudioManager.getInstance().SetSoundVolume(currVolume);
                // Might be mute, so get the actual vol again
                SetVolumeText();

                Common.GetMenuMuteX().setEnabled(AudioManager.getInstance().GetMuteSound()); // Show updated mute or not
            }
        }

        if (Common.TriggeredUp() || Common.TriggeredDown())
        {
            sound.PlayIndependentEvent("BUTTON_GREEN.vente", false, 0);

            if (Common.TriggeredDown())
            {
                if (index < 2)
                    ++index;
            }
            else
            {
                if (index != 0)
                    --index;
            }
            PositionOptionsPointer(pointerObj, index, offset);
        }

        return false; // No back button pressed (stay in options menu)
    }
    void UpdateMainPageInMainMenu(float dt)
    {
        // Update the pointer
        if (Common.TriggeredUp() || Common.TriggeredDown())
        {
            Vector3 currPos = menuPointer.transform.GetGlobalPosition();
            if (Common.TriggeredDown())
            {
                if (menuPointerIndex < (int)Items.I_QUIT)
                {
                    currPos.Y += -amountYToMove;
                    ++menuPointerIndex;
                }
            }
            else
            {
                if (menuPointerIndex > 0)
                {
                    currPos.Y += amountYToMove;
                    --menuPointerIndex;
                }
            }
            menuPointer.transform.SetPosition(currPos);
        }
        if (Input.GetTriggered(0, "MenuSelect") != 0.0f)
        {
            fadeState = 1; // Fade black
            if (menuPointerIndex == (int)Items.I_START)
                fadeTotalTime = 1.2f; // Longer fade out for starting the game
            
            if (menuPointerIndex == (int)Items.I_QUIT)
            {
                fadeState = 0; // Don't fade when clicking QUIT (special case)
                mPrevMainMenuState = mMainMenuState;
                mMainMenuState = menuPointerIndex;
                mLoadingFirstTime = true;
            }
        }
    }

	public void OnUpdate()
	{
        float dt = FrameController.DT();

        if (mLoadingFirstTime)
        {
            TriggerOnStartForMainMenu();
        }
        else if (fadeOpacity >= 1.0f) // handle when screen is complete black, then signal for loadingFirstTime
        {
            mPrevMainMenuState = mMainMenuState;
            mMainMenuState = menuPointerIndex;
            mLoadingFirstTime = true;
        }

        if (fadeState == 0) // Only update inputs during main menu stuff when screen is Not fading
        {
            UpdateMainMenu(dt);
        }
        // pauseMenus
        // pauseMenu_MainMenu
        // pauseMenu_Quit
        UpdateFadeScreen(dt);

        // Cheat code, ctrl+shift+ number to load
        if (Input.GetHeld(0, "SnapGrid") != 0 && Input.GetHeld(0, "SnapShift") != 0)
        {
            if (Input.GetTriggered(0, "1") != 0)
            {
                Common.Reset();
                Map.LoadOnlyMap("Assets/Levels/tutorial_week12.mocha");
            }
            else if (Input.GetTriggered(0, "2") != 0)
            {
                Common.Reset();
                Map.LoadOnlyMap("Assets/Levels/JoshElLETSGO16.mocha");
            }
            else if (Input.GetTriggered(0, "3") != 0)
            {
                Common.Reset();
                Map.LoadOnlyMap("Assets/Levels/StickyCam_RecSave11.mocha");
            }
        }
	}
}
