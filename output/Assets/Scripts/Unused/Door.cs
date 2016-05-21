using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class Door : MochaScript
{
	GameObject player;

    const string doorSoundName = "Door Move.vente";
    CSound mSound;
    CPhysics doorPhysics;

	bool isOpen;
	public bool isLocked {get; set;}
	//public OPENING_DIRECTION openDir {get; set;}
	public int openDir {get; set;}
    Vector3 initPos; // original door pos
    float targetPosVal; // x or y pos value depending on openDir to move in
    public float timeToFullyOpen { get; set; }
    float currTime;
    public float distanceToMove { get; set; }
	bool isMoving;

    Vector3 sideDir; // perpendicular to normal
  	Vector3 normalDir; // direction the door is facing
	float currNormalDist;
  	int isNormalFrontPositive; // 0-not saved yet, -1 - negative normal is front, 1 - positive normal is front
  
	/*
	public enum OPENING_DIRECTION
	{
		OPEN_LEFT = 0,
		OPEN_RIGHT,
		OPEN_UP,
		OPEN_DOWN,
		
		NUM_OPENING_DIRECTIONS
	};
	*/
	
	public void OnStart()
	{
		player = Common.GetStealthPlayer();

        doorPhysics = gameObject.RequireComponent<CPhysics>();
        mSound = gameObject.RequireComponent<CSound>();

        isOpen = false;
		//openDir = 3;//OPENING_DIRECTION.OPEN_LEFT;
		initPos = gameObject.transform.position;
		targetPosVal = 0.0f;
        currTime = 0.0f;
        if (timeToFullyOpen == 0.0f)
            timeToFullyOpen = 1.0f;
        normalDir = gameObject.transform.GetForwardVector();
        sideDir = gameObject.transform.GetRightVector();
        currNormalDist = 0.0f;
        isNormalFrontPositive = 0;
	}

    public void OnCollisionEnter(CollisionData data)
    {
        // Logger.Log(data.collidedObj.GetName());
    }
	public void OnUpdate()
	{
		float distSqPlayerToDoor = Vector3.DistanceSquared(player.transform.position, initPos);
		const float distSqThreshold = 8.2f*8.2f;
		if (isOpen) // state open
		{
			// When player is away or when Locked, close it
			if (distSqPlayerToDoor >= distSqThreshold || isLocked)
			{
				MoveDoor(false);
        
		        // If we already know door's front, means player has already stepped through
		        if (isNormalFrontPositive != 0)
		        {
		            Vector3 doorToPlayer = player.transform.position.Sub(initPos);
                    float nextNormalDist = Vector3.Dot(doorToPlayer, normalDir);
                    if (nextNormalDist * (float)isNormalFrontPositive < 0.0f) // If player is out of range from the back of door
                    {
                        isLocked = true; // Lock it
                        Logger.Log("player is witin door's width. LOCKED");
                    }
		        }
			}
      
			// Check if player is going from infront the door to past the door
			if (isNormalFrontPositive == 0) // Keep checking if not known where front is yet
			{
				Vector3 doorToPlayer = player.transform.position.Sub(initPos);
                
                // Check if player is within the door frame
                float sideDist = Math.Abs(Vector3.Dot(doorToPlayer, sideDir));
                // Assumes door model to be unit width, so scale of 1 = 1 unit width
                if (sideDist <= gameObject.transform.scale.x * 0.5f * 4.0f) // right now door model is 4 unit width at scale 1 (14 feb)
                {
				    float nextNormalDist = Vector3.Dot(doorToPlayer, normalDir);
				    if (nextNormalDist * currNormalDist < 0.0f) // Transitioned from - to + or + to -
				    {
					    // Then the old normalDist sign held where the door's front is
					    isNormalFrontPositive = currNormalDist > 0.0f ? 1 : -1;
					    Logger.Log("normal is set: " + isNormalFrontPositive.ToString() );
				    }
				    currNormalDist = nextNormalDist; // Save the current frame's normal dist
                }
                else
                {
                    currNormalDist = 0.0f; // Reset the prev dist if not consecutive frames
                    // Logger.Log("player is outside of door's width.");
                }
			}
		}
		else // door is closed
		{
			// When player is close to door and it is not locked, open it
			if (distSqPlayerToDoor < distSqThreshold && !isLocked)
			{
				MoveDoor(true);
			}
		}
		
		UpdateDoorMovement();
	}
	
	public void MoveDoor(bool toOpen)
	{
		isOpen = toOpen;
		isMoving = true;

        // If opening, set to true to disable collision resolution
        doorPhysics.mIsTriggered = toOpen;
        if (toOpen)
        {
            // doorPhysics.mColliderType = 1;
        }
		
		switch (openDir)
		{
		case 0/*OPENING_DIRECTION.OPEN_LEFT */: targetPosVal = -distanceToMove; break;
        case 1/*OPENING_DIRECTION.OPEN_RIGHT*/: targetPosVal = distanceToMove; break;
        case 2/*OPENING_DIRECTION.OPEN_UP   */: targetPosVal = distanceToMove; break;
        case 3/*OPENING_DIRECTION.OPEN_DOWN */: targetPosVal = -distanceToMove; break;
		}

        if (openDir == 0 || openDir == 2) // Only 1 of the two doors plays the sound
        {
            string doorSoundName = toOpen ? "DOOR_OPEN.vente" : "DOOR_CLOSE.vente";
            mSound.PlayIndependentEventStatic3D(doorSoundName, initPos, false);
        }
	}
	
	public void UpdateDoorMovement()
	{
        if (isMoving)
		{
			float dt = FrameController.DT();
			
			if (isOpen)
			{
                currTime += dt;
                
                if (currTime >= timeToFullyOpen) // gone past the target, Stop opening
                {
                    currTime = timeToFullyOpen;
                    isMoving = false; // stop moving
                    doorPhysics.mColliderType = 0;
                }
			}
			else
			{
                currTime -= dt;

                if (currTime <= 0.0f) // gone past the start, Stop opening
                {
                    currTime = 0.0f;
                    isMoving = false; // stop moving
                    doorPhysics.mColliderType = 0;
                }
			}

            Vector3 rightOrUpVector = sideDir; // x
            if (openDir == 2 || openDir == 3)
			{
                rightOrUpVector = gameObject.transform.GetUpVector(); // y
			}
            Vector3 newPos = initPos + currTime / timeToFullyOpen * targetPosVal * rightOrUpVector;
            gameObject.transform.SetPosition(newPos);
		}
	}
	
}