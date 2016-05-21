using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class SoundTestStop : MochaScript
{
	CSound sound_;

    public void OnStart()
    {
		sound_ = gameObject.RequireComponent<CSound>();
		// sound_.AddSound("Combat1.vente");
        // sound_.Play("Combat1.vente");
        // sound_.Stop("Combat1.vente");
    }

    public void OnUpdate()
    {
    }

    public void OnEnd()   {    }
}