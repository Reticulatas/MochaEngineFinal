using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TriggerAreaTutorialInteract : TriggerAreaBaseClass
{
    float mDefaultDelayTime = 0.5f;
    public float mInitialDelayTime { get { return mDefaultDelayTime; }
        set { mDefaultDelayTime = value; } }
    float mDefaultEndDelayTime = 2.0f; // end delay time (after door opens)
    public float mEndDelayTime { get { return mDefaultEndDelayTime; } 
        set { mDefaultEndDelayTime = value; } }

    float mDefaultFadeTime = 0.7f;
    public float mFadeTime { get { return mDefaultFadeTime; }
        set { mDefaultFadeTime = value; } }

    public String mTutorialIconName { get; set; }
    GameObject tutorialIconObj;

    float mTimer = 0.0f;
    float mCurrOpacity = 0.0f;

    int isPromptShown = 0; // 0 - nothing, 1 - tweening on, 2 - tweening off, 3 - fully on

    public String doorName { get; set; }
    DoorMain doorScript;
    bool isDoorUnlocked = false;

    public override void OnStart()
    {
        base.OnStart();
        
        tutorialIconObj = GameObject.GetGameObjectByName(mTutorialIconName);
        isactive = false;

        mTimer = mInitialDelayTime;
        Common.SetOpacityToAllUI(0.0f, tutorialIconObj); // Init all to 0 opacity

        if (doorName != null)
        {
            GameObject obj = GameObject.GetGameObjectByName(doorName);
            if (obj != null)
            {
                doorScript = GetScript<DoorMain>(obj);
            }
        }
        if (doorScript == null)
        {
            Logger.Log("TutorialInteract unable to find door script!");
        }
    }

    public void OnUpdate()
    {
        if (!isactive)
        {

        }
        else
        {
            float dt = FrameController.DT();

            if (isDoorUnlocked)
            {
                if (mTimer > 0.0f)
                {
                    mTimer -= dt;
                    if (mTimer <= 0.0f)
                    {
                        isPromptShown = 2; // Start tweening off
                    }
                }
            }
            else
            {
                if (doorScript != null && doorScript.isLocked == false)
                {
                    isDoorUnlocked = true;
                    mTimer = mEndDelayTime;
                }
            }

            switch (isPromptShown)
            {
                case 0:
                    mTimer -= dt;
                    if (mTimer <= 0.0f)
                    {
                        isPromptShown = 1; // Start tweening on
                        mTimer = mEndDelayTime;
                    }
                    break;
                case 1: // Tweening up opacity
                    mCurrOpacity += 1.0f / mFadeTime * dt;
                    if (mCurrOpacity >= 1.0f)
                    {
                        isPromptShown = 3;
                    }
                    Common.SetOpacityToAllUI(mCurrOpacity, tutorialIconObj);
                    break;
                case 2: // Tweening down to 0 opacity
                    mCurrOpacity -= 1.0f / mFadeTime * dt;
                    if (mCurrOpacity <= 0.0f)
                    {
                        isPromptShown = 0;
                        Common.SetOpacityToAllUI(mCurrOpacity, tutorialIconObj);
                        isactive = false; // When 0 opacity, destroy this object, since it will never show again
                        gameObject.Destroy();
                        return;
                    }
                    Common.SetOpacityToAllUI(mCurrOpacity, tutorialIconObj);
                    break;
            }
        }
    }

    public override void OnAreaEnter()
    {
        if (!isactive)
        {
            isactive = true;
            mTimer = mInitialDelayTime;
        }
    }
    public override void OnArea()
    {

    }
}
 