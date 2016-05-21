using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MazeSoundTriggerArea : TriggerAreaBaseClass
{
    CSound mSound;
    uint mMazeAmbientID; // AMB_MAZE, 0-65 (param: TIME), 60-65 is when it stays up. (auto loop)
    const string mMazeAmbientParam = "TIME";

    float mDMaxTimeInMaze = 18.0f;
    public float mMaxTimeInMaze { get { return mDMaxTimeInMaze; } 
        set { mDMaxTimeInMaze = value; } }
    float mDTimeToRelease = 5.0f;
    public float mTimeToRelease { get { return mDTimeToRelease; }
        set { mDTimeToRelease = value; } }

    int mState = 0; // 0 - idle, 1 - amp up, 2 - release quickly
    float mTimeElapsed = 0.0f;

    bool hasCreepSoundPlayed = false;

    public override void OnStart()
    {
        base.OnStart();

        mSound = gameObject.RequireComponent<CSound>();
        mMazeAmbientID = mSound.GetUniqueEvent("AMB_MAZE.vente", 0);
    }

    public void OnUpdate()
    {
        const float maxParamValue = 65.0f;

        switch (mState)
        {
            case 0: break;
            case 1:
                {
                    mTimeElapsed += FrameController.DT();

                    float currParamValue = mTimeElapsed / mMaxTimeInMaze * maxParamValue;
                    if (currParamValue > maxParamValue) // Cap to 65 (max)
                    {
                        mState = 0;
                        currParamValue = maxParamValue;
                    }
                    mSound.SetUniqueEventParam(mMazeAmbientID, mMazeAmbientParam, currParamValue);

                    // Play once the creepy sound when around the middle of the time elapsed
                    if (!hasCreepSoundPlayed && mTimeElapsed >= mMaxTimeInMaze*0.4f)
                    {
                        hasCreepSoundPlayed = true;
                        mSound.PlayIndependentEvent("PERCUSSIVE_A.vente", false, 0);
                    }
                }
                break;
            case 2:
                {
                    mTimeElapsed += FrameController.DT();

                    float currParamValue = mTimeElapsed / mTimeToRelease * maxParamValue;
                    currParamValue = maxParamValue - currParamValue; // Count down from 65 to 0
                    if (currParamValue < 0.0f)
                    {
                        mState = 0;
                        currParamValue = 0.0f;
                        mSound.StopUniqueEvent(mMazeAmbientID, false);
                    }
                    else
                    {
                        mSound.SetUniqueEventParam(mMazeAmbientID, mMazeAmbientParam, currParamValue);
                    }
                }
                break;
        }
    }
    public void TriggerReleaseAmbient()
    {
        mState = 2;
        mTimeElapsed = 0.0f;
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
            mState = 1;
            mSound.PlayUniqueEvent(mMazeAmbientID, false);

            isactive = false;
        }
    }
    public override void OnArea()
    {

    }
}
