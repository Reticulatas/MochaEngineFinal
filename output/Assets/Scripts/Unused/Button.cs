using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class Button : MochaScript
{
	GameObject recv_obj;
	Vector3 origScale;
	CMeshRenderer cr;
	int counter;
	
	public void OnStart()
	{
		recv_obj = GameObject.GetGameObjectByName("pp_base");
		origScale = gameObject.transform.scale;
		cr = gameObject.RequireComponent<CMeshRenderer>();
	}

	public void OnUpdate()
	{
		counter++;
		if (counter == 20)
			cr.GetMaterial().SetColor(0.0f,0.0f,0,1.0f);
	}
	
	public void OnCursorOver()
	{
		gameObject.transform.scale = origScale*1.2f;
	}
	
	public void OnCursorLeave()
	{
		
		gameObject.transform.scale = origScale;
	}
	
	public void OnActivate()
	{
		Invoke(recv_obj, gameObject.GetName());
		cr.GetMaterial().SetColor(1.0f,1.0f,0,1.0f);
		
		counter = 0;
	}
}