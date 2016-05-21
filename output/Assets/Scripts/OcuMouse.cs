using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class OcuMouse : MochaScript
{
    GameObject ocuCam; // camera viewpoint of oculus
    Vector3 mMousePositionFromOcuCam;
    Vector2 mMouseLimits; // x and y bounds (half w and half h)
    public Vector2 mCurrMouseMovement;

    CCamera ocuCameraScreen;

    GameObject cameraCam; // The camera that render to the surveillance screen
    CameraCam camScript;

    FirstPersonPlayer playerScript;
    RCCar rcCarScript;

    List<GameObject> camInMap; // cam icons on OcuPlayer's map
    int camInMapIndex; // current camInMap that has icon over it (used with Xbox controller)

    InteractableObjectBaseClass prevPickedInteractable;
    InteractableObjectBaseClass pickedInteractable;
    GameObject prevPickedObject;
    GameObject pickedObject; // The map screen or the cam icon in ma etc

    bool isPaperHeld = false; // if true, locks the input to put it down on next L click

    public void OnStart()
    {
		Input.SetMouseVisible(false);
        Input.SetMouseLock(true);
		
        playerScript = GetScript<FirstPersonPlayer>(Common.GetStealthPlayer());
        rcCarScript = GetScript<RCCar>(Common.GetRCCarJoystick());
        cameraCam = Common.GetSurveillancePlayerStaticCameraCam();
        camScript = GetScript<CameraCam>(cameraCam);
        ocuCam = Common.GetSurveillancePlayerCam();
        prevPickedInteractable = null;
        pickedInteractable = null;
        prevPickedObject = null;
        pickedObject = null;

        camInMapIndex = 0;
        camInMap = Common.GetCamsInMap();

        mCurrMouseMovement = new Vector2(0.0f, 0.0f);
        mMousePositionFromOcuCam = new Vector3(0.0f, 0.0f, 1.3f);
        // Set the mouse cursor position to center of oculus screen
        gameObject.transform.SetPosition(ocuCam.transform.position +
            (Vector3)ocuCam.transform.GetForwardVector() * mMousePositionFromOcuCam.Z);

        // Calculate the limits for the mouse on the ocuScreen
        CCamera ocuCameraComp = ocuCam.RequireComponent<CCamera>();
        mMouseLimits = new Vector2();
        mMouseLimits.X = mMousePositionFromOcuCam.Z / ocuCameraComp.GetNearZ() * ocuCameraComp.GetNearWindowWidth() * 0.5f;
        mMouseLimits.Y = mMousePositionFromOcuCam.Z / ocuCameraComp.GetNearZ() * ocuCameraComp.GetNearWindowHeight() * 0.5f;

        ocuCameraScreen = Common.GetCameraScreen().RequireComponent<CCamera>();
    }

    void UpdateMousePosition()
    {
        Vector3 xAxis = ocuCam.transform.GetRightVector();
        Vector3 yAxis = ocuCam.transform.GetUpVector();
        Vector3 centerPos = ocuCam.transform.position + (Vector3)ocuCam.transform.GetForwardVector() * mMousePositionFromOcuCam.Z;

        mCurrMouseMovement = new Vector2((float)Input.GetMouseDeltaX(), (float)-Input.GetMouseDeltaY());
        mCurrMouseMovement *= 0.001f; // dt * 0.035f;

        // mouse position in 'screen space' is always saved and updated
        mMousePositionFromOcuCam.X += mCurrMouseMovement.X;
        mMousePositionFromOcuCam.Y += mCurrMouseMovement.Y;
        mMousePositionFromOcuCam.X = MathHelper.Clamp(mMousePositionFromOcuCam.X, -mMouseLimits.X, mMouseLimits.X);
        mMousePositionFromOcuCam.Y = MathHelper.Clamp(mMousePositionFromOcuCam.Y, -mMouseLimits.Y, mMouseLimits.Y);

        // Get the 3D in world, mouse offset from ocuCam's position
        Vector3 offset = new Vector3(0.0f); // Mouse is always a fixed z distance from ocuCam
        offset += xAxis * mMousePositionFromOcuCam.X;
        offset += yAxis * mMousePositionFromOcuCam.Y;

        gameObject.transform.SetPosition(centerPos + offset);
    }
    public void UpdateMouseInputs()
    {
        float dt = FrameController.DT();
        UpdateMousePosition();

        if (isPaperHeld || (rcCarScript != null && rcCarScript.isMouseHeld)) // Lock input until paper is put down
        {
            if (Input.GetTriggered(0, "MouseLClick") == 1.0f)
            {
                if (rcCarScript != null && rcCarScript.isMouseHeld)
                    rcCarScript.isMouseHeld = false;
                else
                {
                    pickedInteractable.OnTriggered();
                    isPaperHeld = false;
                }
            }
        }
        else
        {
            // Apply raycast of ocuPlayer's mouse to find any interactable object every frame (when mouse is not clicked)
            Vector3 ocuCamToMouse = (Vector3)gameObject.transform.position - (Vector3)ocuCam.transform.position;

            // Quick Hack to fix offset
            // Vector3 xAxis = ocuCam.transform.GetRightVector();
            // Vector3 yAxis = ocuCam.transform.GetUpVector();
            // ocuCamToMouse += xAxis * 0.055f;
            // ocuCamToMouse += yAxis * 0.12f;

            ocuCamToMouse.Normalize();
            if (Input.GetHeld(0, "MouseLClick") == 0.0f)
            {
                pickedObject = PhysicEngine.getInstance().RayCast3DWithTag(ocuCam.transform.position, ocuCamToMouse, 10000.0f, Common.tagForInteractables);

                pickedInteractable = null;
                // Check if mouse is over a valid camInMap
                if (pickedObject != null)
                {
                    pickedInteractable = GetScriptClass<InteractableObjectBaseClass>(pickedObject, false);
                    if (pickedInteractable != null)
                    {
                        pickedInteractable.OnMouseOver();

// The logic for ray casting through the camera screen into world objects
                        CameraScreen pickedScreen = pickedInteractable as CameraScreen;
                        if (pickedScreen != null && camScript.gameObjectParentedTo != null &&
                            camScript.gameObjectParentedTo != Common.GetNoiseStatic())
                        {
                            CTransform screenTrans = pickedScreen.gameObject.transform;
                            Vector3 screenPos = screenTrans.position;
                            Vector3 rightVect = screenTrans.GetRightVector();
                            Vector3 upVect = screenTrans.GetUpVector();
                            Vector3 normal = screenTrans.GetForwardVector();
                            Vector3 screenScale = screenTrans.GetScale();

                            Vector3 pointOnScreen = new Vector3();
                            MMath.RayToPlaneIntersection(normal, screenPos, ocuCamToMouse, ocuCam.transform.position, ref pointOnScreen);
                            Vector3 screenToPoint = pointOnScreen - screenPos;
                            float xAmt = -Vector3.Dot(screenToPoint, rightVect); // the camera screen's x is negative (to the left)
                            float yAmt = Vector3.Dot(screenToPoint, upVect);
                            float xScaleOfModel = screenScale.X*0.5f; // Reflect actual camera screen width in pixel units (quad is just nice 1 scale == 1 unit)
                            float yScaleOfModel = screenScale.Y*0.5f;
                            xAmt /= xScaleOfModel; // [-0.5, 0.5]
                            yAmt /= yScaleOfModel;

                            float nearZ = ocuCameraScreen.GetNearZ();
                            float screenWidth = ocuCameraScreen.GetNearWindowWidth();
                            float screenHeight = ocuCameraScreen.GetNearWindowHeight();
                            xAmt *= screenWidth; // [-0.5, 0.5] -> [-w/2, w/2]
                            yAmt *= screenHeight;
                            // Use the x, y and z values through the actual camera look at directions
                            Vector3 innerCameraVect = xAmt * (Vector3)camScript.gameObjectParentedTo.transform.GetRightVector() +
                                yAmt * (Vector3)camScript.gameObjectParentedTo.transform.GetUpVector() +
                                nearZ * (Vector3)camScript.gameObjectParentedTo.transform.GetForwardVector();
                            innerCameraVect.Normalize();

                            Vector3 cameraPos = camScript.gameObjectParentedTo.transform.position +
                                0.5f * innerCameraVect; // Move in front, so it does not ray cast to the screen itself
								
							GameObject cameraInteractable = PhysicEngine.getInstance().RayCast3DWithTag(cameraPos, innerCameraVect, 10000.0f, "camInteract");
                            if (cameraInteractable != null)
                            {
                                // If there is an interactable objec through the screen, check if there is ANY object infront of it
                                GameObject nearestObj = PhysicEngine.getInstance().RayCast3D(cameraPos, innerCameraVect, 10000.0f);
                                if (nearestObj == cameraInteractable)
                                {
                                    pickedInteractable = GetScriptClass<InteractableObjectBaseClass>(cameraInteractable, false);
                                    if (pickedInteractable != null)
                                    {
                                        pickedInteractable.OnMouseOver();
                                    }
                                }
                                else // There is something blocking
                                {
                                    Logger.Log("nearer Obj found: " + nearestObj.GetName());
                                }
                            }
                        }
                    }
                }
                // If prev interactable changed or is lost, call it's mouseOverOff
                if (prevPickedInteractable != pickedInteractable)
                {
                    if (prevPickedInteractable != null)
					{
                        prevPickedInteractable.OnMouseOverOff();
						if (Input.GetReleased(0, "MouseLClick") == 1.0f)
						{
							prevPickedInteractable.OnReleased();
						}
					}
                    prevPickedInteractable = pickedInteractable;
                }
            } // End of if not mouse held

            if (Input.GetTriggered(0, "MouseLClick") == 1.0f)
            {
                if (pickedInteractable != null)
                {
                    pickedInteractable.OnTriggered();

                    // If picked up paper, lock the input
                    Paper pickedPaper = pickedInteractable as Paper;
                    if (pickedPaper != null)
                    {
                        if (pickedPaper.GetIsHeldUp())
                            isPaperHeld = true;
                    }
                }
            }
            if (Input.GetHeld(0, "MouseLClick") == 1.0f)
            {
                if (pickedInteractable != null)
                {
                    pickedInteractable.OnHeldMouse(mCurrMouseMovement.X, mCurrMouseMovement.Y);
                }
            }
            if (Input.GetReleased(0, "MouseLClick") == 1.0f)
            {
				if(pickedInteractable != null)
					pickedInteractable.OnReleased();
            }
        }
	
    }
    public void UpdateXboxInputs()
    {
        float dt = FrameController.DT();

        Vector3 centerPos = ocuCam.transform.position + (Vector3)ocuCam.transform.GetForwardVector() * mMousePositionFromOcuCam.Z;
        gameObject.transform.SetPosition(centerPos); // Always at the center of oculus screen

        // Update dpad Xbox controls for selecting camInMap icons
        bool upTriggered = Input.GetTriggered((char)0, "ArrowUp") != 0;
        bool leftTriggered = Input.GetTriggered((char)0, "ArrowLeft") != 0;
        bool downTriggered = Input.GetTriggered((char)0, "ArrowDown") != 0;
        bool rightTriggered = Input.GetTriggered((char)0, "ArrowRight") != 0;

        if (upTriggered || leftTriggered || downTriggered || rightTriggered)
        {
            if (upTriggered || leftTriggered) // decrements
            {
                if (camInMapIndex > 0) 
                    --camInMapIndex; // [0, length - 1]
            }
            else // downTriggered || rightTriggered // increments
            {
                if (camInMapIndex + 1 < camInMap.Count)
                    ++camInMapIndex; // [0, length - 1]
            }
            
            // Vector3 camInMapPos = (Vector3)camInMap[camInMapIndex].transform.position;
            // camInMapSelector.gameObject.transform.SetPositionX(camInMapPos.X);
            // camInMapSelector.gameObject.transform.SetPositionY(camInMapPos.Y);
        }

        // 'X' to select active camera
        if (Input.GetTriggered(0, "Xbox_X") != 0)
        {
            pickedObject = camInMap[camInMapIndex];
            CameraInMap camMapScript = GetScript<CameraInMap>(pickedObject);
            
            // Only change camera if picked cam icon is disabled (not nonActive or already picked)
            if (camMapScript.isActive && !camMapScript.isEnabled)
            {
                // Set previous camInMap back to disable for feedback
                if (prevPickedObject != null)
                {
                    CameraInMap prevCamMapScript = GetScript<CameraInMap>(prevPickedObject);
                    if (prevCamMapScript.isEnabled && prevCamMapScript.isActive)
                        prevCamMapScript.isEnabled = false;
                }
                // Set that camInMap to green for feedback
                prevPickedObject = pickedObject;
                camMapScript.isEnabled = true;
            } // End of if picked a disabled camera (then enable it)
        }

        // Updating the camera screen rotation based on Right Thumbstick of Xbox
        if (camScript.gameObjectParentedTo != null && camScript.gameObjectParentedTo != Common.GetNoiseStatic())
        {
            float xboxX = Input.GetValue((char)0, "RThumbX");
            float xboxY = Input.GetValue((char)0, "RThumbY");

            if (xboxX != 0.0f || xboxY != 0.0f)
            {
                // The physical camera object in the level is the one with the RotLimit script
                CameraRotationLimit camRotLimit = GetScript<CameraRotationLimit>(camScript.gameObjectParentedTo);
                camRotLimit.xCurrentRotation += xboxX * 1.3f * dt; // script has clamped this rotation, in degrees
                camRotLimit.yCurrentRotation += xboxY * 1.3f * dt;

                camScript.SetUpdateLookAtDirAndPosition();
            }
        }
    }

    public void OnUpdate()
    {
        if (Input.GetTriggered(0, "Space") == 1.0f)
        {
            mMousePositionFromOcuCam = new Vector3(0.0f, 0.0f, 1.3f);
            if (Input.GetHeld(0, "SnapGrid") != 0.0f)
            {
                Input.SetMouseLock(!Input.GetLockMouse());
            }
        }

        if (playerScript.usingXboxControl)
        {
            UpdateMouseInputs();
        }
        else
        {
            UpdateXboxInputs();
        }
    }
}
