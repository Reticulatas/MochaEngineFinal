using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TutorialGlassSound : TriggerAreaBaseClass
{
    bool mIsSoundPlaying = false;
    Vector3 mDirectionToRun;
    float mDistanceRan = 0.0f;
    public float mMaxDistanceToRun { get; set; }

    CSound mSound;
    uint glassCrackID; // GLASS_CRACK
    const string glassCrackParam = "DAMAGE"; // (param: DAMAGE) [1,2,3], 1 - first glass crack, 2- 2nd, 3 is the glass shattering (doesn't stop by itself, waits for param change)

    bool isTriggered = false;
    int glassCrackLevel = 0;

    public override void OnStart()
    {
        base.OnStart();

        mSound = gameObject.RequireComponent<CSound>();
        glassCrackID = mSound.GetUniqueEvent("GLASS_CRACK.vente", 1);
        mDirectionToRun = gameObject.transform.GetForwardVector();

        isactive = false;
    }

    public void TriggerStartGlassSound()
    {
        isTriggered = true;
        mSound.PlayUniqueEvent(glassCrackID, false);
    }

    public void OnUpdate()
    {
        if (isTriggered && glassCrackLevel < 3)
        {
            Vector3 glassToPlayer = (Vector3)Common.GetStealthPlayer().transform.position - (Vector3)gameObject.transform.position;
            float dist = Vector3.Dot(glassToPlayer, mDirectionToRun); // distance player is from trigger
            mDistanceRan = Math.Max(dist, mDistanceRan); // Always take the max
            float paramValue = mDistanceRan / mMaxDistanceToRun * 3; // [0, 3]

            if ((int)Math.Floor(paramValue) > glassCrackLevel)
            {
                ++glassCrackLevel;
                mSound.PlayUniqueEvent(glassCrackID, false);
                mSound.SetUniqueEventParam(glassCrackID, glassCrackParam, (float)glassCrackLevel);
            }
        }
    }
}
 