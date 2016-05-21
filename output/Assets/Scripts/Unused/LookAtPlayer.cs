using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class LookAtPlayer : MochaScript
{
	GameObject player;
	
	public void OnStart()
	{
		player = GameObject.GetGameObjectByName("Camera");
	}

	public void OnUpdate()
	{
		gameObject.transform.LookAt(player.transform.position);
	}
}