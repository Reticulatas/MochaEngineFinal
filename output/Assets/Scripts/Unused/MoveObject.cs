using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class MoveObject : MochaScript
{
	public float velX { get; set; }
	public float velY { get; set; }
	public float velZ { get; set; }

	public void OnStart()
	{

	
	}
	public void OnUpdate()
	{
		float dt = FrameController.DT()*2;
		gameObject.transform.SetPositionX(gameObject.transform.position.x + velX*dt);
		gameObject.transform.SetPositionY(gameObject.transform.position.y + velY*dt);
		gameObject.transform.SetPositionZ(gameObject.transform.position.z + velZ*dt);
	}
}