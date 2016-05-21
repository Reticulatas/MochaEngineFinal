using Microsoft.Xna.Framework;
using MochaInterface;using System;
using System.Collections.Generic;

public class CameraInMapTriggerArea : TriggerAreaBaseClass
{
    MinimapTracker minimapScript;

    public uint camInMapNumberStart { get; set; }
    public uint camInMapNumberEnd { get; set; }
    public uint camToDisableStart { get; set; }
    public uint camToDisableEnd { get; set; }
    public String dMiniMapTextureName = "";
	public String MiniMapTextureName { get { return dMiniMapTextureName; }
        set { dMiniMapTextureName  = value; } }
    public int mLevelNumber { get; set; }

    float timer = 0.0f;

    public override void OnStart()
    {
        base.OnStart();
		
		if (dMiniMapTextureName == null)
			dMiniMapTextureName = "";

        minimapScript = GetScript<MinimapTracker>(Common.GetMapScreen());

        //if (camInMapNumberEnd >= Common.GetCamsInMapArea().Count)
        //    camInMapNumberEnd = (uint)Math.Min(0, Common.GetCamsInMapArea().Count - 1);
        if (camInMapNumberStart > camInMapNumberEnd)
            camInMapNumberStart = camInMapNumberEnd;
        //
        //if (camToDisableEnd >= Common.GetCamsInMapArea().Count)
        //    camToDisableEnd = (uint)Math.Min(0, Common.GetCamsInMapArea().Count - 1);
    }

    public void OnUpdate()
    {
        if (timer > 0.0f) // Reset back to active after a while
        {
            timer -= FrameController.DT();
            if (timer <= 0.0f)
            {
                timer = 0.0f;
                isactive = true;
            }
        }
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
            if (Common.GetCamsInMapArea().Count > 0) // not empty
            {
                camInMapNumberEnd = (uint)Math.Min(camInMapNumberEnd, Common.GetCamsInMapArea().Count - 1);
                camToDisableEnd = (uint)Math.Min(camToDisableEnd, Common.GetCamsInMapArea().Count - 1);

                for (uint i = camToDisableStart; i <= camToDisableEnd; ++i)
                {
                    CameraInMap camScript = GetScript<CameraInMap>(Common.GetCamsInMapArea()[(int)i]);
                    if (camScript == null)
                        continue;
                    camScript.isActive = false; // Set the camera to deactivated
                }
                for (uint i = camInMapNumberStart; i <= camInMapNumberEnd; ++i)
                {
                    CameraInMap camScript = GetScript<CameraInMap>(Common.GetCamsInMapArea()[(int)i]);
                    if (camScript == null)
                        continue;
                    camScript.isActive = true; // Set the camera to activated
                }
            }

            // After triggering the effect of turning on cameras, destroy the object
            //gameObject.Destroy();
			isactive = false;
            if (gameObject.GetName() == Common.nameForHubCamTrigger)
            {
                timer = 10.0f; // will be reset after awhile
            }
			
			if(MiniMapTextureName.Length > 0)
				Common.GetMapScreen().RequireComponent<CMeshRenderer>().GetMaterial().SetTexture(MiniMapTextureName + ".png");

            if (minimapScript.mLevelNumber != mLevelNumber)
            {
                minimapScript.mLevelNumber = mLevelNumber;
                minimapScript.UpdateNewMinimapBounds();
            }
        }
    }
    public override void OnArea()
    {

    }
}
