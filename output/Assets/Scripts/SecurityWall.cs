using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class SecurityWall : MochaScript
{
	StateMachineController smc = new StateMachineController();
	State idle;     
	State finished; 
	State moving;   
	float startY, endY;
	CSound sound;
	
	public void OnStart()
	{
        idle = new State(smc);
        finished = new State(smc);
        moving = new State(smc);
		startY = gameObject.transform.position.y;
		endY = gameObject.transform.position.y + 5;
		
		sound = gameObject.RequireComponent<CSound>();
		// sound.AddSound("Door Move.vente");
		smc.SetState(idle);
		
		moving.OnEnter = State_Moving_Enter;
		moving.OnUpdate = State_Moving_Update;
		idle.OnUpdate = State_Idle_Update;
		//moving.MaxTimeInState = 1.1f;
	}

	public void OnUpdate()
	{
		smc.Update();
	}
	
	public void State_Moving_Enter()
	{
		// sound.Play("Door Move.vente");
	}

  float timer;
	public void State_Moving_Update()
	{
		//Logger.Log(moving.GetPercentageTime().ToString());
    timer += FrameController.DT();

		gameObject.transform.SetPositionY(MMath.Cerp(startY, endY, MMath.Clamp01(timer/1.2f)));
	}
	
	public void State_Idle_Update()
	{
		if (Common.GetStealthPlayer().transform.position.Distance(gameObject.transform.position) < 15.0f)
		{
			smc.SetState(moving);
		}
	}
}
