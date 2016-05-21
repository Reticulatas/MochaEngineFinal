using Microsoft.Xna.Framework;
using MochaInterface;using System;

public class Monster3Light : StealthInteractableObject
{
	CLight light;
	public bool InitialLightState {get;set;}
	bool lightState;
	CSound sound;
	Monster4TriggerArea trigger;
	
	public override void OnStart()
	{
		light = gameObject.GetComponentInChildren<CLight>();
		light.setEnabled(false);
		lightState = InitialLightState;
		gameObject.GetComponent<CForwardRenderer>().mEmissiveColor = new Color(0.1f,0.1f,0.1f,1.0f);
		sound = gameObject.RequireComponent<CSound>();
		
		GameObject tr = GameObject.GetGameObjectByName("Monster4TriggerArea");
		if (tr != null) {
			trigger = GetScript<Monster4TriggerArea>(tr);
			trigger.M4LightsToTurnOff++;
			Logger.Log("M4Light to turn on, found for M4: #" + trigger.M4LightsToTurnOff.ToString());
		}
		
		base.OnStart();
	}
	
	public override void OnUpdate() {}
	
	public override void OnStealthPlayerMouseEnter() {
		if (!lightState)
		{
			gameObject.GetComponent<CForwardRenderer>().mEmissiveColor = new Color(0.5f,0.5f,0.5f,1.0f);
		}
	}
	
	public override void OnStealthPlayerMouseLeave() {
		if (!lightState)
		{
			gameObject.GetComponent<CForwardRenderer>().mEmissiveColor = new Color(0.1f,0.1f,0.1f,1.0f);
		}
	}	
	
	public void TurnLightOn()
	{
		lightState = true;
		light.setEnabled(true);
		gameObject.GetComponent<CForwardRenderer>().mEmissiveColor = new Color(1.0f,1.0f,1.0f,0.3f);
		sound.PlayIndependentEvent("GLASS_CRACK.vente", false, 2);
		
		if (trigger != null)
		{
			trigger.M4LightTurnedOn();
		}
	}
	
	bool interacted = false;
	public override void OnStealthPlayerMouseInteract()
	{
		if (interacted)
			return;
		interacted = true;
		TurnLightOn();
	}
}