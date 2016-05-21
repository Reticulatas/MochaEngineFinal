using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class DebrisTrigger : TriggerAreaBaseClass
{

	List<CPhysics> debris = new List<CPhysics>();
	
	String tagName;
	CSound caution;
	bool activated = false;
	
	public String TagName { get { return tagName; }
        set { tagName = value; } }
		
    public override void OnStart()
    {
		base.OnStart();
        if (String.IsNullOrEmpty(tagName))
			return;
			
        foreach (GameObject obj in GameObject.GetGameObjectsWithTag(tagName))
        {
            debris.Add(obj.RequireComponent<CPhysics>());
        }
        caution = gameObject.RequireComponent<CSound>();
		
		//isActive = false;
    }
    
    public void OnUpdate()
    {

    }
	public override void OnAreaEnter()
    {
		if(!activated)
		{
			foreach(CPhysics obj in debris)
			{
				obj.setEnabled(true);
				obj.mColliderType = 1;
			}
		}
		
    }
    public override void OnArea()
    {

    }
}