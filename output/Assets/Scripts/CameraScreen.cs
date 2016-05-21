using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class CameraScreen : InteractableObjectBaseClass
{
    CameraCam camScript;

    public void OnStart()
    {
        GameObject cameraCam = Common.GetSurveillancePlayerStaticCameraCam();
        camScript = GetScript<CameraCam>(cameraCam);
    }

    public void OnUpdate()
    {
        
    }

    public override void OnHeldMouse(float mouseMoveX, float mouseMoveY)
    {
        if (mouseMoveX == 0.0f && mouseMoveY == 0.0f)
            return;

        // The physical camera object in the level is the one with the RotLimit script
        if (camScript.gameObjectParentedTo != null)
        {
            CameraRotationLimit camRotLimit = GetScript<CameraRotationLimit>(camScript.gameObjectParentedTo, false);
            if (camRotLimit != null)
            {
                camRotLimit.xCurrentRotation += mouseMoveX * 5.0f; // script has clamped this rotation, in degrees
                camRotLimit.yCurrentRotation += mouseMoveY * 5.0f;

                camScript.SetUpdateLookAtDirAndPosition();
            }
        }
    }
    public override void OnMouseOver()
    {
        CFontRenderer sub = GetSubtitleFont();
        if (!sub.isEnabled())
        {
            sub.setEnabled(true);
            sub.mText = "Click Drag to Look Around";
        }
    }
    public override void OnMouseOverOff()
    {
        GetSubtitleFont().setEnabled(false);
    }
}
