using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class Demo_SwitchBackFromLogo : MochaScript
{
	float timer;
	
   	public void OnStart()
	{
		timer = 0;
    }

	public void OnUpdate()
	{
		timer += FrameController.DT();
		
		if (timer > 120.0f)
		{
			Map.LoadMap("Assets/Levels/Demo.mocha");
			timer = 0;
			return;
		}
    }
}