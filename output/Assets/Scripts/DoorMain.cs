using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class DoorMain : MochaScript
{
	GameObject player;

    GameObject ldDoor; // left or down
    GameObject ruDoor; // right or up
    const string doorSoundName = "Door Move.vente";
    CSound mSound;
    CPhysics doorPhysics;

    public bool isSingleDoor { get; set; }
    public bool isOneWayDoor { get; set; }

    bool isManualMode = false;
	bool isOpen;
	public bool isLocked {get; set;} 
	//public OPENING_DIRECTION openDir {get; set;}
	public int openDir {get; set;}
    Vector3 initPos; // original door pos
    Vector3 ldInitPos; // original door pos
    Vector3 ruInitPos; // original door pos
    float targetPosVal; // x or y pos value depending on openDir to move in
    float defaultTimeToFullyOpen = 1.0f;
    public float timeToFullyOpen { get { return defaultTimeToFullyOpen; }
        set { defaultTimeToFullyOpen = value; } }
    float defaultDistanceToMove = 2.0f;
    public float distanceToMove { get { return defaultDistanceToMove; }
        set { defaultDistanceToMove = value; } }
    float defaultDistToOpenForPlayer = 5.0f;
    public float distToOpenForPlayer { get { return defaultDistToOpenForPlayer; }
        set { defaultDistToOpenForPlayer = value; } }
    public float distToCloseForPlayer { get; set; }
    float currTime;
	bool isMoving;

    Vector3 sideDir; // perpendicular to normal
  	Vector3 normalDir; // direction the door is facing
    Vector3 upDir; // the up direction of the door
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

    public bool GetIsClosed()    {
        return (!isMoving && !isOpen);
    }
    public bool GetIsOpen()    {
        return (!isMoving && isOpen);
    }
	
	public void OnStart()
	{
		player = Common.GetStealthPlayer();

        doorPhysics = gameObject.RequireComponent<CPhysics>();
        mSound = gameObject.RequireComponent<CSound>();

        if (isSingleDoor)
        {
            ruDoor = gameObject;
        }
        else
        {
            uint numChildren = gameObject.transform.GetNumberOfChildren();
            if (numChildren >= 2)
            {
                for (uint i = 0; i < numChildren; ++i)
                {
                    GameObject currChild = gameObject.transform.GetChild(i).gameObject;
                    String name = currChild.GetName();
                    if (ldDoor == null && name == Common.nameForLeftDoor)
                    {
                        ldDoor = currChild;
                    }
                    else if (ruDoor == null && name == Common.nameForRightDoor)
                    {
                        ruDoor = currChild;
                    }
                }
            }
        }

        isOpen = false;
		//openDir = 3;//OPENING_DIRECTION.OPEN_LEFT;
		initPos = gameObject.transform.position;
		targetPosVal = 0.0f;
        currTime = 0.0f;
        if (timeToFullyOpen == 0.0f)
            timeToFullyOpen = 1.0f;
        normalDir = gameObject.transform.GetForwardVector();
        sideDir = gameObject.transform.GetRightVector();
        upDir = gameObject.transform.GetUpVector();
        if (ruDoor != null)
        {
            ruInitPos = ruDoor.transform.position;
            if (ldDoor != null)
                ldInitPos = ldDoor.transform.position;

            // normalDir = ruDoor.transform.GetForwardVector();
            // sideDir = ruDoor.transform.GetRightVector();
            // upDir = ruDoor.transform.GetUpVector();
        }
        currNormalDist = 0.0f;
        isNormalFrontPositive = 0;
        if (distToCloseForPlayer <= 0.0f) // if set to 0 or negative, default to open distance
        {
            distToCloseForPlayer = distToOpenForPlayer;
        }
	}

    public void OnCollisionEnter(CollisionData data)
    {
        // Logger.Log(data.collidedObj.GetName());
    }
	public void OnUpdate()
	{
        Vector3 playerPosWOY = player.transform.position;
        playerPosWOY.Y = 0.0f;
        Vector3 initPosWOY = initPos;
        initPosWOY.Y = 0.0f;
        float distSqPlayerToDoor = Vector3.DistanceSquared(playerPosWOY, initPosWOY);
		if (isOpen) // state open
		{
            if (!isManualMode) // Only allow auto closing if NOT manual mode
            {
                // If we already know door's front, means player has already stepped through
                bool ifPlayerIsInFront = true;
                if (isNormalFrontPositive != 0 && isOneWayDoor) // Only lock if it is NOT one-way door
                {
                    // If player is past the door
                    Vector3 doorToPlayer = player.transform.position.Sub(initPos);
                    float nextNormalDist = Vector3.Dot(doorToPlayer, normalDir);
                    if (nextNormalDist * (float)isNormalFrontPositive < 0.0f) // If player is out of range from the back of door
                    {
                        ifPlayerIsInFront = false;
                        // When player is far away enough, close and lock it
                        if (distSqPlayerToDoor >= (distToCloseForPlayer * distToCloseForPlayer) || isLocked)
                        {
                            MoveDoor(false, false);

                            isLocked = true; // Lock it
                            Logger.Log("player is within door's width. LOCKED");
                        }
                    }
                }
                if (ifPlayerIsInFront)
                {
                    // When player is away or when Locked, close it
                    if (distSqPlayerToDoor >= (distToOpenForPlayer * distToOpenForPlayer) || isLocked)
                    {
                        MoveDoor(false, false);
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
                if (sideDist <= distanceToMove)
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
            if (!isManualMode) // if manual, don't try to open door
            {
                // When player is close to door and it is not locked, open it
                if (distSqPlayerToDoor < (distToOpenForPlayer * distToOpenForPlayer) && !isLocked)
                {
                    MoveDoor(true, false);
                }
            }
		}
		
		UpdateDoorMovement();
	}
	
	public void MoveDoor(bool toOpen, bool manualMode)
	{
		isOpen = toOpen;
		isMoving = true;
        isManualMode = manualMode; // true to stay open if opening

        // If opening, set to true to disable collision resolution
        doorPhysics.mIsTriggered = toOpen;
        if (toOpen)
        {
            // doorPhysics.mColliderType = 1;
        }
		
		switch (openDir)
		{
		case 0/*OPENING_DIRECTION.OPEN_LEFT */: targetPosVal = distanceToMove; break;
        case 1/*OPENING_DIRECTION.OPEN_RIGHT*/: targetPosVal = distanceToMove; break;
        case 2/*OPENING_DIRECTION.OPEN_UP   */: targetPosVal = distanceToMove; break;
        case 3/*OPENING_DIRECTION.OPEN_DOWN */: targetPosVal = distanceToMove; break;
		}

        string doorSoundName = toOpen ? "SCI_H_DOOR_OPEN.vente" : "SCI_H_DOOR_CLOSE.vente"; // for openDir == 0/1
        if (openDir == 2 || openDir == 3)
        {
            doorSoundName = toOpen ? "SCI_V_DOOR_OPEN.vente" : "SCI_V_DOOR_CLOSE.vente";
        }
        mSound.PlayIndependentEventStatic3D(doorSoundName, initPos, false);
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
                rightOrUpVector = upDir; // y
			}
            if (isSingleDoor && (openDir == 0 || openDir == 3))
            {
                rightOrUpVector *= -1.0f; // left or down is negative direction
            }
            Vector3 newPosLB = ldInitPos + currTime / timeToFullyOpen * -distanceToMove * rightOrUpVector;
            Vector3 newPosRU = ruInitPos + currTime / timeToFullyOpen * distanceToMove * rightOrUpVector;
            if (ruDoor != null)
            {
                ruDoor.transform.SetPosition(newPosRU);
                if (ldDoor != null)
                    ldDoor.transform.SetPosition(newPosLB);
            }
		}
	}
	public void LockDoor()
	{
		isLocked = true;
		//PLAY LOCK SOUND HERE
	}
	public void UnlockDoor()
	{
		isLocked = false;
        isNormalFrontPositive = 0; // Reset the testing for which side is player on
        currNormalDist = 0.0f;
		//PLAY UNLOCK SOUND HERE
	}
	
}