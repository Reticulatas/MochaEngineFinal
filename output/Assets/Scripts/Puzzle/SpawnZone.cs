using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class SpawnZone : MochaScript
{
	
	CTransform _transform;
	
	float minZ, maxZ;
	float minX, maxX;
	float y;
	
	int _region = 0;
	String _name;
	public int Region  { get { return _region; } set { _region = value; }	}
	public String roomName { get { return _name; } set { _name = value; }	}
	Vector3 position = new Vector3();
	
    public void OnStart()
    {
        CMeshRenderer mesh = gameObject.RequireComponent<CMeshRenderer>();
        mesh.setEnabled(false);

	    GameObject room = GameObject.GetGameObjectByName(_name);
		PuzzleBEnvironmentController envScript = GetScript<PuzzleBEnvironmentController>(room);
		envScript.AddSpawnZone(_region,this);
		
		_transform = gameObject.RequireComponent<CTransform>();
		
		Vector3 pos = _transform.GetPosition(CTransform.TransformSpace.GLOBAL);
		Vector3 scale = _transform.scale;
		
		position = _transform.GetPosition(CTransform.TransformSpace.GLOBAL);
		
		minZ = pos.Z - scale.Z * 0.5f;
		maxZ = pos.Z + scale.Z * 0.5f;
		
		minX = pos.X - scale.X * 0.5f;
		maxX = pos.X + scale.X * 0.5f;
		
		y = pos.Y;
    }

    public void OnUpdate()
    {
        
    }
	
	public Vector3 GetRandomPosition()
	{
		float x = MMath.GetRandomLimitedFloat(minX, maxX);
		float z = MMath.GetRandomLimitedFloat(minZ, maxZ);
		return new Vector3(x,y,z);
	}
	//
	public Vector3 GetPosition()
	{
		return position;
	}
}
 