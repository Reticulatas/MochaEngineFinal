using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class OcuConsoleBlinkingLight : MochaScript
{
    CMeshRenderer mBlinkingLight;
    bool mIsBlinking;
    
    float mDefaultBlinkInterval;
    public float mBlinkInterval { get { return mDefaultBlinkInterval; } 
        set { mDefaultBlinkInterval = value; } }
    float mTimer = 0.0f;

    public void OnStart()
    {
        mBlinkingLight = gameObject.RequireComponent<CMeshRenderer>();
        mBlinkingLight.setEnabled(false);
    }

    public void OnUpdate()
    {
        if (mIsBlinking)
        {
            mTimer -= FrameController.DT();
            if (mTimer <= 0.0f)
            {
                mTimer = mBlinkInterval;
                mBlinkingLight.setEnabled(!mBlinkingLight.isEnabled());
            }
        }
    }
    
    public void TriggerBlinkingLight(bool startBlinking)
    {
        mIsBlinking = startBlinking;
        if (!mIsBlinking)
        {
            mBlinkingLight.setEnabled(false);
            mTimer = 0.0f;
        }
    }
}
