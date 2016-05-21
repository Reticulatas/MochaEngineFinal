using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TransformRiserScript : MochaScript
{

	GameObject parent;
	CTransform transform;
	Vector3 pos;
	Vector3 newPos;
	float currRise = 0.0f;
	float riseAmt = 0.0f;
	float timer = 0.0f;
	
	float countDown;
	
	bool active = false;
	bool finished = false;
	public float RiseAmount { get { return riseAmt; }
        set { riseAmt = value; } }
	public float Timer { get { return timer; }
        set { timer = value; } }
	public bool IsActive { get { return active; }
        set { active = value; } }
    public void OnStart()
    {
		transform = gameObject.transform;
		pos = transform.GetPosition(CTransform.TransformSpace.GLOBAL);
		newPos = pos;
		
		if(riseAmt != 0.0f || timer != 0.0f)
		{
			countDown = timer;
		}
    }

    public void OnUpdate()
    {
		if(!active || riseAmt == 0.0f || timer == 0.0f || finished)
			return;
		
		float dt = FrameController.DT();
		
		countDown -= dt;
		
		currRise = (1.0f - (countDown / timer)) * riseAmt;
		
		newPos = pos;
		newPos.Y += currRise;
		
		transform.SetPositionY(newPos.Y);
		
		if(countDown < 0.0f)
			finished = true;
    }
	
	public void Reset()
	{
		transform.SetPositionY(pos.Y);
		countDown = timer;
		active = false;
		finished = false;
	}

}