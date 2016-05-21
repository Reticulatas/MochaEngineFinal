using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class RespawnBoxBackToPos : MochaScript
{
	Vector3 oldPos = new Vector3();
	CPhysics playerPhysics;
    public float limitY = -1.0f;

    public void OnStart()
    {
		oldPos = gameObject.transform.position;
		
		playerPhysics = gameObject.RequireComponent<CPhysics>();
	}
	public void OnUpdate()
    {
		if(gameObject.transform.position.y < limitY)
		{
			gameObject.transform.SetPosition(oldPos);
			playerPhysics.rigidCom.Body().SetLinearVelocity(Vector3.Zero);
			playerPhysics.rigidCom.Body().SetAngularVelocity(Vector3.Zero);
		}
	}

}