using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class AddPhy : MochaScript
{
	CPhysics Camcphy;
	CBoxColliderComponent bcc;
    CSound sound;
    int testIndex;

	public void OnStart()
	{
		// Camcphy = gameObject.RequireComponent<CPhysics>();
		// bcc     = gameObject.RequireComponent<CBoxColliderComponent>();
        sound = gameObject.RequireComponent<CSound>();
        testIndex = 0;
	}
	public void OnUpdate()
	{
        String[] soundNames = new String[] /*{
            "Combat1", "Combat2", "Door Move", "Elevator Start", "Elevator Stop", 
            "Footstep", "heartbeat", "Hum", "LightTurnOn", "NonCombat", "static", 
			"swell_ambience", "Turret_LaserShot", "Turret_Rotate", "Turret_Targetting"
        };*/
        {
            "C_MOVEMENT", "C_RETREAT", "CAM_FLIGHT", "CAM_RETR", "CAM_TOSS_IMP", 
            "CONSOLE_CLICK", "CONSOLE_CONFIRM", "CONSOLE_DENY", "CONSOLE_TV_CLICK", "CREATURE_CLOSE", "DOOR_CLOSE", 
			"DOOR_OPEN", "DOOR_UNLOCK", "ENV_BIO_AC", "ENV_BIO_DEF", "ENV_SCI_AC", "ENV_SCI_DEF", "ENV_SCI_LIGHT", 
            "GEN_ALARM", "GENERAL_LOOP", "LONG_STABS", "MED_STABS", "MON_TV_CHANNEL", "MON_TV_OFF", "MON_TV_ON", 
            "P_CAM_RETR_A", "P_CAM_RETR_B", "P_CAM_TOSS", "P_CAM_TOSS_IMP", "P_FOOTSTEPS", "P_PAPER_DOWN", "P_PAPER_UP", 
            "PINPAD_CONFIRM", "PINPAD_DENY", "PINPAD_INPUT", "RC_CAR_MOVE", "SHORT_STABS", "SPEAKER_CONFIRM", "SPEAKER_DENY", 
            "TREE_RUSTLE", 
        };

        if (Input.GetTriggered(0, "ArrowUp") != 0.0f)
        {
            testIndex = Math.Min(testIndex + 1, soundNames.Length - 1);
            Logger.Log("sound: " + soundNames[testIndex]);
        }
        if (Input.GetTriggered(0, "ArrowDown") != 0.0f)
        {
            testIndex = Math.Max(testIndex - 1, 0);
            Logger.Log("sound: " + soundNames[testIndex]);
        }
        if (Input.GetTriggered(0, "2") != 0.0f)
        {
            bool toLoop = false;
            if (Input.GetHeld(0, "SnapGrid") != 0.0f)
                toLoop = true;
            sound.PlayIndependentEvent(soundNames[testIndex] + ".vente", toLoop, 1);
        }
	}
}