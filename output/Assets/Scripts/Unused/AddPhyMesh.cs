using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class AddPhyMesh : MochaScript
{
	CPhysics Camcphy;
	CMeshColliderComponent bcc;
	public void OnStart()
	{
		Camcphy = gameObject.RequireComponent<CPhysics>();
		bcc     = gameObject.RequireComponent<CMeshColliderComponent>();
		//Camcphy.mIsDynamic = true;
	}
	public void OnUpdate()
	{
	}
}