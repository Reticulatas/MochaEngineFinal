using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class StickyCameraController : MochaScript
{
    OcuMouse ocuMouse;
	GameObject player;
    FirstPersonPlayer playerScript;
	GameObject camera;
    GameObject wallStuckOn;
    GameObject cameraCam; // The camera that surveillance player sees
    GameObject heldObjectPosition;
    CameraCam camScript;
    CameraRotationLimit camRotLimitScript;

    CSound sound;
    uint flyingSoundID;

    Vector3 mInitialScale;
	CMeshRenderer camMesh;
    CPhysics camPhysics;
    Vector3 startThrowDir;
    bool hasHitWall = false;
    Vector3 startthrowOffset = new Vector3(0.25f, 0.12f, 0.5f);
    public Vector3 startThrowOffset { get { return startthrowOffset; }
        set { startthrowOffset = (Vector3)value; }
    }

    Vector3 cameraUp;
    float dThrowSpeed = 15.0f;
    public float throwSpeed { get { return dThrowSpeed; }
        set { dThrowSpeed = value; } }
    float dTimeAllowedToThrowOut = 3.5f;
    public float timeAllowedToThrowOut { get { return dTimeAllowedToThrowOut; }
        set { dTimeAllowedToThrowOut = value; } }
    float timerWhenThrown;
    String buttonToThrowCam;
    
    public float distanceOffset {get; set;}

    StateMachineController smc = new StateMachineController();
    State nonActive;
    State flyingTowardsWall;
    State activeOnWall;
    State flyingBackToPlayer;

	public void OnStart()
    {
        ocuMouse = GetScript<OcuMouse>(Common.GetSurveillancePlayerMouse() );
    	player = Common.GetStealthPlayer();
        playerScript = GetScript<FirstPersonPlayer>(player);
        camera = Common.GetStealthPlayerCamera();
        cameraCam = Common.GetSurveillancePlayerStaticCameraCam(); 
        camScript = GetScript<CameraCam>(cameraCam);
        camRotLimitScript = GetScript<CameraRotationLimit>(gameObject);

        heldObjectPosition = GameObject.GetGameObjectByName("HeldObjectPosition");

        sound = gameObject.RequireComponent<CSound>();
        flyingSoundID = sound.GetUniqueEvent("CAM_FLIGHT.vente", 2);

        camPhysics = gameObject.RequireComponent<CPhysics>();
        camPhysics.mColliderType = 1;
        camMesh = gameObject.RequireComponent<CMeshRenderer>();
        mInitialScale = (Vector3)gameObject.transform.GetScale();
        
        nonActive = new State(smc, State_NonActive_Enter, State_NonActive_Update, State_NonActive_Exit);
        flyingTowardsWall = new State(smc, State_FlyTowardsWall_Enter, State_FlyTowardsWall_Update, State_FlyTowardsWall_Exit);
        activeOnWall = new State(smc, State_ActiveOnWall_Enter, State_ActiveOnWall_Update, State_ActiveOnWall_Exit);
        flyingBackToPlayer = new State(smc, State_FlyingBackToPlayer_Enter, State_FlyingBackToPlayer_Update, State_FlyingBackToPlayer_Exit);

        smc.SetState(nonActive); // Initial state
    }

	public void OnUpdate()
    {
        buttonToThrowCam = playerScript.usingXboxControl ? "XboxThrow" : "MouseLClick";

        smc.Update();
    }

    void OnCollidingAgainstWall(GameObject wall)
    {
        wallStuckOn = wall;

        camPhysics.rigidCom.Body().SetLinearVelocity(new Vector3(0.0f)); // Stop the camera
        camPhysics.mColliderType = 0; // Static collider

        // Calculate the sticky camera angle when on wall
        Vector3 wallNormal = wallStuckOn.transform.GetForwardVector();
        // Get the normal of the wall that is pointing towards the player
        if (Vector3.Dot(startThrowDir, wallNormal) > 0.0f)
            wallNormal *= -1.0f;
            
        Vector3 pointInFrontOfWall = wallNormal + gameObject.transform.position;
        cameraUp = new Vector3(0.0f, 1.0f, 0.0f);
        if (Vector3.Cross(cameraUp, wallNormal) == new Vector3(0.0f) ) // Make sure up is not parallel to normal
        {
            cameraUp = new Vector3(1.0f, 0.0f, 0.0f);
        }
        gameObject.transform.LookAt(pointInFrontOfWall, cameraUp);
            
        // Shift the camera to be outside the wall
        gameObject.transform.SetPosition(gameObject.transform.position + startThrowDir*-distanceOffset);

        sound.PlayIndependentEvent("CAM_TOSS_IMP.vente", false, 0);

        smc.SetState(activeOnWall);
    }
    public void OnCollisionEnter(CollisionData data)
	{
        if (smc.GetCurrentState() == flyingTowardsWall && 
            data.collidedObj.GetID() != player.GetID())
		{
			CPhysics phyComp = data.collidedObj.GetComponent<CPhysics>();
            if (phyComp != null && !phyComp.mIsTriggered)
            {
                Logger.Log("Collided with: " + data.collidedObj.GetName());

                OnCollidingAgainstWall(data.collidedObj);
                hasHitWall = true;
            }
		}
	}

    // In player's hand
    public void State_NonActive_Enter()
    {
    	camMesh.setEnabled(false);
        camPhysics.mIsTriggered = true; // Disable collision response

        Logger.Log("Cam in non active!");

        if (heldObjectPosition == null)
            Logger.Log("HeldObjectPosition object not found, held object will be attached to player");
    }
    public void State_NonActive_Update()
    {
    	// Keep following the player while not active (same position)
        if (heldObjectPosition == null)
            gameObject.transform.SetPosition(camera.transform.position);
        else
            gameObject.transform.SetPosition(heldObjectPosition.transform.position);
    	
        // When xbox control is pressed
        if (!Common.isPaused && Input.GetTriggered((char)0, buttonToThrowCam) != 0)
        {
            // Set gameobject's physics to add force in that dir
            Vector3 offset = camera.transform.position;
            offset += (Vector3)camera.transform.GetForwardVector() * startthrowOffset.Z;
            offset += (Vector3)camera.transform.GetRightVector() * startthrowOffset.X;
            offset += (Vector3)camera.transform.GetUpVector() * startthrowOffset.Y;
            gameObject.transform.SetPosition(offset);

            // Get player's facing direction
            startThrowDir = camera.transform.GetForwardVector();
            Vector3 playerFacingDir = startThrowDir * throwSpeed;
            Logger.Log("Sticky Cam throw dir: " + startThrowDir.ToString());
            camPhysics.rigidCom.Body().SetLinearVelocity(playerFacingDir * FrameController.DT()*30.0f);
            camPhysics.mColliderType = 1; // Dyanmic collider (allow moving again)
            camPhysics.mIsTriggered = false; // Enable collision response

            Logger.Log("first vel: " + camPhysics.rigidCom.Body().LinearVelocity().ToString());

            // Reset the camera's orientation
            Vector3 pointInFront = startThrowDir + gameObject.transform.position;
        	gameObject.transform.LookAt(pointInFront);

            sound.PlayIndependentEvent("CAM_TOSS_IMP.vente", false, 0);

            smc.SetState(flyingTowardsWall);
        }
    }
    public void State_NonActive_Exit()
    {
    	camMesh.setEnabled(true);
    }

    public void State_FlyTowardsWall_Enter()
    {
        timerWhenThrown = 0.0f;
        sound.PlayUniqueEvent(flyingSoundID, true);

        hasHitWall = false;
    }
    public void State_FlyTowardsWall_Update()
    {
#if ZERO
        // Check if sticky cam velocity has changed direction
        Vector3 dirWithoutY = startThrowDir;
        dirWithoutY.Y = 0.0f;
        float lenSqOfDirWithoutY = Vector3.Dot(dirWithoutY, dirWithoutY);
        Vector3 currVel = camPhysics.rigidCom.Body().LinearVelocity();
        currVel.Y = 0.0f;
        Logger.Log("currVel: " + currVel.ToString() + ", physics trigger: " + camPhysics.mIsTriggered.ToString());
        float dotPdt = Vector3.Dot(currVel, startThrowDir); // length of currVel
        float lenSq = Vector3.Dot(currVel, currVel);
        float difference = Math.Abs(lenSq - (dotPdt * dotPdt)/lenSqOfDirWithoutY) / lenSq;
        if (difference > 0.0001f) // Significant change in direction
        {
            OnCollidingAgainstWall(wallStuckOn);
        }
#endif
        Vector3 currVel = camPhysics.rigidCom.Body().LinearVelocity();
        currVel.Normalize();
        Logger.Log("Sticky Cam vel dir: " + currVel.ToString());

        if (!hasHitWall)
        {
            timerWhenThrown += FrameController.DT();
            if (timerWhenThrown >= timeAllowedToThrowOut)
            {
                smc.SetState(flyingBackToPlayer); // Stop the camera from going too far, start returning
            }
        }
    }
    public void State_FlyTowardsWall_Exit()
    {
        sound.StopUniqueEvent(flyingSoundID, false);
    }

    public void State_ActiveOnWall_Enter()
    {
        if (Common.GetStickyCamScript() != null)
        {
            Common.GetStickyCamScript().isActive = true;
            camRotLimitScript.UpdateVectors(); // Update the correct forward vector away from wall

            //Logger.Log("before forward: " + gameObject.transform.GetForwardVector());

            // Set the wall as parent, so rotation and translation will affect the sticky cam
            //gameObject.transform.SetParent(wallStuckOn.transform); // SOME SERIOUS ISSUE HERE

            //Logger.Log("after forward: " + gameObject.transform.GetForwardVector());

            gameObject.transform.SetScale(mInitialScale);
        }
    }
    public void State_ActiveOnWall_Update()
    {
        // When xbox control is pressed
        if (!Common.isPaused && Input.GetTriggered((char)0, buttonToThrowCam) != 0)
        {
            smc.SetState(flyingBackToPlayer);

            gameObject.transform.SetParent(null); // un-parent from wall
            gameObject.transform.SetScale(mInitialScale);
	    }
    }
    public void State_ActiveOnWall_Exit()
    {
    	camPhysics.mColliderType = 1; // Dyanmic collider (allow moving again)
    }

    public void State_FlyingBackToPlayer_Enter()
    {
    	camPhysics.mIsTriggered = true;
        Logger.Log("Cam coming back!");

        //sound.PlayIndependentEvent("CAM_RETR.vente", false, 0);
    }
    public void State_FlyingBackToPlayer_Update()
    {    
    	Vector3 offset = camera.transform.position;
        offset += (Vector3)camera.transform.GetForwardVector() * startthrowOffset.Z;
        offset += (Vector3)camera.transform.GetRightVector() * startthrowOffset.X;
        offset += (Vector3)camera.transform.GetUpVector() * startthrowOffset.Y;
            
    	// Fly towards player
        Vector3 camToPlayer = offset - gameObject.transform.position;
        float camToPlayerLenSq = camToPlayer.LengthSquared();
        // Check if reached near player
        if (camToPlayerLenSq <= 0.01f*throwSpeed*0.01f*throwSpeed)
        {
        	// Back to non active state
        	camPhysics.rigidCom.Body().SetLinearVelocity(new Vector3(0.0f)); // Stop the camera
        	camPhysics.mColliderType = 0; // Static collider

            //sound.PlayIndependentEvent("CAM_RETR.vente", false, 0);
        	
            smc.SetState(nonActive);
        }
        else // Keep following player's pos and move towards him
        {
            camToPlayer *= throwSpeed / (float)Math.Sqrt(camToPlayerLenSq);
	        // Set gameobject's physics to add force in that dir
	        camPhysics.rigidCom.Body().SetLinearVelocity(camToPlayer * FrameController.DT()*30.0f);

            // Set the surveillance camera view to move with the sticky cam flying back
            if (camScript.gameObjectParentedTo == gameObject)
            {
                cameraCam.transform.SetPosition(gameObject.transform.position);
            }
        }
    }
    public void State_FlyingBackToPlayer_Exit()
    {
        if (Common.GetStickyCamScript() != null)
        {
            Common.GetStickyCamScript().isActive = false;
            camRotLimitScript.ResetVectors();
        }
    }
}