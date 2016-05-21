using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class SoundTest : MochaScript
{
	CSound sound_;
	public string soundtoplay {get; set;}

    public void OnStart()
    {
		sound_ = gameObject.RequireComponent<CSound>();
        sound_.PlayIndependentEvent(soundtoplay, false, 0);
    }

    public void OnUpdate()
    {
    }

    public void OnEnd()   {    }
}