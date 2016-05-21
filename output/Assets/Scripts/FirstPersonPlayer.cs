using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class FirstPersonPlayer : MochaScript
{
    public bool usingXboxControl { get; set; }
    float speedNormal = 0.5f;
    public float SpeedNormal
    {
        get { return speedNormal; }
        set { speedNormal = value; }
    }
    public float speedSprint = 0.9f;
    public float SpeedSprint
    {
        get { return speedSprint; }
        set { speedSprint = value; }
    }
    public float speedCrouch = 0.3f;
    public float SpeedCrouch
    {
        get { return speedCrouch; }
        set { speedCrouch = value; }
    }
    
    float jumpValue = 8.0F;
    public float JumpValue
    {
        get { return jumpValue; }
        set { jumpValue = value; }
    }
    float speedWhenJumped = 6.0F;
    public float SpeedWhenJumped
    {
        get { return speedWhenJumped; }
        set { speedWhenJumped = value; }
    }
	bool canJump;

	StateMachineController smc = new StateMachineController();
	StateMachineController sprint_smc = new StateMachineController();
    State idle;
    State move;
    State jump;
    State crouch;
    State sprinting, notsprinting;

    float speedX;
    float speedZ;
    float thumbstickMagnitude;
    float currSpeed;
    float sprintDeadPercent = 0.75F;
    public bool sprintLocked;
    bool movementLocked = false;
	
	//autoplay
	bool FakeInput = false;

    //bool prevIsPaused = false;
    //Vector3 lastPositionToFreezeAt;
    Vector3 currRespawnPosition;
    Vector3 currRespawnLookDir;
    DeathCheckpointTriggerArea currCheckpointScript = null;
	
	CSound mSound;
    Dictionary<String, uint> mSoundEvents;
    uint footstepID; // P1_FOOTSTEPS

	CAnimationController anim;
    CPhysics playerPhysics;
    GameObject playerCamera;
    GameObject player;
    GameObject playerBox;
    CMeshRenderer playerBoxMesh;
    FirstPersonCamera playerCamScript;
	
	
    public bool IsStealthPlayerInteracting()
    {
        if (Common.isPaused)
            return false;
        return ((usingXboxControl && Input.GetTriggered(0, "XboxInteract") != 0) ||
            (!usingXboxControl && Input.GetTriggered(0, "MouseLClick") != 0));
    }
    public Dictionary<String, uint> GetSoundEvents()
    {
        if (mSoundEvents == null)
            mSoundEvents = new Dictionary<String, uint>();
        return mSoundEvents;
    }
    public CSound GetSoundComponent()
    {
        if (mSound == null)
            mSound = gameObject.RequireComponent<CSound>();
        return mSound;
    }

	public void LockMovement()
	{
		movementLocked = true;
	}
	public void UnlockMovement()
	{
		movementLocked = false;
	}

    public void SetCheckpoint(GameObject obj)
    {
        currCheckpointScript = GetScript<DeathCheckpointTriggerArea>(obj, false);
        if (currCheckpointScript != null)
        {
            currRespawnPosition = currCheckpointScript.respawnPosition;
            currRespawnLookDir = currCheckpointScript.mLookingDir;
        }
    }
    public void SetResetFromDead()
    {
        gameObject.transform.SetPosition(currRespawnPosition); // Set to checkpoint
        Vector3 lookAtPos = gameObject.transform.position - currRespawnLookDir;
        gameObject.transform.LookAt(lookAtPos);
        playerCamera.transform.Rotate(gameObject.transform.GetRotation());

        if (currCheckpointScript != null)
        {
            currCheckpointScript.RespawnToCheckpoint();
        }
    }

	public void OnStart()
	{
        if (gameObject.transform.GetParent() != null)
        {
            gameObject.transform.SetParent(null);
        }

        speedX = 0;
    	speedZ = 0;
        currSpeed = speedNormal;
        speedWhenJumped = 0.0f;
		
		canJump = true;
        
        // playerBox is actually my own gameObject
        playerBox = gameObject; // GameObject.GetGameObjectByName("PlayerBoxCollider");
        playerBoxMesh = playerBox.RequireComponent<CMeshRenderer>();
        playerBoxMesh.setEnabled(false);
        playerCamera = Common.GetStealthPlayerCamera();
        playerCamScript = GetScript<FirstPersonCamera>(playerCamera);
        player = Common.GetStealthPlayerMesh();
        anim = player.RequireComponent<CAnimationController>();
        anim.mIsPlaying = true;

        playerPhysics = gameObject.RequireComponent<CPhysics>();
        playerPhysics.mColliderType = 1;

        idle = new State(smc, State_Idle_Enter, State_Idle_Update, State_Idle_Exit);
        move = new State(smc, State_Move_Enter, State_Move_Update, State_Move_Exit);
        jump = new State(smc, State_Jump_Enter, State_Jump_Update, State_Jump_Exit);
        crouch = new State(smc, State_Crouch_Enter, State_Crouch_Update, State_Crouch_Exit);

        sprinting = new State(smc, State_Sprinting_Enter, State_Sprinting_Update, State_Sprinting_Exit, 0.5f);
        sprinting.NextState = sprinting;
        notsprinting = new State(smc, State_NotSprinting_Enter, State_NotSprinting_Update, State_NotSprinting_Exit);

		smc.SetState(idle);
        sprint_smc.SetState(notsprinting);

        footstepID = GetSoundComponent().GetUniqueEvent("P1_FOOTSTEPS.vente", 0);
	}

    #region SPRINTING STATES
    void State_NotSprinting_Enter()
    {
        playerCamera.transform.rotation.Angles.z = 0;
    }
    void State_NotSprinting_Update()
    {

    }
    void State_NotSprinting_Exit()
    {

    }
    bool sprintDirection;
    float begAngle;
    void State_Sprinting_Enter()
    {
        sprintDirection = !sprintDirection;
        begAngle = playerCamera.transform.rotation.Angles.z;
    }
    void State_Sprinting_Update()
    {
        if (sprintDirection)
            playerCamera.transform.rotation.Angles.z = MMath.Lerp(begAngle, -0.1f, sprinting.GetPercentageTime());
        else
            playerCamera.transform.rotation.Angles.z = MMath.Lerp(begAngle, 0.1f, sprinting.GetPercentageTime());
    }
    void State_Sprinting_Exit()
    {
    }
    #endregion

    public void OnCollisionEnter(CollisionData data)
    {
        //TriggerAreaBaseClass triggerArea = GetScriptClass<TriggerAreaBaseClass>(data.collidedObj, false);
        //if (triggerArea != null) // If colliding against a trigger area
        //{
        //    triggerArea.OnAreaEnter();
        //}
        if (playerCamScript.GetIsDead())
            return;

		List<TriggerAreaBaseClass> triggers = new List<TriggerAreaBaseClass>();
		triggers = GetScripts<TriggerAreaBaseClass>(data.collidedObj, false);
		if(triggers.Count > 0)
		{
			foreach(TriggerAreaBaseClass obj in triggers)
			{
				obj.OnAreaEnter();
			}
		}
        else // If colliding against a real object
        {
            if (!canJump)
            {
                // Check if landed on the floor
                Vector3 upVec = new Vector3(0, 1, 0);
                for (int i = 0; i < data.numContacts; ++i)
                {
                    // if it's A then flip normal
                    Vector3 normal = data.normals[i];
                    if (data.isA) // if 
                        normal *= -1.0f;

                    float dotPro = Vector3.Dot(normal, upVec);
                    // still can't jump
                    if (dotPro <= 0)
                        return;
                }
                // If all dot products are > 0, then landed on floor
                canJump = true;
            }
        }
    }

    // idle state
	void State_Idle_Enter()
	{
		//Logger.Log("Idle");
		anim.mSingleAnim = AssetManager.getInstance().GetAsset("MaleCharacterIdle.anim");
		anim.mAnimSpeed = 0.6f;
	}
	void State_Idle_Update()
    {
        if (Input.GetTriggered((char)0, "XboxCrouch") != 0)
        {
            //smc.SetState(crouch); // Check for Crouch State
            return;
        }
        if (CheckXBoxLeftThumbInput() && PhysicEngine.getInstance().IsActive())
        {
            // playerPhysics.rigidCom.SetAwake(true);
            smc.SetState(move); // Check for Move State
            return;
        }
        if (CheckForInputIntoJump()) // Check for Jump state
        {
            return;
        }

        // Manually friction
        /*Vector3 currVel = (Vector3)playerPhysics.rigidCom.Body().LinearVelocity();
        float currVelY = currVel.Y;
        currVel.Y = 0.0f;
        if (currVel.LengthSquared() > 1.00f)
        {
            currVel *= 0.5f;
            currVel.Y = currVelY; // Do not reduce the y velocity
            playerPhysics.rigidCom.Body().SetLinearVelocity(currVel);
        }
        else*/
        {
		//Logger.Log(playerPhysics.rigidCom.Body().LinearVelocity().ToString());
		
            Vector3 currVel = playerPhysics.rigidCom.Body().LinearVelocity();
			if(currVel.Y * currVel.Y < 0.01f)
			{
				currVel.Y = 0.0f;
				playerPhysics.rigidCom.Body().SetAwake(false);
			}
            currVel.X = 0.0f;
            currVel.Z = 0.0f;
            playerPhysics.rigidCom.Body().SetLinearVelocity(currVel);
        }
		
    }
	void State_Idle_Exit()
	{
		
	}

    // movement state
    void State_Move_Enter()
    {
	//Logger.Log("Move");
        mSound.PlayUniqueEvent(footstepID, true);

		anim.mSingleAnim = AssetManager.getInstance().GetAsset("MaleCharacterWalk.anim");
		anim.mAnimSpeed = 1.0f;

        currSpeed = speedNormal;
    }
    void State_Move_Update()
    {
        if (CheckXBoxLeftThumbInput() && PhysicEngine.getInstance().IsActive() && FrameController.DT() < (1.0f/5.0f))
        {
            // based off of thumbstick magnitude, if keyboard use default 1
            currSpeed = speedNormal;
            float thresholdForSprint = speedSprint * 16.0f * sprintDeadPercent;
            if ((thumbstickMagnitude < sprintDeadPercent) &&
                playerPhysics.rigidCom.Body().LinearVelocity().LengthSq() < thresholdForSprint * thresholdForSprint)
            {
                // unlock sprint if there is no movement on this thumbstick
                sprintLocked = false;
            }
            if (sprintLocked || Input.GetValue((char)0, "LThumb") != 0 || // left thumbstick for xbox
                Input.GetHeld(0, "MoveFaster") != 0) // 'shift' for keyboard
            {
                currSpeed = speedSprint;
                //if (sprint_smc.GetCurrentState() != sprinting)
                //    sprint_smc.SetState(notsprinting);
                
				// lock the sprint
                sprintLocked = true;
            }

            ModifySpeed(currSpeed);
        }
        else
        {
            // sprint_smc.SetState(notsprinting);
            smc.SetState(idle);
            return;
        }

        if (CheckForInputIntoJump()) // check jump
            return;
        if (Input.GetTriggered((char)0, "XboxCrouch") != 0)
        {
            //smc.SetState(crouch); // Check for Crouch State
            return;
        }
    }
    void State_Move_Exit()
    {
        mSound.StopUniqueEvent(footstepID, false); // do not release
    }

    // jump state
    void State_Jump_Enter()
    {
        Vector3 vel = playerPhysics.rigidCom.Body().LinearVelocity();
        vel.Y = jumpValue;
        playerPhysics.rigidCom.Body().SetLinearVelocity(vel * FrameController.DT() * 30.0f);

        speedWhenJumped = currSpeed; // *FrameController.DT() * 30.0f;

        canJump = false;
    }
    void State_Jump_Update()
    {
        if (canJump) // If grounded back
        {
            smc.SetState(idle);
            return;
        }

        if (CheckXBoxLeftThumbInput())
        {
            ModifySpeed(0.9f);
        }
    }
    void State_Jump_Exit()
    {

    }

    // crouch state
    void State_Crouch_Enter()
    {
        currSpeed = speedCrouch;
    }
    void State_Crouch_Update()
    {
        if (Input.GetTriggered((char)0, "XboxCrouch") != 0)
        {
            smc.SetState(idle);
            return;
        }
        if (CheckForInputIntoJump()) // Check for Jump state
        {
            return;
        }

        if (CheckXBoxLeftThumbInput())
        {
            ModifySpeed(currSpeed);
        }

        // Scale the height down when in crouch
        /*if (playerBox.transform.scale.y > 1.0f)
        {
            playerBox.transform.scale.y = Math.Max(1.0f, 
                playerBox.transform.scale.y - 1.0f / 0.6f * FrameController.DT());
        }*/
    }
    void State_Crouch_Exit()
    {
        
    }

    void ModifySpeed(float scale)
    {
        float speed = 16.0f;
        float dt = FrameController.DT();
        scale *= thumbstickMagnitude; // scale the supposed speed on the thumbstick mag

        Vector3 dir = playerCamera.transform.GetForwardVector();
        dir.Y = 0;
        dir.Normalize(); // direction for z axis
        Vector3 right = playerCamera.transform.GetRightVector();
        right.Y = 0;
        right.Normalize(); // direction for x axis

        Vector3 currVel = playerPhysics.rigidCom.Body().LinearVelocity();
        float currentYVelocity = currVel.Y;

        Vector3 newVel = new Vector3(); // has no y component here
        newVel = right * speedX; // update X
        newVel += dir * speedZ; // update Z
        newVel.Normalize();
        newVel = newVel * scale * speed; // scalar for velocity (less speed in air)

        if (smc.GetCurrentState() == jump)
        {
            float limitSpeedInJump = speedWhenJumped * speed;

            newVel = currVel + newVel * dt;
            newVel.X = MathHelper.Clamp(newVel.X, -limitSpeedInJump, limitSpeedInJump);
            newVel.Z = MathHelper.Clamp(newVel.Z, -limitSpeedInJump, limitSpeedInJump);
        }
        else
        {
            newVel.Y = 0;//currentYVelocity;
        }
        playerPhysics.rigidCom.Body().SetLinearVelocity(newVel * dt * 30.0f);

#if ZERO
        float max = 33.0f;
        // cap it to max
        if (newVel.LengthSquared() < max * max)
        {
            newVel.Y = yVel;
            playerPhysics.rigidCom.Body().SetLinearVelocity(newVel);
        }
        else
        {
            newVel.Normalize();
			newVel *= max;
            newVel.Y = yVel;
            playerPhysics.rigidCom.Body().SetLinearVelocity(newVel);
        }
#endif
    }
    bool CheckXBoxLeftThumbInput()
    {
        if (usingXboxControl)
        {
			if (FakeInput)
			{
				speedX = 0.0f;
				speedX += MonkeyInput.GetMovementXFloat();// Input.GetHeld(0, "right");
				//speedX -= MonkeyInput.GetMovementFloat();
				speedZ = 0.0f;
				speedZ += MonkeyInput.GetMovementZFloat();
				//speedZ -= MonkeyInput.GetMovementFloat();
				thumbstickMagnitude = 1;
			}
			else
			{	
				// update how much left thumb is pressed
				speedX = Input.GetValue((char)0, "LThumbX");
				speedZ = Input.GetValue((char)0, "LThumbY");
				Vector2 stickInput = new Vector2(speedX, speedZ);
				// only use values for movment if the thumbstick is out of the deadzone
				thumbstickMagnitude = stickInput.Length();
			}
        }
        else
        {
				speedX = 0.0f;
				speedX += Input.GetHeld(0, "right");
				speedX -= Input.GetHeld(0, "left");
				speedZ = 0.0f;
				speedZ += Input.GetHeld(0, "up");
				speedZ -= Input.GetHeld(0, "down");
				thumbstickMagnitude = 1;
        }

        if (movementLocked || Common.isPaused)
        {
            speedX = 0.0f;
            speedZ = 0.0f;
        }

        /*
        if (Common.isPaused) // Prevent player inputs when paused
        {
            speedX = 0.0f;
            speedZ = 0.0f;
        }

        
        if (Common.isPaused) // Prevent player inputs when paused
        {
            if (prevIsPaused != Common.isPaused)
            {
                lastPositionToFreezeAt = gameObject.transform.GetGlobalPosition();
                playerPhysics.mIsGravityOn = false;
            }
            gameObject.transform.SetPosition(lastPositionToFreezeAt);
            speedX = 0.0f;
            speedZ = 0.0f;
        }
        else
        {
            if (prevIsPaused != Common.isPaused)
            {
                playerPhysics.mIsGravityOn = true;
            }
        }
        prevIsPaused = Common.isPaused;*/

        if (speedX == 0 && speedZ == 0)
            return false;
        else
            return true;
    }
    bool CheckForInputIntoJump()
    {
        return false;

        String inputForJump = usingXboxControl ? "XboxJump" : "Space";
        if (Input.GetTriggered((char)0, inputForJump) != 0 && canJump)
        {
            // playerPhysics.rigidCom.SetAwake(true);
            smc.SetState(jump);
            return true;
        }
        return false;
    }

	public void OnUpdate()
    {
		if (Input.GetTriggered(0, "Autoplay") > 0.0f)
		{
			FakeInput = !FakeInput;
			playerCamScript.FakeInput = FakeInput;
		}	
		
		if (Input.GetTriggered(0, "F1") > 0.0f)
		{
			Map.LoadOnlyMap("Assets/Levels/Monster3.mocha");
			return;
		}
		
        if (!playerCamScript.GetIsDead())
        {
            smc.Update();
            // sprint_smc.Update();
        }
        else if (smc.GetCurrentState() != idle) // if dead, set to idle
        {
            smc.SetState(idle);
            playerPhysics.rigidCom.Body().SetLinearVelocity(Vector3.Zero); // Stop moving immediately
        }

        if (Input.GetTriggered(0, "1") == 1.0f) // Ctrl + 1 to lock mouse
        {
            if (Input.GetHeld(0, "SnapGrid") != 0.0f)
            {
                Input.SetMouseLock(!Input.GetLockMouse());
            }
        }
		
		//snap player to ground if there is one
		Vector3 ray_pos = gameObject.transform.position;
		//ray_pos.Y += 1.0f;
		RayCastResult output;
		output = PhysicEngine.getInstance().RayCast3D_Output(ray_pos, new Vector3(0,-1,0), 1.5f);
		if (output.hit)
		{

			//we found the ground
			gameObject.transform.SetPositionY(output.intersection.y + 0.35f);
			
			Vector3 currVel = playerPhysics.rigidCom.Body().LinearVelocity();
			currVel.Y = 0.0f;

			playerPhysics.rigidCom.Body().SetLinearVelocity(currVel);
			//Logger.Log(output.go.GetName());
			//Logger.Log(output.intersection.y.ToString());
		}

		
        // Scale the height down when in crouch
        /*if (smc.GetCurrentState() != crouch && playerBox.transform.scale.y < 2.0f)
        {
            playerBox.transform.scale.y = Math.Min(2.0f,
                playerBox.transform.scale.y + 1.0f / 0.6f * FrameController.DT());
        }*/
	}
}
