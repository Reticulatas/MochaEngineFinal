using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PuzzleBEnvironmentTrigger : TriggerAreaBaseClass
{

	PuzzleBEnvironmentController envScript;
	int _region = 0;
	
	public int Region  { get { return _region; } set { _region = value; }	}
	public override void OnStart()
    {
        base.OnStart();
        GameObject room = GameObject.GetGameObjectByName("TrapSequenceRoom2");
		envScript = GetScript<PuzzleBEnvironmentController>(room);

    }

    public void OnUpdate()
    {

    }

    public override void OnAreaEnter()
    {
		envScript.ActivateRegion(_region);
    }
    public override void OnArea()
    {

    }

}
 