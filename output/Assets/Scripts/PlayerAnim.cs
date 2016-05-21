using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PlayerAnim : MochaScript
{
	GameObject player;
	
	public void OnStart()
	{
		player = GameObject.GetGameObjectByName("Camera");
	}

	public void OnUpdate()
	{
		gameObject.transform.SetPosition(player.transform.position);
		gameObject.transform.Translate(0,-2.98f,0);
		gameObject.transform.rotation.Angles.y = player.transform.rotation.Angles.y + 3.14f;
	}
}
