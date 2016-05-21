using MochaInterface;
using System;

public class RotateObject : MochaScript
{
	public string hellotext {get; set;}
	public int helloint {get; set;}

	public void OnStart()
	{
		Logger.Log("text: " + hellotext);
	}

	public void OnUpdate()
	{
        gameObject.transform.Rotate(FrameController.DT(), 0, 0);
	}
}