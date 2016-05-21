using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class CameraRotationLimit : MochaScript
{
    CSound mSound;
    uint mCamRotateID; // CAM_ROTATE - (param: MOUSE_SPEED) - [0, 1] 0 - stop/release trigger, up to 1 proportional to mouse speed
    const string mCamRotateParamStr = "MOUSE_SPEED";
    bool isSoundPlaying = false;

    public Vector3 offsetPosition { get; set; } // referenced in CameraCam

    public Vector3 startingFowardDir;
    public Vector3 startingUpDir;
    public Vector3 startingRightDir;

    float prevXCurrRot;
    float prevYCurrRot;
    float xCurrRot; // internal variable in rad
    float yCurrRot; // internal variable in rad
    // Set currentRot in Degrees
    public float xCurrentRotation {
        get { return xCurrRot; }
        set
        {
            float maxLimitInRadians = MathHelper.ToRadians(xRotationLimit);
            float minLimitInRadians = MathHelper.ToRadians(-xRotationLimit);
            xCurrRot = MathHelper.Clamp(value, minLimitInRadians, maxLimitInRadians);
        }
    }
    public float yCurrentRotation
    {
        get { return yCurrRot; }
        set
        {
            float maxLimitInRadians = MathHelper.ToRadians(yRotationLimit);
            float minLimitInRadians = MathHelper.ToRadians(-yRotationLimit);
            yCurrRot = MathHelper.Clamp(value, minLimitInRadians, maxLimitInRadians);
        }
    }

    public float xRotationLimit { get; set; } // In Degrees (set by designer)
    public float yRotationLimit { get; set; }

    public bool shouldPhysicalCameraRotate = true; // for static cameras, the lens will rotate to show where it's looking at

    GameObject camInMapArea;
    public GameObject GetTwin() { return camInMapArea; }
    // public void SetTwin(GameObject obj) { camInMap = obj; } // Only to be called by CameraInMap script
    public CLight spotlight; // Accessed by CameraInMap to turn on or off spotlight
    public CLight pointlight; // Accessed by CameraInMap to turn on or off pointlight
    Color offPointLightColor;
    Color onPointLightColor;

    public void SetPointLight(bool switchOn)
    {
        if (pointlight != null)
        {
            pointlight.SetDiffuse(switchOn ? onPointLightColor : offPointLightColor);
            pointlight.setEnabled(true);
        }
    }

    public void OnStart()
    {
        offPointLightColor = new Color(0.8f, 0.0f, 0.0f, 1.0f);
        onPointLightColor = new Color(0.0f, 0.8f, 0.0f, 1.0f);

        // Initialize into the common list
        GameObject camInMap;
        Common.AddCameraObjectToList(gameObject, out camInMap, out camInMapArea);
        CameraInMap inMapScript = GetScript<CameraInMap>(camInMapArea);
        inMapScript.SetTwin(gameObject); // Set in map to be able to direct to me (camInWorld)
        if (gameObject == Common.GetRCCarCam())
        {
            inMapScript.isActive = true; // always default RCCar to active
        }

        prevXCurrRot = xCurrRot = 0.0f;
        prevYCurrRot = yCurrRot = 0.0f;

        UpdateVectors();

        if (xRotationLimit == 0.0f && yRotationLimit == 0.0f)
        {
            shouldPhysicalCameraRotate = false;
        }

        for (uint i = 0; i < gameObject.transform.GetNumberOfChildren(); ++i)
        {
            GameObject childObj = gameObject.transform.GetChild(i).gameObject;
            if (childObj.GetName() == Common.nameForCamLight)
            {
                spotlight = childObj.RequireComponent<CLight>();
                spotlight.SetDiffuse(new Color(0.706f, 0.96f, 0.714f, 1.0f)); // FFB4FCB6
                // spotlight.SetRange(10.0f);
                // spotlight.SetSpot(10.0f);
                // spotlight.SetAtt(1.0f);
                // spotlight.SetLightType(CLight.LIGHTTYPE.SPOTLIGHT);
                spotlight.setEnabled(false);
            }
            else if (childObj.GetName() == Common.nameForCamPointLight)
            {
                pointlight = childObj.RequireComponent<CLight>();
                SetPointLight(false);
                pointlight.setEnabled(true);
            }
        }

        mSound = gameObject.RequireComponent<CSound>();
        mCamRotateID = mSound.GetUniqueEvent("CAM_ROTATE.vente", 1);
    }

    public void OnUpdate()
    {
        // If there was movement in the camera
        if (prevXCurrRot != xCurrRot || prevYCurrRot != yCurrRot)
        {
            float changeX = xCurrRot - prevXCurrRot;
            float changeY = yCurrRot - prevYCurrRot;
            float currChange = (float)Math.Sqrt(changeX * changeX + changeY * changeY);

            float paramValue = currChange / MathHelper.ToRadians(10.0f);
            paramValue = Math.Min(paramValue, 1.0f);
            if (!isSoundPlaying)
            {
                isSoundPlaying = true;
                //mSound.PlayUniqueEvent(mCamRotateID, false);
            }
            //mSound.SetUniqueEventParam(mCamRotateID, mCamRotateParamStr, paramValue);

            prevXCurrRot = xCurrRot; // Update to latest rotation value
            prevYCurrRot = yCurrRot;
        }
        else // No rotations, then stop the rotation sound
        {
            if (isSoundPlaying)
            {
                isSoundPlaying = false;
                //mSound.SetUniqueEventParam(mCamRotateID, mCamRotateParamStr, 0.0f);
            }
        }
    }

    public void OnEnd()
    {
        CameraCam camScript = Common.GetCameraCamScript();
        if (camScript != null)
        {
            if (camScript.gameObjectParentedTo == gameObject)
            {
                camScript.SetDisableCameraToStatic(); // Disable camera attachment to this object that is being destroyed
            }
        }
        Common.RemoveCameraObjectFromList(gameObject);
    }

    public void UpdateVectors()
    {
        startingFowardDir = gameObject.transform.GetForwardVector();
        startingUpDir = gameObject.transform.GetUpVector();
        startingRightDir = gameObject.transform.GetRightVector();
    }
    public void ResetVectors()
    {
        prevXCurrRot = xCurrRot = 0.0f;
        prevYCurrRot = yCurrRot = 0.0f;
    }
}
