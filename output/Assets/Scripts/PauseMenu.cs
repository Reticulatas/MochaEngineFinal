using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PauseMenu : MochaScript
{
    const string menuPointerName = "pauseMenuPointer";
    GameObject menuPointer; // menuPointer.png
    int menuPointerIndex = -1; // Which option is it pointing at
    Vector3 menuPointerInitPos;
    const float amountYToMove = 1.13f; // For main menu

    CDiffuseRenderer screenImageMesh;
    const string fadeScreenName = "pauseFadeScreen";
    GameObject fadeScreen;
    float fadeOpacity = 0.0f;
    int fadeState = 0; // 0 - idle, 1 - fading black, 2 - fading transparent
    float fadeTotalTime = 0.2f;

    const string optionsFadeScreenName = "fadeScreen";
    GameObject optionsFadeScreen;

    Vector3 optionsPos;
    CDiffuseRenderer pauseOptions;
    CDiffuseRenderer pauseMenuPointer;
    int mOptionsPointerIndex = 0; // 0 - fullscreen, 1 - volume, 2 - mute
    CDiffuseRenderer mFullscreenX;
    CDiffuseRenderer mMuteX;
    CFontRenderer mVolumeFont;
    Vector3 mCamSavedDir;

    float screenFadeOpacity = 0.0f;
    int screenFadeState = 0; // 0 - idle, 1 - fading black, 2 - fading transparent

    Vector2 mCurrPointerPos;
    List<Vector2> mPointerPosition; // for each of the options
    List<Vector2> mPointerPosForMainMenuConfirmation;
    List<Vector2> mPointerPosForQuitConfirmation;

    CSound mSound;

    enum PAUSE_STATE
    {
        MAIN = -1,
        RESUME,
        HOWTOPLAY,
        OPTIONS,
        MAIN_MENU,
        QUIT,

        NUM_PAUSE_STATEs
    };
    bool mLoadingFirstTime;

    int mPrevMainMenuState;
    int mMainMenuState;
    int mHowToPlayState = 0; // 0 - first page, 1 - second page
    int mToMainMenuState = 0; // 0 - into main menu confirmtion, 1 - Yes back to main menu
    bool mIsConfirmationYes = false;

	public void OnStart()
	{
        mSound = gameObject.RequireComponent<CSound>();

        // scale should be 0.221f, 0124f
        menuPointer = GameObject.GetGameObjectByName(Common.prefix + menuPointerName);
        menuPointer.transform.SetParent(null);

        Common.SetOpacityToGameObject(0.0f, menuPointer);
        menuPointerInitPos = new Vector3(-2.75f, 1.23f, -0.05f);
        menuPointer.transform.SetPosition(menuPointerInitPos);

        screenImageMesh = gameObject.RequireComponent<CDiffuseRenderer>();
        screenImageMesh.GetMaterial().SetColor(new Color(1.0f, 1.0f, 1.0f, 0.0f));

        optionsPos = new Vector3(0.0f, -90.0f, 0.0f); // Position to look at options menu
        pauseOptions = Common.GetPauseOptionsScreen().RequireComponent<CDiffuseRenderer>();
        Common.GetPauseOptionsScreen().transform.SetPosition(optionsPos);
        pauseOptions.setEnabled(false);
        GameObject pausePointerObj = Common.GetChildByName(pauseOptions.gameObject, "menuPointer");
        pauseMenuPointer = pausePointerObj.RequireComponent<CDiffuseRenderer>();
        pauseMenuPointer.setEnabled(false);
        Common.GetMenuFullscreenX().setEnabled(false); // Init all options stuff to invisible
        Common.GetMenuMuteX().setEnabled(false);
        Common.GetMenuVolumeFont().setEnabled(false);

        fadeScreen = GameObject.GetGameObjectByName(Common.prefix + fadeScreenName);
        Common.SetOpacityToGameObject(fadeOpacity, fadeScreen); // Start with full black

        optionsFadeScreen = GameObject.GetGameObjectByName(Common.prefix + optionsFadeScreenName);
        Common.SetOpacityToGameObject(fadeOpacity, optionsFadeScreen);

        mLoadingFirstTime = true;
        mPrevMainMenuState = -1;
        mMainMenuState = -1;

        mPointerPosition = new List<Vector2>();
        // 0.227, 0.115
        mPointerPosition.Add(new Vector2(-0.0753f, 0.028f));
        mPointerPosition.Add(new Vector2(-0.0753f, 0.0233f));
        mPointerPosition.Add(new Vector2(-0.0753f, 0.0186f));
        mPointerPosition.Add(new Vector2(-0.0753f, 0.0139f));
        mPointerPosition.Add(new Vector2(-0.0753f, -0.0051f)); // wrong

        mPointerPosForMainMenuConfirmation = new List<Vector2>();
        mPointerPosForMainMenuConfirmation.Add(new Vector2(-0.0693f, 0.0043f));
        mPointerPosForMainMenuConfirmation.Add(new Vector2(-0.0693f, -0.0004f));

        mPointerPosForQuitConfirmation = new List<Vector2>();
        mPointerPosForQuitConfirmation.Add(new Vector2(-0.0693f, -0.0145f));
        mPointerPosForQuitConfirmation.Add(new Vector2(-0.0693f, -0.0193f));

        mCurrPointerPos = mPointerPosition[0];

        // When going to How To Play / Options, change camera to Ortho, at (0,0,0), with 0 rotation
        // Bring up the ortho image
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
                Common.SetOpacityToGameObject(fadeOpacity, optionsFadeScreen);
                break;
            case 2: // Fading to transparent
                fadeOpacity -= 1.0f / fadeTotalTime * dt;
                if (fadeOpacity <= 0.0f)
                {
                    fadeOpacity = 0.0f;
                    fadeState = 0; // Stop fading
                }
                Common.SetOpacityToGameObject(fadeOpacity, fadeScreen);
                Common.SetOpacityToGameObject(fadeOpacity, optionsFadeScreen);
                break;
        }
    }
    void UpdateScreenFadeScreen(float dt)
    {
        switch (screenFadeState)
        {
            case 0: break;
            case 1: // Fading to black
                screenFadeOpacity += 1.0f / fadeTotalTime * dt;
                if (screenFadeOpacity >= 1.0f)
                {
                    screenFadeOpacity = 1.0f;
                    screenFadeState = 0; // Stop fading
                }
                Color clr = screenImageMesh.GetMaterial().GetColor();
                clr.a = screenFadeOpacity;
                screenImageMesh.GetMaterial().SetColor(clr);

                Common.SetOpacityToGameObject(screenFadeOpacity, menuPointer);
                break;
            case 2: // Fading to transparent
                screenFadeOpacity -= 1.0f / fadeTotalTime * dt;
                if (screenFadeOpacity <= 0.0f)
                {
                    screenFadeOpacity = 0.0f;
                    screenFadeState = 0; // Stop fading
                }
                Color color = screenImageMesh.GetMaterial().GetColor();
                color.a = screenFadeOpacity;
                screenImageMesh.GetMaterial().SetColor(color);

                Common.SetOpacityToGameObject(screenFadeOpacity, menuPointer);
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
    void SetPointerPos(Vector2 xyPos)
    {
        mCurrPointerPos = xyPos;
    }
    void UpdatePointerPos()
    {
        // Right vector is pointing in oppo direction, and z dir too
        Vector3 posToSet = -mCurrPointerPos.X * (Vector3)gameObject.transform.GetRightVector() +
                mCurrPointerPos.Y * (Vector3)gameObject.transform.GetUpVector() +
                0.001f * (Vector3)gameObject.transform.GetForwardVector();
        posToSet += gameObject.transform.GetGlobalPosition();
        menuPointer.transform.SetPosition(posToSet);
        menuPointer.transform.LookAt(Common.GetStealthPlayerCamera().transform.GetGlobalPosition(),
            Common.GetStealthPlayerCamera().transform.GetUpVector());

        Vector3 forward = menuPointer.transform.GetForwardVector();
        menuPointer.transform.RotateAround(menuPointer.transform.GetForwardVector(), -90.0f);//, CTransform.TransformSpace.LOCAL);
    }
    void PositionMenuPointerMainMenuForConfirmation()
    {
        SetPointerPos(mPointerPosForMainMenuConfirmation[mIsConfirmationYes ? 0 : 1]);
    }
    void PositionMenuPointerQuitForConfirmation()
    {
        SetPointerPos(mPointerPosForQuitConfirmation[mIsConfirmationYes ? 0 : 1]);
    }
    void ExitTheGame()
    {
        MainMenuScript.ExitTheGame();
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
            case (int)PAUSE_STATE.MAIN:
                if (mPrevMainMenuState == (int)PAUSE_STATE.OPTIONS)
                {
                    Common.isOptions = false;
                    Common.GetStealthPlayerCamera().RequireComponent<CCamera>().mActiveProj = CCamera.CamProj.PERSP;
                    Common.GetStealthPlayerCamera().transform.Rotate(mCamSavedDir); // Set back to saved dir

                    pauseOptions.setEnabled(false);
                    pauseMenuPointer.setEnabled(false);
                }

                // Set pointer back to where it was clicked into
                menuPointerIndex = (mPrevMainMenuState == -1) ? 0 : mPrevMainMenuState;
                mPrevMainMenuState = 0;

                screenImageMesh.GetMaterial().SetTexture("pauseMenu_new.png");
                Common.SetOpacityToGameObject(1.0f, menuPointer);

                SetPointerPos(mPointerPosition[menuPointerIndex]);
                break;
            case (int)PAUSE_STATE.RESUME:
                // Unpause the game, make the screens disappear
                Common.isPaused = false;
                break;
            case (int)PAUSE_STATE.HOWTOPLAY:
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
            case (int)PAUSE_STATE.OPTIONS:
                // Fly camera to 0,0,0, to look at options menu
                Common.isOptions = true; // This prevents first person camera from updating
                mCamSavedDir = Common.GetStealthPlayerCamera().transform.GetRotation();
                Common.GetStealthPlayerCamera().transform.Rotate(Vector3.Zero);
                Vector3 camPos = optionsPos;
                camPos.Z -= 1.0f;
                Common.GetStealthPlayerCamera().transform.SetPosition(camPos);
                Common.GetStealthPlayerCamera().RequireComponent<CCamera>().mActiveProj = CCamera.CamProj.ORTHO;

                pauseOptions.setEnabled(true);
                pauseMenuPointer.setEnabled(true);

                mOptionsPointerIndex = 0;
                MainMenuScript.PositionOptionsPointer(pauseMenuPointer.gameObject, mOptionsPointerIndex, optionsPos);

                Common.GetMenuFullscreenX().setEnabled(Engine.getInstance().GetGameWnd().IsFullScreen());
                Common.GetMenuMuteX().setEnabled(AudioManager.getInstance().GetMuteSound());
                Common.GetMenuVolumeFont().setEnabled(true);
                MainMenuScript.SetVolumeText();

                screenImageMesh.GetMaterial().SetTexture("Menu_options.png");
                break;
            case (int)PAUSE_STATE.MAIN_MENU:
                if (mToMainMenuState == 0)
                {
                    Common.SetOpacityToGameObject(1.0f, menuPointer);
                    screenImageMesh.GetMaterial().SetTexture("pauseMenu_MainMenu_new.png");

                    // Position the pointer
                    mIsConfirmationYes = false;
                    PositionMenuPointerMainMenuForConfirmation(); // default at no
                }
                else // Faded to black after YES confirmation, then load main menu level
                {
                    Common.Reset();
                    Map.LoadOnlyMap("Assets/Levels/mainMenu1.mocha");
                }
                break;
            case (int)PAUSE_STATE.QUIT:
                Common.SetOpacityToGameObject(1.0f, menuPointer);
                screenImageMesh.GetMaterial().SetTexture("pauseMenu_Quit_new.png");
                
                // Position the pointer
                mIsConfirmationYes = false;
                PositionMenuPointerQuitForConfirmation(); // default at no
                break;
        }
    }
    void UpdatePauseMenu(float dt)
    {
        switch (mMainMenuState)
        {
            case (int)PAUSE_STATE.MAIN:
                UpdateMainPageInPauseMenu(dt);
                break;
            case (int)PAUSE_STATE.RESUME: break; // Nothing, already not in pause menu
            case (int)PAUSE_STATE.HOWTOPLAY:
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
            case (int)PAUSE_STATE.OPTIONS:
                if (MainMenuScript.UpdateOptionsMenu(dt, pauseMenuPointer.gameObject, 
                    ref mOptionsPointerIndex, optionsPos, mSound))
                {
                    fadeState = 1; // Fade black
                    menuPointerIndex = -1; // Back to main menu page
                }
                break;
            case (int)PAUSE_STATE.MAIN_MENU:
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
                    if (mIsConfirmationYes) // YES, exit game immediately
                    {
                        mToMainMenuState = 1; // Signal that this is to go back to main menu
                        fadeState = 1; // Fade to black
                    }
                    else // NO
                    {
                        menuPointerIndex = -1; // Back to main menu page
                        mPrevMainMenuState = mMainMenuState;
                        mMainMenuState = menuPointerIndex;
                        mLoadingFirstTime = true;
                    }
                }

                if (Common.TriggeredUp() || Common.TriggeredDown())
                {
                    mIsConfirmationYes = !Common.TriggeredDown();
                    PositionMenuPointerMainMenuForConfirmation();
                }
                break;
            case (int)PAUSE_STATE.QUIT:
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
                    if (mIsConfirmationYes) // YES, exit game immediately
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

                if (Common.TriggeredUp() || Common.TriggeredDown())
                {
                    mIsConfirmationYes = !Common.TriggeredDown();
                    PositionMenuPointerQuitForConfirmation();
                }
                break;
        } 
    }
    void UpdateMainPageInPauseMenu(float dt)
    {
        if (Input.GetTriggered(0, "MenuBack") != 0.0f ||
            Input.GetTriggered(0, "exit") != 0.0f) // If escape, go back to game
        {
            screenFadeState = 2; // Fade black
            mMainMenuState = (int)PAUSE_STATE.RESUME;
            menuPointerIndex = (int)PAUSE_STATE.RESUME;
            return;
        }
        // Update the pointer
        if (Common.TriggeredUp() || Common.TriggeredDown())
        {
            if (Common.TriggeredDown())
            {
                if (menuPointerIndex < (int)PAUSE_STATE.QUIT)
                {
                    ++menuPointerIndex;
                }
            }
            else
            {
                if (menuPointerIndex > 0)
                {
                    --menuPointerIndex;
                }
            }
            Vector2 currPos = mPointerPosition[menuPointerIndex];
            SetPointerPos(currPos);
        }
        if (Input.GetTriggered(0, "MenuSelect") != 0.0f)
        {
            if (menuPointerIndex == (int)PAUSE_STATE.RESUME)
            {
                screenFadeState = 2; // Fade out pause menu
                mMainMenuState = (int)PAUSE_STATE.RESUME;
            }
            if (menuPointerIndex == (int)PAUSE_STATE.HOWTOPLAY || 
                menuPointerIndex == (int)PAUSE_STATE.OPTIONS)
            {
                fadeState = 1; // Fade black
            }
            else if (menuPointerIndex == (int)PAUSE_STATE.MAIN_MENU ||
                menuPointerIndex == (int)PAUSE_STATE.QUIT)
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

        if (Common.isPaused)
        {
            if (mLoadingFirstTime)
            {
                TriggerOnStartForMainMenu();
            }
            else
            {
                if ((mPrevMainMenuState == -1 && mMainMenuState == -1 && screenFadeOpacity >= 1.0f) || // When game to pause
                    (mMainMenuState == (int)PAUSE_STATE.RESUME && screenFadeOpacity <= 0.0f) || // When pause to game
                    (fadeOpacity >= 1.0f) ) // handle when screen is complete black, then signal for loadingFirstTime
                {
                    mPrevMainMenuState = mMainMenuState;
                    mMainMenuState = menuPointerIndex;
                    mLoadingFirstTime = true;
                }
            }

            if (fadeState == 0 && screenFadeState == 0) // Only update inputs during main menu stuff when screen is Not fading
            {
                UpdatePauseMenu(dt);
            }

            // Position the screen to player's face every frame
            Vector3 cameraPos = Common.GetStealthPlayerCamera().transform.GetGlobalPosition();
            CCamera playerCameraComponent = Common.GetStealthPlayerCamera().RequireComponent<CCamera>();
            float z = playerCameraComponent.GetNearZ() * 1.5f; // Shift it infront of the camera
            cameraPos += (Vector3)Common.GetStealthPlayerCamera().transform.GetForwardVector() * z;
            gameObject.transform.SetPosition(cameraPos);

            gameObject.transform.LookAt(Common.GetStealthPlayerCamera().transform.GetGlobalPosition(),
                Common.GetStealthPlayerCamera().transform.GetUpVector());

            UpdatePointerPos();
        }
        else // Not paused yet, so in normal gameplay
        {
            if (Input.GetTriggered(0, "exit") != 0.0f)
            {
                screenFadeState = 1; // Fade black
                Common.isPaused = true;
                mLoadingFirstTime = false;
                mPrevMainMenuState = mMainMenuState = menuPointerIndex = -1; // Load to start of pause menu
            }
        }
        UpdateFadeScreen(dt);
        UpdateScreenFadeScreen(dt);
	}
}
