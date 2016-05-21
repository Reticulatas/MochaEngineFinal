using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class ConsoleSequenceInitiator : TriggerAreaBaseClass
{

	GameObject consoleScreen;
	OcuConsoleScreen consoleScript;

	int seqNum;
	public int SequenceNumber { get { return seqNum; }
        set { seqNum = value; } }
    public override void OnStart()
    {
		base.OnStart();
        
        consoleScreen = GameObject.GetGameObjectByName("ConsoleScreen");
		consoleScript = GetScript<OcuConsoleScreen>(consoleScreen);
    }

    public void OnUpdate()
    {

    }

    public override void OnAreaEnter()
    {
        if (!isactive)
            return;
		
		if( seqNum == 1)
			consoleScript.QueueMessage("Laboratory Access Sequence " + seqNum.ToString() + " : <#1,0,0,1#>RED  <#0,0,1,1#>BLUE  <#0,1,0,1#>GREEN"
                    , 0.0f, true, true);
		else
			consoleScript.QueueMessage("Laboratory Access Sequence " + seqNum.ToString() + " : <#0,0,1,1#>BLUE  <#0,1,0,1#>GREEN  <#1,0,0,1#>RED"
                    , 0.0f, true, true);

        isactive = false;
    }
    public override void OnArea()
    {

    }

}