using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class EndTrigger : MochaScript
{
	GameObject player;
	
	public void OnStart()
	{
		player = GameObject.GetGameObjectByName("Camera");
	}

	public void OnUpdate()
	{
		float disttoend = player.transform.position.Distance(gameObject.transform.position);
		if (disttoend < 15.0f)
		{
			Engine.getInstance().SetPauseMode(false);
			// Sound.StopAllAudio();
			Map.LoadOnlyMap("Assets/Levels/mainMenu.mocha");
		}
	}
}
