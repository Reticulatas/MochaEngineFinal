using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MazeSoundStopTriggerArea : TriggerAreaBaseClass
{
    public override void OnStart()
    {
        base.OnStart();
    }

    public void OnUpdate()
    {
        
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
            GameObject mazeTrigger = GameObject.GetGameObjectByName(Common.prefix + "MazeSoundEventTrigger");
            if (mazeTrigger == null)
                Logger.Log("Could not find MazeSoundEventTrigger to stop.");
            else
            {
                MazeSoundTriggerArea mazeScript = GetScript<MazeSoundTriggerArea>(mazeTrigger);
                mazeScript.TriggerReleaseAmbient();
            }

            isactive = false;
        }
    }
    public override void OnArea()
    {

    }
}
