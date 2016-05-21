using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class FirstPersonStealthInteractor : MochaScript
{
	GameObject lastSelectedObj;
	bool skipFrame;
	
	public void OnStart()
	{
        skipFrame = false;
	}

	public void OnUpdate()
	{
		GameObject gobj = PhysicEngine.getInstance().RayCast3DWithTag(gameObject.transform.position,
                                                         gameObject.transform.GetForwardVector(), 5.0f, Common.tagForStealthInteractables);
		
		if (lastSelectedObj != gobj) 
		{
			if (gobj != null)
			{
				GetScriptClass<StealthInteractableObject>(gobj).OnStealthPlayerMouseEnter();
			}
			if (lastSelectedObj != null)
			{
				GetScriptClass<StealthInteractableObject>(lastSelectedObj).OnStealthPlayerMouseLeave();
			}
		}
		
		if (gobj != null)
		{
            if (Common.GetStealthPlayerScript().IsStealthPlayerInteracting() )
            {
				GetScriptClass<StealthInteractableObject>(gobj).OnStealthPlayerMouseInteract();
				// Logger.Log(gobj.GetName() + " was interacted with.");
			}
		}
		
		lastSelectedObj = gobj;
	}
}
