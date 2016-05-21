using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class Monster3Woman : MochaScript
{
	GameObject player;
	GameObject monster3;
	float speed;
	float timer;
	StateMachineController smc;
	State normal, runTo, killing, runAway;
	float beginY;	//y to bob from
	float MINDISTTOPLAYER = 7.0f;
	float MINDISTTOMONSTER = 3.0f;
	float MINTIMETOKILL = 2.0f;
	
    public void OnStart()
    {
		smc = new StateMachineController();
		normal = new State(smc, Normal_Start, Normal_Update, null);
		runTo = new State(smc, null, runTo_Update, null);
		runAway = new State(smc, null, runAway_Update, null, 5.0f, normal);
		killing = new State(smc, killing_Start, killing_Update, null);
		smc.SetState(normal);
		
        player = Common.GetStealthPlayerCamera();
		monster3 = GameObject.GetGameObjectByName("Monster3");
		speed = 3.0f;
		timer = 0;
		
		beginY = gameObject.transform.position.y;
    }

    public void OnUpdate()
    {
		smc.Update();
		timer += FrameController.DT();
	}
	
	void Bob()
	{
		//bob head
		gameObject.transform.SetPositionY(beginY + (float)Math.Sin(timer));	
	}
	
	void LookAtPlayer()
	{
		//look at player
        gameObject.transform.LookAt(player.transform.position);
	}
	
	float DistanceTo(Vector3 pt)
	{
		return (Vector3.Distance(gameObject.transform.position,pt));
	}
	
	void MoveTowards(Vector3 pt, float speedmod)
	{
		Vector3 dir = pt - gameObject.transform.position;
		dir.Y = 0;
		dir.Normalize();
			
		gameObject.transform.Translate(dir*FrameController.DT()*speed*speedmod);
	}
	
	public void Normal_Start()
	{
		Logger.Log("Monster3Woman entered normal roaming state");
	}
	
	public void Normal_Update()
	{		
		LookAtPlayer();
		Bob();
		
		//move towards monster3
		if (DistanceTo(monster3.transform.position) > MINDISTTOMONSTER)
		{
			MoveTowards(monster3.transform.position, 2.0f);
		}
    }    
	
	public void RunTo()
	{
		smc.SetState(runTo);
	}
	public void Reset()
	{
		smc.SetState(normal);
	}
	public void RunAway()
	{
		smc.SetState(runAway);
	}
	
	public void runTo_Update()
	{
		LookAtPlayer();
		Bob();
		
		//move towards player
		if (DistanceTo(monster3.transform.position) > MINDISTTOPLAYER)
		{
			MoveTowards(monster3.transform.position, 5.0f);
		}
		else
		{
			smc.SetState(runTo);
		}
	}
	
	public void runAway_Update()
	{
		Vector3 mp = monster3.transform.position;
		Vector3 pp = player.transform.position;
		Vector3 d = mp + ((mp-pp) * 20);
		
		MoveTowards( d, 12.0f);
	}
	
	float killTimer = 0;
	public void killing_Start()
	{
		Logger.Log("Monster3Woman killing player");
		killTimer = 0;
	}
	
	public void killing_Update()
	{
		LookAtPlayer();
		Bob();
		
		killTimer += FrameController.DT();
		
		if (killTimer > MINTIMETOKILL)
		{
			killTimer = 0;
			Logger.Log("Player killed by Monster3Woman");
		}
	}
}
