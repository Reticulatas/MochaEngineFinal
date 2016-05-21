using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class HowToPlaySurveillance : MochaScript
{
	public void OnStart()
	{
	}

	public void OnUpdate()
	{
		float right = Math.Max(Input.GetTriggered(0,"right"),Input.GetTriggered(0,"arrowRight"));
		float left = Math.Max(Math.Abs(Input.GetTriggered(0,"left")),Input.GetTriggered(0,"arrowLeft"));
		
		if(left > 0.0f || right < 0.0f || Input.GetTriggered(0,"exit") > 0.0f || Input.GetTriggered(0,"MenuBack") > 0.0f)
		{
			Map.UnloadMap();
		}
	}
}