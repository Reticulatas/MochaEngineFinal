using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class SpinTop : InteractableObjectBaseClass
{
    float spin = 0;
    CLight light;
    GameObject spookg;
    CSkinMeshRenderer spook;
    bool spookEnabled;
    float startY;

    public void OnStart()
    {
        gameObject.SetFlag(Common.tagForInteractables);
        light = gameObject.GetComponentInChildren<CLight>();

        spookg = GameObject.GetGameObjectByName("zSpook", true);
        startY = spookg.transform.position.y;
        spookg.transform.SetPositionY(-1000);
    }

    public void OnUpdate()
    {
        if (spin >= 0)
        {
            spin -= FrameController.DT()*2;
            gameObject.transform.RotateAround(Vector3.Up, spin);
            light.mIntensity = spin / 30.0f;
            light.mRange = light.mIntensity;
        }

        if (spookEnabled)
        {
            Vector3 forw = (Vector3)Common.GetSurveillancePlayerCam().transform.GetForwardVector();
            if (Vector3.Dot(forw,(Vector3)spookg.transform.GetForwardVector()) < -0.88f) 
            {
                Logger.Log("boo");
                spookEnabled = false;
                spook = null;
                spookg.transform.SetPositionY(-1000);
                gameObject.RequireComponent<CSound>().PlayIndependentEvent("C1_ANGRY.vente", false, 0);
            }
        }
    }

    public override void OnTriggered() {
        spin += FrameController.DT() * 30;
        if (spin > 15.0f)
        {
            light.mDiffuse.r = 1.0f;
            spookEnabled = true;
            spookg.transform.SetPositionY(startY);
        }
    }

    public override void OnMouseOver()
    {
        CFontRenderer sub = GetSubtitleFont();
        if (!sub.isEnabled())
        {
            sub.setEnabled(true);
            sub.mText = "Spin!";
        }
    }
    public override void OnMouseOverOff()
    {
        GetSubtitleFont().setEnabled(false);
    }

}