using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class CameraInMap : InteractableObjectBaseClass
{
    bool isvisible = false;
    bool isVisible
    {
        get { return isvisible; }
        set
        {
            InitMesh();

            isvisible = value;
            camInMapMesh.setEnabled(isvisible);
        }
    }

    bool isactive = false; // whether is can be selected
    public bool isActive
    {
        get { return isactive; }
        set
        {
            InitMesh();

            isactive = value;
            if (isactive == false)
                isEnabled = false; // Auto set to disabled if inactive

            if (camInMapMesh != null)
            {
                //Color currColor = isactive ? disabledCamColor : notActiveCamColor;
                //camInMapMesh.GetMaterial().SetColor(currColor);
                if (isactive)
                {
                    isVisible = true;
                    camInMapMesh.GetMaterial().SetColor(originalColor);
                }
                else // If camera is deactivated
                {
                    camInMapMesh.GetMaterial().SetColor(notActiveCamColor1);

                    if (gameObject == Common.GetStickyCamInMapArea())
                    {
                        camInMapMesh.setEnabled(false);
                    }
                    // And ocuPlayer is looking through it
                    if (camScript.gameObjectParentedTo == camInWorld)
                    {
                        camScript.SetDisableCameraToStatic();
                    }
                }
            }
        }
    }

    bool isenabled = false;
    public bool isEnabled // Whether it is selected as current camera
    {
        get { return isenabled; }
        set 
        {
            InitMesh();
            isenabled = value;

            if (camInMapMesh != null)
            {
                if (isenabled) // If selected this, set ocuCamera to bind to my twin (physical camera)
                {
                    camScript.SetBindToPhysicalCamera(camInWorld);

                    if (currEnabledCamInMap != null &&
                        currEnabledCamInMap != this &&
                        currEnabledCamInMap.isActive)
                    {
                        currEnabledCamInMap.isEnabled = false; // disable the old enabled icon
                    }
                    currEnabledCamInMap = this; // Set curr enabled to myself

                    sound.PlayIndependentEvent("P2_TV_CHANNEL.vente", false, 0); // "MON_TV_CHANNEL.vente"
                }

                CameraRotationLimit camInWorldScript = GetScript<CameraRotationLimit>(camInWorld);
                if (camInWorldScript != null)
                {
                    if (camInWorldScript.spotlight != null)
                    {
                        // Enable/Disable spotlight and the flickering if there is
                        camInWorldScript.spotlight.setEnabled(isenabled);
                        LightFlicker lightFlickerScript = GetScript<LightFlicker>(camInWorldScript.spotlight.gameObject, false);
                        if (lightFlickerScript != null)
                        {
                            lightFlickerScript.activated = isenabled;
                        }
                    }
                    camInWorldScript.SetPointLight(isenabled);
                }
                Color currColor = isenabled ? enabledCamColor1 : originalColor;
                camInMapMesh.GetMaterial().SetColor(currColor);
            }
        }
    }

    Color notActiveCamColor1;
    Color disabledCamColor1;
    Color enabledCamColor1;
    Color originalColor;
    // Color notActiveCamColor { get { return notActiveCamColor1; } }
    // Color disabledCamColor { get { return disabledCamColor1; } }
    // Color enabledCamColor { get { return enabledCamColor1; } }


    CSound sound;

    GameObject ocuCam;
    CameraCam camScript;
    GameObject camInMapIcon;
    GameObject camInWorld;
    public GameObject GetTwin() { return camInWorld; }
    CForwardRenderer camInMapMesh;
    CForwardRenderer camInMapSelector; // icon overlay to show what is currently selected

    // Accessed in Common.RemoveCameraObjectFromList to check and nullify it, if camInMapArea is destroyed
    public static CameraInMap currEnabledCamInMap = null; // All camInMap knows who is the currently enabled one

    bool hasInit = false;

    public void SetTwin(GameObject inCamInWorld)
    {
        camInWorld = inCamInWorld;
    }

    void InitMesh()
    {
        if (!hasInit)
        {
            if (gameObject.transform.GetParent() != null)
            {
                hasInit = true;

                camInMapIcon = gameObject.transform.GetParent().gameObject;
                camInMapMesh = camInMapIcon.RequireComponent<CForwardRenderer>();
                originalColor = camInMapMesh.GetMaterial().GetColor();

                GameObject cameraCam = Common.GetSurveillancePlayerStaticCameraCam();
                camScript = GetScript<CameraCam>(cameraCam);
                ocuCam = Common.GetSurveillancePlayerCam();

                sound = gameObject.RequireComponent<CSound>();

                camInMapSelector = Common.GetCameraInMapSelector().RequireComponent<CForwardRenderer>();
                camInMapSelector.setEnabled(false);

                notActiveCamColor1 = new Color(0.1f, 0.1f, 0.1f, 1.0f);
                disabledCamColor1 = new Color(SharpMocha.YELLOW);
                enabledCamColor1 = new Color(SharpMocha.GREEN);

                isVisible = isvisible;
                isActive = isactive; // To call the set functions
                if (isactive)
                    isEnabled = isenabled;
            }
        }
    }
    public void OnStart()
    {
        InitMesh();
        /*
        String name = gameObject.GetName();
        if (name.StartsWith(Common.nameForCamInMapArea)) // normal camera
        {
            String camIndexStr = name.Substring(Common.nameForCamInMapArea.Length);
            String mapInWorldName = Common.nameForCamInWorld + camIndexStr;
            camInWorld = GameObject.GetGameObjectByName(mapInWorldName);

            int camIndex = Convert.ToInt32(camIndexStr);
            camInMapIcon = Common.GetCamsInMap()[camIndex];
            camInMapMesh = camInMapIcon.RequireComponent<CForwardRenderer>();
            originalColor = camInMapMesh.GetMaterial().GetColor();
        }
        else if (name == Common.nameForStickyCamInMap) // sticky cam
        {
            camInWorld = Common.GetStickyCamera();

            isActive = false;
        }
        else if (name == Common.nameForRCCarCamInMap)
        {
            camInWorld = Common.GetRCCar();
        }

        CameraRotationLimit camWorldScript = GetScript<CameraRotationLimit>(camInWorld);
        camWorldScript.SetTwin(camInMapIcon); // Set my twin's twin to be me
        */
    }

    public void OnUpdate()
    {

    }

    public override void OnTriggered()
    {
        if (isActive && !isEnabled)
        {
            isEnabled = true;
        }
    }
    public override void OnMouseOver()
    {
        if (isActive)
        {
            camInMapSelector.setEnabled(true);
            Vector3 camDir = ocuCam.transform.GetForwardVector();
            // Move it infront of the camInMap Icon
            camInMapSelector.gameObject.transform.SetPosition(
                (Vector3)camInMapIcon.transform.position - camDir * 0.001f);


            CFontRenderer sub = GetSubtitleFont();
            if (!sub.isEnabled())
            {
                sub.setEnabled(true);
                sub.mText = "Click to Switch Camera";
            }
        }
    }
    public override void OnMouseOverOff()
    {
        camInMapSelector.setEnabled(false);

        GetSubtitleFont().setEnabled(false);
    }
}
