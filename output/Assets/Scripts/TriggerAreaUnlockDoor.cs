using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TriggerAreaUnlockDoor : TriggerAreaBaseClass
{
  
    public String DoorToOpen { get; set; }

    DoorMain doorScript;
    private bool isUnlocked = false;

    public override void OnStart()
    {
        base.OnStart();

       
    }

    public void OnUpdate()
    {
    
    }

    public override void OnAreaEnter()
    {
        if (isUnlocked)
            return;

         GameObject door = GameObject.GetGameObjectByName(DoorToOpen);
        if (door != null)
        {
            GetScript<DoorMain>(door).isLocked = false;
        }

        isUnlocked = true;
    }
    public override void OnArea()
    {

    }
}
