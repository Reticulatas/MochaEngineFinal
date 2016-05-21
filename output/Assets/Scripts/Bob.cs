using MochaInterface;	
using System;
using Microsoft.Xna.Framework;

public class Bob : MochaScript
{
	public float BobAmount {get; set;}
	public float BobSpeed {get; set;}
	float startY;
	float timer;
	
	public void OnUpdate()
	{
		timer += FrameController.DT();
		gameObject.transform.SetPositionY(startY + (float)Math.Sin(timer*BobSpeed)*BobAmount,
			CTransform.TransformSpace.GLOBAL);
	}
	
	public void OnStart()
	{
		startY = gameObject.transform.position.y;		
	}
}