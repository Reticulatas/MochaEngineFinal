using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PulsingLightScript : MochaScript
{
	protected CLight compLight;
	bool isactive = false;
	float startAtt = 4.0f;
	float endAtt = 0.5f;
	float   pulseRate = 1.0f;
	float currAtt;
	bool fadeOut = true;
	
	public bool isActive
    {
        get { return isactive; }
        set { isactive = value; }
    }
	public float StartAttenuation
    {
        get { return startAtt; }
        set { startAtt = value; }
    }
	public float EndAttenuation
    {
        get { return endAtt; }
        set { endAtt = value; }
    }
	public float PulseRate
    {
        get { return pulseRate; }
        set { pulseRate = value; }
    }
    public void OnStart()
    {
		compLight = gameObject.RequireComponent<CLight>();
		compLight.SetAtt(startAtt);
		currAtt = startAtt;
    }

    public void OnUpdate()
    {
		if(!isactive)
			return;
		else
		{
			float dt = FrameController.DT() * pulseRate;
		
			if(fadeOut)
			{
				currAtt -= dt;
				compLight.SetAtt(currAtt);
				
				if(currAtt < endAtt)
					fadeOut = false;
			}
			else
			{
				currAtt += dt;
				compLight.SetAtt(currAtt);
				
				if(currAtt > startAtt)
					fadeOut = true;
			
			}
		}
		
    }


}
