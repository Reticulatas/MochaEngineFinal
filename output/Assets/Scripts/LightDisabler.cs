using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class LightDisabler : TriggerAreaBaseClass
{

	List<CLight> lights = new List<CLight>();
	
	String tagName;
	float timeBetweenLights = 0.0f;
	float currTimeBetweenLight = -10.0f;
	
	public String TagName { get { return tagName; }
        set { tagName = value; } }
	public float TimeBetweenLights { get { return timeBetweenLights; }
        set { timeBetweenLights = value; } }
		
	bool finished = false;
    bool activated = false;
    public override void OnStart()
    {
        base.OnStart();
        if (String.IsNullOrEmpty(tagName))
			return;
			
        foreach (GameObject obj in GameObject.GetGameObjectsWithTag(tagName))
        {
            lights.Add(obj.RequireComponent<CLight>());
        }
        
		//isActive = false;
    }
    
    public void OnUpdate()
    {
        if (finished || !activated)
			return;
		
		float dt = FrameController.DT();
		
		foreach(CLight obj in lights)
		{
			if(!obj.isEnabled())
				continue;
			else
			{
                if (currTimeBetweenLight < 0.0f)
				{
					currTimeBetweenLight = timeBetweenLights;
					obj.setEnabled(false);
				}
				else
                    currTimeBetweenLight -= dt;
					
				break;
			}
		}
    }
	public override void OnAreaEnter()
    {
        activated = true;
    }
    public override void OnArea()
    {

    }
}