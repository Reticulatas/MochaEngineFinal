using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class RotateObjectY : MochaScript
{
	public void OnUpdate()
	{
		gameObject.transform.rotation.Angles.y += 1.1f;
	}
}