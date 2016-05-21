using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class BGAudio : MochaScript
{
	CSound sound_;
	int index_;
	float timer_;
	float maxTime_;
	
	public void OnStart()
	{
		sound_ = gameObject.RequireComponent<CSound>();
        // sound_.PlayIndependentEvent("swell_ambience.vente", true);
        /*
		sound_.AddSound("Hum.vente");
		sound_.AddSound("Combat1.vente");
		sound_.AddSound("Combat2.vente");
        sound_.AddSound("NonCombat.vente");
		//sound_.Volume(0.25f);
		sound_.Play("NonCombat.vente");
		//sound_.Play("Door Move.vente");
		sound_.Loop(true,"Hum.vente");
		//sound_.Volume(0.5f);
		
		sound_.LoopAll(true);
		index_ = 0;
		maxTime_ = 8.0f;
		timer_ = 0.0f;
		
		gameObject.transform.SetParent(Common.GetStealthPlayer().transform);
		gameObject.transform.SetPosition(0.0f,0.0f,0.0f);
        */
	}

    int testIndex = 0;
	public void OnUpdate()
	{
        if (Input.GetTriggered(0, "1") == 1.0f) // Ctrl + 1 to lock mouse
        {
            if (Input.GetHeld(0, "SnapGrid") != 0.0f)
            {
                sound_.StopAllGlobalEvents();
            }
            else
            {
                sound_.StopAllMyEvents(false);
            }
        }
        if (Input.GetTriggered(0, "2") == 1.0f)
        {
            
        }

		String[] soundNames = new String[] /*{
            "Combat1", "Combat2", "Door Move", "Elevator Start", "Elevator Stop", 
            "Footstep", "heartbeat", "Hum", "LightTurnOn", "NonCombat", "static", 
			"swell_ambience", "Turret_LaserShot", "Turret_Rotate", "Turret_Targetting"
        };*/
        /*{
            "C_MOVEMENT", "C_RETREAT", "CAM_FLIGHT", "CAM_RETR", "CAM_TOSS_IMP", 
            "CONSOLE_CLICK", "CONSOLE_CONFIRM", "CONSOLE_DENY", "CONSOLE_TV_CLICK", "CREATURE_CLOSE", "DOOR_CLOSE", 
			"DOOR_OPEN", "DOOR_UNLOCK", "ENV_BIO_AC", "ENV_BIO_DEF", "ENV_SCI_AC", "ENV_SCI_DEF", "ENV_SCI_LIGHT", 
            "GEN_ALARM", "GENERAL_LOOP", "LONG_STABS", "MED_STABS", "MON_TV_CHANNEL", "MON_TV_OFF", "MON_TV_ON", 
            "P_CAM_RETR_A", "P_CAM_RETR_B", "P_CAM_TOSS", "P_CAM_TOSS_IMP", "P_FOOTSTEPS", "P_PAPER_DOWN", "P_PAPER_UP", 
            "PINPAD_CONFIRM", "PINPAD_DENY", "PINPAD_INPUT", "RC_CAR_MOVE", "SHORT_STABS", "SPEAKER_CONFIRM", "SPEAKER_DENY", 
            "TREE_RUSTLE", 
        };*/
		{
            "AMB_CAVES",
			"AMB_HALLWAY", 
			"AMB_HUB", 
			"AMB_MAZE", 
			//"AMB_MONSTER_ROOM", 
			"AMB_SCIENCE", 
			//"B_LOOP", 
			"BUTTON_GREEN", 
			"BUTTON_RED", // LONG DELAY AT START, FIX
			"BUTTON_YELLOW", 
			"C1_ANGRY", 
			"C1_CHATTER", 
			"C1_CLOSE_LOOP", 
			"C1_LAUGH", 
			"C1_PAIN", 
            "C2_CLOSE_LOOP",
			"CAM_FLIGHT", 
			"CAM_LIGHT", 
			"CAM_RETR", 
			"CAM_ROTATE", 
			"CAM_TOSS_IMP", 
            "CAUTIOUS",
            "CAV_V_DOOR_CLOSE",
            "CAV_V_DOOR_OPEN",
            "F_LIGHT_FIZZLE",
            "F_LIGHT_LOOP",
            "F_LIGHT_SWITCH_OFF",
            "F_LIGHT_SWITCH_ON",
			//"F_LOOP", 
			"GEN_ALARM", 
			"GLASS_CRACK", 
			//"H_DOOR_CLOSE", 
			//"H_DOOR_OPEN", 
            "HORN",
			//"K_LOOP", 
            "MUSIC_CREDITS",
			"MUSIC_ESCAPE", 
			"MUSIC_HUB_LOOP",
            "MUSIC_INTRO_MENU",
            "ORCHESTRAL",
            "P1_CAM_RETR_A", 
            "P1_CAM_RETR_B", 
            "P1_CAM_TOSS", 
            "P1_CAM_TOSS_IMP", 
			"P1_FOOTSTEPS",
            //"P2_CLICK",
            //"P2_CONFIRM",
            //"P2_DENY",
            //"P2_PAPER_DOWN",
            //"P2_PAPER_UP",
            "P2_MON_TEXT",
            "P2_TV_CHANNEL",
            //"P2_TV_CLICK",
            //"P2_TV_OFF",
            //"P2_TV_ON",
            "PERCUSSIVE",
            "PERCUSSIVE_A",
            "PERCUSSIVE_B",
            "PINPAD_CONFIRM",
            "PINPAD_DENY",
            "PINPAD_INPUT",
            "RC_CAR_MOVE",
            "RISE_A",
            "RISE_B",
            "RISE_C",
            "RISE_D",
            "SPEAKER_CONFIRM",
            "SPEAKER_DENY",
            "SCI_H_DOOR_CLOSE",
            "SCI_H_DOOR_OPEN",
            "SCI_V_DOOR_CLOSE",
            "SCI_V_DOOR_OPEN",
            "TREE_RUSTLE",
            //"V_DOOR_CLOSE",
            //"V_DOOR_OPEN",

		};
	    
	    if (Input.GetTriggered(0, "ArrowRight") != 0.0f)
	    {
            testIndex = Math.Min(testIndex + 1, soundNames.Length - 1);
		    Logger.Log("sound: " + soundNames[testIndex]);
	    }
	    if (Input.GetTriggered(0, "ArrowLeft") != 0.0f)
	    {
		    testIndex = Math.Max(testIndex - 1, 0);
		    Logger.Log("sound: " + soundNames[testIndex]);
	    }
	    if (Input.GetTriggered(0, "3") != 0.0f)
	    {
            sound_.PlayIndependentEvent(soundNames[testIndex] + ".vente", false, 1);
	    }
	/*
		timer_ += FrameController.DT();
		
		if(timer_ > maxTime_)
		{
			timer_ = 0.0f;
			switch(index_)
			{
			case 0:
			sound_.Stop("NonCombat.vente");
			sound_.Play("Combat1.vente");
			break;
			
			case 1:
			sound_.Stop("Combat1.vente");
			sound_.Play("Combat2.vente");
			break;
			
			case 2:
			sound_.Stop("Combat2.vente");
			sound_.Play("NonCombat.vente");
			break;
			}
			index_ = (index_ + 1 ) % 3;
		}*/
	}
}