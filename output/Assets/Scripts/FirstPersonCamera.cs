using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class FirstPersonCamera : MochaScript
{
	float deadTimer;
    float rtX; // input from mouse or xbox controller amount
    float rtY;
    float defaultXboxSensitity = 1.25f;
    public float xboxSensitivity { get { return defaultXboxSensitity; } 
        set { defaultXboxSensitity = value; } }
    float defaultMouseSensitivity = 1.1f;
    public float mouseSensitivity { get { return defaultMouseSensitivity; }
        set { defaultMouseSensitivity = value; } }

    float cosAngleThreshold = 0.98481f; // cos(10 degrees)
	public bool mouseLookOnly {get; set;}
	
	public bool FakeInput = false;

    Vector3 deadForwardVector;
    float deathAngleToRotate;
    const float totalTimeToDie = 3.5f;
    const float timeToDeathRotate = 0.9f;
    const float timeToFadeScreen = totalTimeToDie - 1.1f;
    int fadeState = 0; // 0-idle, 1-fade to black, 2-fade to transparent
    float fadeOpacity = 0.0f;

    GameObject lastSelectedObject;
	GameObject deathTurret;
	//CSound sound;
    CCamera playerCameraComponent;
	GameObject player;
    FirstPersonPlayer playerScript;
    GameObject playerBox;
	
	StateMachineController smc = new StateMachineController();
	State idle;
    State dead;
    public bool GetIsDead()
    {
        return smc.GetCurrentState() == dead;
    }

	public void OnStart()
	{
        if (gameObject.transform.GetParent() != null)
        {
            gameObject.transform.SetParent(null);
        }

        idle = new State(smc, State_Idle_Enter, State_Idle_Update, State_Idle_Exit);
        dead = new State(smc, State_Dead_Enter, State_Dead_Update, State_Dead_Exit);
        smc.SetState(idle);

		//sound = gameObject.RequireComponent<CSound>();
		//sound.Select("PLAYER_DEATH_TEMP.wav");
		//sound.Stop();
        
        rtX = 0;
        rtY = 0;
        playerCameraComponent = gameObject.RequireComponent<CCamera>();
		
		if (!mouseLookOnly) {
			player = Common.GetStealthPlayerMesh();

			playerBox = Common.GetStealthPlayer(); // GameObject.GetGameObjectByName("PlayerBoxCollider");
			playerScript = GetScript<FirstPersonPlayer>(playerBox);
			// Init player's facing to box's
			gameObject.transform.Rotate(playerBox.transform.GetRotation());
		}
        
        Common.SetOpacityToGameObject(fadeOpacity, Common.GetStealthFadeScreen());

        cosAngleThreshold = (float)Math.Cos(MathHelper.ToRadians(10.0f));
	}

	public void OnUpdate()
	{
		smc.Update();

        if (Input.GetHeld(0, "SnapGrid") != 0.0f && Input.GetTriggered(0, "2") != 0.0f) // Ctrl + 2 to kill player
        {
            Kill();
        }

        if (smc.GetCurrentState() != dead && !Common.isOptions)
		{
            // Follow the player around
			if (!mouseLookOnly)
			{
				gameObject.transform.SetPosition(playerBox.transform.position);
				gameObject.transform.Translate(0, 2.0f/*playerBox.transform.scale.y*/, 0);

				Vector3 camDir = gameObject.transform.GetForwardVector();
				camDir.Y = 0.0f;
				if (Vector3.Dot(camDir, camDir) > 0.0001f * 0.0001f) // if not a zero vector
				{
					camDir.Normalize();
					playerBox.transform.LookAt(playerBox.transform.position - camDir);
				}
			}
#if ZERO
			//select objects
       		GameObject gobj = PhysicEngine.getInstance().RayCast3DWithTag(
       			gameObject.transform.position,
        		gameObject.transform.GetForwardVector(), 10.0f, "Raycast");
        	if (gobj != null)
        	{
        		Invoke(gobj, "OnCursorOver", false);
        	    if (Input.GetTriggered(0, "Xbox_X") == 1.0f)
        		{
        			Invoke(gobj, "OnActivate", false);
        		}
        	}    
			if (lastSelectedObject != gobj)
			{
				if (lastSelectedObject != null)
					Invoke(lastSelectedObject, "OnCursorLeave", false);
				lastSelectedObject = gobj;
			}
#endif
		}
	}
	
	public bool CheckInput()
	{
        rtX = 0.0f;
        rtY = 0.0f;
        if (Common.isOptions || Common.isPaused)
            return false;

        if (playerScript.usingXboxControl)
        {
			if (FakeInput)
			{
				rtX = MonkeyInput.GetLookFloat()*xboxSensitivity;
				rtY = 0;
			}
			else
			{
				rtX = (float)Input.GetValue((char)0, "RThumbX") * xboxSensitivity;
				rtY = (float)Input.GetValue((char)0, "RThumbY") * xboxSensitivity;
			}
        }
        else
        {
            if (Input.GetLockMouse())
            {
                rtX = (float)Input.GetMouseDeltaX();
                rtY = (float)-Input.GetMouseDeltaY();
            }
        }
        if (rtX != 0 || rtY != 0)
        {
            return true;
        }
        else
        {
            return false;
        }
	}
    public void UpdateCameraRotFromInput()
    {
        float dx = 0.0f;
        float dy = 0.0f;
        if (playerScript.usingXboxControl)
        {
            float dt = FrameController.DT();

            dx = rtX * Math.Abs(rtX); // x^2 == [0, 1]
            dy = -rtY * Math.Abs(rtY);
            float scaleAmt = dt * 75.0f; // 75 deg in 1 sec
            dx *= scaleAmt;
            dy *= scaleAmt;
        }
        else
        {
            dx = rtX * 0.3f * mouseSensitivity; // 0.005f
            dy = -rtY * 0.3f * mouseSensitivity;

            const float maximumDInAFrame = 15.0f;
            dx = MathHelper.Clamp(dx, -maximumDInAFrame, maximumDInAFrame);
            dy = MathHelper.Clamp(dy, -maximumDInAFrame, maximumDInAFrame);
            // dx *= ((float)Math.PI / 180.0f);
            // dy *= ((float)Math.PI / 180.0f);
        }

        if (dx != 0.0f || dy != 0.0f)
        {
            //Vector3 currRot = gameObject.transform.GetConstrainedRotation();
            //Logger.Log("currRot: " + currRot.ToString());

            playerCameraComponent.Pitch(MathHelper.ToRadians(dy));
            playerCameraComponent.RotateY(MathHelper.ToRadians(dx));

            Vector3 forward = gameObject.transform.GetForwardVector();
            if (Math.Abs(Vector3.Dot(forward, Vector3.Up)) > cosAngleThreshold) 
            {
                playerCameraComponent.Pitch(MathHelper.ToRadians(-dy)); // Pitch it back
            }
        }
    }

    // Idle State
    public void State_Idle_Enter()
    {
    }
	public void State_Idle_Update()
    {
        if (CheckInput())
        {
            UpdateCameraRotFromInput();
        }

        if (fadeState == 2)
        {
            fadeOpacity -= 1.0f / timeToFadeScreen * FrameController.DT();
            if (fadeOpacity <= 0.0f)
            {
                fadeOpacity = 0.0f;
                fadeState = 0; // stop fading in
            }
            Common.SetOpacityToGameObject(fadeOpacity, Common.GetStealthFadeScreen());
        }
    }
	public void State_Idle_Exit()
    {
    }
    
    public void Kill()
    {
        if (smc.GetCurrentState() != dead)
            smc.SetState(dead);
    }

    // Dead State
    public void State_Dead_Enter()
    {
        deadTimer = totalTimeToDie;
        deadForwardVector = gameObject.transform.GetForwardVector();
        deathAngleToRotate = MMath.GetRandomInt(0, 2) == 0 ? -80.0f : 80.0f; // randomise ccw or cw

        // Fade out Oculus screen to black
        fadeState = 1;
        fadeOpacity = 0.0f;
        
        // deadX = gameObject.transform.position.x;
        // deadY = gameObject.transform.position.y;
        // deadR = gameObject.transform.rotation.Angles.z;

        // CLight cl = gameObject.RequireComponent<CLight>();
        // cl.SetLightType(0);
        // cl.SetRange(300.0f);
        // cl.SetSpot(10.0f);
        // cl.mInnerAngle = 10.0f;
        // cl.mOuterAngle = 200.0f;
        // cl.SetDiffuse(new Color(1.0f, 0.0f, 0.0f));
        // cl.SetAtt(20.0f);
    }
    public void State_Dead_Update()
    {
        float dt = FrameController.DT();
        deadTimer -= dt;

        if (deadTimer >= totalTimeToDie - timeToDeathRotate)
        {
            gameObject.transform.RotateAround(deadForwardVector, deathAngleToRotate / timeToDeathRotate * dt); // finish rotation in 1.2 sec
            // gameObject.transform.position.y = MMath.Cerp(deadY, 2.0f, deadTimer);
            // gameObject.transform.position.x = MMath.Cerp(deadX, -1.0f, deadTimer);
            // gameObject.transform.rotation.Angles.z = MMath.Cerp(deadR, -1.8f, deadTimer);
        }
        else if (deadTimer <= 0.0f)
        {
            playerScript.SetResetFromDead();
            smc.SetState(idle);
        }

        if (fadeState == 1)
        {
            fadeOpacity += 1.0f / timeToFadeScreen * dt;
            if (fadeOpacity >= 1.0f)
            {
                fadeOpacity = 1.0f;
                fadeState = 0; // stop fading in
            }
            Common.SetOpacityToGameObject(fadeOpacity, Common.GetStealthFadeScreen());
        }
    }
    public void State_Dead_Exit()
    {
        // gameObject.RequireComponent<CLight>().Destroy();

        // Print stuff console
        Common.GetConsoleScreenScript().QueueMessage("<#1,0,0,1#>Subject Handler has perished.", 0.0f, true, true);
        
        fadeState = 2; // Start fading out the black screen
    }
}
