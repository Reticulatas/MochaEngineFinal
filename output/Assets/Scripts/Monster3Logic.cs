using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class Monster3Logic : MochaScript
{
	float DEFAULT_STALK_RANGE = 15.0f;
	float STALK_RANGE;
    const float MAX_DIST_FROM_PLAYER = 70.0f;
	
	StateMachineController smc;
	State resetting, stalking, incoming, attacking, panicking, inactive;
	
	//cross link
	//CMeshRenderer cmesh;	
	Monster3Woman monster3Woman;
	GameObject stealth_player;
	CSound sound;
	MonsterHelper helper = null;

	//vars
	int stalk_dir;
	
	public void OnStart()
	{
		STALK_RANGE = DEFAULT_STALK_RANGE;
		
		//dependencies
		//cmesh = gameObject.RequireComponent<CMeshRenderer>();
		stealth_player = Common.GetStealthPlayer();
		monster3Woman = GetScript<Monster3Woman>(GameObject.GetGameObjectByName("Monster3Woman"));
		sound = gameObject.RequireComponent<CSound>();
	
		//states
		smc = new StateMachineController();
        inactive = new State(smc, null, null, null);
        incoming = new State(smc, State_Incoming_Start, State_Incoming_Update, null);
		stalking = new State(smc, State_Stalking_Start, State_Stalking_Update, null, 5.0f, incoming);
		resetting = new State(smc, State_Reset_Start, State_Reset_Update, null, 18.0f, stalking);
		attacking = new State(smc, State_Attacking_Start, State_Attacking_Update, State_Attacking_End, 4.0f, inactive);
        panicking = new State(smc, State_Panicking_Start, State_Panicking_Update, State_Panicking_End, 1.5f, resetting);

		helper = new MonsterHelper();
		
		smc.SetState(resetting);
	}

    int checkLightTimer = 0;
	public void OnUpdate()
	{
		gameObject.transform.LookAt(stealth_player.transform.position);
		
        checkLightTimer++;
        if (checkLightTimer > 4)
        {
			if (smc.GetCurrentState() != panicking && helper.CheckLights(gameObject))
            {
                //we are in the light
                smc.SetState(panicking);
            }
            checkLightTimer = 0;
        }
		smc.Update();
	}
	
	public void GetSteerVectors(ref Vector3 to_player, ref Vector3 steer_dir, ref Vector3 perp_circle)
    {
        //compute circle steer vectors
        to_player = (Vector3)stealth_player.transform.position - (Vector3)gameObject.transform.position;
        to_player.Normalize();
        float x = to_player.X;
        float z = to_player.Z;
        to_player.Y = 0;
        x = -x;

        perp_circle = new Vector3(z, 0, x);
        if (stalk_dir == 0)
            perp_circle = -perp_circle;
        perp_circle.Normalize();

        steer_dir = MMath.Slerp(perp_circle, to_player, stalking.GetPercentageTime());
        steer_dir.Normalize();
    }

    #region STATES
    void State_Stalking_Start()
	{
		//turn on the mesh and move him near the player
		//cmesh.setEnabled(true);	
		//find somewhere near the player to spawn
		STALK_RANGE = DEFAULT_STALK_RANGE;
        for (int times_to_attempt_spawn = 0; times_to_attempt_spawn <= 5; ++times_to_attempt_spawn)
        {
            gameObject.transform.SetPosition(MMath.GetRandomPointInCircle(stealth_player.transform.position, STALK_RANGE));
            if (!helper.CheckLights(gameObject))
                break;
			STALK_RANGE += 5.0f;
            if (times_to_attempt_spawn == 5)
                smc.SetState(resetting);
        }

		stalk_dir = MMath.GetRandomInt(0,2);
		
		sound.PlayIndependentEvent("C1_CLOSE_LOOP.vente",false,2);
	}
	
	void State_Stalking_Update()
	{
        Vector3 to_player = new Vector3();
        Vector3 steer_dir = new Vector3();
        Vector3 perp_circle = new Vector3();
        GetSteerVectors(ref to_player, ref steer_dir, ref perp_circle);

        //reset if player gets too far away
        if (Vector3.Distance(stealth_player.transform.position, gameObject.transform.position) > MAX_DIST_FROM_PLAYER)
        {
            smc.SetState(resetting);
            return;
        }

        //head towards the perp 
        gameObject.transform.LookAt((Vector3)stealth_player.transform.position);
        gameObject.transform.localPosition += perp_circle * FrameController.DT() * 15.0f;
	}

    void State_Incoming_Start()
    {
        gameObject.transform.LookAt(stealth_player.transform.position);
        Logger.Log("Monster3 is incoming");
		monster3Woman.RunTo();
		sound.PlayIndependentEvent("C1_ANGRY.vente",false,2);
    }

    void State_Incoming_Update()
    {
        Vector3 to_player = new Vector3();
        Vector3 steer_dir = new Vector3();
        Vector3 perp_circle = new Vector3();
        GetSteerVectors(ref to_player, ref steer_dir, ref perp_circle);

        //head towards the steer
        gameObject.transform.LookAt(stealth_player.transform.position);
        gameObject.transform.localPosition += steer_dir * FrameController.DT() * 19.0f;

        //if he gets close enough, start the kill procedure
        if (gameObject.transform.position.Distance(stealth_player.transform.position) < 6.0f)
        {
            smc.SetState(attacking);
            return;
        }
    }
	
	void State_Reset_Start()
	{
		sound.PlayIndependentEvent("C1_PAIN.vente", false, 2);

		//turn off the mesh and move him away
		//cmesh.setEnabled(false);
		gameObject.transform.SetPosition(0,-100,0);
        Logger.Log("Monster3 has reset");
		
		resetting.MaxTimeInState = MMath.GetRandomLimitedFloat(18.0f,30.0f);
	}
	
	void State_Reset_Update()
	{
		
	}
	
	void State_Attacking_Start()
	{
        Logger.Log("Monster3 attacks player");
		monster3Woman.RunTo();
		sound.PlayIndependentEvent("C1_CHATTER.vente", false, 2);
	}
	
	void State_Attacking_Update()
	{
		
	}
	
	void State_Attacking_End()
	{
        GetScript<FirstPersonCamera>(Common.GetStealthPlayerCamera()).Kill();
    }
	
	Vector3 beginRot;
	void State_Panicking_Start()
	{
		monster3Woman.RunTo();
        //set animation
        Logger.Log("Monster3 is panicking");
		//monster3Woman.RunAway();
		sound.PlayIndependentEvent("C1_PAIN.vente", false, 2);
		beginRot = gameObject.transform.GetConstrainedRotation();
	}
	
	void State_Panicking_Update()
	{
        gameObject.transform.Rotate(beginRot * MMath.GetRandomLimitedFloat(0.9f,1.1f));
		sound.PlayIndependentEvent("C1_PAIN.vente", false,2);
		
	}
	
	void State_Panicking_End()
	{
		gameObject.transform.Rotate(beginRot);
		
        //kill animation
		monster3Woman.Reset();
    }
    #endregion


}