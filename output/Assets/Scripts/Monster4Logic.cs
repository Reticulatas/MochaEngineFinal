using MochaInterface;
using System;
using Microsoft.Xna.Framework;

public class Monster4Logic : MochaScript
{
    #region VARIABLES   
    GameObject player;
    GameObject binds;
    CMeshRenderer mesh;
	CSound sound;
    StateMachineController smc;
	StateMachineController lightsmc;
	StateMachineController bindssmc;
	StateMachineController lightarraysmc;
    CParticleEmitter attackParticle, poofParticle;
	MonsterHelper helper;
	CLight mlight;
	OcuConsoleScreen screen;
	
	bool _isActive = false;
	public bool IsActive {get { return _isActive;} set {_isActive = value;}}

    const float RESPAWNTIME = 20.0f;
    const float RESPAWNTIMEVARIANCE = 1.0f;
    const float SPEED = 3.0f;
    const float BINDSOFFSET = 10.0f;
	const float SPAWNRADIUS = 50.0f;
	const float MAXMOVETIME = 1.3f;
	const float MINMOVETIME = 0.4f;
	const float FASTERMOVERADIUS = 50.0f;
	const float ATTACKRADIUS = 5.0f;
	const float INCOMINGRADIUS = 10.0f;

    float corruptiontimer;
	public int CorruptionLevel = 0;
	
    State respawning,       //invisible, not found
        hunting,            //appear/disappear behavior to player
        incoming,           //direct lunge to player (more speed)
        attacking_intro,    //spawning guards around player
        attacking,          //siphon corruption
		hurting,			//flail and re-spawn after
		dying,				//Kill()
		idle;				//nothing
		
	State turningoff,turningon, hold;
	State bindsmoveup, bindsmovedown, bindhold;

    #endregion

    public void OnStart()
    {
        smc = new StateMachineController();
		lightsmc = new StateMachineController();
        player = Common.GetStealthPlayer();
        binds = GameObject.GetGameObjectByName("Monster4Binds");
        attackParticle = GameObject.GetGameObjectByName("Monster4AttackParts").GetComponent<CParticleEmitter>();
		attackParticle.setEnabled(false);
        poofParticle = GameObject.GetGameObjectByName("Monster4Poof").GetComponent<CParticleEmitter>();
		mlight = gameObject.GetComponentInChildren<CLight>();
		sound = gameObject.RequireComponent<CSound>();

		screen = Common.GetConsoleScreenScript();
		
        mesh = GameObject.GetGameObjectByName("M4Mesh").RequireComponent<CMeshRenderer>();

        hunting = new State(smc, S_hunting_Start, S_hunting_Update, null);
        respawning = new State(smc, S_respawning_Start, null, S_respawning_End, RESPAWNTIME, hunting);
        attacking = new State(smc, S_attacking_Start, S_attacking_Update, S_attacking_End);
        attacking_intro = new State(smc, S_attacking_intro_Start, S_attacking_intro_Update, null, 1.0f, attacking);
        incoming = new State(smc, S_incoming_Start, S_incoming_Update, null, 3.0f, respawning);
		hurting = new State(smc, S_hurting_Start, S_hurting_Update, null, 1.0f, respawning);
		idle = new State(smc);
		dying = new State(smc, S_dying_Start, null, S_dying_End, 2.0f, idle);
		
		//light smc
		hold = new State(lightsmc, null, null, null);
		turningoff = new State(lightsmc, null, S_turningoff_Update, null, 0.5f, hold);
		turningon = new State(lightsmc, null, S_turningon_Update, null, 0.5f, hold);
		
		//binds smc
		bindssmc = new StateMachineController();
		bindhold = new State(bindssmc, null, null, null);
		bindsmoveup = new State(bindssmc, S_bindsmoveup_Start, S_bindsmoveup_Update, null, 0.5f, bindhold);
		bindsmovedown = new State(bindssmc, null, S_bindsmovedown_Update, S_bindsmovedown_End, 0.5f, bindhold);		
		
		helper = new MonsterHelper();
		
        smc.SetState(respawning);
		lightsmc.SetState(hold);
		
		DisplayCorruption();
    }

    public void OnUpdate()
    {
		if (IsActive) {
			gameObject.transform.LookAt(player.transform.position);

			smc.Update();
			//lightsmc.Update();
			bindssmc.Update();
		}
    }

    void KillPlayer()
    {
        screen.QueueMessage("<#1,0,0,1#>HANDLER CORRUPTION AT FATAL LEVELS.", 0.0f, true, false);
        Common.GetStealthPlayerCameraScript().Kill();
    }
	
	public void Kill()
	{
		if (smc.GetCurrentState() != dying)
			smc.SetState(dying);
	}

    #region HELPERS
	
	void DisplayCorruption()
	{
        if (CorruptionLevel == 50)
            screen.QueueMessage("<#1,0,0,1#>HANDLER CORRUPTION AT DAMAGING LEVELS.", 0.0f, true, false);
        if (CorruptionLevel == 80)
            screen.QueueMessage("<#1,0,0,1#>HANDLER CORRUPTION AT CRITICAL LEVELS.", 0.0f, true, false);
		screen.QueueMessage("<#1,0,0,1#>CORRUPTION LEVEL AT: " + CorruptionLevel.ToString() + "%", 0.0f, true, false);
	}

    void MoveTowardsPlayer(float amount)
    {
        Vector3 plVec = (Vector3)player.transform.position - (Vector3)gameObject.transform.position;

        plVec.Y = 0;
        plVec.Normalize();
        plVec *= amount;

        gameObject.transform.SetPosition(gameObject.transform.position + plVec);
    }

	void FindSpawnPoint()
	{
        Vector3 p = MMath.GetRandomPointInCircle(player.transform.position, SPAWNRADIUS);
		gameObject.transform.SetPosition(p);
        if (helper.CheckLights(gameObject))
        {
            gameObject.transform.SetPositionX(-10000);
            smc.SetState(respawning);
            Logger.Log("Monster4 could not spawn, respawning...");
        }
	}

    #endregion

    #region STATES
	void S_dying_Start()
	{
		//death
		sound.PlayIndependentEvent("C1_PAIN.vente", false, 2);
	}
	
	void S_dying_End()
	{
		gameObject.Destroy();
		sound.PlayIndependentEvent("C1_PAIN.vente", false, 2);
	}
	
	void S_bindsmoveup_Start()
	{
		binds.transform.SetPosition(player.transform.position);
        binds.transform.SetPositionY(player.transform.position.y - BINDSOFFSET);
		mlight.mRange = 10;
		mlight.mIntensity = 0.5f;
	}
	void S_bindsmovedown_Update()
	{
		binds.transform.SetPositionY(MMath.Cerp(player.transform.position.y-BINDSOFFSET,player.transform.position.y, bindsmovedown.GetPercentageTime()));
	}	
	void S_bindsmoveup_Update()
	{
		binds.transform.SetPositionY(MMath.Cerp(player.transform.position.y, player.transform.position.y-BINDSOFFSET, bindsmovedown.GetPercentageTime()));
	}	
	void S_bindsmovedown_End()
	{
		binds.transform.SetPositionY(-100);
		mlight.mRange = 2;
	}
	void S_turningoff_Update()
	{
		turningoff.MaxTimeInState = hunting.MaxTimeInState;
		mlight.mIntensity = MMath.Lerp(1.0f,0.0f,turningoff.GetPercentageTime());
	}
	void S_turningon_Update()
	{
		turningon.MaxTimeInState = hunting.MaxTimeInState;
		mlight.mIntensity = MMath.Lerp(0.0f,1.0f,turningon.GetPercentageTime());		
	}
    void S_respawning_Start()
    {		
		poofParticle.TriggerOneShot();

        mesh.setEnabled(false);
        gameObject.transform.SetPositionY(-100);

    }
    void S_respawning_End()
    {
		Logger.Log("Monster4 respawned");
		poofParticle.TriggerOneShot();
		FindSpawnPoint();
        mesh.setEnabled(true);
        gameObject.transform.SetPositionY(player.transform.position.y);
		sound.PlayIndependentEvent("C1_CHATTER.vente", false, 2);
    }
    void S_hunting_Start()
    {
		lightsmc.SetState(turningon);
		Logger.Log("Monster4 has moved to hunt");
        mesh.setEnabled(true);
    }
    void S_hunting_Update()
    {
        MoveTowardsPlayer(SPEED*FrameController.DT());

        float pldist = gameObject.transform.position.Distance(player.transform.position);

        if (helper.CheckLights(gameObject))
        {
            smc.SetState(hurting);
            return;
        }

        if (pldist < INCOMINGRADIUS)
        {
            smc.SetState(incoming);
            return;
        }
    }

    void S_incoming_Start()
    {
		sound.PlayIndependentEvent("C1_LAUGH.vente", false, 2);

		Logger.Log("Monster4 is incoming");
    }
    void S_incoming_Update()
    {
        MoveTowardsPlayer(SPEED * 5 * FrameController.DT());
		
		if (helper.CheckLights(gameObject)) {
			Logger.Log("Monster4 exposed to light");
			smc.SetState(hurting);
		}
		
		float pldist = gameObject.transform.position.Distance(player.transform.position);	
		
		if (pldist < ATTACKRADIUS)
		{
			smc.SetState(attacking_intro);
		}
    }
    void S_attacking_intro_Start()
    {
		sound.PlayIndependentEvent("C1_ANGRY.vente", false, 2);

		bindssmc.SetState(bindsmoveup);
        GetScript<FirstPersonPlayer>(player).LockMovement();
    }
    void S_attacking_intro_Update()
    {
		if (helper.CheckLights(gameObject)) {
			smc.SetState(hurting);
			bindssmc.SetState(bindsmovedown);
		}
    }
    void S_attacking_Start()
    {
        corruptiontimer = 0;
		Logger.Log("Monster4 is attacking");
        attackParticle.setEnabled(true);
        //attackParticle.mEmitterProperty.mAttractorPos = player.transform.position;
		//attackParticle.mEmitterProperty.mAttractorPos.y += 1.0f;
    }
    void S_attacking_Update()
    {
		//increase corruption
		//broadcast message to console
        corruptiontimer += FrameController.DT();
        if (corruptiontimer > 5.0f)
        {
            CorruptionLevel += 10;
            DisplayCorruption();
            if (CorruptionLevel >= 100)
                KillPlayer();
            corruptiontimer = 0;
        }
		
		if (helper.CheckLights(gameObject)) {
			smc.SetState(hurting);
		}
    }
    void S_attacking_End()
    {

		bindssmc.SetState(bindsmovedown);

        attackParticle.setEnabled(false);
    }
	void S_hurting_Start()
	{
		sound.PlayIndependentEvent("C1_PAIN.vente", false, 2);

		Logger.Log("Monster4 is hurting");

        GetScript<FirstPersonPlayer>(player).UnlockMovement();
		//play hurt sound
	}
	void S_hurting_Update()
	{
		//flail
	}
    #endregion

}