using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TutorialMonsterTriggerScare : TriggerAreaBaseClass
{
	CSkinMeshRenderer girl;
	float timer = 0;
	GameObject player;

    CSound sound;
	
	public override void OnStart()
	{		
		base.OnStart();
		
		player = Common.GetStealthPlayer();
	
		girl = GameObject.GetGameObjectByName("MonsterGirl_Hall").GetComponent<CSkinMeshRenderer>();
		girl.setEnabled(false);

        sound = gameObject.RequireComponent<CSound>();
	}
		
	public void OnUpdate()
	{	
		if (timer > 0.0f)
		{
			timer -= FrameController.DT();
			if (timer <= 0.0f)
			{
				girl.setEnabled(false);
				GetScript<FirstPersonPlayer>(player).UnlockMovement();
			}
		}
	}
		
	bool activated = false;
	public override void OnAreaEnter()
    {
		if (activated)
			return;
		activated = true;
		Logger.Log("Boo!");
		girl.gameObject.transform.SetPosition(player.transform.position - (Vector3)player.transform.GetForwardVector()*1.5f);
        girl.gameObject.transform.SetPositionY(gameObject.transform.position.y - 0.1f);
		girl.setEnabled(true);
		timer = 1.5f;
		GetScript<FirstPersonPlayer>(player).LockMovement();

        sound.PlayIndependentEvent("HORN.vente", false, 0);
    }
    
	public override void OnArea()
    {
    }
}