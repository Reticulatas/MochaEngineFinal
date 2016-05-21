using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TriggerAreaTutorialSprint : TriggerAreaBaseClass
{
    float mDefaultDelayTime = 0.5f;
    public float mInitialDelayTime { get { return mDefaultDelayTime; }
        set { mDefaultDelayTime = value; } }
    float mDefaultEndDelayTime = 4.0f; // end delay time (after player started sprinting)
    public float mEndDelayTime { get { return mDefaultEndDelayTime; }
        set { mDefaultEndDelayTime = value; } }
    float mDefaultFadeTime = 0.3f;
    public float mFadeTime { get { return mDefaultFadeTime; }
        set { mDefaultFadeTime = value; } }

    public String mTutorialIconName { get; set; }
    GameObject tutorialIconObj;

    float mTimer = 0.0f;
    float mCurrOpacity = 0.0f;

    int isPromptShown = 0; // 0 - nothing, 1 - tweening on, 2 - tweening off, 3 - fully on
    FirstPersonPlayer playerScript;
    bool hasPlayerSprinted = false;

    bool mIsSoundPlaying = false;
    CSound mSound; // MUSIC_ESCAPE, (param: DISTANCE_TRAVELED) start playing when Sprint Prompt comes up, [0, 100, >100], 0 is bass, 0-100 is ramping up, > 100 it releases
    const string mEscapeParam = "DISTANCE_TRAVELED";
    const float mMaxEscapeParamValue = 100.0f;
    uint mEscapeEventID;
    Vector3 mDirectionToRun;
    float mDistanceRan = 0.0f;
    public float mMaxDistanceToRun { get; set; }

    public override void OnStart()
    {
        base.OnStart();

        mSound = gameObject.RequireComponent<CSound>();
        mEscapeEventID = mSound.GetUniqueEvent("MUSIC_ESCAPE.vente", 0);
        mDirectionToRun = gameObject.transform.GetForwardVector();

        tutorialIconObj = GameObject.GetGameObjectByName(mTutorialIconName);
        playerScript = GetScript<FirstPersonPlayer>(Common.GetStealthPlayer());

        isactive = false;

        Common.SetOpacityToAllUI(0.0f, tutorialIconObj); // Init all to 0 opacity
    }

    public void OnUpdate()
    {
        if (mIsSoundPlaying)
        {
            Vector3 triggerToPlayer = (Vector3)Common.GetStealthPlayer().transform.position - (Vector3)gameObject.transform.position;
            float dist = Vector3.Dot(triggerToPlayer, mDirectionToRun); // distance player is from trigger
            mDistanceRan = Math.Max(dist, mDistanceRan); // Always take the max
            float paramValue = mDistanceRan / mMaxDistanceToRun * mMaxEscapeParamValue; // [0, maxParam]
            mSound.SetUniqueEventParam(mEscapeEventID, mEscapeParam, paramValue);
        }

        if (!isactive)
        {

        }
        else
        {
            float dt = FrameController.DT();

            if (hasPlayerSprinted)
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
                if (playerScript != null && playerScript.sprintLocked == true)
                {
                    hasPlayerSprinted = true;
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
                        return;
                    }
                    Common.SetOpacityToAllUI(mCurrOpacity, tutorialIconObj);
                    break;
            }
        }
    }

    public override void OnAreaEnter()
    {
        if (!isactive && !hasPlayerSprinted)
        {
            isactive = true;
            mTimer = mInitialDelayTime;

            // Play the escape effect once Sprint Prompt is fully shown
            mSound.PlayUniqueEvent(mEscapeEventID, false);
            mIsSoundPlaying = true;
        }
    }
    public override void OnArea()
    {

    }
}
 