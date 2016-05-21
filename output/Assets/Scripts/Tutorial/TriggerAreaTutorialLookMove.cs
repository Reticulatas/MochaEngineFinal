using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TriggerAreaTutorialLookMove : TriggerAreaBaseClass
{
    float mDefaultDelayTime = 1.0f;
    public float mInitialDelayTime { get { return mDefaultDelayTime; }
        set { mDefaultDelayTime = value; } }
    float mDefaultFadeTime = 0.7f;
    public float mFadeTime { get { return mDefaultFadeTime; }
        set { mDefaultFadeTime = value; } }

    public String mTutorialIconName { get; set; }
    GameObject tutorialIconObj;

    float mTimer = 0.0f;
    float mCurrOpacity = 0.0f;

    int isPromptShown = 0; // 0 - nothing, 1 - tweening on, 2 - tweening off, 3 - fully on
   
    public override void OnStart()
    {
        base.OnStart();

        tutorialIconObj = GameObject.GetGameObjectByName(mTutorialIconName);

        mTimer = mInitialDelayTime;
        Common.SetOpacityToAllUI(0.0f, tutorialIconObj); // Init all to 0 opacity
    }

    public void OnUpdate()
    {
        if (!isactive)
        {
            gameObject.Destroy();
            return;
        }
        else
        {
            float dt = FrameController.DT();
            switch (isPromptShown)
            {
                case 0:
                    mTimer -= dt;
                    if (mTimer <= 0.0f)
                    {
                        // Start tweening on
                        isPromptShown = 1;
                        mTimer = mFadeTime;
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
                        isactive = false; // When 0 opacity, destroy this object, since it will never show again
                    }
                    Common.SetOpacityToAllUI(mCurrOpacity, tutorialIconObj);
                    break;
            }
        }
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
            isPromptShown = 2; // Immediately start fading out
        }
    }
    public override void OnArea()
    {

    }
}
 