using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class Elevator : MochaScript
{
    float startY;
    public float endY { get; set; }
    public float moveTime { get; set; }
	public bool TeleportToHub {get; set;}
	public int DoorToUnlock {get; set;}			//at top
	public bool RemoveAfterUse {get; set;}
	public bool ReturnAfterUse {get; set;}
	bool hasReturned = false;
    float timer;

	GameObject player;
    FirstPersonPlayer playerScript;
	CZone elevatorZone;

	List<DoorMain> doors;
	
    StateMachineController smc = new StateMachineController();
    State idle;
    State beginMoving;
    State moving;
	State done;
	State returning;
	CSound sound_;
	
	/*
	 * ELEVATOR ROOT (THIS OBJ) MUST BE ON THE FLOOR OF THE ELEVATOR FOR THIS TO WORK
	 */

    public void OnStart()
    {
		//dependencies
		doors = GetScriptsInChildren<DoorMain>(gameObject);
				
        idle = new State(smc);
        idle.OnEnter = State_Idle_Enter;
		idle.OnUpdate = State_Idle_Update;
		idle.OnExit = State_Idle_Exit;
		
		done = new State(smc);
		done.OnUpdate = State_done_Update;
		
        moving = new State(smc);
        moving.OnEnter = State_Moving_Enter;
		moving.OnUpdate = State_Moving_Update;
		moving.OnExit = State_Moving_Exit;
		moving.MaxTimeInState = moveTime;
		moving.NextState = done;
		
		returning = new State(smc);
		returning.OnEnter = State_returning_Enter;
		returning.OnUpdate = State_returning_Update;
		returning.OnExit = State_returning_Exit;
		returning.MaxTimeInState = moveTime;
		returning.NextState = idle;

        beginMoving = new State(smc, null, State_beginMoving_Update, null);

		player = Common.GetStealthPlayer();
        playerScript = Common.GetStealthPlayerScript();

		elevatorZone = gameObject.RequireComponent<CZone>();
		sound_ = gameObject.RequireComponent<CSound>();
		//sound_.AddSound("Elevator Start.vente");
		//sound_.AddSound("Elevator Stop.vente");

		smc.SetState(idle);
        timer = 0;
		startY = gameObject.transform.position.y;
		endY = startY + endY;
    }
	
	public void Reset()
	{
		smc.SetState(idle);
	}

    public void OnUpdate()
    {
        smc.Update();
    }

    public void OnEnd()   {    }

    ////
	
	void State_done_Update()
	{
		if (RemoveAfterUse)
		{
			if (Common.GetStealthPlayer().transform.position.Distance(gameObject.transform.position) > 50)
			{
				Logger.Log("Elevator removed, player far enough away");
				gameObject.Destroy();				
			}
		}
		else if (ReturnAfterUse && !hasReturned)
		{
			if (Common.GetStealthPlayer().transform.position.Distance(gameObject.transform.position) > 50)
			{
				Logger.Log("Elevator returning, player far enough away");
				hasReturned = true;
				smc.SetState(returning);
			}
		}
	}

    void State_beginMoving_Update()
    {
        if (!elevatorZone.IsPointWithin(Common.GetStealthPlayer().transform.position))
        {
            smc.SetState(idle);
            return;
        }

        foreach (DoorMain door in doors)
        {
            if (!door.GetIsClosed())
                return;
        }

        smc.SetState(moving);
    }
	
	void LockDoors()
	{
		//lock doors
        foreach (DoorMain door in doors)
        {
			door.MoveDoor(false, true);
            door.LockDoor();
        }
	}
	void UnlockAllDoors()
	{
		//unlock doors
        foreach (DoorMain door in doors)
        {
			door.MoveDoor(false, false);
            door.UnlockDoor();
        }
	}
	
	void State_returning_Enter()
	{
		LockDoors();
		timer = 0;
	}
	
	void State_returning_Exit()
	{
		gameObject.transform.SetPositionY(startY);
		UnlockAllDoors();
	}
	
	void State_returning_Update()
	{
		timer += FrameController.DT();
		if (moveTime == 0) {
			Logger.Log("Elevator moveTime is 0, cannot use script.");
			return;
		}
		float y = MMath.Cerp(endY, startY, MMath.Clamp01(timer / moveTime));
        gameObject.transform.SetPositionY(y);	
	}

    void State_Moving_Enter()
    {
		timer = 0;
		// sound_.StopAll();
		// sound_.Play("Elevator Start.vente");

        playerScript.LockMovement();

        sound_.PlayIndependentEvent("CAUTIOUS.vente", false, 1); // Play clunky elevator starting up
		
		//reset has returned, in case this elevator is used twice
		if (ReturnAfterUse)
			hasReturned = false;
		
		LockDoors();
    }
	
    void State_Moving_Update()
    {
        timer += FrameController.DT();
		if (moveTime == 0) {
			Logger.Log("Elevator moveTime is 0, cannot use script.");
			return;
		}
		
		if (timer/moveTime > 0.5f)
		{
			if (TeleportToHub)
			{
				GameObject marker = GameObject.GetGameObjectByName("hubelevatorteleportmarker");
				float distx = marker.transform.position.x - gameObject.transform.position.x;
				float distz = marker.transform.position.z - gameObject.transform.position.z;
				player.transform.SetPositionX(player.transform.position.x + distx);
				player.transform.SetPositionZ(player.transform.position.z + distz);
				gameObject.transform.SetPosition(marker.transform.position);
				TeleportToHub = false;
			}
		}
		
        float y = MMath.Cerp(startY, endY, MMath.Clamp01(timer / moveTime));
        gameObject.transform.SetPositionY(y);
        player.transform.SetPositionY(y+0.2f);
    }
	
    void State_Moving_Exit()
    {
		gameObject.transform.SetPositionY(endY);
        playerScript.UnlockMovement();
		
		if (DoorToUnlock < doors.Count)
		{
			DoorMain door = doors[DoorToUnlock];
            door.MoveDoor(true, false);
            door.UnlockDoor();
		}
        
    }
	
    void State_Idle_Enter()
    {
		// sound_.StopAll();
		// sound_.Play("Elevator Stop.vente");
    }
	
    void State_Idle_Update()
    {
        //check is player on me
        if (elevatorZone.IsPointWithin(Common.GetStealthPlayer().transform.position))
        {
            smc.SetState(beginMoving);
			return;
        }
    }
	
    void State_Idle_Exit()
    {
        timer = 0;
        startY = gameObject.transform.position.y;
    }
}