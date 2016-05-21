using Microsoft.Xna.Framework;
using MochaInterface;using System;
using System.Collections.Generic;

public class CameraCam : MochaScript
{
    // This is only for the virtual camera cam, which is the source RT for surveillance
    CCamera camComponent;
    float defaultCamFoV = 1.6f;
    public float camFoV { get { return defaultCamFoV; }
        set { defaultCamFoV = value; } }

    public GameObject gameObjectParentedTo; // 'parented' means looking at and at its position
    public String gameObjectParentedToName { get; set; }
    bool isParentObjStatic = true;

    bool hasInit = false;

    GameObject noiseStatic;

    public void OnStart()
    {
        camComponent = gameObject.RequireComponent<CCamera>();

        camComponent.FoVmodifier(camFoV);

        gameObjectParentedTo = null;
        noiseStatic = Common.GetNoiseStatic();

        // Set default starting camera bound object
        gameObjectParentedTo = null;
    }
    void InitStartingParentedCamera()
    {
        if (hasInit)
            return;

        if (gameObjectParentedTo == null && gameObjectParentedToName != null)
        {
            GameObject objToStartWith = GameObject.GetGameObjectByName(gameObjectParentedToName);
            if (objToStartWith != null)
            {
                CameraInMap camInMapScript = GetScript<CameraInMap>(objToStartWith);
                if (camInMapScript != null)
                {
                    camInMapScript.isActive = true;
                    camInMapScript.OnTriggered();

                    hasInit = true;
                }
            }
        }
        if (gameObjectParentedTo == null)
        {
            // SetDisableCameraToStatic(); // Start off having the static screen, if not specified
        }
    }

    public void OnUpdate()
    {
        InitStartingParentedCamera(); // Keep trying to link up with starting camera (so it is order independent)

        if (!isParentObjStatic)
        {
            // Always update to parent camera's position and rotation
            SetUpdateLookAtDirAndPosition();
        }
    }

    public void SetBindToPhysicalCamera(GameObject camObj)
    {
        gameObjectParentedTo = camObj;
        gameObjectParentedToName = camObj.GetName();

        // load back using CameraRotationLimit's x and y angles
        SetUpdateLookAtDirAndPosition();

        // If the camObj is sticky camera, AND sticky camera is stuck on a non-static object
        isParentObjStatic = true;
        if (camObj == Common.GetStickyCamera())
        {
            CTransform wallObj = camObj.transform.GetParent();
            if (wallObj != null)
            {
                CPhysics phy = wallObj.gameObject.GetComponent<CPhysics>();
                if (phy != null && phy.mColliderType != 0)
                {
                    isParentObjStatic = false;
                }
            }
        }
        else if (camObj == Common.GetRCCarCam())
        {
            isParentObjStatic = false;
        }
    }
    public void SetUpdateLookAtDirAndPosition() // Update angles at currently attached physical camera
    {
        // camComponent.FoVmodifier(camFoV);

        CameraRotationLimit camRotLimit = GetScript<CameraRotationLimit>(gameObjectParentedTo);
        // Only check and update to the actual forward vector if camera is NOT meant to rotate via camera screen
        if (!camRotLimit.shouldPhysicalCameraRotate) // ie. x/yRotationLimit should be 0.0f
        {
            Vector3 currForward = gameObjectParentedTo.transform.GetForwardVector();
            if (currForward != camRotLimit.startingFowardDir)
            {
                camRotLimit.UpdateVectors();
            }
        }
        Vector3 newLookAtDir = camRotLimit.startingFowardDir;
        newLookAtDir += (Vector3)camRotLimit.startingRightDir * camRotLimit.xCurrentRotation;
        newLookAtDir += (Vector3)camRotLimit.startingUpDir * camRotLimit.yCurrentRotation;
        gameObject.transform.LookAt(newLookAtDir + gameObject.transform.position);

        /*Vector3 currCameraRotation = gameObject.transform.GetRotation();
        Logger.Log("currCameraRotation: " + currCameraRotation.ToString());
        gameObjectParentedTo.transform.Rotate(currCameraRotation);
        Logger.Log("the physical camera: " + gameObjectParentedTo.transform.GetRotation().ToString());
         */
        gameObjectParentedTo.transform.LookAt(newLookAtDir + gameObjectParentedTo.transform.position);

        gameObject.transform.SetPosition(gameObjectParentedTo.transform.position +
            camRotLimit.offsetPosition.X * camRotLimit.startingRightDir +
            camRotLimit.offsetPosition.Y * camRotLimit.startingUpDir +
            camRotLimit.offsetPosition.Z * camRotLimit.startingFowardDir);
    }

    public void SetDisableCameraToStatic()
    {
        gameObjectParentedTo = noiseStatic;
        gameObjectParentedToName = noiseStatic.GetName();

        Vector3 noiseNormal = noiseStatic.transform.GetForwardVector();
        gameObject.transform.LookAt(-noiseNormal + gameObject.transform.position);
        gameObject.transform.SetPosition(noiseStatic.transform.position + 0.8f * noiseNormal);
    }
}
