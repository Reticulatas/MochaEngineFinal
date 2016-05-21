using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class DeathCheckpointTriggerArea : TriggerAreaBaseClass
{
    bool isdefault = false;
    public bool isDefault { get { return isdefault; }
        set { isdefault = value; } }

    Vector3 respawnpos = new Vector3();
    public Vector3 respawnPosition { get { return respawnpos; }
        set { respawnpos = (Vector3)value; } }
    Vector3 dLookingDir = new Vector3(0.0f,0.0f,1.0f);
    public Vector3 mLookingDir { get { return dLookingDir; }
        set { dLookingDir = (Vector3)value; } }

    public DeathCheckpointTriggerArea()
    {
        // respawnpos = new Vector3();
        // Logger.Log("created respawn");
    }

    public override void OnStart()
    {
        base.OnStart();

        if (respawnpos == Vector3.Zero) // default to checkpoint's position if 0
        {
            respawnpos = gameObject.transform.position;
        }
        if (isDefault)
        {
            Common.GetStealthPlayerScript().SetCheckpoint(gameObject);
        }
    }

    public void OnUpdate()
    {
        
    }

    public void RespawnToCheckpoint()
    {
        // Differentiate by checkpoint name
        if (gameObject.GetName() == "Checkpoint1a")
        {
            // Reset booby trap event and Unlock door to area 1B
            GameObject boobyControllerObj = GameObject.GetGameObjectByName("TSR_TrapController");//trapControllerName);
            BoobyTrapDeactivatorController controllerScript = GetScript<BoobyTrapDeactivatorController>(boobyControllerObj);
            controllerScript.ResetUponDeathOfPlayer();
        }
        else if (gameObject.GetName() == "Checkpoint3")
        {
            // Reset panels event
            GameObject deathControllerObj = GameObject.GetGameObjectByName("PuzzleRoom3");//trapControllerName);
            DeathPanelController controllerScript = GetScript<DeathPanelController>(deathControllerObj);
            controllerScript.ResetUponDeathOfPlayer();
        }

        // Reset the console screen
        Common.GetConsoleScreenScript().RestoreToSavedState();
    }
    public override void OnAreaEnter()
    {
        if (isactive)
        {
            // Update the player's respawn pos to this one
            Common.GetStealthPlayerScript().SetCheckpoint(gameObject);

            // Console screen needs to create a save state at this point
            Common.GetConsoleScreenScript().CreateSavedState();

            Logger.Log("checkpoint " + gameObject.GetName() + " triggered.");
            isactive = false;
            // gameObject.Destroy();
        }
    }
    public override void OnArea()
    {

    }
}
