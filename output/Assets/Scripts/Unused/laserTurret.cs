using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class laserTurret : MochaScript
{
	CSound sound_;
	GameObject player;
	
	public void OnStart()
	{
		player = GameObject.GetGameObjectByName("Camera");
		sound_ = gameObject.RequireComponent<CSound>();
		// sound_.AddSound("TURRET_LASER_TEMP.wav");
		// sound_.Stop();
	}

	public void OnUpdate()
	{
		float disttodeath = player.transform.position.z - gameObject.transform.position.z;
		disttodeath = Math.Abs(disttodeath);
		// if (disttodeath < 11.0f)
        //     sound_.Play("TURRET_LASER_TEMP.wav");
	}
}
