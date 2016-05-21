using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class IntroDigipen : MochaScript
{
	float timer_;
	
	public void OnStart()
	{
		timer_ = 0.0f;
	}

	public void OnUpdate()
	{
		timer_ += FrameController.DT();
		if(timer_ > 1.0f)
		{
			timer_ = 0.0f;
			Map.LoadMap("Assets/Levels/introDevices.mocha");
		}
	}
}
