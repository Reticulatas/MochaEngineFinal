using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class turret : MochaScript
{
    CSound mSound;
    GameObject player;
    FirstPersonCamera playerCamScript;

    const float radiusOfFire = 55.0f;
    float angleArcOfFire; // half angle
    float cosAngleArcOfFire;

    int currAngleIndex;
    int currentState; // 0-on guard, 1-rotating
    public float TIME_TO_ON_GUARD { get; set; }
    public float TIME_TO_ROTATE { get; set; }
    float timerToStayOnGuard;
    float timerToRotate;
    float startAngle;
	
	public float Angle1 {get; set;}
	public float Angle2 {get; set;}
	List<float> angleList; 
	// public List<float> angles {get { return _angles;} set { _angles = value; }}
	
	public void OnStart()
	{
        player = Common.GetStealthPlayer();
        playerCamScript = GetScript<FirstPersonCamera>(Common.GetStealthPlayerCamera());

		mSound = gameObject.RequireComponent<CSound>();
		//mSound.Stop("Turret_Rotate.vente");
        //mSound.Stop("Turret_Targetting.vente");
        // mSound.Volume(0.3f);
        //mSound.AddSound("Turret_LaserShot.vente");

        angleList = new List<float>();
        angleList.Add(Angle1);
        angleList.Add(Angle2);
        // Start with the turret at the first angle
        currAngleIndex = 0;
        gameObject.transform.rotation.Angles.y = angleList[currAngleIndex];
        currentState = 1;
        startAngle = gameObject.transform.rotation.Angles.y;

        angleArcOfFire = MathHelper.ToRadians(35.0f); // half angle
        cosAngleArcOfFire = (float)Math.Cos((double)angleArcOfFire);
        
        timerToStayOnGuard = TIME_TO_ON_GUARD;
        timerToRotate = TIME_TO_ROTATE;
	}

	public void OnUpdate()
	{
        float dt = FrameController.DT();

        Vector3 turretToPlayer = (Vector3)player.transform.position - (Vector3)gameObject.transform.position;
        turretToPlayer.Y = 0.0f;
        float turretToPlayerSq = Vector3.Dot(turretToPlayer, turretToPlayer);
        if (turretToPlayerSq > radiusOfFire * radiusOfFire)
            return; // Do not update if player is too far away
       
        if (currentState == 0)
        {
            timerToStayOnGuard -= dt;

            if (!playerCamScript.GetIsDead()) // Do not need to update if player is already dead
            {
                // Check if player is within sights, and not hiding behind a wall
                Vector3 turretDir = gameObject.transform.GetForwardVector();
                float dotPdt = Vector3.Dot(turretToPlayer, turretDir);
                // dotPdt^2 = lengthSq * cos(angle)^2. The smaller the angle, the larger the dot pdt
                if ((dotPdt > 0.0f) &&
                    (dotPdt * dotPdt >= cosAngleArcOfFire * cosAngleArcOfFire * turretToPlayerSq))
                {
                    turretToPlayer.Normalize();
                    Vector3 turretPos = (Vector3)gameObject.transform.position;
                    turretPos.Y = 1.0f;
                    // Raycast towards player and check if he is behind a wall
                    GameObject wall = PhysicEngine.getInstance().RayCast3DWithTag(
                        turretPos, turretToPlayer, 10000.0f, "TurretWall");
                    if (wall == null)
                    {
                        mSound.PlayIndependentEvent("Turret_LaserShot.vente", false, 1);
                        playerCamScript.Kill();
                    }
                }
            }

            if (timerToStayOnGuard <= 0.0f)
            {
                timerToRotate = TIME_TO_ROTATE; // Reset the timer
                currentState = 1; // Start rotating
                // mSound.Play("Turret_Targetting.vente"); //Turret_Rotate.vente");

                startAngle = gameObject.transform.rotation.Angles.y; // Save current angle
                // Select the angle to rotate to
                currAngleIndex = (currAngleIndex + 1 >= angleList.Count) ? 0 : currAngleIndex + 1;
            }
        }
        else // rotating
        {
            timerToRotate -= dt;
            // Tween the turret's rotation to target angle
            gameObject.transform.rotation.Angles.y += 
                (angleList[currAngleIndex] - startAngle) / TIME_TO_ROTATE * dt;

            if (timerToRotate <= 0.0f)
            {
                gameObject.transform.rotation.Angles.y = angleList[currAngleIndex];

                currentState = 0; // Start rotating
                timerToStayOnGuard = TIME_TO_ON_GUARD;
            }
        }
	}
}
