using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class ConsoleUniqueMessageScript : TriggerAreaBaseClass
{
	int seqNum;
	public int SequenceNumber { get { return seqNum; }
        set { seqNum = value; } }
    public override void OnStart()
    {
		base.OnStart();
    }

    public void OnUpdate()
    {

    }

    public override void OnAreaEnter()
    {
        if (!isactive)
            return;
		switch(seqNum)
		{
			case 0:
				Common.GetConsoleScreenScript().QueueMessage("Subject [REDACTED] : Deduction test initialized"
						, 0.0f, false, true);
						break;
			case 1:
				Common.GetConsoleScreenScript().QueueMessage("Subject [REDACTED] : Physical resistance test initialized"
						, 0.0f, false, true);
						break;
			default:

						break;
		}
        isactive = false;
    }
    public override void OnArea()
    {

    }

}