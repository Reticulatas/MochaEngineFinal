using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TriggerAreaSnapRCCar : TriggerAreaBaseClass
{
    public override void OnStart()
    {
        base.OnStart();
    }

    public void OnUpdate()
    {
    
    }

    bool entered = false;
    public override void OnAreaEnter()
    {
        if (entered)
            return;
        entered = true;
        GameObject player = Common.GetStealthPlayer();
        Common.GetRCCar().transform.SetPosition(player.transform.position);
    }
    public override void OnArea()
    {

    }
}
