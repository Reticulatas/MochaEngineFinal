using Microsoft.Xna.Framework;
using MochaInterface;using System;

public class StealthInteractableObject : MochaScript
{
	//override this
	public virtual void OnStart()
	{
        gameObject.SetFlag(Common.tagForStealthInteractables);
	}

	//implement these
	public virtual void OnUpdate() {}
	public virtual void OnStealthPlayerMouseEnter() {}
	public virtual void OnStealthPlayerMouseLeave() {}
	public virtual void OnStealthPlayerMouseInteract() {}
}