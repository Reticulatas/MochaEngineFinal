using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class Fps : MochaScript
{
	CFontRenderer fontComp;
	
	public void OnStart()
	{
		fontComp = gameObject.RequireComponent<CFontRenderer>();
	}

	public void OnUpdate()
	{
		float dt = FrameController.DT();
		dt = 1.0f / dt;
		string fpsStr = dt.ToString();
		fontComp.mText = fpsStr;
	}
}