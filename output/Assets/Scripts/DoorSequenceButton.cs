using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class DoorSequenceButton : StealthInteractableObject //InteractableObjectBaseClass 
{
	public String doorName { get; set; }	
	public bool isButtonActive { get; set; }
	public bool completed;

    GameObject panelParent;
	GameObject DoorUnlock;
	DoorUnlockSequence doorUnlockSeq;
	String doorButtonTag = "doorButton";
	Mouse playerMouse;
    GameObject player;
    FirstPersonPlayer playerScript; // to know whether using Xbox controller
	
	CSound mSound;
	
	// Use this for initialization
	public override void OnStart () 
    {
        base.OnStart();
		
		mSound = gameObject.RequireComponent<CSound>();
		
        DoorUnlock = GameObject.GetGameObjectByName("DoorUnlockSequence");
		doorUnlockSeq = GetScript<DoorUnlockSequence>(DoorUnlock);
        panelParent = gameObject.transform.GetParent().GetParent().gameObject;

		playerMouse = GetScript<Mouse>(Common.GetStealthPlayerMouse() );
        player = Common.GetStealthPlayer();
        playerScript = GetScript<FirstPersonPlayer>(player);
    }
	
	public override void OnUpdate () 
    {
	}

    public override void OnStealthPlayerMouseEnter()
    {
        
    }
    public override void OnStealthPlayerMouseLeave()
    {

    }
    public override void OnStealthPlayerMouseInteract()
    {
		if(completed)
			return;
		
        if (isButtonActive)
        {
            if (doorName != null)
            {
                doorUnlockSeq.SetParticipants(gameObject, panelParent, doorName);
            }

			doorUnlockSeq.CenterPull(1);
			
            mSound.PlayIndependentEvent("PINPAD_INPUT.vente", false, 1);
        }
    }
}
