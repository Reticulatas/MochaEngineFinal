using MochaInterface;
using Microsoft.Xna.Framework;	
using System;
using System.Collections.Generic;

public class Credits : MochaScript
{
	public void OnStart()
	{
	}

	public void OnUpdate()
	{
		float right = Input.GetTriggered(0,"right");
		float left = Input.GetTriggered(0,"left");
		
		if(left > 0.0f || right < 0.0f || Input.GetTriggered(0,"exit") > 0.0f || Input.GetTriggered(0,"MenuBack") > 0.0f)
		{
			Map.UnloadMap();
		}
	}
}
