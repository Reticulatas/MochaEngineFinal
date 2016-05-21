using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class GeneralAmbientTriggerArea : TriggerAreaBaseClass
{
    // AMB_SCIENCE, AMB_HUB, AMB_MONSTER_ROOM, MUSIC_HUB_LOOP
    public String mVenteFilename1 { get; set; }
    public String mVenteFilename2 { get; set; }
    public float mTimeToPlay1 { get; set; }
    public float mTimeToPlay2 { get; set; }
    List<uint> mEventID = new List<uint>(2);
    List<bool> mIsEventPlayed = new List<bool>(2);
    float mTimer;

    float resetTimer = 0.0f;

    public override void OnStart()
    {
        base.OnStart();

        mVenteFilename1 = InitFilenameParams(mVenteFilename1, 0);
        mVenteFilename2 = InitFilenameParams(mVenteFilename2, 1);
    }
    String InitFilenameParams(String filename, int index)
    {
        if (filename != null)
        {
            if (filename.Length > 0)
            {
                filename = filename + ".vente";
                if (!Common.GetStealthPlayerScript().GetSoundEvents().ContainsKey(filename))
                {
                    uint thisEventID = Common.GetStealthPlayerScript().GetSoundComponent().GetUniqueEvent(filename, 0);
                    mEventID.Add(thisEventID);
                    Common.GetStealthPlayerScript().GetSoundEvents().Add(filename, thisEventID);

                    mIsEventPlayed.Add(false); // Always have size of 2
                }
                return filename;
            }
        }
        mIsEventPlayed.Add(true);
        return null;
    }

    public void OnUpdate()
    {
        if (!isactive)
        {
            mTimer += FrameController.DT();

            for (int i = 0; i < mIsEventPlayed.Count; ++i)
            {
                if (!mIsEventPlayed[i])
                {
                    bool toPlay = false;
                    switch (i)
                    {
                        case 0: toPlay = mTimer >= mTimeToPlay1; break;
                        case 1: toPlay = mTimer >= mTimeToPlay2; break;
                    }
                    if (toPlay)
                    {
                        mIsEventPlayed[i] = true;
                        Common.GetStealthPlayerScript().GetSoundComponent().PlayUniqueEvent(mEventID[i], false);
                    }
                }
            }
        }

        if (resetTimer > 0.0f) // Reset back to active after a while
        {
            resetTimer -= FrameController.DT();
            if (resetTimer <= 0.0f)
            {
                resetTimer = 0.0f;
                isactive = true;
            }
        }
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
            // Stop playing other ambients, and play this one
            foreach (KeyValuePair<string,uint> soundPair in Common.GetStealthPlayerScript().GetSoundEvents())
            {
                String soundFilename = soundPair.Key;
                uint soundID = soundPair.Value;

                int doesExist = -1;
                for (int i = 0; i < 2; ++i)
                {
                    // If found a matching filename
                    if ((i == 0 && mVenteFilename1 == soundFilename) ||
                        (i == 1 && mVenteFilename2 == soundFilename))
                    {
                        doesExist = i;
                        break;
                    }
                }

                if (doesExist == -1)
                {
                    Common.GetStealthPlayerScript().GetSoundComponent().StopUniqueEvent(soundID, false);
                }
                else // If the sound already exists in player CSound
                {
                    if (!Common.GetStealthPlayerScript().GetSoundComponent().GetUniqueEventIsPlaying(soundID))
                    {
                        // Play it if it hasn't
                        Common.GetStealthPlayerScript().GetSoundComponent().PlayUniqueEvent(soundID, false);
                    }
                    mIsEventPlayed[doesExist] = true; // Set flag that it is already playing
                }
            }

            isactive = false;

            if (gameObject.GetName() == Common.nameForHubAmbientEnter || gameObject.GetName() == Common.nameForHubAmbientLeave)
            {
                resetTimer = 10.0f;
            }
        }
    }
    public override void OnArea()
    {

    }
}
