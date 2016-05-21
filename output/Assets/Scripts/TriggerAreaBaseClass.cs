using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class TriggerAreaBaseClass : MochaScript
{
    protected CMeshRenderer triggerMesh;
    protected CPhysics triggerPhysics;

    protected bool isactive = true; // whether is can be selected
    public bool isActive
    {
        get { return isactive; }
        set { isactive = value; }
    }

    public virtual void OnStart()
    {
		gameObject.mLayerMask.AddLayer(LayerMask.Layers.LAYER_NORAYCAST);
		
        // Disable Collision trigger stuff
        triggerPhysics = gameObject.RequireComponent<CPhysics>();
        triggerPhysics.mColliderType = 0; // static
        triggerPhysics.mIsTriggered = true; // Disable collision response

        // Disable CMeshRenderer
        triggerMesh = gameObject.RequireComponent<CMeshRenderer>();
        triggerMesh.setEnabled(false);
    }

    public virtual void OnAreaEnter() { }
    public virtual void OnArea() { }
}
