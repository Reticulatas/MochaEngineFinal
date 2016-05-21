using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class SplashScreen : MochaScript
{
    const string screenImageName = "screenImage"; // for fullscreen main menu images
    GameObject screenImage;
    CMeshRenderer screenImageMesh;
    const string fadeScreenName = "fadeScreen";
    GameObject fadeScreen;
    float fadeOpacity = 1.0f;
    int fadeState = 0; // 0 - idle, 1 - fading black, 2 - fading transparent
    float fadeTotalTime = 0.7f;

    enum LOADING_STATE
    {
        MENU_DIGIPEN = 0,
        MENU_TEAMLOGO,
        MENU_INTRODEVICES,
        MENU_MAIN,

        NUM_LOADING_STATEs
    };
    int mLoadingState; // which state of splash screen
    bool mLoadingFirstTime;
    float mLoadingTimer;

	public void OnStart()
	{
        screenImage = GameObject.GetGameObjectByName(Common.prefix + screenImageName);
        screenImageMesh = screenImage.RequireComponent<CMeshRenderer>();
        fadeScreen = GameObject.GetGameObjectByName(Common.prefix + fadeScreenName);
        Common.SetOpacityToGameObject(fadeOpacity, fadeScreen); // Start with full black

        mLoadingState = (int)LOADING_STATE.MENU_DIGIPEN;
        mLoadingTimer = 0.0f;
        mLoadingFirstTime = true;
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
    void UpdateSplashScreen(float dt)
    {
        if (mLoadingState == (int)LOADING_STATE.MENU_DIGIPEN ||
            mLoadingState == (int)LOADING_STATE.MENU_TEAMLOGO)
        {
            // Skip splash screen
            if (Input.GetTriggered(0, "exit") != 0.0f ||
                Input.GetTriggered(0, "MenuSelect") != 0.0f ||
                Input.GetTriggered(0, "Space") != 0.0f ||
                Input.GetTriggered(0, "Xbox_A") != 0.0f ||
                Input.GetTriggered(0, "Xbox_B") != 0.0f ||
                Input.GetTriggered(0, "Xbox_X") != 0.0f ||
                Input.GetTriggered(0, "Xbox_Y") != 0.0f || 
                Input.GetTriggered(0, "MouseRClick") != 0.0f ||
                Input.GetTriggered(0, "MouseLClick") != 0.0f)
            {
                mLoadingTimer = 10.0f; // Fast forward to start fading out
                mLoadingState = (int)LOADING_STATE.MENU_TEAMLOGO; // Skip to after team logo
            }
        }

        mLoadingTimer += dt;
        switch (mLoadingState)
        {
            case (int)LOADING_STATE.MENU_DIGIPEN:
                if (mLoadingFirstTime)
                {
                    fadeState = 2; // Fade transparent
                    screenImageMesh.GetMaterial().SetTexture("intro_digipen.png");
                    mLoadingFirstTime = false;
                    mLoadingTimer = 0.0f;
                }
                else if (mLoadingTimer >= 2.0f)
                {
                    if (fadeOpacity >= 1.0f)
                    {
                        ++mLoadingState; // Move on to Team Logo screen
                        mLoadingFirstTime = true;
                    }
                    else if (fadeState != 1)
                        fadeState = 1; // Fade black
                }
                break;
            case (int)LOADING_STATE.MENU_TEAMLOGO:
                if (mLoadingFirstTime)
                {
                    fadeState = 2; // Fade transparent
                    screenImageMesh.GetMaterial().SetTexture("SubRed1080.png");
                    mLoadingFirstTime = false;
                    mLoadingTimer = 0.0f;
                }
                else if (mLoadingTimer >= 2.0f)
                {
                    if (fadeOpacity >= 1.0f)
                    {
                        ++mLoadingState; // Move on to Intro devices
                        mLoadingFirstTime = true;
                    }
                    else if (fadeState != 1)
                        fadeState = 1; // Fade black
                }
                break;
            case (int)LOADING_STATE.MENU_INTRODEVICES:
                if (mLoadingFirstTime)
                {
                    fadeState = 2; // Fade transparent
                    screenImageMesh.GetMaterial().SetTexture("intro_devices.png");
                    mLoadingFirstTime = false;
                    mLoadingTimer = 0.0f;
                }
                else if (mLoadingTimer >= 2.5f)
                {
                    if (fadeOpacity >= 1.0f)
                    {
                        ++mLoadingState; // Move on to Main Menu
                        mLoadingFirstTime = true;
                        Map.LoadOnlyMap("Assets/Levels/mainMenu1.mocha");
                    }
                    else if (fadeState != 1)
                        fadeState = 1; // Fade black
                }
                break;
        }
    }
    
	public void OnUpdate()
	{
        float dt = FrameController.DT();
        UpdateSplashScreen(dt);
        UpdateFadeScreen(dt);
	}
}
