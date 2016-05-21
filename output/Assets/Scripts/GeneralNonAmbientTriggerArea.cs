using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class GeneralNonAmbientTriggerArea : TriggerAreaBaseClass
{
    // AMB_SCIENCE, AMB_HUB, AMB_MONSTER_ROOM, MUSIC_HUB_LOOP
    public String NonAmbientSound { get; set; }

	float mTimer;
	public float TimeToPlay { get { return mTimer; }
        set { mTimer = value; } }

	CSound sound;
	
	bool activated = false;
	bool finished = false;
    public override void OnStart()
    {
        base.OnStart();
		sound = gameObject.RequireComponent<CSound>();
		
    }

    public void OnUpdate()
    {
		if(activated)
		{
			if(mTimer > 0)
			{
				mTimer -= FrameController.DT();
			}
			else
			{
                String filename = NonAmbientSound + ".vente";
                sound.PlayIndependentEvent(filename, false, 0);
				activated = false;
			}
		}
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
			activated = true;
            isactive = false;
        }
    }
    public override void OnArea()
    {

    }
}
